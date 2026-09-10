#include "Pulse.hpp"
#include "support/ByteIO.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima {
MiniNumber Pulse::PULSE_VERSION(1);
std::vector<uint8_t> Pulse::serialise() const {
    auto out=wire::join(PULSE_VERSION,MiniNumber(int64_t(blocks().size())));for(const auto& block:blocks()) wire::append(out,block.serialise());return out;
}
void Pulse::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mBlockList=std::make_shared<std::vector<MiniData>>();
    auto version=MiniNumber::deserialise(data,offset);
    if(!version.isEqual(MiniNumber::ONE)) {MinimaLogger::log("UNKNOWN PULSE Version "+version.toString());return;}
    int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i) blocks().push_back(MiniData::deserialise(data,offset));
}
Pulse Pulse::deserialise(std::span<const uint8_t> data,size_t& offset) {Pulse pulse;pulse.readDataStream(data,offset);return pulse;}
Pulse Pulse::createPulse() {
    Pulse pulse;
    sqlRequired(MinimaDB::getDB()).readLock(true);
    try {pulse.setSharedBlockList(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getSharedPulseList());}
    catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
    catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    sqlRequired(MinimaDB::getDB()).readLock(false);
    return pulse;
}
}
