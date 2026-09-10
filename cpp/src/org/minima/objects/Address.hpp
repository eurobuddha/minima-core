#pragma once
#include "base/MiniData.hpp"
#include "base/MiniString.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class Address {
    MiniString mScript;
    MiniData mAddressData;
    std::string mMinimaAddress;
public:
    static const Address TRUE_ADDRESS;
    Address()=default;
    explicit Address(const std::string& script);
    explicit Address(MiniData address);
    json::JSONObject toJSON() const;
    std::string toString() const {return mAddressData.to0xString();}
    std::string getScript() const {return mScript.toString();}
    const MiniData& getAddressData() const {return mAddressData;}
    const std::string& getMinimaAddress() const {return mMinimaAddress;}
    bool isEqual(const Address& other) const {return mAddressData.isEqual(other.mAddressData);}
    std::vector<uint8_t> serialise() const;
    static Address deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::string makeMinimaAddress(const MiniData& address);
    static MiniData convertMinimaAddress(const std::string& address);
};
}
