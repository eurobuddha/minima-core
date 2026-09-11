#include "burn.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/JavaNullableNumberSort.hpp"
namespace minima::commands {
burn::burn():Command("burn","View Burn metrics"){}
std::optional<std::string> burn::getFullHelp() const {return std::string(
"\nburn\n"
"\n"
"View the number of burn transactions and the maximum, median, average and minimum burn metrics for the last 1, 10 and 50 blocks.\n"
"\n"
"Use as an indicator for an appropriate burn amount for transactions.\n"
"\n"
"Examples:\n"
"\n"
"burn\n");}
std::shared_ptr<MiniNumber> burn::getMediaValue(Values* values){
    if(!values)throw JavaNullPointerException("null median list");
    if(values->empty()){static const auto zero=std::make_shared<MiniNumber>(MiniNumber::ZERO);return zero;}
    javaNullableNumberSort(*values);
    return values->at(values->size()/2);
}
void burn::checkBurn(TxPoW* txpow,int32_t counter){
    if(!txpow)throw JavaNullPointerException("null burn TxPoW");if(!txpow->isTransaction())return;
    auto value=std::make_shared<MiniNumber>(txpow->getBurn());
    auto add=[&](Window& window){
        if(value->isMore(window.maximum))window.maximum=*value;
        if(value->isLess(window.minimum))window.minimum=*value;
        window.count=window.count.increment();window.total=window.total.add(*value);window.values.push_back(value);
    };
    if(counter==0)add(block1);if(counter<10)add(block10);add(block50);
}
json::JSONObject burn::metrics(Window& window){
    auto number=[](const MiniNumber& value){return json::JSONValue::raw(value.toString());};
    json::JSONObject result;result.put("txns",number(window.count));result.put("max",number(window.maximum));
    auto median=getMediaValue(&window.values);result.put("med",median?number(*median):json::JSONValue());
    result.put("avg",number(window.count.isMore(MiniNumber::ZERO)?window.total.div(window.count):MiniNumber::ZERO));
    result.put("min",number(window.minimum));return result;
}
json::JSONValue burn::runCommand(){
    auto result=getJSONReply();auto action=getParam("action","list");json::JSONObject response;
    auto database=sqlRequired(MinimaDB::getDB()).getTxPoWDB();
    if(action=="list"){
        auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();int32_t counter=0;
        while(tip&&counter<50){
            auto top=tip->getSharedTxPoW();checkBurn(tip->getSharedTxPoW().get(),counter);
            for(const auto blockIDs=sqlRequired(top).getBlockTransactions();const auto id:TxBody::required(blockIDs).range()){
                auto txpow=sqlRequired(database).getTxPoW(TxBody::required(id).to0xString());if(txpow)checkBurn(txpow.get(),counter);
            }
            tip=tip->getParent();++counter;
        }
        if(block1.count.isEqual(MiniNumber::ZERO))block1.minimum=MiniNumber::ZERO;
        if(block10.count.isEqual(MiniNumber::ZERO))block10.minimum=MiniNumber::ZERO;
        if(block50.count.isEqual(MiniNumber::ZERO))block50.minimum=MiniNumber::ZERO;
        response.put("1block",metrics(block1));response.put("10block",metrics(block10));response.put("50block",metrics(block50));
    }
    result.put("response",response);return result;
}
}
