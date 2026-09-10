#include "MMREntry.hpp"
#include <bit>
namespace minima {
bool MMREntry::checkPosition(const MMREntry& entry) const {return mRow==entry.mRow && mEntryNumber.isEqual(entry.mEntryNumber);}
bool MMREntry::checkPosition(const std::vector<MMREntry>& entries) const {for(const auto& entry:entries) if(checkPosition(entry)) return true;return false;}
json::JSONObject MMREntry::toJSON() const {json::JSONObject obj;obj.put("row",mRow);obj.put("entry",mEntryNumber.toString());obj.put("data",mMMRData?json::JSONValue(mMMRData->toJSON()):json::JSONValue(nullptr));return obj;}
int MMREntry::getParentRow() const {return std::bit_cast<int32_t>(uint32_t(mRow)+1u);}
int MMREntry::getChildRow() const {return std::bit_cast<int32_t>(uint32_t(mRow)-1u);}
bool MMREntry::isLeft() const {return mEntryNumber.modulo(MMREntryNumber(2)).isEqual(MMREntryNumber(0));}
std::vector<uint8_t> MMREntry::serialise() const {if(!mMMRData) throw std::runtime_error("Empty MMR entry");return wire::join(MiniNumber(mRow),mEntryNumber,*mMMRData);}
MMREntry MMREntry::deserialise(std::span<const uint8_t> data,size_t& offset) {
    int row=MiniNumber::deserialise(data,offset).getAsInt();auto entry=MMREntryNumber::deserialise(data,offset);auto mmr=MMRData::deserialise(data,offset);return MMREntry(row,std::move(entry),std::move(mmr));
}
}
