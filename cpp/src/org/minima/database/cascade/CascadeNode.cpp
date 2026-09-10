#include "CascadeNode.hpp"
namespace minima {
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
