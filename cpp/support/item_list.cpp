#include "org/minima/utils/json/ItemList.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static const JSONObject& object(const JSONValue& value){return *std::get<std::shared_ptr<JSONObject>>(value.value);}
static const JSONArray& array(const JSONValue& value){return *std::get<std::shared_ptr<JSONArray>>(value.value);}
static bool flag(const JSONObject& value,const std::string& key){return !value.get(key).isNull()&&value.get(key).asBool();}
static int32_t number(const JSONObject& value,const std::string& key){return int32_t(std::get<int64_t>(value.get(key).value));}
static ItemList::String string(const JSONValue& value){return value.isNull()?std::nullopt:ItemList::String(value.asUTF16());}
static JSONValue hex(const ItemList::String& text){if(!text)return nullptr;std::string out;for(auto c:*text)for(int shift=12;shift>=0;shift-=4)out+="0123456789ABCDEF"[(c>>shift)&15];return out;}
struct LimitedList:ItemList::List {void add(ItemList::Item value) override {if(size()==4)throw JavaTypedException("java.lang.IllegalStateException","fixture append limit");ItemList::List::add(std::move(value));}};
struct Probe {ItemList::String text=u"before";int calls=0;bool fail=false,append=false;std::weak_ptr<ItemList::List> target;std::string toString(){++calls;if(append){append=false;target.lock()->add(std::u16string(u"added during render"));}if(fail)throw JavaTypedException("java.lang.IllegalStateException","fixture render failure");return text?unicode::encodeUTF8(*text):"null";}};
static ItemList::Item rawItem(const JSONValue& value){
    if(value.isString())return value.asUTF16();
    return std::visit([](const auto& v)->ItemList::Item{using T=std::decay_t<decltype(v)>;if constexpr(std::is_same_v<T,RawJSON>||std::is_same_v<T,std::u16string>)return MessageObject();else return MessageObject(v);},value.value);
}
static JSONValue observe(const ItemList::Item& item){
    if(auto text=std::get_if<std::u16string>(&item))return hex(*text);
    const auto& box=std::get<MessageObject>(item);if(box.isNull())return nullptr;
    try{auto p=box.as<std::shared_ptr<Probe>>();JSONObject row;row.put("probe",p->text?JSONValue(*p->text):JSONValue());row.put("calls",p->calls);return row;}catch(const std::bad_any_cast&){}
    try{return box.as<bool>();}catch(const std::bad_any_cast&){}
    try{return box.as<int64_t>();}catch(const std::bad_any_cast&){}
    try{return box.as<double>();}catch(const std::bad_any_cast&){}
    try{JSONValue value;value.value=box.as<std::shared_ptr<JSONObject>>();return value;}catch(const std::bad_any_cast&){}
    try{JSONValue value;value.value=box.as<std::shared_ptr<JSONArray>>();return value;}catch(const std::bad_any_cast&){}
    throw std::runtime_error("unsupported fixture object");
}
static JSONValue snapshot(const std::shared_ptr<ItemList::List>& list){if(!list)return nullptr;JSONArray values;for(const auto& value:*list)values.add(observe(value));return values;}
static JSONArray run(const JSONObject& input){
    auto list=std::make_shared<ItemList>(),other=std::make_shared<ItemList>();auto alias=list->getItems();auto probe=std::make_shared<Probe>();JSONArray outputs;
    for(const auto& raw:array(input.get("ops")).elements()){
        const auto& op=object(raw);JSONObject row;JSONValue result;
        try{
            auto kind=op.getString("op");auto s=string(op.get("s")),sep=string(op.get("sep"));
            if(kind=="new"){auto arity=number(op,"arity");list=arity==0?std::make_shared<ItemList>():arity==1?std::make_shared<ItemList>(s):arity==2?std::make_shared<ItemList>(s,sep):std::make_shared<ItemList>(s,sep,flag(op,"multi"));}
            else if(kind=="alias"){alias=list->getItems();result=alias==list->getItems();}
            else if(kind=="setSP")list->setSP(sep);
            else if(kind=="add"){if(op.containsKey("index"))list->add(number(op,"index"),s);else list->add(s);}
            else if(kind=="raw")alias->add(flag(op,"probe")?ItemList::Item(MessageObject(probe,std::function<std::string(const Probe&)>([](const Probe& p){return const_cast<Probe&>(p).toString();}))):rawItem(op.get("value")));
            else if(kind=="rawSet")result=observe(alias->set(size_t(number(op,"index")),rawItem(op.get("value"))));
            else if(kind=="rawClear")alias->clear();
            else if(kind=="rawRemove")result=observe(alias->remove(size_t(number(op,"index"))));
            else if(kind=="get")result=hex(list->get(number(op,"index")));
            else if(kind=="array")list->getArray();
            else if(kind=="join")result=hex(op.containsKey("sep")?list->toString(sep):list->toString());
            else if(kind=="clear")list->clear();else if(kind=="reset")list->reset();
            else if(kind=="other")other=std::make_shared<ItemList>(s,sep,flag(op,"multi"));
            else if(kind=="all"){if(flag(op,"object"))list->addAll(flag(op,"null")?nullptr:flag(op,"self")?list.get():other.get());else if(op.containsKey("multi"))list->addAll(s,sep,flag(op,"multi"));else if(op.containsKey("sep"))list->addAll(s,sep);else list->addAll(s);}
            else if(kind=="split"){
                std::shared_ptr<ItemList::List> target=flag(op,"limited")?std::make_shared<LimitedList>():flag(op,"null")?nullptr:flag(op,"alias")?alias:other->getItems();
                try{if(op.containsKey("multi"))list->split(s,sep,target,flag(op,"multi"));else list->split(s,sep,target);}catch(...){row.put("append",snapshot(target));throw;}row.put("append",snapshot(target));
            }else if(kind=="probe"){probe->text=s;probe->fail=flag(op,"fail");probe->append=flag(op,"append");probe->target=list->getItems();}
            else throw std::runtime_error("unsupported fixture operation");
            row.put("value",result);
        }catch(const std::exception& error){auto text=nativeExceptionText(error);if(text.starts_with("java.lang.ClassCastException"))text="java.lang.ClassCastException";row.put("exception",text);}
        row.put("items",snapshot(list->getItems()));row.put("alias",snapshot(alias));row.put("other",snapshot(other->getItems()));row.put("size",list->size());row.put("probeCalls",probe->calls);outputs.add(row);
    }return outputs;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
