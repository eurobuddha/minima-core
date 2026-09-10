#include "coins.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/commands/send/send.hpp"
#include "org/minima/system/commands/txn/txnutils.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <algorithm>
namespace minima::commands {
coins::coins():Command("coins","(relevant:true) (sendable:true) (coinid:) (amount:) (address:) (tokenid:) (coinage:) (checkmempool:) (order:) - Search for coins"){}
std::optional<std::string> coins::getFullHelp() const {return std::string(
"\ncoins\n"
"\n"
"Search for coins that are relevant to you or in the unpruned chain.\n"
"\n"
"relevant: (optional)\n"
"    true or false, true will only return coins you are tracking.\n"
"    false will search all coins in the unpruned chain.\n"
"    Default is false unless no other parameters are provided.\n"
"\n"
"sendable: (optional)\n"
"    true only, filter out coins that are not sendable, they might be locked in a contract.\n"
"    Default is to return sendable and unsendable coins.\n"
"\n"
"coinid: (optional)\n"
"    A coinid, to search for a single coin.\n"
"\n"
"amount: (optional)\n"
"    The coin value to search for.\n"
"\n"
"address: (optional)\n"
"    Address of a coin to search for, could be a script address.\n"
"    Can be a 0x or Mx address.\n"
"\n"
"tokenid: (optional)\n"
"    A tokenid, to search for coins of a specific token. Minima is 0x00.\n"
"\n"
"checkmempool: (optional)\n"
"    Check if the coin is in the mempool.\n"
"\n"
"coinage: (optional)\n"
"    How old does the coin have to be.\n"
"\n"
"depth: (optional)\n"
"    How many blocks back to check from Blockchain tip.\n"
"\n"
"order: (optional)\n"
"    Order asc or desc (Ascending or Decending).\n"
"\n"
"megammr: (optional)\n"
"    Search the MegaMMR for coins too.\n"
"\n"
"Examples:\n"
"\n"
"coins\n"
"\n"
"coins relevant:true sendable:true\n"
"\n"
"coins relevant:true amount:10\n"
"\n"
"coins coinid:0xEECD7..\n"
"\n"
"coins relevant:true tokenid:0xFED5..\n"
"\n"
"coins relevant:true address:0xCEF6.. tokenid:0x00\n"
"\n"
"coins relevant:true address:MxABC9..\n");}
std::vector<std::string> coins::getValidParams() const {return {"relevant","sendable","coinid","amount","address","tokenid","checkmempool","order","coinage","simplestate","totalamount","depth","state","megammr"};}
std::shared_ptr<Command> coins::getFunction() const {return std::make_shared<coins>();}
json::JSONValue coins::runCommand(){
    auto ret=getJSONReply();
    bool hardsetrel=!existsParam("relevant")&&!existsParam("coinid")&&!existsParam("address")&&!existsParam("state")&&!existsParam("tokenid");
    bool relevant=existsParam("relevant")||hardsetrel;
    bool simplestate=getBooleanParam("simplestate",false),simple=getBooleanParam("sendable",false);
    bool scoinid=existsParam("coinid");auto coinid=MiniData::ZERO_TXPOWID;
    if(scoinid)coinid=MiniData::fromHex(getParam("coinid","0x01"));
    bool samount=existsParam("amount");auto amount=MiniNumber::ZERO;if(samount)amount=getNumberParam("amount");
    bool saddress=existsParam("address");auto address=MiniData::ZERO_TXPOWID;if(saddress)address=MiniData::fromHex(getAddressParam("address"));
    bool stokenid=existsParam("tokenid");auto tokenid=MiniData::ZERO_TXPOWID;if(stokenid)tokenid=MiniData::fromHex(getParam("tokenid","0x01"));
    bool sstate=existsParam("state");auto statesearch=getParam("state","");auto coinage=getNumberParam("coinage",MiniNumber::ZERO);
    auto depth=getNumberParam("depth",MiniNumber::BILLION).getAsInt();auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    if(!tip){ret.put("response",json::JSONArray());return ret;}
    bool mega=getBooleanParam("megammr",false);if(mega)mega=GeneralParams::IS_MEGAMMR;
    auto found=TxPoWSearcher::searchCoins(tip,relevant,scoinid,coinid,samount,amount,saddress,address,stokenid,tokenid,sstate,statesearch,true,simple,depth,mega);
    std::vector<Coin> aged;auto minimum=tip->getBlockNumber().sub(coinage);
    for(const auto& coin:found)if(coin.getBlockCreated().isLessEqual(minimum))aged.push_back(coin);
    bool checkmempool=getBooleanParam("checkmempool",false);std::vector<Coin> selected;
    if(checkmempool){
        auto db=sqlRequired(MinimaDB::getDB()).getTxPoWDB();auto miner=txnutils::getMiner();
        for(const auto& coin:aged){
            if(sqlRequired(miner).checkForMiningCoin(coin.getCoinID().to0xString()))continue;
            if(sqlRequired(db).checkMempoolCoins(coin.getCoinID()))continue;
            selected.push_back(coin);
        }
    }else selected=std::move(aged);
    auto order=getParam("order","desc");
    if(order=="asc")std::stable_sort(selected.begin(),selected.end(),[](const Coin& a,const Coin& b){return a.getBlockCreated().isLess(b.getBlockCreated());});
    if(existsParam("totalamount")){
        auto total=getNumberParam("totalamount");auto tokenamount=total;
        if(!tokenid.isEqual(Token::TOKENID_MINIMA)&&!selected.empty()){
            if(!selected.front().getToken())throw JavaNullPointerException("null selection token");
            tokenamount=selected.front().getToken()->getScaledMinimaAmount(total);
        }
        selected=send::selectCoins(selected,tokenamount);
    }
    auto block=tip->getTxPoW().getBlockNumber();json::JSONArray result;
    for(const auto& coin:selected){auto item=coin.toJSON(simplestate);item.put("age",block.sub(coin.getBlockCreated()).toString());result.add(item);}
    ret.put("response",result);return ret;
}
}
