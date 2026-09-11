#pragma once
#include "keys/Signature.hpp"
#include "CoinProof.hpp"
#include "ScriptProof.hpp"
namespace minima {
class Witness:public Streamable {
public:
    using Signatures=std::vector<Signature>;
    using CoinProofs=std::vector<CoinProof>;
    using Scripts=std::vector<ScriptProof>;
private:
    // Copies model Java references; each exposed list owns its lifetime independently.
    struct Data {
        std::shared_ptr<Signatures> signatures=std::make_shared<Signatures>();
        std::shared_ptr<CoinProofs> coins=std::make_shared<CoinProofs>();
        std::shared_ptr<Scripts> scripts=std::make_shared<Scripts>();
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    void clearSignatures() {mData->signatures->clear();}
    void addSignature(Signature signature);
    void addSignature(const std::shared_ptr<Signature>& signature);
    std::shared_ptr<Signatures> getAllSignatures() const {return mData->signatures;}
    std::vector<MiniData> getAllSignatureKeys() const;
    bool isSignedBy(const std::optional<std::string>& publicKey) const;
    void clearCoinProofs() {mData->coins->clear();}
    void addCoinProof(CoinProof proof) {mData->coins->push_back(std::move(proof));}
    std::shared_ptr<CoinProofs> getAllCoinProofs() const {return mData->coins;}
    void clearScriptProofs() {mData->scripts->clear();}
    void addScript(ScriptProof script);
    const ScriptProof* getScript(const MiniData& address) const;
    std::shared_ptr<Scripts> getAllScripts() const {return mData->scripts;}
    json::JSONObject toJSON() const;
    std::string toString() const {return toJSON().toJSONString();}
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static Witness deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
