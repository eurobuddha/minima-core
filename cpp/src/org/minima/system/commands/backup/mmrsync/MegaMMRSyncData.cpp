#include "MegaMMRSyncData.hpp"
#include "support/ByteIO.hpp"
namespace minima {
std::vector<uint8_t> MegaMMRSyncData::serialise() const {
    auto out=MiniNumber(1).serialise();
    for(const auto& list:{mAllAddresses,mAllPublicKeys}){
        if(!list)throw std::invalid_argument("Null sync data list");
        wire::append(out,MiniNumber(int64_t(list->size())).serialise());
        for(const auto& value:*list){if(!value)throw std::invalid_argument("Null sync data");wire::append(out,value->serialise());}
    }
    return out;
}
void MegaMMRSyncData::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    (void)MiniNumber::deserialise(data,offset).getAsInt();
    for(auto* list:{&mAllAddresses,&mAllPublicKeys}){
        *list=std::make_shared<DataList>();int count=MiniNumber::deserialise(data,offset).getAsInt();
        for(int i=0;i<count;++i)(*list)->push_back(std::make_shared<MiniData>(MiniData::deserialise(data,offset)));
    }
}
MegaMMRSyncData MegaMMRSyncData::deserialise(std::span<const uint8_t> data,size_t& offset) {
    MegaMMRSyncData value;value.readDataStream(data,offset);return value;
}
}
