#include "consolidatefrom.hpp"
#include "support/CommandResult.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include <algorithm>
#include <bit>
namespace minima::commands {
consolidatefrom::consolidatefrom():Command("consolidatefrom","[fromaddress:] [address:] [amount:] (tokenid:) [script:] [privatekey:] [keyuses:] (burn:) (mine:) - Send Minima or Tokens from a certain address"){}
std::vector<std::string> consolidatefrom::getValidParams() const {return {"fromaddress","tokenid","script","privatekey","keyuses","mine","burn","maxcoins"};}
std::shared_ptr<Command> consolidatefrom::getFunction() const {return std::make_shared<consolidatefrom>();}
json::JSONObject consolidatefrom::runCommand(const std::string& command){return command_result::first(command);}
json::JSONValue consolidatefrom::runCommand(){
    auto ret=getJSONReply();auto from=getAddressParam("fromaddress");auto token=getAddressParam("tokenid","0x00");
    auto maxcoins=getNumberParam("maxcoins",MiniNumber(50)).getAsInt();auto burn=getNumberParam("burn",MiniNumber::ZERO);
    if(burn.isMore(MiniNumber::ZERO)&&token!="0x00")throw CommandException("Currently BURN only works for Minima.. tokenid:0x00.. not tokens.");
    auto script=getParam("script");auto privatekey=getAddressParam("privatekey");auto keyuses=getNumberParam("keyuses");
    auto id=MiniData::getRandomData(32).to0xString();auto result=runCommand("txncreate id:"+id);bool mine=getBooleanParam("mine",true);
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();auto current=sqlRequired(tip).getBlockNumber();
    auto fromdata=MiniData::fromHex(from);auto tokendata=MiniData::fromHex(token);
    auto coins=TxPoWSearcher::searchCoins(tip,false,false,MiniData::ZERO_TXPOWID,false,MiniNumber::ZERO,true,fromdata,true,tokendata,false,"",true,false,INT32_MAX,GeneralParams::IS_MEGAMMR);
    std::stable_sort(coins.begin(),coins.end(),[](const Coin& left,const Coin& right){return left.getAmount().isMore(right.getAmount());});
    auto txpdb=sqlRequired(MinimaDB::getDB()).getTxPoWDB();auto minage=MiniNumber(3);std::vector<Coin> valid;
    for(const auto& coin:coins){bool good=!current.sub(coin.getBlockCreated()).isLess(minage);if(good&&sqlRequired(txpdb).checkMempoolCoins(coin.getCoinID()))good=false;if(good)valid.push_back(coin);}
    if(valid.empty())throw CommandException("No valid coins found to consolidate. Coins must be "+minage.toString()+" blocks old and not already in mempool.");
    int32_t count=1;auto total=MiniNumber::ZERO;
    for(const auto& coin:valid){total=total.add(coin.getTokenAmount());count=std::bit_cast<int32_t>(uint32_t(count)+1u);result=runCommand("txninput id:"+id+" coinid:"+coin.getCoinID().to0xString());if(count>maxcoins)break;}
    if(token=="0x00"&&burn.isMore(MiniNumber::ZERO)){
        if(burn.isLess(total))total=total.sub(burn);
        else{runCommand("txndelete id:"+id);throw CommandException("Burn greater than total amount added "+burn.toString()+" / "+total.toString());}
    }
    auto small=total.div(MiniNumber(10));
    for(int i=0;i<10;++i)result=runCommand("txnoutput id:"+id+" amount:"+small.toString()+" address:"+from+" tokenid:"+token);
    runCommand("txnscript id:"+id+" scripts:{\""+script+"\":\"\"}");runCommand("txnmmr id:"+id);
    runCommand("txnsign id:"+id+" publickey:custom privatekey:"+privatekey+" keyuses:"+keyuses.toString());
    result=runCommand("txnpost id:"+id+" mine:"+(mine?"true":"false"));runCommand("txndelete id:"+id);ret.put("response",result.get("response"));return ret;
}
std::shared_ptr<json::JSONObject> consolidatefrom::createConsolidate(const std::vector<std::shared_ptr<Coin>>* coins,const MiniNumber* burn,
    const std::optional<std::string>& from,const std::optional<std::string>& token,const std::optional<std::string>& script,
    const std::optional<std::string>& privatekey,const MiniNumber* keyuses){
    auto id=MiniData::getRandomData(32).to0xString();auto result=runCommand("txncreate id:"+id);auto total=MiniNumber::ZERO;
    if(!coins)throw JavaNullPointerException("null coin list");
    for(const auto& coin:*coins){total=total.add(sqlRequired(coin).getTokenAmount());runCommand("txninput id:"+id+" coinid:"+sqlRequired(coin).getCoinID().to0xString());}
    if(!burn)throw JavaNullPointerException("null burn");
    if(burn->isMore(MiniNumber::ZERO)){
        if(burn->isLess(total))total=total.sub(*burn);
        else{runCommand("txndelete id:"+id);throw CommandException("Burn greater than total amount added "+burn->toString()+" / "+total.toString());}
    }
    result=runCommand("txnoutput id:"+id+" amount:"+total.toString()+" address:"+from.value_or("null")+" tokenid:"+token.value_or("null"));
    runCommand("txnscript id:"+id+" scripts:{\""+script.value_or("null")+"\":\"\"}");runCommand("txnmmr id:"+id);
    if(!keyuses)throw JavaNullPointerException("null key uses");
    runCommand("txnsign id:"+id+" publickey:custom privatekey:"+privatekey.value_or("null")+" keyuses:"+keyuses->toString());
    result=runCommand("txnpost id:"+id+" mine:true");runCommand("txndelete id:"+id);
    const auto& response=result.get("response");if(response.isNull())return nullptr;
    auto object=std::get_if<std::shared_ptr<json::JSONObject>>(&response.value);if(!object)throw JavaClassCastException("Cannot cast post response to JSONObject");return *object;
}
}
