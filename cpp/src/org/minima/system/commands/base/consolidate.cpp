#include "consolidate.hpp"
#include "../send/send.hpp"
#include "support/CommandResult.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <bit>
namespace minima::commands {
consolidate::consolidate():Command("consolidate","[tokenid:] (coinage:) (maxcoins:) (maxsigs:) (burn:) (debug:) (dryrun:) - Consolidate coins by sending them back to yourself"){}
std::optional<std::string> consolidate::getFullHelp() const {return std::string(
"\nconsolidate\n"
"\n"
"Consolidate multiple coins (UTxOs) into one by sending them back to yourself. Must have at least 3 coins.\n"
"\n"
"Useful to prevent having many coins of tiny value and to manage the number of coins you are tracking.\n"
"\n"
"Optionally set the minimum coin age (in blocks), maximum number of coins and maximum number of signatures for the transaction.\n"
"\n"
"tokenid:\n"
"    The tokenid for Minima or custom token to consolidate coins for. Minima is 0x00.\n"
"\n"
"coinage: (optional)\n"
"    The minimum number of blocks deep (confirmations) a coin needs to be. Default is 3.\n"
"\n"
"maxcoins: (optional)\n"
"    The maximum number of coins to consolidate. Minimum 3, up to 20.\n"
"    Coins are first sorted by value (smallest first) before adding to the transaction.\n"
"\n"
"maxsigs: (optional)\n"
"    The maximum number of signatures for the transaction, up to 5.\n"
"    Coins are then sorted by address to minimize the number of signatures required.\n"
"\n"
"burn: (optional)\n"
"    Amount of Minima to burn with the transaction.\n"
"\n"
"debug: (optional)\n"
"    true or false, true will print more detailed logs.\n"
"\n"
"dryrun: (optional)\n"
"    true or false, true will simulate the consolidate transaction but not execute it.\n"
"\n"
"password: (optional)\n"
"    If your Wallet is password locked you can unlock it for this one transaction - then relock it.\n "
"\n"
"Examples:\n"
"\n"
"consolidate tokenid:0x00\n"
"\n"
"consolidate tokenid:0x77.. coinage:10\n"
"\n"
"consolidate tokenid:0x00 maxcoins:5 password:your_password\n"
"\n"
"consolidate tokenid:0x00 coinage:10 maxcoins:8 burn:1\n"
"\n"
"consolidate tokenid:0x00 coinage:10 maxcoins:8 maxsigs:3 burn:1 dryrun:true\n");}
std::vector<std::string> consolidate::getValidParams() const {return {"tokenid","coinage","maxcoins","maxsigs","burn","debug","dryrun","password"};}
std::shared_ptr<Command> consolidate::getFunction() const {return std::make_shared<consolidate>();}
json::JSONValue consolidate::runCommand(){
    auto ret=getJSONReply();auto tokenid=getParam("tokenid");auto burn=getNumberParam("burn",MiniNumber::ZERO);
    bool debug=getBooleanParam("debug",false);bool dryrun=getBooleanParam("dryrun",false);if(dryrun)debug=true;
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto coinage=getNumberParam("coinage",GlobalParams::MINIMA_CONFIRM_DEPTH);
    if(coinage.isLess(GlobalParams::MINIMA_CONFIRM_DEPTH))throw CommandException("Coinage MUST be >= "+GlobalParams::MINIMA_CONFIRM_DEPTH.toString());
    auto found=TxPoWSearcher::getRelevantUnspentCoins(tip,tokenid,true);std::vector<Coin> coins;
    auto minimum=sqlRequired(tip).getBlockNumber().sub(coinage);
    for(const auto& coin:found)if(coin.getBlockCreated().isLessEqual(minimum))coins.push_back(coin);
    coins=send::orderCoins(coins);auto totalcoins=int32_t(coins.size());
    if(totalcoins<3)throw CommandException("Not enough coins ("+std::to_string(totalcoins)+") to consolidate");
    auto maxsigs=getNumberParam("maxsigs",MiniNumber(5)).getAsInt();auto maxcoins=getNumberParam("maxcoins",MiniNumber(20)).getAsInt();
    std::string currentaddress;auto total=MiniNumber::ZERO;int32_t signatures=0,count=0;
    auto increment=[](int32_t value){return std::bit_cast<int32_t>(uint32_t(value)+1u);};
    for(const auto& coin:coins){
        auto address=coin.getAddress().to0xString();auto amount=coin.getAmount();
        if(coin.getTokenID().to0xString()!="0x00"){
            if(!coin.getToken())throw JavaNullPointerException("null token");amount=coin.getToken()->getScaledTokenAmount(coin.getAmount());
        }
        if(currentaddress!=address){
            if(increment(signatures)>maxsigs){if(debug)MinimaLogger::log("Consolidate - max sigs reached "+std::to_string(signatures));break;}
            currentaddress=address;signatures=increment(signatures);
        }
        total=total.add(amount);count=increment(count);
        if(debug)MinimaLogger::log("Consolidate - add coin "+amount.toString()+" totalcoins:"+std::to_string(count)+"  totalsigs:"+std::to_string(signatures)+" coinid:"+coin.getCoinID().to0xString());
        if(count>=maxcoins){if(debug)MinimaLogger::log("Consolidate - max coins reached "+std::to_string(count));break;}
    }
    auto row=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getDefaultAddress();auto address=sqlRequired(row).getAddress();
    if(!address)throw JavaNullPointerException("null default address");auto mine=MiniData::fromHex(*address);
    std::string command="send coinage:"+coinage.toString()+" split:2 dryrun:"+(dryrun?"true":"false")+" debug:"+(debug?"true":"false")+" burn:"+burn.toString()+" amount:"+total.toString()+" address:"+mine.to0xString()+" tokenid:"+tokenid;
    if(existsParam("password")){
        if(debug)MinimaLogger::log("Consolidate command : "+command+" password:####");
        command+=" password:\""+getParam("password")+"\"";
    }else if(debug)MinimaLogger::log("Consolidate command : "+command);
    auto result=command_result::first(command);
    if(command_result::status(result))ret.put("response",result.get("response"));
    else{ret.put("status",false);if(!result.get("message").isNull())ret.put("message",result.get("message"));else if(!result.get("error").isNull())ret.put("message",result.get("error"));else ret.put("message",result);}
    return ret;
}
}
