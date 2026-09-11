#pragma once
#include "Magic.hpp"
#include "support/JavaExceptions.hpp"
#include <optional>
namespace minima {
class TxHeader:public Streamable {
public:
    using Parents=std::vector<std::shared_ptr<MiniData>>;
    static std::shared_ptr<MiniData> MAIN_NET,TEST_NET;
private:
    struct Data {
        std::shared_ptr<MiniNumber> mNonce;
        std::shared_ptr<MiniData> mChainID;
        std::shared_ptr<MiniNumber> mTimeMilli;
        std::shared_ptr<MiniNumber> mBlockNumber;
        std::shared_ptr<MiniData> mBlockDifficulty;
        std::shared_ptr<Parents> mSuperParents;
        std::shared_ptr<Magic> mMagic;
        std::shared_ptr<MiniData> mMMRRoot;
        std::shared_ptr<MiniNumber> mMMRTotal;
        std::shared_ptr<MiniData> mCustomHash;
        std::shared_ptr<MiniData> mTxBodyHash;
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    TxHeader();
    // Public Java reference fields, shared by every retained header wrapper.
    std::shared_ptr<MiniNumber>& mNonce() {return mData->mNonce;}
    std::shared_ptr<MiniNumber> mNonce() const {return mData->mNonce;}
    std::shared_ptr<MiniData>& mChainID() {return mData->mChainID;}
    std::shared_ptr<MiniData> mChainID() const {return mData->mChainID;}
    std::shared_ptr<MiniNumber>& mTimeMilli() {return mData->mTimeMilli;}
    std::shared_ptr<MiniNumber> mTimeMilli() const {return mData->mTimeMilli;}
    std::shared_ptr<MiniNumber>& mBlockNumber() {return mData->mBlockNumber;}
    std::shared_ptr<MiniNumber> mBlockNumber() const {return mData->mBlockNumber;}
    std::shared_ptr<MiniData>& mBlockDifficulty() {return mData->mBlockDifficulty;}
    std::shared_ptr<MiniData> mBlockDifficulty() const {return mData->mBlockDifficulty;}
    std::shared_ptr<Parents>& mSuperParents() {return mData->mSuperParents;}
    std::shared_ptr<Parents> mSuperParents() const {return mData->mSuperParents;}
    std::shared_ptr<Magic>& mMagic() {return mData->mMagic;}
    std::shared_ptr<Magic> mMagic() const {return mData->mMagic;}
    std::shared_ptr<MiniData>& mMMRRoot() {return mData->mMMRRoot;}
    std::shared_ptr<MiniData> mMMRRoot() const {return mData->mMMRRoot;}
    std::shared_ptr<MiniNumber>& mMMRTotal() {return mData->mMMRTotal;}
    std::shared_ptr<MiniNumber> mMMRTotal() const {return mData->mMMRTotal;}
    std::shared_ptr<MiniData>& mCustomHash() {return mData->mCustomHash;}
    std::shared_ptr<MiniData> mCustomHash() const {return mData->mCustomHash;}
    std::shared_ptr<MiniData>& mTxBodyHash() {return mData->mTxBodyHash;}
    std::shared_ptr<MiniData> mTxBodyHash() const {return mData->mTxBodyHash;}
    template<class T> static T& required(const std::shared_ptr<T>& value) {
        if(!value)throw JavaNullPointerException("null TxHeader field");return *value;
    }
    static std::shared_ptr<MiniData>& parentAt(const std::shared_ptr<Parents>& parents,int index) {
        auto& values=required(parents);
        if(index<0||size_t(index)>=values.size())throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(index)+" out of bounds for length "+std::to_string(values.size()));
        return values[size_t(index)];
    }
    virtual std::shared_ptr<MiniData> getBodyHash() const {return mTxBodyHash();}
    virtual json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static TxHeader deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::optional<TxHeader> convertMiniDataVersion(const MiniData& data);
    static std::optional<TxHeader> convertMiniDataVersion(const MiniData* data);
};
}
