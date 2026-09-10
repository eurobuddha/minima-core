#include "mempool.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/database/txpowtree/CoinDB.hpp"
#include <bit>
namespace minima::commands {
mempool::mempool():Command("mempool","Check the mempool"){}
std::shared_ptr<Command> mempool::getFunction() const {return std::make_shared<mempool>();}
json::JSONValue mempool::runCommand(){
    auto ret=getJSONReply();auto db=sqlRequired(MinimaDB::getDB()).getTxPoWDB();json::JSONArray txpows;
    auto pool=sqlRequired(db).getCompleteMemPool();int32_t size=0,cascade=0,transactions=0,blocks=0;
    auto increment=[](int32_t value){return std::bit_cast<int32_t>(uint32_t(value)+1u);};
    for(const auto& ram:sqlRequired(pool).elements()){
        if(!sqlRequired(ram).isInCascade()){
            auto txpow=ram->getTxPoW();size=increment(size);json::JSONObject value;
            value.put("txpowid",sqlRequired(txpow).getTxPoWID());value.put("transaction",txpow->isTransaction());value.put("block",txpow->isBlock());txpows.add(value);
            if(txpow->isTransaction())transactions=increment(transactions);if(txpow->isBlock())blocks=increment(blocks);
        }else cascade=increment(cascade);
    }
    json::JSONObject response;response.put("txpow",txpows);response.put("total",std::bit_cast<int32_t>(uint32_t(pool->size())));
    response.put("onchain",size);response.put("cascade",cascade);response.put("transactions",transactions);response.put("blocks",blocks);
    response.put("sqlcoindb",sqlRequired(CoinDB::getTxPoWTreeCoinDB()).getSize());ret.put("response",response);return ret;
}
}
