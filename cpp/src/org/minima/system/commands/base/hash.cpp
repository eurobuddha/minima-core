#include "hash.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/Crypto.hpp"
namespace minima::commands {
hash::hash():Command("hash","(data:) (file:) (type:sha2|sha3)- Hash the data - default SHA3"){}
std::optional<std::string> hash::getFullHelp() const {return std::string(
"\nhash\n"
"\n"
"Hash the data or file - default SHA3.\n"
"\n"
"Returns the hash of the data provided using the algorithm specified.\n"
"\n"
"data:\n"
"    The data to hash. Can be HEX (0x) or a string in quotes.\n"
"    String data will return the the byte representation of the string.\n"
"\n"
"file:\n"
"    The file path - can be the full path or relative to your base folder\n"
"\n"
"type: (optional)\n"
"    sha2 or sha3. The hashing algorithm to use, default is SHA3.\n"
"    BTC and ETH support sha2 or sha3.\n"
"\n"
"Examples:\n"
"\n"
"hash data:0x1C8AFF950685C2ED4BC3174F3472287B56D9517B9C948127319A09A7A36DEAC8\n"
"\n"
"hash file:myfile.txt\n"
"\n"
"hash data:\"this is my secret\" type:sha2\n");}
std::vector<std::string> hash::getValidParams() const {return {"data","type","file"};}
json::JSONValue hash::runCommand() {
    auto result=getJSONReply();std::string input;MiniData data;bool isFile=false;std::filesystem::path file;
    if(existsParam("file")){
        isFile=true;file=MiniFile::createBaseFile(getParam("file"));std::error_code ec;
        if(!std::filesystem::exists(file,ec))throw CommandException("File doesn't exist : "+std::filesystem::absolute(file).string());
        data=MiniData(MiniFile::readCompleteFile(file));
    }else{
        if(isParamJSONObject("data"))input=getJSONObjectParam("data")->toString();
        else if(isParamJSONArray("data"))input=getJSONArrayParam("data")->toString();
        else input=getParam("data");
        data=input.starts_with("0x")?MiniData::fromHex(input):MiniData(MiniString(input).getData());
    }
    auto type=getParam("type",std::string("sha3"));std::vector<uint8_t> digest;
    if(type=="sha2")digest=Crypto::getInstance().hashSHA2(data.bytes());
    else if(type=="sha3")digest=Crypto::getInstance().hashData(data.bytes());
    else throw CommandException("Invalid hash type : "+type);
    json::JSONObject response;
    if(isFile){std::error_code ec;auto length=std::filesystem::file_size(file,ec);response.put("file",file.filename().string());response.put("path",std::filesystem::absolute(file).string());response.put("size",ec?int64_t(0):int64_t(length));}
    else{response.put("input",input);response.put("data",data.to0xString());}
    response.put("type",type);response.put("hash",MiniData(digest).to0xString());result.put("response",response);return result;
}
std::shared_ptr<Command> hash::getFunction() const {return std::make_shared<hash>();}
}
