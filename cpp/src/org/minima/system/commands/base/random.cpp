#include "random.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "support/Base64.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaLowerCase.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
namespace minima::commands {
random::random():Command("random","(size:) (type:) - Generate a random hash value, defaults to 32 bytes"){}
std::optional<std::string> random::getFullHelp() const {return std::string(
"\nrandom\n"
"\n"
"Generate a random hash value, defaults to 32 bytes.\n"
"\n"
"size: (optional)\n"
"    Integer number of bytes for the hash value.\n"
"\n"
"type: (optional)\n"
"    sha3 (default) or sha2.\n"
"\n"
"Examples:\n"
"\n"
"random\n"
"\n"
"random size:64\n");}
std::vector<std::string> random::getValidParams() const {return {"size","type"};}
json::JSONValue random::runCommand(){
    auto result=getJSONReply();auto size=getNumberParam("size",MiniNumber(32));auto count=size.getAsInt();if(count<0)throw JavaNegativeArraySizeException(std::to_string(count));auto bytes=MiniData::getRandomData(count);auto type=getParam("type",std::string("sha3"));std::vector<uint8_t> digest;
    if(type=="sha2")digest=Crypto::getInstance().hashSHA2(bytes.bytes());else if(type=="sha3")digest=Crypto::getInstance().hashData(bytes.bytes());else throw CommandException("Invalid hash type : "+type);
    json::JSONObject response;response.put("size",size.toString());response.put("random",bytes.to0xString());response.put("hashed",MiniData(digest).to0xString());response.put("type",type);
    if(count>=16){auto base32=BaseConverter::encode32(bytes.bytes());if(base32.size()<26)throw std::out_of_range("Random keycode substring");std::string code;for(size_t i:{2,7,12,17,22}){if(!code.empty())code+='-';code+=base32.substr(i,4);}response.put("keycode",code);}
    result.put("response",response);return result;
}
std::shared_ptr<Command> random::getFunction() const {return std::make_shared<random>();}
}
