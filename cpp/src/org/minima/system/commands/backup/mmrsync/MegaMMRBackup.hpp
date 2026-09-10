#pragma once
#include "org/minima/objects/IBD.hpp"
#include "org/minima/objects/mmr/MegaMMR.hpp"
namespace minima {
class MegaMMRBackup {
    std::shared_ptr<MegaMMR> mMegaMMR;
    std::shared_ptr<IBD> mIBD;
public:
    MegaMMRBackup()=default;
    MegaMMRBackup(std::shared_ptr<MegaMMR> mmr,std::shared_ptr<IBD> ibd):mMegaMMR(std::move(mmr)),mIBD(std::move(ibd)){}
    std::shared_ptr<MegaMMR> getMegaMMR() const {return mMegaMMR;}
    std::shared_ptr<IBD> getIBD() const {return mIBD;}
    std::vector<uint8_t> serialise();
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static MegaMMRBackup deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
