#include "MegaMMRBackup.hpp"
namespace minima {
std::vector<uint8_t> MegaMMRBackup::serialise() {
    auto out=MiniNumber(1).serialise();
    if(!mMegaMMR)throw std::invalid_argument("Null MegaMMR");
    wire::append(out,mMegaMMR->serialise());
    if(!mIBD)throw std::invalid_argument("Null IBD");
    wire::append(out,mIBD->serialise());return out;
}
void MegaMMRBackup::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    (void)MiniNumber::deserialise(data,offset).getAsInt();
    mMegaMMR=std::make_shared<MegaMMR>();mMegaMMR->readDataStream(data,offset);
    mIBD=std::make_shared<IBD>(IBD::deserialise(data,offset));
}
MegaMMRBackup MegaMMRBackup::deserialise(std::span<const uint8_t> data,size_t& offset) {
    MegaMMRBackup value;value.readDataStream(data,offset);return value;
}
}
