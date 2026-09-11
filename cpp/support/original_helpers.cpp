#include "../test/org/minima/utils/TestUtils.hpp"
#include "../test/org/minima/system/network/p2p/testingutils/QuickClients.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static int integer(const JSONObject& q,const std::string& key){return int(std::get<int64_t>(q.get(key).value));}
static std::optional<std::string> text(const JSONObject& q,const std::string& key){return q.get(key).isNull()?std::nullopt:std::optional(q.getString(key));}
static JSONObject run(const JSONObject& q){JSONObject r;try{
 if(q.getString("op")=="clients"){
  auto clients=p2p::testing::QuickClients::generateClientInfoList(text(q,"prefix"),integer(q,"count"),integer(q,"port"),text(q,"uid"),q.getBoolean("incoming"));JSONArray rows;for(const auto& client:clients->snapshot())rows.add(client->toJSON());r.put("clients",rows);auto alias=clients;std::shared_ptr<NIOClientInfo> old=clients->size()?clients->get(0):nullptr;clients->clear();r.put("aliasSize",int64_t(alias->size()));r.put("retained",old?JSONValue(old->toJSON()):JSONValue(nullptr));
 }else{
  auto id=q.getString("id");auto tx=q.getBoolean("short")?testing::TestUtils::getTxPoW(id):testing::TestUtils::getTxPoW(id,integer(q,"block"),integer(q,"weight"));auto second=testing::TestUtils::getTxPoW(id);r.put("id",tx->getTxPoWID());r.put("block",tx->getBlockNumber().toString());r.put("weight",tx->getWeight().toString());r.put("isBlock",tx->isBlock());r.put("isTransaction",tx->isTransaction());r.put("parent",tx->getParentID().to0xString());r.put("hasBody",tx->hasBody());r.put("distinct",tx!=second);r.put("initialTransactions",int64_t(tx->getTransactions().size()));tx->addTestTransaction("child");r.put("transactions",int64_t(tx->getTransactions().size()));r.put("otherTransactions",int64_t(second->getTransactions().size()));
 }
 }catch(const std::exception& e){auto value=nativeExceptionText(e);r.put("error",value.substr(0,value.find(':')));}return r;}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto q=JSONValue::parseWithException(line);std::cout<<run(*std::get<std::shared_ptr<JSONObject>>(q.value)).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
