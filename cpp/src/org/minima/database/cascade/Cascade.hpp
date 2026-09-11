#pragma once
#include <filesystem>
#include "CascadeNode.hpp"
namespace minima {
class Cascade {
    std::shared_ptr<CascadeNode> mTip;
    MMREntryNumber mTotalWeight;
public:
    static void main(const std::vector<std::string>* args);
    void addToTip(const TxPoW& txpow);
    std::shared_ptr<CascadeNode> getTip() const {return mTip;}
    const MMREntryNumber& getTotalWeight() const {return mTotalWeight;}
    int32_t getLength() const;
    void cascadeChain();
    Cascade deepCopy() const;
    std::string printCascade() const;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static Cascade deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::optional<Cascade> convertMiniDataVersion(const MiniData& data);
    static std::optional<Cascade> convertMiniDataVersion(const MiniData& data,const std::function<void(const wire::IOException&)>& log);
    static bool checkCascadeCorrect(const Cascade& cascade);
    static bool checkCascadeCorrect(const Cascade& cascade,const std::function<void(const std::string&)>& log);
    static bool checkPastNodeExists(std::shared_ptr<CascadeNode> node,const std::string& id);
    void loadDB(const std::filesystem::path& file);
    void saveDB(const std::filesystem::path& file);
};
}
