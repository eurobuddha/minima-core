#pragma once
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class Magic:public Streamable {
public:
    // Java final references: owners are fixed, their Streamable objects are mutable.
    static const std::shared_ptr<MiniNumber> MIN_HASHES,DEFAULT_TXPOW_SIZE,DEFAULT_KISSVM_OPERATIONS,DEFAULT_TXPOW_TXNS;
    static const BigInt MIN_TXPOW_VAL;
    static const std::shared_ptr<MiniData> MIN_TXPOW_WORK;
private:
    struct Data {
        std::shared_ptr<MiniNumber> mCurrentMaxTxPoWSize=DEFAULT_TXPOW_SIZE;
        std::shared_ptr<MiniNumber> mCurrentMaxKISSVMOps=DEFAULT_KISSVM_OPERATIONS;
        std::shared_ptr<MiniNumber> mCurrentMaxTxnPerBlock=DEFAULT_TXPOW_TXNS;
        std::shared_ptr<MiniData> mCurrentMinTxPoWWork=MIN_TXPOW_WORK;
        std::shared_ptr<MiniNumber> mDesiredMaxTxPoWSize=DEFAULT_TXPOW_SIZE;
        std::shared_ptr<MiniNumber> mDesiredMaxKISSVMOps=DEFAULT_KISSVM_OPERATIONS;
        std::shared_ptr<MiniNumber> mDesiredMaxTxnPerBlock=DEFAULT_TXPOW_TXNS;
        std::shared_ptr<MiniData> mDesiredMinTxPoWWork=MIN_TXPOW_WORK;
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    // Lvalue accessors preserve public field assignment across retained Magic aliases.
    std::shared_ptr<MiniNumber>& mCurrentMaxTxPoWSize() {return mData->mCurrentMaxTxPoWSize;}
    std::shared_ptr<MiniNumber> mCurrentMaxTxPoWSize() const {return mData->mCurrentMaxTxPoWSize;}
    std::shared_ptr<MiniNumber>& mCurrentMaxKISSVMOps() {return mData->mCurrentMaxKISSVMOps;}
    std::shared_ptr<MiniNumber> mCurrentMaxKISSVMOps() const {return mData->mCurrentMaxKISSVMOps;}
    std::shared_ptr<MiniNumber>& mCurrentMaxTxnPerBlock() {return mData->mCurrentMaxTxnPerBlock;}
    std::shared_ptr<MiniNumber> mCurrentMaxTxnPerBlock() const {return mData->mCurrentMaxTxnPerBlock;}
    std::shared_ptr<MiniData>& mCurrentMinTxPoWWork() {return mData->mCurrentMinTxPoWWork;}
    std::shared_ptr<MiniData> mCurrentMinTxPoWWork() const {return mData->mCurrentMinTxPoWWork;}
    std::shared_ptr<MiniNumber>& mDesiredMaxTxPoWSize() {return mData->mDesiredMaxTxPoWSize;}
    std::shared_ptr<MiniNumber> mDesiredMaxTxPoWSize() const {return mData->mDesiredMaxTxPoWSize;}
    std::shared_ptr<MiniNumber>& mDesiredMaxKISSVMOps() {return mData->mDesiredMaxKISSVMOps;}
    std::shared_ptr<MiniNumber> mDesiredMaxKISSVMOps() const {return mData->mDesiredMaxKISSVMOps;}
    std::shared_ptr<MiniNumber>& mDesiredMaxTxnPerBlock() {return mData->mDesiredMaxTxnPerBlock;}
    std::shared_ptr<MiniNumber> mDesiredMaxTxnPerBlock() const {return mData->mDesiredMaxTxnPerBlock;}
    std::shared_ptr<MiniData>& mDesiredMinTxPoWWork() {return mData->mDesiredMinTxPoWWork;}
    std::shared_ptr<MiniData> mDesiredMinTxPoWWork() const {return mData->mDesiredMinTxPoWWork;}
    template<class T> static T& required(const std::shared_ptr<T>& value) {
        if(!value)throw JavaNullPointerException("null Magic field");return *value;
    }
    virtual json::JSONObject toJSON() const;
    virtual bool checkSame(const Magic* other) const;
    bool checkSame(const Magic& other) const {return checkSame(&other);}
    virtual std::shared_ptr<MiniNumber> getMaxTxPoWSize() const {return mCurrentMaxTxPoWSize();}
    virtual std::shared_ptr<MiniNumber> getMaxKISSOps() const {return mCurrentMaxKISSVMOps();}
    virtual std::shared_ptr<MiniNumber> getMaxNumTxns() const {return mCurrentMaxTxnPerBlock();}
    virtual std::shared_ptr<MiniData> getMinTxPowWork() const {return mCurrentMinTxPoWWork();}
    virtual void setDesiredKISSVM(std::shared_ptr<MiniNumber> value) {mDesiredMaxKISSVMOps()=std::move(value);}
    void setDesiredKISSVM(MiniNumber value) {setDesiredKISSVM(std::make_shared<MiniNumber>(std::move(value)));}
    virtual void setDesiredMaxTxPoWSize(std::shared_ptr<MiniNumber> value) {mDesiredMaxTxPoWSize()=std::move(value);}
    void setDesiredMaxTxPoWSize(MiniNumber value) {setDesiredMaxTxPoWSize(std::make_shared<MiniNumber>(std::move(value)));}
    virtual void setDesiredMaxTxns(std::shared_ptr<MiniNumber> value) {mDesiredMaxTxnPerBlock()=std::move(value);}
    void setDesiredMaxTxns(MiniNumber value) {setDesiredMaxTxns(std::make_shared<MiniNumber>(std::move(value)));}
    virtual Magic calculateNewCurrent() const;
    virtual std::string toString() const {return toJSON().toJSONString();}
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static Magic deserialise(std::span<const uint8_t> data,size_t& offset);
    static void main();
};
}
