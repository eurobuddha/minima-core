#include "MMREntry.hpp"
#include <bit>
#include "support/JavaExceptions.hpp"
namespace minima {
bool MMREntry::checkPosition(const MMREntry& entry) const {return mRow==entry.mRow && mEntryNumber.isEqual(entry.mEntryNumber);}
bool MMREntry::checkPosition(const std::vector<MMREntry>& entries) const {for(const auto& entry:entries) if(checkPosition(entry)) return true;return false;}
json::JSONObject MMREntry::toJSON() const {json::JSONObject obj;obj.put("row",mRow);obj.put("entry",mEntryNumber.toString());obj.put("data",mMMRData?json::JSONValue(mMMRData->toJSON()):json::JSONValue(nullptr));return obj;}
int MMREntry::getParentRow() const {return std::bit_cast<int32_t>(uint32_t(mRow)+1u);}
int MMREntry::getChildRow() const {return std::bit_cast<int32_t>(uint32_t(mRow)-1u);}
bool MMREntry::isLeft() const {return mEntryNumber.modulo(MMREntryNumber(2)).isEqual(MMREntryNumber(0));}
void MMREntry::writeDataStream(std::vector<uint8_t>& output) const {wire::append(output,MiniNumber(mRow).serialise());mEntryNumber.writeDataStream(output);if(!mMMRData)throw JavaNullPointerException("null MMR data");mMMRData->writeDataStream(output);}
std::vector<uint8_t> MMREntry::serialise() const {return Streamable::serialise();}
void MMREntry::readDataStream(std::span<const uint8_t> data,size_t& offset){mIsEmpty=false;mRow=MiniNumber::deserialise(data,offset).getAsInt();mEntryNumber=MMREntryNumber::deserialise(data,offset);mMMRData=std::make_shared<MMRData>(MMRData::deserialise(data,offset));}
MMREntry MMREntry::deserialise(std::span<const uint8_t> data,size_t& offset) {
    int row=MiniNumber::deserialise(data,offset).getAsInt();auto entry=MMREntryNumber::deserialise(data,offset);auto mmr=MMRData::deserialise(data,offset);return MMREntry(row,std::move(entry),std::move(mmr));
}
}
