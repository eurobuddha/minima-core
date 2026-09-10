#include "scanchain.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/JavaDateString.hpp"
#include <bit>
namespace minima::commands {
scanchain::scanchain():Command("scanchain","(depth:) (offset:) - Scan back through the chain and see all transaction data"){}
json::JSONValue scanchain::runCommand(){
    auto result=getJSONReply();auto tree=sqlRequired(MinimaDB::getDB()).getTxPoWTree();
    auto tip=sqlRequired(tree).getTip();if(!tip)throw CommandException("NO Blocks yet..");
    int32_t counter=0;json::JSONArray blocks;
    auto offset=getNumberParam("offset",MiniNumber::ZERO).getAsInt();
    auto depth=getNumberParam("depth",MiniNumber(16)).getAsInt();
    auto increment=[&]{counter=std::bit_cast<int32_t>(uint32_t(counter)+1u);};
    while(tip&&counter<=offset){tip=tip->getParent();increment();}
    if(tip){
        auto start=tip->getBlockNumber();counter=0;
        while(tip&&counter<=depth){
            auto top=tip->getSharedTxPoW();json::JSONObject block;
            block.put("block",json::JSONValue::raw(top->getBlockNumber().toString()));
            block.put("depth",json::JSONValue::raw(start.sub(top->getBlockNumber()).toString()));
            block.put("timemilli",json::JSONValue::raw(top->getTimeMilli().toString()));
            block.put("date",javaDateString(top->getTimeMilli().getAsLong()));
            block.put("txpowid",top->getTxPoWID());json::JSONArray transactions;
            auto append=[&](TxPoW* value){auto detail=getTransactionDetails(value);transactions.add(detail?json::JSONValue(*detail):json::JSONValue());};
            if(top->isTransaction())append(top.get());
            for(const auto& id:top->getBlockTransactions()){
                auto value=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getTxPoW(id.to0xString());
                if(value)append(value.get());
            }
            block.put("transactions",transactions);blocks.add(block);tip=tip->getParent();increment();
        }
    }
    json::JSONObject response;response.put("depth",depth);response.put("offset",offset);response.put("blocks",blocks);
    result.put("response",response);return result;
}
std::shared_ptr<json::JSONObject> scanchain::getTransactionDetails(TxPoW* txpow){
    if(!txpow)throw JavaNullPointerException("null scanchain TxPoW");json::JSONObject result;
    result.put("txpowid",txpow->getTxPoWID());result.put("istransaction",!txpow->getTransaction().isEmpty());
    if(!txpow->getTransaction().isEmpty())result.put("transaction",txpow->getTransaction().toJSON());
    result.put("isburntransaction",!txpow->getBurnTransaction().isEmpty());
    if(!txpow->getBurnTransaction().isEmpty())result.put("burntransaction",txpow->getBurnTransaction().toJSON());
    return std::make_shared<json::JSONObject>(std::move(result));
}
}
