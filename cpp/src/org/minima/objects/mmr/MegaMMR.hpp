#pragma once
#include <filesystem>
#include "MMR.hpp"
#include "org/minima/objects/TxBlock.hpp"
namespace minima {
class MegaMMR {
    std::shared_ptr<MMR> mMMR=std::make_shared<MMR>();
    JavaHashtable<std::shared_ptr<Coin>> mAllUnspentCoins;
    bool isPrunable(const Coin& coin) const;
    void scanUnspendable();
    void pruneUnspendable(bool scanMMR);
public:
    static void main(const std::vector<std::string>* args);
    MMR& getMMR() {return *mMMR;}
    const MMR& getMMR() const {return *mMMR;}
    std::shared_ptr<MMR> getSharedMMR() const {return mMMR;}
    auto& getAllCoins() {return mAllUnspentCoins;}
    const auto& getAllCoins() const {return mAllUnspentCoins;}
    bool isEmpty() const {return mMMR->getTotalEntries()==0;}
    void addBlock(const TxBlock& block);
    void clear();
    std::vector<uint8_t> serialise();
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static MegaMMR deserialise(std::span<const uint8_t> data,size_t& offset);
    static MMRData getCoinData(const MiniNumber& number=MiniNumber(0));
    void loadMMR(const std::filesystem::path& file);
    void saveMMR(const std::filesystem::path& file);
};
}
