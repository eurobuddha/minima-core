#include "tokens.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
namespace minima::commands {
tokens::tokens():Command("tokens","(tokenid:) (action:import|export) (data:) - List, import or export tokens on the chain"){}
std::optional<std::string> tokens::getFullHelp() const {return std::string(
"\ntokens\n"
"\n"
"List all tokens in the unpruned chain.\n"
"\n"
"Optionally import or export tokens to share token data.\n"
"\n"
"tokenid: (optional)\n"
"    The tokenid of the token to search for or export.\n"
"\n"
"action: (optional)\n"
"    import : List your existing public keys.\n"
"    export : Create a new key.\n"
"\n"
"data: (optional)\n"
"    The data of the token to import, generated from the export.\n"
"\n"
"Examples:\n"
"\n"
"tokens\n"
"\n"
"tokens tokenid:0xFED5..\n"
"\n"
"tokens action:export tokenid:0xFED5..\n"
"\n"
"tokens action:import data:0x000..\n");}
std::vector<std::string> tokens::getValidParams() const {return {"tokenid","action","data"};}
json::JSONValue tokens::runCommand(){
    auto result=getJSONReply();auto tokenid=getParam("tokenid",std::string("")),action=getParam("action",std::string(""));
    if(action=="export"){auto token=TxPoWSearcher::getToken(MiniData::fromHex(tokenid));if(!token)throw CommandException("Token not found : "+tokenid);json::JSONObject response;response.put("tokenid",tokenid);response.put("data",MiniData(token->serialise()).to0xString());result.put("response",response);}
    else if(action=="import"){auto token=Token::convertMiniDataVersion(MiniData::fromHex(getParam("data")));auto imported=token?std::make_shared<Token>(*token):nullptr;TxPoWSearcher::importToken(imported);if(!imported)throw std::runtime_error("Null decoded token");json::JSONObject response;response.put("token",imported->toJSON());result.put("response",response);}
    else if(tokenid.empty()){
        json::JSONArray tokens;json::JSONObject minima;minima.put("name","Minima");minima.put("tokenid","0x00");minima.put("total","1000000000");minima.put("decimals",MiniNumber::MAX_DECIMAL_PLACES);minima.put("scale",1);tokens.add(minima);
        for(const auto& token:TxPoWSearcher::getAllTokens()){if(!token)throw std::runtime_error("Null token");tokens.add(token->toJSON());}result.put("response",tokens);
    }else {auto token=TxPoWSearcher::getToken(MiniData::fromHex(tokenid));if(!token)throw CommandException("Token not found : "+tokenid);result.put("response",token->toJSON());}return result;
}
std::shared_ptr<Command> tokens::getFunction() const {return std::make_shared<tokens>();}
}
