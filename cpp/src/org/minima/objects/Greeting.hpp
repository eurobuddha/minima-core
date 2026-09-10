#pragma once
#include "base/MiniData.hpp"
#include "base/MiniNumber.hpp"
#include "base/MiniString.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include <optional>
#include "support/GreetingServices.hpp"
namespace minima {
class Greeting {
    MiniString mVersion;
    std::optional<json::JSONObject> mExtraData{std::in_place};
    MiniNumber mTopBlock;
    std::vector<MiniData> mChain;
public:
    static GreetingServices services;
    Greeting();
    Greeting& createGreeting();
    json::JSONObject& getExtraData() {return mExtraData.value();}
    const json::JSONObject& getExtraData() const {return mExtraData.value();}
    // The source getter returns a nullable reference; callers that forward it need no dereference.
    const std::optional<json::JSONObject>& getExtraDataOptional() const {return mExtraData;}
    std::optional<std::string> getExtraDataValue(const std::string& key) const;
    void setTopBlock(MiniNumber top) {mTopBlock=std::move(top);}
    const MiniNumber& getTopBlock() const {return mTopBlock;}
    const MiniString& getVersion() const {return mVersion;}
    MiniNumber getRootBlock() const;
    auto& getChain() {return mChain;}
    const auto& getChain() const {return mChain;}
    std::vector<uint8_t> serialise() const;
    static Greeting deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
