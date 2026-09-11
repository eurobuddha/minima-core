#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
static JSONObject& object(const JSONValue& value){return *std::get<std::shared_ptr<JSONObject>>(value.value);}
static JSONArray& array(const JSONValue& value){return *std::get<std::shared_ptr<JSONArray>>(value.value);}
static int64_t number(const JSONObject& q,const std::string& key){return std::get<int64_t>(q.get(key).value);}
static std::shared_ptr<MiniData> item(const JSONValue& value){return value.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(value.asString()));}
static JSONValue text(const std::shared_ptr<MiniData>& value){return value?JSONValue(value->to0xString()):JSONValue();}
static JSONArray snapshot(const std::shared_ptr<TxBody::IDs>& list){JSONArray result;for(const auto value:TxBody::required(list).range())result.add(text(value));return result;}
static std::string category(const std::exception& e){auto result=javaExceptionText(e);return result.substr(0,result.find(':'));}
static bool removeValue(const std::shared_ptr<TxBody::IDs>& list,const std::shared_ptr<MiniData>& value){return TxBody::required(list).remove([&](const auto& old){return value?old&&value->isEqual(*old):!old;});}
static void mutate(const std::shared_ptr<MiniData>& value){auto bytes=MiniData::fromHex("0x99").serialise();size_t offset=0;TxBody::required(value).readDataStream(bytes,offset);}
static JSONArray collection(const JSONObject& q){
    TxPoW owner;auto list=owner.getBlockTransactions();std::shared_ptr<MiniData> held;
    for(const auto& value:array(q.get("initial")).elements())list->add(item(value));
    std::optional<TxBody::IDs::Iterator> iterator;JSONArray output;
    for(const auto& raw:array(q.get("ops")).elements()){
        const auto& op=object(raw);auto action=op.getString("op");JSONValue result("ok");
        try{
            if(action=="iter")iterator.emplace(TxBody::required(list).iterator());
            else if(action=="has")result=iterator->hasNext();
            else if(action=="next")result=text(iterator->next());
            else if(action=="add")TxBody::required(list).add(item(op.get("value")));
            else if(action=="get")result=text(TxBody::required(list).get(size_t(number(op,"index"))));
            else if(action=="set")result=text(TxBody::required(list).set(size_t(number(op,"index")),item(op.get("value"))));
            else if(action=="remove")result=removeValue(list,item(op.get("value")));
            else if(action=="clear")TxBody::required(list).clear();
            else if(action=="replace"){owner.getTxBody()->mTxPowIDList()=std::make_shared<TxBody::IDs>();list=owner.getBlockTransactions();}
            else if(action=="release"){owner.clearBody();list.reset();}
            else if(action=="serial")result=snapshot(list);
            else if(action=="retain")held=TxBody::required(list).get(size_t(number(op,"index")));
            else if(action=="held")result=text(held);
            else if(action=="addHeld")TxBody::required(list).add(held);
            else if(action=="mutate")mutate(held);
            else if(action=="sameList")result=owner.hasBody()&&list==owner.getBlockTransactions();
            else throw std::invalid_argument("Unknown operation");
        }catch(const std::exception& e){result=category(e);}
        output.add(result);
    }
    return output;
}
static JSONObject consumer(const JSONObject& q){
    TxPoW owner;auto ids=owner.getBlockTransactions();
    for(const auto& value:array(q.get("initial")).elements())ids->add(item(value));
    // The source's test constructor wraps the real supplied TxPoW without mining.
    TxPoWTreeNode node(owner);auto pow=node.getSharedTxPoW();JSONArray calls,again;int count=0;
    auto action=q.getString("action");auto trigger=number(q,"trigger");auto stop=q.getBoolean("stop");
    auto exists=[&](const std::string& id){
        calls.add(id);++count;
        if(count==trigger){
            if(action=="add")ids->add(std::make_shared<MiniData>(MiniData::fromHex("0x99")));
            else if(action=="clear")ids->clear();
            else if(action=="set")ids->set(size_t(number(q,"index")),item(q.get("value")));
            else if(action=="remove")removeValue(ids,item(q.get("value")));
            else if(action=="replace"){pow->getTxBody()->mTxPowIDList()=std::make_shared<TxBody::IDs>();pow->addBlockTxPOW(MiniData::fromHex("0xAA"));}
            else if(action=="clearOwner")pow->clearBody();
            else if(action=="mutate")mutate(ids->get(size_t(number(q,"index"))));
        }
        return !(count==trigger&&stop);
    };
    JSONObject result;
    try{result.put("first",node.checkFullTxns(exists));}catch(const std::exception& e){result.put("first",category(e));}
    result.put("calls",calls);result.put("heldList",snapshot(ids));result.put("bodyList",snapshot(pow->getBlockTransactions()));result.put("hasBody",pow->hasBody());
    if(pow->hasBody())pow->addBlockTxPOW(MiniData::fromHex("0xEE"));
    result.put("beforeSecond",snapshot(pow->getBlockTransactions()));
    try{result.put("second",node.checkFullTxns([&](const std::string& id){again.add(id);return false;}));}catch(const std::exception& e){result.put("second",category(e));}
    result.put("again",again);return result;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{const auto parsed=JSONValue::parseWithException(line);auto& q=object(parsed);std::cout<<(q.getString("kind")=="collection"?JSONValue(collection(q)):JSONValue(consumer(q))).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
