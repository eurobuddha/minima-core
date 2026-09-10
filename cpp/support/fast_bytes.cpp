#include "org/minima/utils/FastByteArrayStream.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/Crypto.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
class Stream:public FastByteArrayStream{public:using FastByteArrayStream::FastByteArrayStream;size_t capacity()const{return buf.size();}void own(int32_t off,int32_t len){writeData(buf,off,len);}};
static int32_t integer(const json::JSONObject& o,const std::string& k){return int32_t(std::get<int64_t>(o.get(k).value));}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto data=std::get<std::shared_ptr<json::JSONObject>>(json::JSONValue::parseWithException(line).value);Stream stream(integer(*data,"initial"));auto ops=std::get<std::shared_ptr<json::JSONArray>>(data->get("ops").value);std::string out;for(size_t i=0;i<ops->size();++i){auto op=std::get<std::shared_ptr<json::JSONObject>>(ops->get(i).value);auto command=op->get("op").asString();std::string result="OK";try{
    if(command=="reset")stream.reset();else if(command=="close")stream.close();else if(command=="byte")stream.write(integer(*op,"value"));else if(command=="own")stream.own(integer(*op,"offset"),integer(*op,"length"));
    else if(command=="text"){json::JSONObject object;object.put("text",stream.toString());object.put("high",stream.toString(integer(*op,"high")));result=object.toJSONString();}
    else if(command=="output"){std::ostringstream bytes;stream.writeTo(bytes);auto text=bytes.str();result=MiniData(std::vector<uint8_t>(text.begin(),text.end())).sha3().to0xString();}
    else{std::optional<MiniData> bytes;if(!op->get("data").isNull())bytes=MiniData::fromHex(op->get("data").asString());const auto* p=bytes?&bytes->bytes():nullptr;if(command=="fast")stream.writeData(p,integer(*op,"offset"),integer(*op,"length"));else stream.write(p,integer(*op,"offset"),integer(*op,"length"));}
}catch(const std::exception&){result="ERROR";}out+=result+":"+std::to_string(stream.size())+":"+std::to_string(stream.capacity())+":"+MiniData(stream.toByteArray()).sha3().to0xString()+"|";}std::cout<<out<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
