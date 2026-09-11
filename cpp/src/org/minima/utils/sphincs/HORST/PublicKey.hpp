#pragma once
#include "PrivateKey.hpp"
#include "org/minima/objects/mmr/MMR.hpp"
namespace minima::sphincs::horst {
class PublicKey {
    std::vector<std::shared_ptr<MiniData>> mPublicKeys;
    std::shared_ptr<MMR> mPublicKeyTree;
    std::shared_ptr<MMRData> mPublicKeyTreeRoot;
public:
    explicit PublicKey(const PrivateKey* privateKey);
    explicit PublicKey(const PrivateKey& privateKey):PublicKey(&privateKey){}
    int getSize() const {return int(mPublicKeys.size());}
    std::shared_ptr<MiniData> getKey(int position) const;
    std::shared_ptr<MMRData> getPublicKeyTreeRoot() const {return mPublicKeyTreeRoot;}
    MMRProof getKeyTreeProof(int position) const;
};
}
