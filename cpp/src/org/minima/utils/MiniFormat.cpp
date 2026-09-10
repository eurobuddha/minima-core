#include "MiniFormat.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include <bit>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
namespace minima {
json::JSONArray MiniFormat::convertToJSON(const std::string& text) {auto value=json::parser::JSONParser().parse(text);auto array=std::get<std::shared_ptr<json::JSONArray>>(value.value);if(!array) throw std::invalid_argument("Null JSON array");return *array;}
std::string MiniFormat::JSONPretty(const json::JSONArray& array,bool debug) {std::string result=array.size()>1?"[":"";bool first=true;for(const auto& value:array.elements()) {if(!first) result+=",";first=false;auto object=std::get<std::shared_ptr<json::JSONObject>>(value.value);if(!object) throw std::invalid_argument("Null JSON object");result+=JSONPretty(*object,debug);}if(array.size()>1) result+="]";return result;}
std::string MiniFormat::JSONPretty(const json::JSONObject& object,bool debug) {
    auto work=object.toJSONString();if(debug) std::clog<<"JSONPRETTY obj:"<<work<<'\n';std::string result;
    try {int tabs=0;size_t current=0;std::string tab;
        for(;;) {size_t quote=work.find('"',current),open=work.find('{',current),close=work.find('}',current),comma=work.find(',',current);auto next=std::min({quote,open,close,comma});if(next==std::string::npos){result+=work.substr(current);break;}result+=work.substr(current,next-current);
            if(open==next) {++tabs;tab=std::string(size_t(std::max(tabs,0))*2,' ');result+="{\n"+tab;current=open+1;}
            else if(close==next) {--tabs;tab=std::string(size_t(std::max(tabs,0))*2,' ');result+="\n"+tab+"}";current=close+1;}
            else if(quote==next) {auto end=work.find('"',quote+1);if(end==std::string::npos) throw std::out_of_range("Unterminated pretty quote");result+=work.substr(quote,end+1-quote);current=end+1;}
            else {result+=",\n"+tab;current=comma+1;}
        }
        for(size_t p=0;(p=result.find("\\/",p))!=std::string::npos;) result.replace(p,2,"/");
        for(size_t p=0;(p=result.find("\\n",p))!=std::string::npos;++p) result.replace(p,2,"\n");
    }catch(const std::exception&) {return work;}return result;
}
std::string MiniFormat::formatSize(int64_t bytes) {
    if(bytes<1024) return std::to_string(bytes)+" bytes";int group=(std::bit_width(uint64_t(bytes))-1)/10;double value=double(bytes)/double(uint64_t(1)<<(group*10));
    value=std::floor(value*10+0.5)/10;std::ostringstream out;out.imbue(std::locale::classic());out<<std::fixed<<std::setprecision(1)<<value<<' '<<" KMGTPE"[group]<<'B';return out.str();
}
std::string MiniFormat::zeroPad(int32_t total,const MiniNumber& number) {auto text=number.floor().toString();int32_t add=std::bit_cast<int32_t>(uint32_t(total)-uint32_t(text.size()));if(add>0) text.insert(0,size_t(add),'0');return text;}
std::string MiniFormat::ConvertMilliToTime(int64_t millis) {int64_t days=millis/86400000,years=days/365;days%=365;auto months=days/30;days%=30;auto weeks=days/7;days%=7;auto hours=millis/3600000-(millis/86400000)*24;auto minutes=millis/60000-(millis/3600000)*60;auto seconds=millis/1000-(millis/60000)*60;return std::to_string(years)+" Years "+std::to_string(months)+" Months "+std::to_string(weeks)+" Weeks "+std::to_string(days)+" Days "+std::to_string(hours)+" Hours "+std::to_string(minutes)+" Minutes "+std::to_string(seconds)+" Seconds";}
std::string MiniFormat::createRandomString(int32_t length) {
    if(length<0) throw std::invalid_argument("Negative array size");auto seedbytes=MiniData::getRandomData(8);uint64_t seed=0;for(auto byte:seedbytes.bytes()) seed=(seed<<8)|byte;constexpr uint64_t mask=(uint64_t(1)<<48)-1;seed=(seed^0x5DEECE66DULL)&mask;
    std::vector<uint8_t> bytes;bytes.reserve(size_t(length));while(bytes.size()<size_t(length)){seed=(seed*0x5DEECE66DULL+0xB)&mask;auto random=uint32_t(seed>>16);for(int i=0;i<4 && bytes.size()<size_t(length);++i){bytes.push_back(uint8_t(random));random>>=8;}}
    auto number=BigInt::fromMagnitudeBytes(bytes);if(number.isZero()) return "0";std::string text;while(!number.isZero()){auto digit=(number%BigInt(32)).toDecimalString();text.push_back("0123456789ABCDEFGHIJKLMNOPQRSTUV"[std::stoi(digit)]);number=number/BigInt(32);}std::reverse(text.begin(),text.end());return text;
}
}
