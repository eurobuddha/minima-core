#pragma once
#include "TxHeader.hpp"
#include "TxBody.hpp"
#include "mmr/MMREntryNumber.hpp"
#include "support/JavaExceptions.hpp"
#include <functional>
namespace minima {
class TxPoW {
    TxHeader mHeader;
    std::shared_ptr<TxBody> mBody=std::make_shared<TxBody>();
    std::string mTxPOWIDStr="0x00";
    MiniData mTxPOWID=MiniData::fromHex("0x00");
protected:
    bool mIsBlockPOW=false,mIsTxnPOW=false;
private:
    int mSuperBlock=0;
    int64_t mTxPoWSize=0;
    MMREntryNumber mBlockWeight;
    int mCheckNumber=0;
    bool mIsTesting=false;
    MiniNumber mTestBlockNumber;
    bool mTestIsBlock=true,mTestIsTxn=false;
    MiniData mTestParent=MiniData::fromHex("0x00");
    std::vector<std::string> mTestTransactions;
public:
    TxPoW()=default;
    TxPoW(std::string id,int block,int weight,bool isBlock=true,std::string parent="0x00",bool isTransaction=false);
    void addTestTransaction(std::string id) {mTestTransactions.push_back(std::move(id));}
    std::vector<std::string> getTransactions() const;
    TxHeader& getTxHeader() {return mHeader;}
    const TxHeader& getTxHeader() const {return mHeader;}
    const MiniData& getTxHeaderBodyHash() const {return mHeader.getBodyHash();}
    void setHeaderBodyHash();
    bool isMonotonic() const;
    std::shared_ptr<TxBody> getTxBody() const {return mBody;}
    bool hasBody() const {return bool(mBody);}
    void clearBody() {mBody.reset();}
    void setNonce(MiniNumber nonce) {mHeader.mNonce=std::move(nonce);}
    const MiniNumber& getNonce() const {return mHeader.mNonce;}
    const MiniData& getChainID() const {return mHeader.mChainID;}
    Magic& getMagic() {return mHeader.mMagic;}
    const Magic& getMagic() const {return mHeader.mMagic;}
    void setMagic(Magic magic) {mHeader.mMagic=std::move(magic);}
    void setTxDifficulty(std::shared_ptr<MiniData> difficulty) {TxBody::required(mBody).mTxnDifficulty()=std::move(difficulty);}
    void setTxDifficulty(MiniData difficulty) {setTxDifficulty(std::make_shared<MiniData>(std::move(difficulty)));}
    const MiniData& getTxnDifficulty() const {if(!mBody)throw JavaNullPointerException("null TxPoW body");return TxBody::required(mBody->mTxnDifficulty());}
    Transaction& getTransaction() {auto owner=getSharedTransaction();if(!owner)throw JavaNullPointerException("null TxPoW Transaction");return *owner;}
    const Transaction& getTransaction() const {auto owner=getSharedTransaction();if(!owner)throw JavaNullPointerException("null TxPoW Transaction");return *owner;}
    Transaction& getBurnTransaction() {auto owner=getSharedBurnTransaction();if(!owner)throw JavaNullPointerException("null TxPoW BurnTransaction");return *owner;}
    const Transaction& getBurnTransaction() const {auto owner=getSharedBurnTransaction();if(!owner)throw JavaNullPointerException("null TxPoW BurnTransaction");return *owner;}
    std::shared_ptr<Transaction> getSharedTransaction() const {if(!mBody)throw JavaNullPointerException("null TxPoW body");return mBody->mTransaction();}
    void setTransaction(std::shared_ptr<Transaction> txn) {if(!mBody)throw JavaNullPointerException("null TxPoW body");mBody->mTransaction()=std::move(txn);}
    void setTransaction(Transaction txn) {setTransaction(std::make_shared<Transaction>(std::move(txn)));}
    std::shared_ptr<Witness> getSharedWitness() const {if(!mBody)throw JavaNullPointerException("null TxPoW body");return mBody->mWitness();}
    void setWitness(std::shared_ptr<Witness> witness) {if(!mBody)throw JavaNullPointerException("null TxPoW body");mBody->mWitness()=std::move(witness);}
    void setWitness(Witness witness) {setWitness(std::make_shared<Witness>(std::move(witness)));}
    std::shared_ptr<Transaction> getSharedBurnTransaction() const {if(!mBody)throw JavaNullPointerException("null TxPoW body");return mBody->mBurnTransaction();}
    void setBurnTransaction(std::shared_ptr<Transaction> txn) {if(!mBody)throw JavaNullPointerException("null TxPoW body");mBody->mBurnTransaction()=std::move(txn);}
    void setBurnTransaction(Transaction txn) {setBurnTransaction(std::make_shared<Transaction>(std::move(txn)));}
    std::shared_ptr<Witness> getSharedBurnWitness() const {if(!mBody)throw JavaNullPointerException("null TxPoW body");return mBody->mBurnWitness();}
    void setBurnWitness(std::shared_ptr<Witness> witness) {if(!mBody)throw JavaNullPointerException("null TxPoW body");mBody->mBurnWitness()=std::move(witness);}
    void setBurnWitness(Witness witness) {setBurnWitness(std::make_shared<Witness>(std::move(witness)));}
    Witness& getWitness() {auto owner=getSharedWitness();if(!owner)throw JavaNullPointerException("null TxPoW Witness");return *owner;}
    const Witness& getWitness() const {auto owner=getSharedWitness();if(!owner)throw JavaNullPointerException("null TxPoW Witness");return *owner;}
    Witness& getBurnWitness() {auto owner=getSharedBurnWitness();if(!owner)throw JavaNullPointerException("null TxPoW BurnWitness");return *owner;}
    const Witness& getBurnWitness() const {auto owner=getSharedBurnWitness();if(!owner)throw JavaNullPointerException("null TxPoW BurnWitness");return *owner;}
    void addBlockTxPOW(std::shared_ptr<MiniData> id) {TxBody::required(TxBody::required(mBody).mTxPowIDList()).push_back(std::move(id));}
    void addBlockTxPOW(MiniData id) {addBlockTxPOW(std::make_shared<MiniData>(std::move(id)));}
    std::shared_ptr<TxBody::IDs> getBlockTransactions() const {return mBody?mBody->mTxPowIDList():std::make_shared<TxBody::IDs>();}
    const MiniData& getBlockDifficulty() const {return mHeader.mBlockDifficulty;}
    void setBlockDifficulty(MiniData difficulty) {mHeader.mBlockDifficulty=std::move(difficulty);}
    MiniData getParentID() const {return mIsTesting?mTestParent:getSuperParent(0);}
    void setSuperParent(int level,MiniData parent) {mHeader.mSuperParents.at(size_t(level))=std::move(parent);}
    const MiniData& getSuperParent(int level) const {return mHeader.mSuperParents.at(size_t(level));}
    MiniNumber getBurn() const {return getTransaction().getBurn().add(getBurnTransaction().getBurn());}
    void setTimeMilli();
    void setTimeMilli(MiniNumber millis) {mHeader.mTimeMilli=std::move(millis);}
    const MiniNumber& getTimeMilli() const {return mHeader.mTimeMilli;}
    void setBlockNumber(MiniNumber block) {mHeader.mBlockNumber=std::move(block);}
    const MiniNumber& getBlockNumber() const {return mIsTesting?mTestBlockNumber:mHeader.mBlockNumber;}
    const MiniData& getMMRRoot() const {return mHeader.mMMRRoot;}
    void setMMRRoot(MiniData root) {mHeader.mMMRRoot=std::move(root);}
    const MiniNumber& getMMRTotal() const {return mHeader.mMMRTotal;}
    void setMMRTotal(MiniNumber total) {mHeader.mMMRTotal=std::move(total);}
    const MiniData& getCustomHash() const {return mHeader.mCustomHash;}
    void setCustomHash(MiniData hash) {mHeader.mCustomHash=std::move(hash);}
    int getCheckRejectNumber() const {return mCheckNumber;}
    void incrementCheckRejectNumber();
    json::JSONObject toJSON() const;
    std::string toString() const {return toJSON().toJSONString();}
    std::vector<uint8_t> serialise() const;
    static TxPoW deserialise(std::span<const uint8_t> data,size_t& offset);
    TxPoW deepCopy() const;
    static std::optional<TxPoW> convertMiniDataVersion(const MiniData& data);
    static std::optional<TxPoW> convertMiniDataVersion(const MiniData& data,
        const std::function<void(const wire::IOException&)>& onError);
    void calculateTransactionID();
    const std::string& getTxPoWID() const {return mTxPOWIDStr;}
    const MiniData& getTxPoWIDData() const {return mTxPOWID;}
    int getSuperLevel() const {return mSuperBlock;}
    bool isBlock() const {return mIsTesting?mTestIsBlock:mIsBlockPOW;}
    const MMREntryNumber& getWeight() const {return mBlockWeight;}
    bool isTransaction() const;
    int64_t getSizeinBytes() const {return mTxPoWSize;}
    int64_t getSizeinBytesWithoutBlockTxns() const;
    void calculateTXPOWID();
};
}
