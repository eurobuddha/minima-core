#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include <memory>
namespace minima {
class MegaMMRSyncData {
public:
    using DataList=std::vector<std::shared_ptr<MiniData>>;
private:
    std::shared_ptr<DataList> mAllAddresses,mAllPublicKeys;
public:
    MegaMMRSyncData()=default;
    MegaMMRSyncData(std::shared_ptr<DataList> addresses,std::shared_ptr<DataList> keys):mAllAddresses(std::move(addresses)),mAllPublicKeys(std::move(keys)){}
    std::shared_ptr<DataList> getAllAddresses() const {return mAllAddresses;}
    std::shared_ptr<DataList> getAllPublicKeys() const {return mAllPublicKeys;}
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static MegaMMRSyncData deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
