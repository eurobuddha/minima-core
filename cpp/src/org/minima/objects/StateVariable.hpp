#pragma once
#include "base/MiniByte.hpp"
#include "base/MiniString.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class StateVariable {
    MiniByte mType,mPort;
    MiniString mData;
    StateVariable()=default;
public:
    static const MiniByte STATETYPE_HEX,STATETYPE_NUMBER,STATETYPE_STRING,STATETYPE_BOOL;
    StateVariable(int port,const std::string& data);
    int getPort() const {return mPort.getValue();}
    MiniByte getType() const {return mType;}
    const MiniString& getData() const {return mData;}
    std::string toString() const {return mData.toString();}
    json::JSONObject toJSON() const;
    std::vector<uint8_t> serialise() const;
    static StateVariable deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
