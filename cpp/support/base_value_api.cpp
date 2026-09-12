#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/mmr/MMREntryNumber.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
#include <bit>
#include "BaseDispatchFixture.hpp"
#include "org/minima/objects/base/MiniByte.hpp"
#include "org/minima/objects/base/MiniString.hpp"
using namespace minima;using namespace minima::json;
static JSONObject decimal(const MMREntryNumber& value){JSONObject r;r.put("coefficient",value.getUnscaled().toDecimalString());r.put("scale",value.getScale());r.put("text",value.toBigDecimalString());return r;}
static JSONObject helpers(const JSONObject& q){
    JSONObject r;auto op=q.getString("op");
    if(op=="helperWrite"){
        auto out=MiniData::fromHex(q.getString("prefix")).bytes();auto kind=q.getString("kind");
        if(kind=="byte")MiniByte::WriteToStream(out,q.getBoolean("value"));
        else if(kind=="number")MiniNumber::WriteToStream(out,int32_t(std::get<int64_t>(q.get("value").value)));
        else if(kind=="string")MiniString::WriteToStream(out,q.getString("value"));
        else MiniData::WriteToStream(out,MiniData::fromHex(q.getString("value")).bytes());
        r.put("wire",MiniData(out).to0xString());
    }else if(op=="helperRead"){
        auto data=MiniData::fromHex(q.getString("wire"));size_t offset=size_t(std::get<int64_t>(q.get("offset").value));auto kind=q.getString("kind");
        try{std::string value;
            if(kind=="byte")value=MiniByte::ReadFromStream(data.bytes(),offset).toString();
            else if(kind=="number")value=MiniNumber::ReadFromStream(data.bytes(),offset).toString();
            else if(kind=="string")value=MiniString::ReadFromStream(data.bytes(),offset).toString();
            else if(kind=="hash")value=MiniData::ReadHashFromStream(data.bytes(),offset).to0xString();
            else if(kind=="mmr")value=MMREntryNumber::ReadFromStream(data.bytes(),offset).toString();
            else value=(q.containsKey("expected")?MiniData::ReadFromStream(data.bytes(),offset,int32_t(std::get<int64_t>(q.get("expected").value))):MiniData::ReadFromStream(data.bytes(),offset)).to0xString();
            r.put("value",value);
        }catch(const std::exception& e){r.put("error",nativeExceptionText(e));}r.put("offset",int64_t(offset));
    }else if(op=="helperVersion"){
        struct Writer:Streamable {mutable int calls=0;std::string fault;std::vector<uint8_t> bytes;
            void readDataStream(std::span<const uint8_t>,size_t&) override{}
            void writeDataStream(std::vector<uint8_t>& out)const override{++calls;out.insert(out.end(),{0xAA,0x55});if(fault=="io")throw wire::IOException("fixture IO");if(fault=="runtime")throw JavaTypedException("java.lang.IllegalStateException","fixture runtime");out.insert(out.end(),bytes.begin(),bytes.end());}
        }object;object.fault=q.getString("fault");object.bytes=MiniData::fromHex(q.getString("bytes")).bytes();
        auto previous=MinimaLogger::getRuntime();std::u16string captured;auto capture=previous;capture.println=[&](const auto& text){captured+=text;};MinimaLogger::setRuntime(capture);
        try{auto value=object.fault=="reference"?MiniData::getMiniDataVersion(object):MiniData::getMiniDataVersion(object.fault=="null"?nullptr:&object);r.put("value",value?JSONValue(value->to0xString()):JSONValue(nullptr));}
        catch(const std::exception& e){auto text=nativeExceptionText(e);r.put("errorClass",text.substr(0,text.find(':')));}MinimaLogger::setRuntime(previous);
        r.put("calls",object.calls);r.put("logged",captured.find(u"java.io.IOException: fixture IO")!=std::u16string::npos);
    }else if(op=="helperByteConstants"){
        auto old=MiniByte::FALSE;size_t at=0;auto b=std::vector<uint8_t>{uint8_t(std::get<int64_t>(q.get("falseValue").value))};MiniByte::FALSE.readDataStream(b,at);
        auto value=MiniByte(int(std::get<int64_t>(q.get("value").value)));r.put("false",value.isFalse());r.put("true",value.isTrue());r.put("falseValue",MiniByte::FALSE.getValue());MiniByte::FALSE=old;
    }else if(op=="helperMMRDecimal"){
        MMREntryNumber value(BigInt(q.getString("coefficient")),int32_t(std::get<int64_t>(q.get("scale").value)));auto before=value.getBigDecimal();auto data=MiniData::fromHex(q.getString("wire"));size_t offset=0;value.readDataStream(data.bytes(),offset);r.put("before",decimal(before));r.put("after",decimal(value.getBigDecimal()));
    }return r;
}
static JSONObject run(const JSONObject& q){JSONObject r;try{
    if(q.getString("op").starts_with("dispatch"))return base_dispatch_fixture::run(q);
    if(q.getString("op").starts_with("helper"))return helpers(q);
    if(q.getString("op")=="limits"){
        r.put("maximum",decimal(MiniNumber::MAX_MININUMBER));r.put("minimum",decimal(MiniNumber::MIN_MININUMBER));r.put("maximumNumber",decimal(MiniNumber::MAXIMUM.getAsBigDecimal()));
    }else if(q.getString("op")=="data"){
        auto value=MiniData::fromHex(q.getString("value")),other=MiniData::fromHex(q.getString("other"));std::optional<MMREntryNumber> before;
        if(q.getBoolean("warm"))before=value.getDataValueDecimal();
        if(!q.get("replacement").isNull()){auto wire=MiniData::fromHex(q.getString("replacement")).serialise();size_t offset=0;value.readDataStream(wire,offset);}
        r.put("before",before?JSONValue(decimal(*before)):JSONValue(nullptr));r.put("decimal",decimal(value.getDataValueDecimal()));r.put("bytes",value.to0xString());
        r.put("compare",value.compare(other));r.put("lessEqual",value.isLessEqual(other));r.put("moreEqual",value.isMoreEqual(other));r.put("equalBytes",value.isEqual(other));
        r.put("limited",value.to0xString(int(std::get<int64_t>(q.get("length").value))));
    }else{
        auto value=q.getString("op")=="decimal"?MiniNumber(MMREntryNumber(BigInt(q.getString("value")),int32_t(std::get<int64_t>(q.get("scale").value)))):q.getString("op")=="bigint"?MiniNumber(BigInt(q.getString("value"))):MiniNumber(q.getString("value"));auto before=value.getAsBigDecimal();
        if(!q.get("wire").isNull()){auto bytes=MiniData::fromHex(q.getString("wire"));size_t offset=0;value.readDataStream(bytes.bytes(),offset);}
        r.put("before",decimal(before));r.put("decimal",decimal(value.getAsBigDecimal()));r.put("wire",MiniData(value.serialise()).to0xString());
        r.put("compareToZero",value.compareTo(MiniNumber::ZERO));r.put("decimalCompareToZero",value.getAsBigDecimal().compareTo(MMREntryNumber::ZERO));
    }
}catch(const std::exception& error){r.put("error",nativeExceptionText(error));}return r;}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto q=JSONValue::parseWithException(line);std::cout<<run(*std::get<std::shared_ptr<JSONObject>>(q.value)).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
