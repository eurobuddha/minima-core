#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/mmr/MMREntryNumber.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static JSONObject decimal(const MMREntryNumber& value){JSONObject r;r.put("coefficient",value.getUnscaled().toDecimalString());r.put("scale",value.getScale());r.put("text",value.toBigDecimalString());return r;}
static JSONObject run(const JSONObject& q){JSONObject r;try{
    if(q.getString("op")=="data"){
        auto value=MiniData::fromHex(q.getString("value")),other=MiniData::fromHex(q.getString("other"));std::optional<MMREntryNumber> before;
        if(q.getBoolean("warm"))before=value.getDataValueDecimal();
        if(!q.get("replacement").isNull()){auto wire=MiniData::fromHex(q.getString("replacement")).serialise();size_t offset=0;value.readDataStream(wire,offset);}
        r.put("before",before?JSONValue(decimal(*before)):JSONValue(nullptr));r.put("decimal",decimal(value.getDataValueDecimal()));r.put("bytes",value.to0xString());
        r.put("compare",value.compare(other));r.put("lessEqual",value.isLessEqual(other));r.put("moreEqual",value.isMoreEqual(other));r.put("equalBytes",value.isEqual(other));
        r.put("limited",value.to0xString(int(std::get<int64_t>(q.get("length").value))));
    }else{
        auto value=q.getString("op")=="bigint"?MiniNumber(BigInt(q.getString("value"))):MiniNumber(q.getString("value"));auto before=value.getAsBigDecimal();
        if(!q.get("wire").isNull()){auto bytes=MiniData::fromHex(q.getString("wire"));size_t offset=0;value.readDataStream(bytes.bytes(),offset);}
        r.put("before",decimal(before));r.put("decimal",decimal(value.getAsBigDecimal()));r.put("wire",MiniData(value.serialise()).to0xString());
        r.put("compareToZero",value.compareTo(MiniNumber::ZERO));r.put("decimalCompareToZero",value.getAsBigDecimal().compareTo(MMREntryNumber::ZERO));
    }
}catch(const std::exception& error){r.put("error",nativeExceptionText(error));}return r;}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto q=JSONValue::parseWithException(line);std::cout<<run(*std::get<std::shared_ptr<JSONObject>>(q.value)).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
