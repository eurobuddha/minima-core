#include "StateVariable.hpp"
#include "Address.hpp"
#include "base/MiniNumber.hpp"
#include "support/ByteIO.hpp"
#include "support/JavaStringIgnoreCase.hpp"
#include <algorithm>
namespace minima {
const MiniByte StateVariable::STATETYPE_HEX(1),StateVariable::STATETYPE_NUMBER(2),StateVariable::STATETYPE_STRING(4),StateVariable::STATETYPE_BOOL(8);
StateVariable::StateVariable(int port,const std::string& data):mPort(port) {
    if(port<0||port>255) throw std::invalid_argument("State Variable port MUST be 0-255");
    if(std::all_of(data.begin(),data.end(),[](unsigned char c){return c<=32;})) throw std::invalid_argument("State Variable cannot be blank");
    auto lower=data;for(char& c:lower) if(c>='A'&&c<='Z') c+=32;
    if(lower.starts_with("mx")) {mType=STATETYPE_HEX;mData=MiniString(Address::convertMinimaAddress(data).to0xString());}
    else if(lower.starts_with("0x")) {mType=STATETYPE_HEX;mData=MiniString(MiniData::fromHex(data).to0xString());}
    else if(javaStringEqualsIgnoreCase(data,"true")) {mType=STATETYPE_BOOL;mData=MiniString("TRUE");}
    else if(javaStringEqualsIgnoreCase(data,"false")) {mType=STATETYPE_BOOL;mData=MiniString("FALSE");}
    else if(data.starts_with('[')&&data.ends_with(']')) {mType=STATETYPE_STRING;mData=MiniString(data);}
    else {mType=STATETYPE_NUMBER;mData=MiniString(MiniNumber(data).toString());}
}
json::JSONObject StateVariable::toJSON() const {json::JSONObject obj;obj.put("port",mPort.getValue());obj.put("type",mType.getValue());obj.put("data",mData.toString());return obj;}
void StateVariable::writeDataStream(std::vector<uint8_t>& out) const {
    mPort.writeDataStream(out);mType.writeDataStream(out);const auto text=mData.toString();
    switch(mType.getValue()) {
    case 1:wire::append(out,MiniData::fromHex(text).serialise());break;
    case 2:wire::append(out,MiniNumber(text).serialise());break;
    case 4:wire::append(out,mData.serialise());break;
    case 8:wire::append(out,MiniByte(text=="TRUE").serialise());break;
    default:break; // The source writer emits only port/type for an unknown tag.
    }
}
std::vector<uint8_t> StateVariable::serialise() const {return Streamable::serialise();}
void StateVariable::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mPort=MiniByte::deserialise(data,offset);mType=MiniByte::deserialise(data,offset);
    switch(mType.getValue()) {
    case 1:mData=MiniString(MiniData::deserialise(data,offset).to0xString());break;
    case 2:mData=MiniString(MiniNumber::deserialise(data,offset).toString());break;
    case 4:mData=MiniString::deserialise(data,offset);break;
    case 8:mData=MiniString(MiniByte::deserialise(data,offset).isTrue()?"TRUE":"FALSE");break;
    default:throw wire::IOException("Invalid StateVariable type : "+mType.toString());
    }
}
StateVariable StateVariable::deserialise(std::span<const uint8_t> data,size_t& offset){StateVariable state;state.readDataStream(data,offset);return state;}
}
