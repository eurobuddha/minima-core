#pragma once
#include "MMREntryNumber.hpp"
#include "MMRData.hpp"
#include <optional>
#include <memory>
namespace minima {
class MMREntry {
    MMREntryNumber mEntryNumber;
    int32_t mRow;
    std::shared_ptr<MMRData> mMMRData;
public:
    MMREntry(int row,MMREntryNumber entry):mEntryNumber(std::move(entry)),mRow(row){}
    MMREntry(int row,MMREntryNumber entry,MMRData data):mEntryNumber(std::move(entry)),mRow(row),mMMRData(std::make_shared<MMRData>(std::move(data))){}
    MMREntry(int row,MMREntryNumber entry,std::shared_ptr<MMRData> data):mEntryNumber(std::move(entry)),mRow(row),mMMRData(std::move(data)){}
    const MMREntryNumber& getEntryNumber() const {return mEntryNumber;}
    int getRow() const {return mRow;}
    const std::shared_ptr<MMRData>& getMMRData() const {return mMMRData;}
    bool isEmpty() const {return !mMMRData;}
    bool checkPosition(const MMREntry& entry) const;
    bool checkPosition(const std::vector<MMREntry>& entries) const;
    json::JSONObject toJSON() const;
    std::string toString() const {return toJSON().toJSONString();}
    int getParentRow() const;
    int getChildRow() const;
    bool isLeft() const;
    bool isRight() const {return !isLeft();}
    MMREntryNumber getLeftSibling() const {return mEntryNumber.decrement();}
    MMREntryNumber getRightSibling() const {return mEntryNumber.increment();}
    MMREntryNumber getSibling() const {return isLeft()?getRightSibling():getLeftSibling();}
    MMREntryNumber getParentEntry() const {return mEntryNumber.div2().floor();}
    MMREntryNumber getLeftChildEntry() const {return mEntryNumber.mult2();}
    MMREntryNumber getRightChildEntry() const {return getLeftChildEntry().increment();}
    std::vector<uint8_t> serialise() const;
    static MMREntry deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
