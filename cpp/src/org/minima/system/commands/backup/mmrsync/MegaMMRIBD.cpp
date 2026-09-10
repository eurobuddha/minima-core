#include "MegaMMRIBD.hpp"
namespace minima {
std::vector<uint8_t> MegaMMRIBD::serialise() const {
    auto out=MiniNumber(2).serialise();
    if(!mInitialIBD)throw std::invalid_argument("Null IBD");
    wire::append(out,mInitialIBD->serialise());
    if(!mAllCoinProofs)throw std::invalid_argument("Null coin proofs");
    wire::append(out,MiniNumber(int64_t(mAllCoinProofs->size())).serialise());
    for(const auto& proof:*mAllCoinProofs){if(!proof)throw std::invalid_argument("Null coin proof");wire::append(out,proof->serialise());}
    if(!mPeersList)throw std::invalid_argument("Null peers list");
    wire::append(out,MiniString(*mPeersList).serialise());return out;
}
void MegaMMRIBD::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    (void)MiniNumber::deserialise(data,offset).getAsInt();
    mInitialIBD=std::make_shared<IBD>();mInitialIBD->readDataStream(data,offset);
    mAllCoinProofs=std::make_shared<Proofs>();
    int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i)mAllCoinProofs->push_back(std::make_shared<CoinProof>(CoinProof::deserialise(data,offset)));
    mPeersList=MiniString::deserialise(data,offset).toString();
}
MegaMMRIBD MegaMMRIBD::deserialise(std::span<const uint8_t> data,size_t& offset) {
    MegaMMRIBD value;value.readDataStream(data,offset);return value;
}
}
