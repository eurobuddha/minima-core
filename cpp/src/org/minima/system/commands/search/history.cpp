#include "history.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/database/txpowdb/sql/TxPoWSqlDB.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include "support/JavaHashtable.hpp"
#include "support/JavaConcurrentMap.hpp"
#include <bit>
namespace minima::commands {
history::history():Command("history","(action:) (max:) (offset:) (relevant:) - Search for all relevant TxPoW"){}
std::optional<std::string> history::getFullHelp() const {return std::string(
"\nhistory\n"
"\n"
"Return all TxPoW relevant to you. Default to 100 max (can be slow)\n"
"\n"
"action: (optional) default is 'list'\n"
"	     list : List your transactions\n"
"      size : Count how many transaction you have in History\n"
"      customsize : use with 'where' to search TxPoWDB\n"
"      transactions : How Many transactions in chain for 'depth' blocks"
"\n"
"max: (optional)\n"
"    Maximum number of TxPoW to retrieve.\n"
"\n"
"offset: (optional)\n"
"    Start the list from this point.\n"
"\n"
"depth: (optional)\n"
"    How far down chain to search for transactions.\n"
"\n"
"relevant: (optional)\n"
"    Do you want YOUR transactions or ALL transactions (defaults to true).\n"
"\n"
"where: (optional)\n"
"    Use with customsize to search for a specific set. This is the WHERE clause in SQL query.\n"
"\n"
"Examples:\n"
"\n"
"history\n"
"\n"
"history max:20\n"
"\n"
"history action:size relevant:false\n"
"\n"
"history action:transactions depth:1720\n"
"\n"
"history action:customsize where:\"isblock=1 AND timemilli>1728037509020\"\n"
"\n"
"history max:20 offset:45\n");}
std::vector<std::string> history::getValidParams() const {return {"depth","max","offset","action","relevant","startmilli","where"};}
std::shared_ptr<Command> history::getFunction() const {return std::make_shared<history>();}
json::JSONValue history::runCommand(){
    auto ret=getJSONReply();auto action=getParam("action","list");json::JSONObject response;
    bool relevant=getBooleanParam("relevant",true);
    auto sql=[](){return sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getSQLDB();};
    if(action=="size"){
        if(relevant)response.put("size",sqlRequired(sql()).getRelevantSize());
        else{
            auto defaultstart=std::bit_cast<int64_t>(uint64_t(TimerMessage::currentTimeMillis())-86400000u);
            auto start=getNumberParam("startmilli",MiniNumber(defaultstart)).getAsLong();auto size=sqlRequired(sql()).getLatestTxPoWSize(start);
            response.put("startmilli",start);response.put("size",size);
        }
    }else if(action=="customsize"){
        auto where=getParam("where");response.put("size",sqlRequired(sql()).customSizeQuery(where));
    }else if(action=="transactions"){
        auto depth=getNumberParam("depth",MiniNumber(1720)).getAsInt();auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
        auto start=sqlRequired(tip).getBlockNumber();int32_t count=0,total=0;
        while(count<depth&&tip){
            const auto& txpow=tip->getTxPoW();if(txpow.isTransaction())total=std::bit_cast<int32_t>(uint32_t(total)+1u);
            total=std::bit_cast<int32_t>(uint32_t(total)+uint32_t(txpow.getTransactions().size()));
            count=std::bit_cast<int32_t>(uint32_t(count)+1u);tip=tip->getParent();
        }
        response.put("startblock",json::JSONValue::raw(start.toString()));response.put("depth",count);response.put("transactions",total);
    }else if(action=="list"){
        auto maximum=getNumberParam("max",TxPoWSqlDB::MAX_RELEVANT_TXPOW).getAsInt();auto offset=getNumberParam("offset",MiniNumber::ZERO).getAsInt();
        auto txpows=relevant?sqlRequired(sql()).getAllRelevant(maximum,offset):sqlRequired(sql()).getLatestTxPoW(maximum,offset);
        json::JSONArray txns,details;
        for(const auto& txpow:txpows){txns.add(sqlRequired(txpow).toJSON());auto detail=getTxnDetails(txpow.get());details.add(detail?json::JSONValue(*detail):json::JSONValue());}
        response.put("relevant",relevant);response.put("txpows",txns);response.put("details",details);response.put("size",std::bit_cast<int32_t>(uint32_t(txns.size())));
    }else throw CommandException("Invalid action:"+action);
    ret.put("response",response);return ret;
}
std::shared_ptr<json::JSONObject> history::getTxnDetails(TxPoW* txpow){
    json::JSONObject result;JavaHashtable<MiniNumber> inputs,outputs;
    if(!txpow)throw JavaNullPointerException("null history TxPoW");auto transaction=txpow->getSharedTransaction();auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    auto accumulate=[&](const auto& coins,auto& table){
        for(const auto& coin:coins){
            auto address=coin.getAddress().to0xString();auto token=coin.getTokenID().to0xString();
            if(sqlRequired(wallet).isAddressRelevant(address)){
                auto previous=table.get(token);auto total=previous?*previous:MiniNumber::ZERO;
                total=total.add(token=="0x00"?coin.getAmount():coin.getTokenAmount());table.put(token,total);
            }
        }
    };
    accumulate(*sqlRequired(transaction).getAllInputs(),inputs);accumulate(*transaction->getAllOutputs(),outputs);
    JavaHashMap<bool> tokens;json::JSONObject ins,outs,differences;
    for(const auto& key:inputs.keys()){ins.put(key,inputs.get(key)->toString());tokens.put(key,true);}
    for(const auto& key:outputs.keys()){outs.put(key,outputs.get(key)->toString());tokens.put(key,true);}
    for(const auto& key:tokens.keys()){
        auto in=inputs.get(key),out=outputs.get(key);auto difference=(out?*out:MiniNumber::ZERO).sub(in?*in:MiniNumber::ZERO);differences.put(key,difference.toString());
    }
    result.put("inputs",ins);result.put("outputs",outs);result.put("difference",differences);return std::make_shared<json::JSONObject>(std::move(result));
}
}
