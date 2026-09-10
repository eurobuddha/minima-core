#include "convert.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "support/Base64.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaLowerCase.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
namespace minima::commands {
convert::convert():Command("convert","[from:] [to:] [data:] - Convert between different data types (String, HEX, Mx, Base64)"){}
std::optional<std::string> convert::getFullHelp() const {return std::string(
"\nconvert\n"
"\n"
"Convert between different data types\n"
"\n"
"Returns converted data.\n"
"\n"
"from:\n"
"    The type of the data param.\n"
"\n"
"to:\n"
"    The type you want to convert to.\n"
"\n"
"data:\n"
"    The the data to convert.\n"
"\n"
"Examples:\n"
"\n"
"convert from:String to:HEX data:hello\n"
"\n"
"convert from:HEX to:Mx data:0XFFFF\n"
"\n"
"convert from:String to:Base64 data:hello\n");}
std::vector<std::string> convert::getValidParams() const {return {"from","to","data"};}
json::JSONValue convert::runCommand(){
    auto result=getJSONReply();auto from=javaLowerCase(getParam("from")),to=javaLowerCase(getParam("to"));std::string data;
    if(isParamJSONObject("data"))data=getJSONObjectParam("data")->toString();else if(isParamJSONArray("data"))data=getJSONArrayParam("data")->toString();else data=getParam("data");
    std::string converted;
    try{MiniData bytes;
        if(from=="hex")bytes=MiniData::fromHex(data);else if(from=="mx")bytes=Address::convertMinimaAddress(data);else if(from=="string")bytes=MiniData(MiniString(data).getData());else if(from=="base64")bytes=MiniData(decodeBase64(data));else throw CommandException("Invalid FROM type : "+from);
        if(to=="hex")converted=bytes.to0xString();else if(to=="mx")converted=Address::makeMinimaAddress(bytes);else if(to=="string")converted=MiniString(bytes.bytes()).toString();else if(to=="base64")converted=encodeBase64(bytes.bytes());else throw CommandException("Invalid TO type : "+to);
    }catch(const CommandException&){throw;}catch(const std::exception& e){throw CommandException(javaExceptionText(e));}
    json::JSONObject response;response.put("conversion",converted);result.put("response",response);return result;
}
std::shared_ptr<Command> convert::getFunction() const {return std::make_shared<convert>();}
}
