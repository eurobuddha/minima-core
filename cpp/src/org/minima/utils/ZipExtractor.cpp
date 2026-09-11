#include "ZipExtractor.hpp"
#include "MinimaLogger.hpp"
#include "support/ZipInput.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaUnixCanonicalFile.hpp"
#include "support/BufferedFileOutput.hpp"
namespace minima {
namespace {
bool fileExists(const std::filesystem::path& path) {
    if(path.string().find('\0')!=std::string::npos)return false;
    std::error_code error;return std::filesystem::exists(path,error);
}
bool directory(const std::filesystem::path& path) {
    if(path.string().find('\0')!=std::string::npos)return false;
    std::error_code error;return std::filesystem::is_directory(path,error);
}
const std::filesystem::path& required(const std::optional<std::filesystem::path>& path) {
    if(!path)throw JavaNullPointerException("");return *path;
}
}
std::filesystem::path ZipExtractor::newFile(const std::optional<std::filesystem::path>& destination,const std::string& name) {
    auto file=destination?javaUnixChildFile(*destination,name):*javaUnixFile(std::filesystem::path(name));
    if(fileExists(file))MinimaLogger::log("processing zip entry "+required(destination).string()+" but it already exists");
    auto parent=javaUnixCanonicalFile(required(destination));auto child=javaUnixCanonicalFile(file);
    if(!child.starts_with(parent+"/"))throw wire::IOException("Entry is outside of the target dir: "+name);
    return file;
}
void ZipExtractor::unzip(Reader archive,const std::optional<std::filesystem::path>& destination,std::function<void()> close) {
    std::array<uint8_t,1024> buffer;
    ZipInput input(std::move(archive),std::move(close));
    auto entry=input.getNextEntry();
    while(entry) {
        auto file=newFile(javaUnixFile(destination),entry->name);
        if(entry->isDirectory()) {
            if(!directory(file) && !javaUnixMkdirs(file))throw wire::IOException("Failed to create directory "+file.string());
        } else {
            auto parent=file.parent_path();
            if(parent.empty())throw JavaNullPointerException("");
            if(!directory(parent) && !javaUnixMkdirs(parent))throw wire::IOException("Failed to create directory "+parent.string());
            BufferedFileOutput output(file);
            while(auto count=input.read(buffer))output.writeUnbuffered(std::span(buffer).first(count));
            output.close();
        }
        entry=input.getNextEntry();
    }
    input.closeEntry();input.close();
}
}
