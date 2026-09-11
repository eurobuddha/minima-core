#pragma once
#include "PrivateKey.hpp"
#include "PublicKey.hpp"
#include "HORSTSignature.hpp"
#include <optional>
namespace minima::sphincs::horst {
class HORST {
    std::shared_ptr<PrivateKey> mPrivateKey;
    std::shared_ptr<PublicKey> mPublicKey;
public:
    static constexpr int NUMBER_OF_CHUNK_BYTES=16;
    explicit HORST(const MiniData* seed);
    explicit HORST(const MiniData& seed):HORST(&seed){}
    std::shared_ptr<MMRData> getHORSTRoot() const {return mPublicKey->getPublicKeyTreeRoot();}
    std::shared_ptr<PublicKey> getPublicKey() const {return mPublicKey;}
    std::shared_ptr<PrivateKey> getPrivateKey() const {return mPrivateKey;}
    HORSTSignature signMessage(const MiniData* message) const;
    HORSTSignature signMessage(const MiniData& message) const {return signMessage(&message);}
    static bool verifySignature(const MiniData* message,const HORSTSignature* signature,const MMRData* root);
    static bool verifySignature(const MiniData& message,const HORSTSignature& signature,const MMRData& root) {return verifySignature(&message,&signature,&root);}
    static int getKeyRef(int position,const MiniData* original);
    static int getKeyRef(int position,const MiniData& original) {return getKeyRef(position,&original);}
    static void log(const std::optional<std::string>& message);
    static void main();
};
}
