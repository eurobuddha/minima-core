#pragma once
#include <bit>
#include "org/minima/objects/base/MiniByte.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"

namespace base_dispatch_fixture {
using namespace minima;using namespace minima::json;
inline int integer(const JSONObject& q,const std::string& key){return int(std::get<int64_t>(q.get(key).value));}
class ByteValue final:public MiniByte {
    const JSONObject& q;std::string name;JSONArray& calls;int mask;
    bool intercept(int bit,const std::string& method) const {if(!(mask&bit))return false;auto event=name+"."+method;calls.add(event);if(event==q.getString("fault"))throw JavaTypedException("java.lang.IllegalStateException",event);return true;}
public:
    ByteValue(const JSONObject& q,std::string name,JSONArray& calls):MiniByte(integer(q,name+"Raw")),q(q),name(name),calls(calls),mask(integer(q,name+"Mask")){}
    int getValue()const override {return intercept(1,"getValue")?integer(q,name+"Value"):MiniByte::getValue();}
    std::int8_t getByteValue()const override {return intercept(2,"getByteValue")?std::bit_cast<std::int8_t>(uint8_t(integer(q,name+"Value"))):MiniByte::getByteValue();}
    bool isEqual(const MiniByte& other)const override {return intercept(4,"isEqual")?q.getBoolean("answer"):MiniByte::isEqual(other);}
    bool isFalse()const override {return intercept(8,"isFalse")?q.getBoolean("answer"):MiniByte::isFalse();}
    bool isTrue()const override {return intercept(16,"isTrue")?q.getBoolean("answer"):MiniByte::isTrue();}
    std::string toString()const override {return intercept(32,"toString")?q.getString("text"):MiniByte::toString();}
};
class StringValue final:public MiniString {
    const JSONObject& q;JSONArray& calls;int mask;std::string text;
    bool intercept(int bit,const std::string& method)const {if(!(mask&bit))return false;calls.add(method);if(method==q.getString("fault"))throw JavaTypedException("java.lang.IllegalStateException",method);return true;}
public:
    StringValue(const JSONObject& q,JSONArray& calls):MiniString(q.getString("raw")),q(q),calls(calls),mask(integer(q,"mask")),text(q.getString("text")){}
    const std::string& toString()const override {return intercept(1,"toString")?text:MiniString::toString();}
    std::vector<uint8_t> getData()const override {return intercept(2,"getData")?MiniData::fromHex(q.getString("bytes")).bytes():MiniString::getData();}
    bool isEqual(const std::string& value)const override {return intercept(4,"isEqual")?q.getBoolean("answer"):MiniString::isEqual(value);}
};
inline JSONObject run(const JSONObject& q){
    JSONObject r;JSONArray calls;auto action=q.getString("action");
    if(q.getString("op")=="dispatchByte"){
        ByteValue left(q,"left",calls),right(q,"right",calls);const MiniByte& base=left;
        try{
            if(action=="getValue")r.put("value",base.getValue());else if(action=="getByteValue")r.put("value",int(base.getByteValue()));
            else if(action=="isEqual")r.put("value",base.isEqual(right));else if(action=="isFalse")r.put("value",base.isFalse());else if(action=="isTrue")r.put("value",base.isTrue());else if(action=="toString")r.put("value",base.toString());
            else if(action=="baseEqual")r.put("value",left.MiniByte::isEqual(right));else if(action=="baseFalse")r.put("value",left.MiniByte::isFalse());else if(action=="baseTrue")r.put("value",left.MiniByte::isTrue());else if(action=="baseString")r.put("value",left.MiniByte::toString());else throw std::invalid_argument(action);
        }catch(const std::exception& error){r.put("error",nativeExceptionText(error));}r.put("wire",MiniData(base.serialise()).to0xString());
    }else{
        StringValue value(q,calls);const MiniString& base=value;std::vector<uint8_t> bytes{0xAA};
        try{
            if(action=="toString")r.put("value",base.toString());else if(action=="getData")r.put("value",MiniData(base.getData()).to0xString());
            else if(action=="isEqual")r.put("value",base.isEqual(q.getString("text")));else if(action=="baseEqual")r.put("value",value.MiniString::isEqual(q.getString("text")));
            else if(action=="copy"){MiniString copy(base);r.put("value",copy.toString());r.put("data",MiniData(copy.getData()).to0xString());}
            else if(action=="write")base.writeDataStream(bytes);else throw std::invalid_argument(action);
        }catch(const std::exception& error){r.put("error",nativeExceptionText(error));}r.put("wire",MiniData(bytes).to0xString());
    }r.put("calls",calls);return r;
}
}
