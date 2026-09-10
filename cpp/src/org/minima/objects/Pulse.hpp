#pragma once
#include "base/MiniData.hpp"
#include "base/MiniNumber.hpp"
#include "support/JavaExceptions.hpp"
#include <memory>
namespace minima {
class Pulse {
    std::shared_ptr<std::vector<MiniData>> mBlockList=std::make_shared<std::vector<MiniData>>();
    auto& blocks() const {if(!mBlockList)throw JavaNullPointerException("null pulse list");return *mBlockList;}
public:
    static MiniNumber PULSE_VERSION;
    void setBlockList(std::vector<MiniData> blocks) {mBlockList=std::make_shared<std::vector<MiniData>>(std::move(blocks));}
    void setSharedBlockList(std::shared_ptr<std::vector<MiniData>> blocks) {mBlockList=std::move(blocks);}
    std::shared_ptr<std::vector<MiniData>> getSharedBlockList() const {return mBlockList;}
    auto& getBlockList() {return blocks();}
    const auto& getBlockList() const {return blocks();}
    static Pulse createPulse();
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    std::vector<uint8_t> serialise() const;
    static Pulse deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
