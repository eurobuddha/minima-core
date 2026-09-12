#include "CascadeNode.hpp"
namespace minima {
namespace {
// Only nodes whose destructor has started surrender their parent reference.
// Never detach a live node based on use_count(): a weak owner can concurrently
// become a strong owner between that check and the detach.
struct CascadeRelease {
    std::shared_ptr<CascadeNode> next;
    // Ordinary chains use only the inline slot. A custom deleter can reenter
    // cleanup for independent chains, so it must not overwrite pending work.
    std::vector<std::shared_ptr<CascadeNode>> pending;
    void enqueue(std::shared_ptr<CascadeNode> parent) {
        if(!parent)return;
        if(!next)next=std::move(parent);
        else pending.push_back(std::move(parent));
    }
};
// A trivially destructible pointer also works when cleanup originates in a
// thread-local owner's destructor. The work itself lives on the caller's stack.
thread_local CascadeRelease* activeRelease=nullptr;
}
CascadeNode::~CascadeNode() {
    if(!mParent)return;
    if(activeRelease){activeRelease->enqueue(std::move(mParent));return;}
    CascadeRelease release;
    activeRelease=&release;
    release.enqueue(std::move(mParent));
    while(release.next || !release.pending.empty()) {
        if(!release.next) {
            release.next=std::move(release.pending.back());
            release.pending.pop_back();
        }
        auto current=std::move(release.next);
        // A final release invokes another destructor, which enqueues its parent
        // and returns immediately. Shared nodes simply keep their existing links.
        current.reset();
    }
    activeRelease=nullptr;
}
CascadeNode::CascadeNode(const TxPoW& txpow):mTxPoW(txpow.deepCopy()),mSuperLevel(txpow.getSuperLevel()) {mTxPoW.clearBody();}
MMREntryNumber CascadeNode::getCurrentWeight() const {
    if(mCurrentLevel<0 || mCurrentLevel>999999999) throw std::invalid_argument("Invalid operation");
    BigInt factor(1),base(2);int exponent=mCurrentLevel;
    while(exponent) {if(exponent&1) factor=factor*base;exponent>>=1;if(exponent) base=base*base;}
    return mTxPoW.getWeight().multiplyDecimal32(factor);
}
std::string CascadeNode::toString() const {
    return "["+std::to_string(mCurrentLevel)+"/"+std::to_string(mSuperLevel)+"] "+mTxPoW.getTxPoWID()+" @ "+mTxPoW.getBlockNumber().toString()+" weight:"+getCurrentWeight().toBigDecimalString()+" @ "+mTxPoW.getTxHeader().toJSON().getString("date");
}
std::vector<uint8_t> CascadeNode::serialise() const {return wire::join(mTxPoW,MiniNumber(mCurrentLevel));}
CascadeNode CascadeNode::deserialise(std::span<const uint8_t> data,size_t& offset) {
    CascadeNode node;node.mTxPoW=TxPoW::deserialise(data,offset);node.mCurrentLevel=MiniNumber::deserialise(data,offset).getAsInt();node.mSuperLevel=node.mTxPoW.getSuperLevel();return node;
}
}
