#include "tokenvalidate.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/utils/RPCClient.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "support/RawTxnCommands.hpp"
#include "support/JavaStringTrim.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::commands {
tokenvalidate::tokenvalidate():Command("tokenvalidate","[tokenid:] - validate the signature and web link in a token"){}
std::optional<std::string> tokenvalidate::getFullHelp() const {return std::string(
"\ntokenvalidate\n"
"\n"
"Validate the signature and webvalidate link in a token.\n"
"\n"
"tokenid:\n"
"    The tokenid of the custom token/NFT to validate.\n"
"\n"
"Examples:\n"
"\n"
"tokenvalidate tokenid:0xFED5..\n");}
json::JSONValue tokenvalidate::runCommand(){
    auto result=getJSONReply();auto id=getParam("tokenid");
    if(id=="0x00")throw CommandException("The Main Minima Token 0x00 has no signature");
    auto tokens=TxPoWSearcher::getAllTokens();std::shared_ptr<Token> selected;
    for(const auto& token:tokens){auto tokenid=sqlRequired(token).getTokenID();if(!tokenid)throw JavaNullPointerException("null token ID");if(tokenid->to0xString()==id){selected=token;break;}}
    if(!selected)throw CommandException("Token not found : "+id);
    auto parsed=json::parser::JSONParser().parse(selected->getName().toString());const auto& description=raw_detail::object(parsed);
    auto stringCast=[](const json::JSONValue& value)->std::optional<std::string>{if(value.isNull())return {};if(!value.isString())throw JavaClassCastException("token metadata is not a String");return value.asString();};
    auto data=[](const std::optional<std::string>& text){if(!text)throw JavaNullPointerException("null token metadata bytes");return MiniData::fromHex(*text);};
    json::JSONObject response,sign;
    if(description.containsKey("signature")&&description.containsKey("signedby")){
        sign.put("signed",true);auto signature=Signature::convertMiniDataVersion(data(stringCast(description.get("signature"))));
        if(!signature)throw JavaNullPointerException("null token signature");auto root=signature->getRootPublicKey();
        auto publickey=data(stringCast(description.get("signedby")));
        if(!publickey.isEqual(root)){sign.put("valid",false);sign.put("reason","Public key does not match signedby : "+root.to0xString());}
        else{sign.put("signedby",publickey.to0xString());auto coinid=selected->getCoinID();TreeKey key;key.setPublicKey(signature->getRootPublicKey());bool valid=key.verify(coinid,*signature);sign.put("valid",valid);if(!valid)sign.put("reason","Signature fails");}
    }else sign.put("signed",false);
    response.put("signature",sign);json::JSONObject web;
    if(description.containsKey("webvalidate")){
        web.put("webvalidate",true);auto url=stringCast(description.get("webvalidate"));web.put("url",url?json::JSONValue(*url):json::JSONValue());
        try{auto value=javaStringTrim(RPCClient::sendGET(url));auto tokenid=selected->getTokenID();if(!tokenid)throw JavaNullPointerException("null token ID");bool valid=value==tokenid->to0xString();web.put("valid",valid);if(!valid)web.put("reason","Data in file does not match tokenid : "+value);}
        catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
        catch(const std::exception& error){web.put("valid",false);web.put("reason","Could not download web file : "+nativeExceptionText(error));}
    }else web.put("webvalidate",false);
    response.put("web",web);result.put("response",response);return result;
}
}
