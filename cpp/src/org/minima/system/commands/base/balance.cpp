#include "balance.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "support/RawTxnCommands.hpp"
#include <unordered_map>
#include <algorithm>
namespace minima::commands {
balance::balance():Command("balance","(address:) (tokenid:) (confirmations:) (megammr:) - Show your total balance of Minima and tokens"){}
std::optional<std::string> balance::getFullHelp() const {return std::string(
"\nbalance\n"
"\n"
"Show your total balance of Minima and tokens.\n"
"\n"
"address: (optional)\n"
"    Show the balance for a specific 0x or Mx address.\n"
"\n"
"tokenid: (optional)\n"
"    Show the balance for a specific tokenid. Minima is 0x00.\n"
"\n"
"tokendetails: (optional)\n"
"    true or false, show the complete details for the tokens\n"
"\n"
"confirmations: (optional)\n"
"    Set the number of block confirmations required before a coin is considered confirmed in your balance. Default is 3.\n"
"\n"
"megammr: (optional)\n"
"    Search the MegaMMR for coins too.\n"
"\n"
"simple: (optional)\n"
"    true or flase - show a much simpler view.. just the name and confirmed amount.\n"
"\n"
"coinlist: (optional)\n"
"    true or flase - List all the valid coins.\n"
"\n"
"tokendetails: (optional)\n"
"    true or flase - Show all the token details.\n"
"\n"
"Examples:\n"
"\n"
"balance\n"
"\n"
"balance simple:true\n"
"\n"
"balance tokenid:0xFED5.. confirmations:10\n"
"\n"
"balance address:0xFF..\n");}
std::vector<std::string> balance::getValidParams() const {return {"address","tokenid","confirmations","tokendetails","megammr","simple","coinlist"};}
std::shared_ptr<Command> balance::getFunction() const {return std::make_shared<balance>();}
json::JSONValue balance::runCommand(){
    auto ret=getJSONReply();auto address=getAddressParam("address","");
    auto confirmations=getNumberParam("confirmations",GlobalParams::MINIMA_CONFIRM_DEPTH);
    bool debug=getBooleanParam("debug",false),simpleview=getBooleanParam("simple",false),listcoins=getBooleanParam("coinlist",false);
    bool tokendetails=getBooleanParam("tokendetails",false);auto onlytokenid=getParam("tokenid","");
    auto tree=sqlRequired(MinimaDB::getDB()).getTxPoWTree();
    if(!sqlRequired(tree).getTip())throw CommandException("No blocks yet..");
    json::JSONArray balances;
    std::vector<std::string> alltokens;
    std::unordered_map<std::string,Token> tokens;
    std::unordered_map<std::string,MiniNumber> confirmed,unconfirmed,sendable,totalcoins;
    std::unordered_map<std::string,std::vector<Coin>> coinlist;
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    bool checkmegammr=getBooleanParam("megammr",false);if(checkmegammr)checkmegammr=GeneralParams::IS_MEGAMMR;
    auto coins=address.empty()?TxPoWSearcher::getAllRelevantUnspentCoins(tree->getTip()):
        TxPoWSearcher::searchCoins(tree->getTip(),false,false,MiniData::ZERO_TXPOWID,false,MiniNumber::ZERO,
            true,MiniData::fromHex(address),false,MiniData::ZERO_TXPOWID,false,"",true,false,INT32_MAX,checkmegammr);
    auto topblock=sqlRequired(tree->getTip()).getBlockNumber();
    auto minima=Token::TOKENID_MINIMA.to0xString();alltokens.push_back(minima);totalcoins.emplace(minima,MiniNumber::ZERO);coinlist.emplace(minima,std::vector<Coin>{});
    if(debug)MinimaLogger::log("List of found relevant coins");
    auto add=[](auto& table,const std::string& id,const MiniNumber& amount){auto found=table.find(id);if(found==table.end())table.emplace(id,amount);else found->second=found->second.add(amount);};
    for(const auto& coin:coins){
        if(!address.empty()&&coin.getAddress().to0xString()!=address)continue;
        if(!onlytokenid.empty()&&coin.getTokenID().to0xString()!=onlytokenid)continue;
        if(debug)MinimaLogger::log("Coin : "+coin.toJSON().toString());
        auto amount=coin.getAmount();auto tokenid=coin.getTokenID().to0xString();
        if(std::find(alltokens.begin(),alltokens.end(),tokenid)==alltokens.end()){
            alltokens.push_back(tokenid);
            if(!coin.getToken())throw JavaNullPointerException("null token in Hashtable.put");
            tokens.emplace(tokenid,*coin.getToken());
        }
        auto depth=topblock.sub(coin.getBlockCreated());bool isconfirmed=!depth.isLess(confirmations);
        add(isconfirmed?confirmed:unconfirmed,tokenid,amount);add(totalcoins,tokenid,MiniNumber::ONE);coinlist[tokenid].push_back(coin);
        if(debug){bool simple=sqlRequired(wallet).isAddressSimple(coin.getAddress().to0xString());MinimaLogger::log(std::string(simple?"SENDABLE : ":"NON-SENDABLE : ")+coin.toJSON().toString());}
        if(tokenid=="0x00"){
            if(isconfirmed&&sqlRequired(wallet).isAddressSimple(coin.getAddress().to0xString()))add(sendable,tokenid,amount);
        }else{
            if(!coin.getToken())throw JavaNullPointerException("null coin token");
            if(coin.getToken()->getTokenScript().toString()=="RETURN TRUE"&&isconfirmed&&sqlRequired(wallet).isAddressSimple(coin.getAddress().to0xString()))add(sendable,tokenid,amount);
        }
    }
    auto get=[](const auto& table,const std::string& id){auto found=table.find(id);return found==table.end()?MiniNumber::ZERO:found->second;};
    for(const auto& token:alltokens){
        auto unconf=get(unconfirmed,token),conf=get(confirmed,token),send=get(sendable,token),count=get(totalcoins,token);json::JSONObject tokbal;
        if(token=="0x00"){
            if(!onlytokenid.empty()&&onlytokenid!="0x00")continue;
            tokbal.put("token","Minima");tokbal.put("tokenid",token);tokbal.put("confirmed",conf.toString());tokbal.put("unconfirmed",unconf.toString());tokbal.put("sendable",send.toString());tokbal.put("coins",count.toString());
            if(listcoins){json::JSONArray list;for(const auto& coin:coinlist.at(token))list.add(coin.toJSON());tokbal.put("coinlist",list);}
            tokbal.put("total","1000000000");
        }else{
            const auto& tok=tokens.at(token);
            // The source overwrites its conditional String put with MiniString:
            // JSONValue's fallback writes that name verbatim, even invalid JSON.
            tokbal.put("token",json::JSONValue::raw(tok.getName().toString()));tokbal.put("tokenid",token);
            tokbal.put("confirmed",tok.getScaledTokenAmount(conf).toString());tokbal.put("unconfirmed",tok.getScaledTokenAmount(unconf).toString());tokbal.put("sendable",tok.getScaledTokenAmount(send).toString());tokbal.put("coins",count.toString());
            if(listcoins){json::JSONArray list;for(const auto& coin:coinlist.at(token))list.add(coin.toJSON());tokbal.put("coinlist",list);}
            tokbal.put("total",tok.getTotalTokens().toString());
            if(tokendetails){json::JSONObject details;details.put("decimals",json::JSONValue::raw(tok.getDecimalPlaces().toString()));details.put("script",tok.getTokenScript().toString());details.put("totalamount",tok.getAmount().toString());details.put("scale",tok.getScale().toString());details.put("created",tok.getCreated().toString());tokbal.put("details",details);}
        }
        balances.add(tokbal);
    }
    if(simpleview){
        json::JSONArray simplebalance;
        for(const auto& value:balances.elements()){
            const auto& oldbal=raw_detail::object(value);json::JSONObject newbal;auto id=oldbal.getString("tokenid");
            if(id=="0x00")newbal.put("name","Minima");
            else{auto parsed=json::parser::JSONParser().parse(tokens.at(id).getName().toString());newbal.put("name",raw_detail::object(parsed).get("name"));}
            newbal.put("amount",oldbal.getString("confirmed"));simplebalance.add(newbal);
        }
        ret.put("response",simplebalance);
    }else ret.put("response",balances);
    return ret;
}
}
