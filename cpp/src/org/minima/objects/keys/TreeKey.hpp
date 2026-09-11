#pragma once
#include "TreeKeyNode.hpp"
#include "Signature.hpp"
#include <functional>
namespace minima {
class TreeKey {
    std::unique_ptr<TreeKeyNode> mRoot;
    int mLevels=0,mKeysPerLevel=0,mUses=0,mMaxUses=0;
    std::optional<MiniData> mPrivateSeed,mPublicKey;
    std::function<void(const std::string&)> mLogger;
    void log(const std::string& text) const;
    static std::vector<int> baseConversion(int number,int base,int levels);
public:
    static constexpr int MAX_KEY_LEVELS=8,DEFAULT_KEYSPERLEVEL=64,DEFAULT_LEVELS=3;
    static TreeKey createDefault(const MiniData& seed) {return TreeKey(seed,DEFAULT_KEYSPERLEVEL,DEFAULT_LEVELS);}
    TreeKey()=default;
    TreeKey(const MiniData& seed,int keys,int levels);
    void setPublicKey(MiniData key) {mPublicKey=std::move(key);}
    void setPublicKey(const MiniData* key) {mPublicKey=key?std::optional<MiniData>(*key):std::nullopt;}
    const std::optional<MiniData>& getPublicKey() const {return mPublicKey;}
    const std::optional<MiniData>& getPrivateKey() const {return mPrivateSeed;}
    int getMaxUses() const {return mMaxUses;}
    int getUses() const {return mUses;}
    void setUses(int uses) {mUses=uses;}
    int getSize() const {return mKeysPerLevel;}
    int getDepth() const {return mLevels;}
    void setLogger(std::function<void(const std::string&)> logger){mLogger=std::move(logger);}
    Signature sign(const MiniData* data);
    Signature sign(const MiniData& data){return sign(&data);}
    bool verify(const MiniData* data,const Signature& signature) const;
    bool verify(const MiniData& data,const Signature& signature) const {return verify(&data,signature);}
    bool verify(const MiniData* data,const Signature* signature) const;
    bool verify(const MiniData& data,const Signature* signature) const;
};
}
