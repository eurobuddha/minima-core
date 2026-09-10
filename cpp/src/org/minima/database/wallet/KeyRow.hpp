#pragma once
#include "support/SqlRow.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class KeyRow {
public:
    int32_t mSize=0,mDepth=0,mUses=0,mMaxUses=0;
    std::optional<std::string> mModifier,mPublicKey,mPrivateKey;
    explicit KeyRow(SqlRow& row);
    KeyRow(int32_t size,int32_t depth,int32_t uses,int32_t maxUses,std::optional<std::string> modifier,std::optional<std::string> privateKey,std::optional<std::string> publicKey)
        :mSize(size),mDepth(depth),mUses(uses),mMaxUses(maxUses),mModifier(std::move(modifier)),mPublicKey(std::move(publicKey)),mPrivateKey(std::move(privateKey)){}
    int32_t getSize() const {return mSize;}
    int32_t getDepth() const {return mDepth;}
    int32_t getUses() const {return mUses;}
    int32_t getMaxUses() const {return mMaxUses;}
    const auto& getModifier() const {return mModifier;}
    const auto& getPrivateKey() const {return mPrivateKey;}
    const auto& getPublicKey() const {return mPublicKey;}
    json::JSONObject toJSON() const;
};
}
