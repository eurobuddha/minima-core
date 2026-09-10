#pragma once
#include "keys/Signature.hpp"
#include "CoinProof.hpp"
#include "ScriptProof.hpp"
namespace minima {
class Witness {
    std::vector<Signature> mSignatureProofs;
    std::vector<CoinProof> mCoinProofs;
    std::vector<ScriptProof> mScriptProofs;
public:
    void clearSignatures() {mSignatureProofs.clear();}
    void addSignature(Signature signature);
    void addSignature(const std::shared_ptr<Signature>& signature);
    const std::vector<Signature>& getAllSignatures() const {return mSignatureProofs;}
    std::vector<Signature>& getAllSignatures() {return mSignatureProofs;}
    std::vector<MiniData> getAllSignatureKeys() const;
    bool isSignedBy(const std::optional<std::string>& publicKey) const;
    void clearCoinProofs() {mCoinProofs.clear();}
    void addCoinProof(CoinProof proof) {mCoinProofs.push_back(std::move(proof));}
    const std::vector<CoinProof>& getAllCoinProofs() const {return mCoinProofs;}
    std::vector<CoinProof>& getAllCoinProofs() {return mCoinProofs;}
    void clearScriptProofs() {mScriptProofs.clear();}
    void addScript(ScriptProof script);
    const ScriptProof* getScript(const MiniData& address) const;
    const std::vector<ScriptProof>& getAllScripts() const {return mScriptProofs;}
    std::vector<ScriptProof>& getAllScripts() {return mScriptProofs;}
    json::JSONObject toJSON() const;
    std::string toString() const {return toJSON().toJSONString();}
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static Witness deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
