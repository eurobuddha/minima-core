#pragma once
#include "org/minima/objects/IBD.hpp"
namespace minima {
class MegaMMRIBD {
public:
    using Proofs=std::vector<std::shared_ptr<CoinProof>>;
private:
    std::shared_ptr<IBD> mInitialIBD;
    std::shared_ptr<Proofs> mAllCoinProofs;
    std::optional<std::string> mPeersList="";
public:
    MegaMMRIBD()=default;
    MegaMMRIBD(std::shared_ptr<IBD> ibd,std::shared_ptr<Proofs> proofs):mInitialIBD(std::move(ibd)),mAllCoinProofs(std::move(proofs)){}
    std::shared_ptr<IBD> getIBD() const {return mInitialIBD;}
    std::shared_ptr<Proofs> getAllCoinProofs() const {return mAllCoinProofs;}
    void setPeersList(std::optional<std::string> peers){mPeersList=std::move(peers);}
    const std::optional<std::string>& getPeersList() const {return mPeersList;}
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static MegaMMRIBD deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
