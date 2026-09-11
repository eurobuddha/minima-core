#include "TxPoW.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include <chrono>
#include <bit>
#include <iostream>
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
TxPoW::TxPoW(std::string id,int block,int weight,bool isBlock,std::string parent,bool isTransaction)
:mBody(nullptr),mTxPOWIDStr(std::move(id)),mBlockWeight(weight),mIsTesting(true),mTestBlockNumber(block),mTestIsBlock(isBlock),mTestIsTxn(isTransaction),mTestParent(MiniData::fromHex(parent)){}
std::vector<std::string> TxPoW::getTransactions() const {if(mIsTesting) return mTestTransactions;std::vector<std::string> ids;if(mBody) for(const auto blockIDs=mBody->mTxPowIDList();const auto id:TxBody::required(blockIDs)) ids.push_back(TxBody::required(id).to0xString());return ids;}
void TxPoW::setHeaderBodyHash() {mHeader.mTxBodyHash=Crypto::getInstance().hashObject(TxBody::required(mBody));}
bool TxPoW::isMonotonic() const {return getTransaction().isCheckedMonotonic()&&(getBurnTransaction().isEmpty()||getBurnTransaction().isCheckedMonotonic());}
void TxPoW::setTimeMilli() {setTimeMilli(MiniNumber(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()));}
void TxPoW::incrementCheckRejectNumber() {mCheckNumber=std::bit_cast<int32_t>(uint32_t(mCheckNumber)+1u);}
json::JSONObject TxPoW::toJSON() const {
    json::JSONObject obj;
    if(mIsTesting) {obj.put("txpowid",mTxPOWIDStr);obj.put("parentid",mTestParent.to0xString());obj.put("blocknumber",json::JSONValue::raw(mTestBlockNumber.toString()));obj.put("weight",json::JSONValue::raw(mBlockWeight.toString()));obj.put("isblock",mTestIsBlock);obj.put("istxn",mTestIsTxn);}
    else {obj.put("txpowid",mTxPOWID.to0xString());obj.put("isblock",mIsBlockPOW);obj.put("istransaction",mIsTxnPOW);obj.put("superblock",mSuperBlock);obj.put("size",getSizeinBytes());obj.put("burn",json::JSONValue::raw(getBurn().toString()));obj.put("header",mHeader.toJSON());obj.put("hasbody",hasBody());obj.put("body",mBody?json::JSONValue(mBody->toJSON()):json::JSONValue("null"));}
    return obj;
}
std::vector<uint8_t> TxPoW::serialise() const {auto out=mHeader.serialise();wire::append(out,MiniByte(hasBody()).serialise());if(mBody) wire::append(out,mBody->serialise());return out;}
TxPoW TxPoW::deserialise(std::span<const uint8_t> data,size_t& offset) {TxPoW txpow;txpow.mHeader=TxHeader::deserialise(data,offset);if(MiniByte::deserialise(data,offset).isTrue()) txpow.mBody=std::make_shared<TxBody>(TxBody::deserialise(data,offset));else txpow.clearBody();txpow.calculateTXPOWID();return txpow;}
TxPoW TxPoW::deepCopy() const {auto data=serialise();size_t offset=0;return deserialise(data,offset);}
std::optional<TxPoW> TxPoW::convertMiniDataVersion(const MiniData& data) {
    return convertMiniDataVersion(data,[](const wire::IOException& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));});
}
std::optional<TxPoW> TxPoW::convertMiniDataVersion(const MiniData& data,
    const std::function<void(const wire::IOException&)>& onError) {
    try {size_t offset=0;return deserialise(data.bytes(),offset);}
    catch(const wire::IOException& error) {onError(error);return std::nullopt;}
}
void TxPoW::calculateTransactionID() {getTransaction().calculateTransactionID();getBurnTransaction().calculateTransactionID();}
bool TxPoW::isTransaction() const {if(mIsTesting) return mTestIsTxn;return hasBody()&&(!getTransaction().isEmpty()||!getBurnTransaction().isEmpty());}
int64_t TxPoW::getSizeinBytesWithoutBlockTxns() const {return mTxPoWSize-std::bit_cast<int32_t>(32u*uint32_t(TxBody::required(getBlockTransactions()).size()));}
void TxPoW::calculateTXPOWID() {
    mTxPOWID=Crypto::getInstance().hashObject(mHeader);mTxPOWIDStr=mTxPOWID.to0xString();mIsBlockPOW=mTxPOWID.isLess(getBlockDifficulty());mBlockWeight=MMREntryNumber(0);
    if(mIsBlockPOW) mBlockWeight=MMREntryNumber::dividePositive(Crypto::MAX_VAL,getBlockDifficulty().getDataValue(),7);
    mIsTxnPOW=false;if(mBody) {calculateTransactionID();mIsTxnPOW=mTxPOWID.isLess(getTxnDifficulty())&&!getTransaction().isEmpty();}
    auto quotient=getBlockDifficulty().getDataValue()/mTxPOWID.getDataValue();auto magnitude=MiniData(quotient).bytes();
    size_t first=0;while(first<magnitude.size()&&magnitude[first]==0) ++first;
    mSuperBlock=first==magnitude.size()?-1:int((magnitude.size()-first-1)*8+std::bit_width(unsigned(magnitude[first])))-1;
    if(mSuperBlock>=GlobalParams::MINIMA_CASCADE_LEVELS) mSuperBlock=GlobalParams::MINIMA_CASCADE_LEVELS-1;
    mTxPoWSize=int64_t(serialise().size());
}
}
