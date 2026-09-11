#include "MiniFile.hpp"
#include "MiniFormat.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/Gzip.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaExceptions.hpp"
#include "support/NativeThrowable.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <array>
#include <bit>
namespace minima {
namespace fs=std::filesystem;
namespace {
std::vector<fs::path> children(const fs::path& folder) {std::error_code ec;std::vector<fs::path> result;fs::directory_iterator it(folder,ec),end;while(!ec && it!=end){result.push_back(it->path());it.increment(ec);}return result;}
int64_t length(const fs::path& file) {std::error_code ec;auto value=fs::file_size(file,ec);return ec?0:std::bit_cast<int64_t>(uint64_t(value));}
bool fileExists(const fs::path& file) {std::error_code ec;return fs::exists(file,ec);}
bool isDirectory(const fs::path& file) {std::error_code ec;return fs::is_directory(file,ec);}
struct OutputFile {
    int descriptor;
    ~OutputFile(){if(descriptor>=0)::close(descriptor);}
};
}
fs::path MiniFile::createBaseFile(const std::string& name) {
    auto file=*javaUnixFile(fs::path(name));
    if(name.find_first_of("/\\")==std::string::npos&&!GeneralParams::BASE_FILE_FOLDER.empty())file=javaUnixChildFile(GeneralParams::BASE_FILE_FOLDER,name);
    if(file.has_parent_path())javaUnixMkdirs(file.parent_path());return file;
}
void MiniFile::prepareFile(const fs::path& file,bool append) {
    auto path=*javaUnixFile(file);auto text=path.string();
    auto parent=fs::path(javaUnixAbsoluteFile(path)).parent_path();
    if(!fileExists(parent))javaUnixMkdirs(parent);
    if(text.find('\0')!=std::string::npos)throw wire::IOException("Invalid file path");
    bool create=!fileExists(path);
    if(!create&&!append){std::error_code ignored;fs::remove(path,ignored);create=true;}
    if(create){
        // File.createNewFile is exclusive; a failed delete does not authorize
        // replacement here. An existing destination is opened by the next step.
        int descriptor;
        do {descriptor=::open(path.c_str(),O_WRONLY|O_CREAT|O_EXCL,0666);}while(descriptor<0&&errno==EINTR);
        if(descriptor<0){if(errno!=EEXIST)throw wire::IOException(std::strerror(errno));}
        else ::close(descriptor);
    }
}
std::ofstream MiniFile::openObjectOutput(const fs::path& file) {
    auto path=*javaUnixFile(file);prepareFile(path,false);
    std::ofstream out(path,std::ios::binary|std::ios::trunc);
    if(!out.is_open())throw wire::TypedIOException("java.io.FileNotFoundException",path.string()+" ("+std::strerror(errno)+")");
    out.exceptions(std::ios::badbit|std::ios::failbit);return out;
}
void MiniFile::logObjectMissing(const fs::path& file) {
    MinimaLogger::log("Load Object file does not exist : "+javaUnixAbsoluteFile(file));
}
void MiniFile::logObjectOutput(const wire::IOException& error) {
    MinimaLogger::log(std::make_shared<NativeThrowable>(error));
}
void MiniFile::writeDataToFile(const fs::path& file,std::span<const uint8_t> data,bool append) {
    auto path=*javaUnixFile(file);prepareFile(path,append);
    int descriptor;
    do {descriptor=::open(path.c_str(),O_WRONLY|O_CREAT|(append?O_APPEND:O_TRUNC),0666);}while(descriptor<0&&errno==EINTR);
    if(descriptor<0)throw wire::TypedIOException("java.io.FileNotFoundException",path.string()+" ("+std::strerror(errno)+")");
    OutputFile out{descriptor};
    while(!data.empty()){
        ssize_t count;do {count=::write(out.descriptor,data.data(),data.size());}while(count<0&&errno==EINTR);
        if(count<0)throw wire::IOException(std::strerror(errno));
        if(count==0)throw wire::IOException("Write failed");
        data=data.subspan(size_t(count));
    }
    auto closing=out.descriptor;out.descriptor=-1;
    if(::close(closing)<0)throw wire::IOException(std::strerror(errno));
}
std::vector<uint8_t> MiniFile::readCompleteFile(const fs::path& file) {
    auto path=*javaUnixFile(file);auto text=path.string();
    int32_t size=std::bit_cast<int32_t>(uint32_t(javaUnixFileLength(path)));
    if(size<0)throw JavaNegativeArraySizeException(std::to_string(size));
    std::vector<uint8_t> bytes(size);
    if(text.find('\0')!=std::string::npos)throw wire::TypedIOException("java.io.FileNotFoundException","Invalid file path");
    auto fail=[&](int error){throw wire::TypedIOException("java.io.FileNotFoundException",text+" ("+std::strerror(error)+")");};
    if(isDirectory(path))fail(EISDIR);
    errno=0;std::ifstream in(path,std::ios::binary);if(!in)fail(errno?errno:ENOENT);
    in.read(reinterpret_cast<char*>(bytes.data()),size);
    if(in.bad())throw wire::IOException(errno?std::strerror(errno):"Read error");
    return bytes;
}
std::vector<uint8_t> MiniFile::readAllBytes(std::istream& stream) {std::array<char,1024> buffer;std::vector<uint8_t> out;while(stream){stream.read(buffer.data(),buffer.size());auto count=stream.gcount();out.insert(out.end(),buffer.begin(),buffer.begin()+count);}if(stream.bad()) throw std::ios_base::failure("Stream read failed");return out;}
void MiniFile::copyFile(const fs::path& original,const fs::path& copy) {if(!fileExists(original)){std::clog<<"Trying to copy file that does not exist "<<fs::absolute(original).string()<<'\n';return;}std::ifstream in(original,std::ios::binary);if(!in) throw std::ios_base::failure("Cannot open source file");std::ofstream out(copy,std::ios::binary|std::ios::trunc);out.exceptions(std::ios::badbit|std::ios::failbit);std::array<char,16384> buffer;while(in){in.read(buffer.data(),buffer.size());out.write(buffer.data(),in.gcount());}if(in.bad())throw std::ios_base::failure("Copy read failed");out.close();}
void MiniFile::copyFileOrFolder(const fs::path& original,const fs::path& copy) {if(!fileExists(original)){std::clog<<"Trying to copy file that does not exist "<<fs::absolute(original).string()<<'\n';return;}if(isDirectory(original)){std::error_code ec;fs::create_directories(copy,ec);for(const auto& child:children(original))copyFileOrFolder(child,copy/child.filename());}else copyFile(original,copy);}
void MiniFile::deleteFileOrFolder(const std::string& parentCheck,const fs::path& file) {if(!fileExists(file))return;if(isDirectory(file))for(const auto& child:children(file))deleteFileOrFolder(parentCheck,child);auto absolute=fs::absolute(file).string();if(parentCheck.empty() || absolute.starts_with(parentCheck)){std::error_code ec;if(!fs::remove(file,ec))std::clog<<"ERROR deleting file "<<absolute<<'\n';}else std::clog<<"Attempt to delete File NOT child of parent check "<<absolute<<" / "<<parentCheck<<'\n';}
int64_t MiniFile::getTotalFileSize(const fs::path& folder) {uint64_t total=0;for(const auto& file:children(folder))total+=uint64_t(isDirectory(file)?getTotalFileSize(file):length(file));return std::bit_cast<int64_t>(total);}
int64_t MiniFile::getTotalFileSizeWithNames(const fs::path& folder,json::JSONObject& result,int32_t maxDepth,int32_t depth) {
    if(!isDirectory(folder))return 0;json::JSONObject dirs;uint64_t total=0;for(const auto& file:children(folder)){if(isDirectory(file)){json::JSONObject sub;total+=uint64_t(getTotalFileSizeWithNames(file,sub,maxDepth,std::bit_cast<int32_t>(uint32_t(depth)+1u)));dirs.put(file.filename().string(),sub);}else total+=uint64_t(length(file));}
    auto value=std::bit_cast<int64_t>(total);result.put("total",MiniFormat::formatSize(value));if(depth<maxDepth && dirs.size()>0)result.put("dirs",dirs);return value;
}
std::string MiniFile::getContentType(const std::string& file) {
    auto dot=file.rfind('.');if(dot==std::string::npos)return "text/plain";auto ext=file.substr(dot+1);
    for(const auto& [suffix,type]:std::array<std::pair<const char*,const char*>,20>{{{"html","text/html"},{"htm","text/html"},{"css","text/css"},{"js","text/javascript"},{"txt","text/plain"},{"xml","text/xml"},{"jpg","image/jpeg"},{"jpeg","image/jpeg"},{"png","image/png"},{"gif","image/gif"},{"svg","image/svg+xml"},{"ico","image/ico"},{"ttf","font/ttf"},{"zip","application/zip"},{"pdf","application/pdf"},{"wasm","application/wasm"},{"mp3","audio/mp3"},{"wav","audio/wav"},{"","text/plain"},{"unknown","text/plain"}}})if(ext==suffix)return type;return "text/plain";
}
bool MiniFile::isChild(const fs::path& parent,const fs::path& child) {auto normalize=[](const fs::path& path){auto text=fs::absolute(path).lexically_normal().string();while(text.size()>1 && text.back()=='/')text.pop_back();return text;};return normalize(child).starts_with(normalize(parent));}
void MiniFile::decompressGzipFile(const fs::path& gzip,const fs::path& file) {try {auto data=decompressGzip(readCompleteFile(gzip));std::ofstream out(file,std::ios::binary|std::ios::trunc);out.exceptions(std::ios::badbit|std::ios::failbit);out.write(reinterpret_cast<const char*>(data.data()),std::streamsize(data.size()));out.close();}catch(const std::exception& e){log(e);}}
void MiniFile::compressGzipFile(const fs::path& file,const fs::path& gzip) {try {auto data=compressGzip(readCompleteFile(file));std::ofstream out(gzip,std::ios::binary|std::ios::trunc);out.exceptions(std::ios::badbit|std::ios::failbit);out.write(reinterpret_cast<const char*>(data.data()),std::streamsize(data.size()));out.close();}catch(const std::exception& e){log(e);}}
}
