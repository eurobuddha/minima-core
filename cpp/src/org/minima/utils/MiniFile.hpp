#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/encrypt/PasswordCrypto.hpp"
#include "support/ByteIO.hpp"
#include "support/JavaIOExceptionText.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
namespace minima {
class MiniFile {
    template<class T> static void readObject(T& object,std::span<const uint8_t> bytes) {size_t offset=0;if constexpr(requires {object.readDataStream(bytes,offset);}) object.readDataStream(bytes,offset);else object=T::deserialise(bytes,offset);}
    static void log(const std::exception& error) {if(auto io=dynamic_cast<const wire::IOException*>(&error))std::clog<<javaIOExceptionText(*io)<<'\n';else std::clog<<error.what()<<'\n';}
public:
    static std::filesystem::path createBaseFile(const std::string& name);
    static void writeDataToFile(const std::filesystem::path& file,std::span<const uint8_t> data,bool append=false);
    template<class T> static void writeObjectToFile(const std::filesystem::path& file,T& object,bool append=false) {writeDataToFile(file,object.serialise(),append);}
    static std::vector<uint8_t> readCompleteFile(const std::filesystem::path& file);
    static std::vector<uint8_t> readAllBytes(std::istream& stream);
    template<class T> static void loadObject(const std::filesystem::path& file,T& object) {loadObjectSlow(file,object);}
    template<class T> static void loadObjectSlow(const std::filesystem::path& file,T& object) {
        std::error_code ec;if(!std::filesystem::exists(file,ec)) {logObjectMissing(file);return;}
        try {auto bytes=readCompleteFile(file);readObject(object,bytes);}catch(const std::ios_base::failure& e){log(e);}catch(const wire::IOException& e){log(e);}
    }
    template<class T> static void loadObjectEncrypted(const std::string& password,const std::filesystem::path& file,T& object) {
        std::error_code ec;if(!std::filesystem::exists(file,ec)) {std::clog<<"Load Object file does not exist : "<<std::filesystem::absolute(file).string()<<'\n';return;}
        try {auto bytes=PasswordCrypto::decryptPassword(password,MiniData(readCompleteFile(file)));readObject(object,bytes.bytes());}catch(const std::exception& e){log(e);}
    }
    template<class T> static void saveObject(const std::filesystem::path& file,T& object) {try {writeDataToFile(file,object.serialise());}catch(const std::ios_base::failure& e){log(e);}catch(const wire::IOException& e){log(e);}}
    template<class T> static void saveObjectDirect(const std::filesystem::path& file,T& object) {try {auto out=openObjectOutput(file);auto bytes=object.serialise();out.write(reinterpret_cast<const char*>(bytes.data()),std::streamsize(bytes.size()));out.flush();out.close();}catch(const std::ios_base::failure& e){logObjectOutput(wire::IOException(e.what()));}catch(const wire::IOException& e){logObjectOutput(e);}}
    template<class T> static void saveObjectEncrypted(const std::string& password,const std::filesystem::path& file,T& object) {try {auto bytes=PasswordCrypto::encryptPassword(password,MiniData(object.serialise()));writeDataToFile(file,bytes.bytes());}catch(const std::exception& e){log(e);}}
    static void copyFile(const std::filesystem::path& original,const std::filesystem::path& copy);
    static void copyFileOrFolder(const std::filesystem::path& original,const std::filesystem::path& copy);
    static void deleteFileOrFolder(const std::string& parentCheck,const std::filesystem::path& file);
    static int64_t getTotalFileSize(const std::filesystem::path& folder);
    static int64_t getTotalFileSizeWithNames(const std::filesystem::path& folder,json::JSONObject& result,int32_t maxDepth,int32_t depth);
    static std::string getContentType(const std::string& file);
    static bool isChild(const std::filesystem::path& parent,const std::filesystem::path& child);
    static void decompressGzipFile(const std::filesystem::path& gzip,const std::filesystem::path& file);
    static void compressGzipFile(const std::filesystem::path& file,const std::filesystem::path& gzip);
private:
    static std::ofstream openObjectOutput(const std::filesystem::path& file);
    static void logObjectOutput(const wire::IOException& error);
    static void logObjectMissing(const std::filesystem::path& file);
    static void prepareFile(const std::filesystem::path& file,bool append);
};
}
