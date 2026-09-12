#pragma once
#include "org/minima/objects/TxPoW.hpp"
#include <memory>
namespace minima {
class CascadeNode {
    TxPoW mTxPoW;
    int32_t mCurrentLevel=0,mSuperLevel=0;
    std::shared_ptr<CascadeNode> mParent;
    CascadeNode()=default;
public:
    explicit CascadeNode(const TxPoW& txpow);
    ~CascadeNode();
    CascadeNode(const CascadeNode&)=default;
    CascadeNode(CascadeNode&&)=default;
    CascadeNode& operator=(const CascadeNode&)=default;
    CascadeNode& operator=(CascadeNode&&)=default;
    TxPoW& getTxPoW() {return mTxPoW;}
    const TxPoW& getTxPoW() const {return mTxPoW;}
    int32_t getLevel() const {return mCurrentLevel;}
    void setLevel(int32_t level) {mCurrentLevel=level;}
    int32_t getSuperLevel() const {return mSuperLevel;}
    void setParent(std::shared_ptr<CascadeNode> parent) {mParent=std::move(parent);}
    std::shared_ptr<CascadeNode> getParent() const {return mParent;}
    MMREntryNumber getCurrentWeight() const;
    std::string toString() const;
    std::vector<uint8_t> serialise() const;
    static CascadeNode deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
