#pragma once
#include "SPHINCSSignature.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
namespace minima::sphincs {
class SPHINCS {
    std::shared_ptr<MiniData> mWotsSeed,mForsBaseSeed,mPublicKey,mPrivateKey;
    std::shared_ptr<const BigInt> mWotsKeyNum;
    static constexpr int WOTS_DEPTH=4,WOTS_KEYSPERLEVEL=192;
public:
    SPHINCS()=default;
    explicit SPHINCS(const MiniData* seed) {initSeed(seed);}
    explicit SPHINCS(const MiniData& seed):SPHINCS(&seed){}
    void initSeed(const MiniData* seed);
    void initSeed(const MiniData& seed) {initSeed(&seed);}
    void initPrivateKey(std::shared_ptr<MiniData> key);
    void initPrivateKey(const MiniData& key) {initPrivateKey(std::make_shared<MiniData>(key));}
    std::shared_ptr<MiniData> getPublicKey() const {return mPublicKey;}
    std::shared_ptr<MiniData> getPrivateKey() const {return mPrivateKey;}
    std::shared_ptr<const BigInt> getTotalWotsKeys() const {return mWotsKeyNum;}
    SPHINCSSignature signMessage(const MiniData* message) const;
    SPHINCSSignature signMessage(const MiniData& message) const {return signMessage(&message);}
    static SPHINCSSignature signMessage(const MiniData* message,std::shared_ptr<MiniData> privateKey);
    static SPHINCSSignature signMessage(const MiniData& message,const MiniData& privateKey) {return signMessage(&message,std::make_shared<MiniData>(privateKey));}
    static bool verifySignature(const MiniData* message,const SPHINCSSignature* signature,const MiniData* publicKey);
    static bool verifySignature(const MiniData& message,const SPHINCSSignature& signature,const MiniData& publicKey) {return verifySignature(&message,&signature,&publicKey);}
    static void log(const std::optional<std::string>& message);
    static void main();
};
}
