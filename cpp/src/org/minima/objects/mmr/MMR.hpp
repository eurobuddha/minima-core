#pragma once
#include "MMREntry.hpp"
#include "MMRProof.hpp"
#include <array>
#include <functional>
#include <set>
#include "support/JavaHashtable.hpp"
namespace minima {
class MMR {
public:
    using Entry=std::shared_ptr<MMREntry>;
    using Data=std::shared_ptr<MMRData>;
    // The database adapter supplies the MegaMMR during node integration.
    static std::function<std::shared_ptr<MMR>()> megaMMRLookup;
private:
    JavaHashtable<Entry> mEntries;
    MiniNumber mBlockTime;
    std::shared_ptr<MMR> mParent;
    bool mUseMegaMMR=false;
    MMREntryNumber mEntryNumber;
    int mMaxRow=0;
    std::array<Entry,256> mMaxEntries{};
    bool mFinalized=false;
    Data mFinalizedRoot;
    std::vector<Entry> mFinalizedPeaks;
    std::set<std::string> mPrunedCoins;
    static std::string key(int row,const MMREntryNumber& entry);
    Entry localEntry(const std::string& key) const;
    void put(Entry entry);
    void remove(const Entry& entry);
    MMRProof getPeakToRoot(const MMRData& peak) const;
    void prune(const Entry& entry);
    bool scanUnspendable(const Entry& entry);
public:
    MMR()=default;
    explicit MMR(std::shared_ptr<MMR> parent);
    void calculateEntryNumberFromPeaks();
    json::JSONObject toJSON(bool entries=true) const;
    void finalizeSet();
    void setFinalized(bool finalized) {mFinalized=finalized;}
    bool isFinalized() const {return mFinalized;}
    void setBlockTime(MiniNumber time) {mBlockTime=std::move(time);}
    const MiniNumber& getBlockTime() const {return mBlockTime;}
    const MMREntryNumber& getEntryNumber() const {return mEntryNumber;}
    void clearParent();
    void setParent(std::shared_ptr<MMR> parent);
    const std::shared_ptr<MMR>& getParent() const {return mParent;}
    int getTotalEntries() const {return int(mEntries.size());}
    std::vector<Entry> getAllEntries() const;
    Entry setEntry(int row,MMREntryNumber entry,Data data);
    Entry setEntry(int row,MMREntryNumber entry,MMRData data) {return setEntry(row,std::move(entry),std::make_shared<MMRData>(std::move(data)));}
    Entry getEntry(int row,const MMREntryNumber& entry,const MiniNumber& maxBack=MiniNumber(0)) const;
    Entry addEntry(Data data);
    Entry addEntry(MMRData data) {return addEntry(std::make_shared<MMRData>(std::move(data)));}
    void updateEntry(const MMREntryNumber& entry,const MMRProof& oldProof,Data newData);
    void updateEntry(const MMREntryNumber& entry,const MMRProof& oldProof,MMRData data) {updateEntry(entry,oldProof,std::make_shared<MMRData>(std::move(data)));}
    MMRProof getProof(const MMREntryNumber& entry) const;
    MMRProof getProofToPeak(const MMREntryNumber& entry) const;
    bool checkProof(const MMRData& data,const MMRProof& proof) const;
    bool checkProofTimeValid(const MMREntryNumber& entry,const MMRData& data,const MMRProof& proof) const;
    std::vector<Entry> getPeaks() const;
    Data getRoot() const;
    const MMR* getParentAtTime(const MiniNumber& time) const;
    std::vector<uint8_t> serialise() const;
    MMR deepCopy() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static MMR deserialise(std::span<const uint8_t> data,size_t& offset);
    void pruneTree();
    const std::set<std::string>& getPrunedUnspendableCoins() const {return mPrunedCoins;}
    void scanUnspendableTree();
};
}
