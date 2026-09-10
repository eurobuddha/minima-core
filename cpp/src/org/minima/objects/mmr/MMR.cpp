#include "MMR.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
namespace minima {
std::function<std::shared_ptr<MMR>()> MMR::megaMMRLookup;
std::string MMR::key(int row,const MMREntryNumber& entry) {return std::to_string(row)+":"+entry.toString();}
MMR::Entry MMR::localEntry(const std::string& name) const {auto entry=mEntries.get(name);return entry?*entry:nullptr;}
void MMR::put(Entry entry) {auto name=key(entry->getRow(),entry->getEntryNumber());mEntries.put(std::move(name),std::move(entry));}
void MMR::remove(const Entry& entry) {mEntries.remove(key(entry->getRow(),entry->getEntryNumber()));}
std::vector<MMR::Entry> MMR::getAllEntries() const {return mEntries.elements();}
MMR::MMR(std::shared_ptr<MMR> parent) {
    if(!parent) return;
    setParent(parent);if(!parent->isFinalized()) parent->finalizeSet();mBlockTime=parent->getBlockTime().increment();
    BigInt total;for(const auto& peak:parent->getPeaks()) {setEntry(peak->getRow(),peak->getEntryNumber(),peak->getMMRData());BigInt count(1);for(int i=0;i<peak->getRow();++i) count=count*BigInt(2);total=total+count;}
    mEntryNumber=MMREntryNumber(total);
}
void MMR::calculateEntryNumberFromPeaks() {BigInt total;for(const auto& peak:getPeaks()) {setEntry(peak->getRow(),peak->getEntryNumber(),peak->getMMRData());BigInt count(1);for(int i=0;i<peak->getRow();++i) count=count*BigInt(2);total=total+count;}mEntryNumber=MMREntryNumber(total);}
json::JSONObject MMR::toJSON(bool entries) const {
    json::JSONObject obj;obj.put("block",json::JSONValue::raw(mBlockTime.toString()));obj.put("entrynumber",json::JSONValue::raw(mEntryNumber.toString()));obj.put("size",getTotalEntries());
    if(entries) {json::JSONArray arr;for(const auto& entry:getAllEntries()) arr.add(entry->toJSON());obj.put("entries",arr);}
    obj.put("maxrow",mMaxRow);json::JSONArray maxima;for(const auto& entry:mMaxEntries) if(entry) maxima.add(entry->toJSON());obj.put("maxentries",maxima);
    auto root=getRoot();obj.put("root",root?json::JSONValue(root->toJSON()):json::JSONValue(nullptr));return obj;
}
void MMR::finalizeSet() {mFinalized=false;mFinalizedPeaks=getPeaks();mFinalizedRoot=getRoot();mFinalized=true;}
void MMR::clearParent() {mParent.reset();mUseMegaMMR=GeneralParams::IS_MEGAMMR;}
void MMR::setParent(std::shared_ptr<MMR> parent) {mParent=std::move(parent);mUseMegaMMR=GeneralParams::IS_MEGAMMR;}
MMR::Entry MMR::setEntry(int row,MMREntryNumber number,Data data) {
    if(mFinalized) return nullptr;
    if(row<0||row>=256) throw std::out_of_range("MMR row outside 0..255");
    mMaxRow=std::max(mMaxRow,row);auto entry=std::make_shared<MMREntry>(row,std::move(number),std::move(data));put(entry);
    auto& maximum=mMaxEntries[row];if(!maximum||maximum->getEntryNumber().isLessEqual(entry->getEntryNumber())) maximum=entry;return entry;
}
MMR::Entry MMR::getEntry(int row,const MMREntryNumber& number,const MiniNumber& maxBack) const {
    auto name=key(row,number);const MMR* current=this;bool megaCheck=false;std::shared_ptr<MMR> megaOwner;
    while(current) {
        if(current->getBlockTime().isLess(maxBack)) break;
        auto entry=current->localEntry(name);if(entry) return entry;
        if(megaCheck) break;
        current=current->mParent.get();
        if(!current && mUseMegaMMR) {
            if(!megaMMRLookup) throw std::logic_error("MegaMMR database lookup is not configured");
            megaCheck=true;megaOwner=megaMMRLookup();current=megaOwner.get();
        }
    }
    return std::make_shared<MMREntry>(row,number);
}
MMR::Entry MMR::addEntry(Data data) {
    auto entry=setEntry(0,mEntryNumber,std::move(data));auto result=entry;mEntryNumber=mEntryNumber.increment();
    if(!entry) throw std::logic_error("Cannot add to finalized MMR");
    while(entry->isRight()) {
        auto sibling=getEntry(entry->getRow(),entry->getSibling());
        if(sibling->isEmpty()) throw std::logic_error("Missing MMR sibling");
        auto parent=MMRData::CreateMMRDataParentNode(*sibling->getMMRData(),*entry->getMMRData());entry=setEntry(entry->getParentRow(),entry->getParentEntry(),std::move(parent));
    }
    return result;
}
void MMR::updateEntry(const MMREntryNumber& number,const MMRProof& oldProof,Data newData) {
    auto peaks=getPeaks();size_t proofIndex=0;auto entry=setEntry(0,number,std::move(newData));
    if(!entry) throw std::logic_error("Cannot update finalized MMR");
    while(!std::any_of(peaks.begin(),peaks.end(),[&](const auto& p){return entry->checkPosition(*p);})) {
        auto sibling=getEntry(entry->getRow(),entry->getSibling(),oldProof.getBlockTime());
        auto data=sibling->isEmpty()?std::make_shared<MMRData>(oldProof.getProofChunk(proofIndex).getMMRData()):sibling->getMMRData();++proofIndex;
        sibling=setEntry(sibling->getRow(),sibling->getEntryNumber(),std::move(data));
        auto parent=entry->isLeft()?MMRData::CreateMMRDataParentNode(*entry->getMMRData(),*sibling->getMMRData()):MMRData::CreateMMRDataParentNode(*sibling->getMMRData(),*entry->getMMRData());
        entry=setEntry(entry->getParentRow(),entry->getParentEntry(),std::move(parent));
    }
}
MMRProof MMR::getProofToPeak(const MMREntryNumber& number) const {
    auto entry=getEntry(0,number);MMRProof proof(mBlockTime);auto sibling=getEntry(entry->getRow(),entry->getSibling());
    while(!sibling->isEmpty()) {
        proof.addProofChunk(sibling->isLeft(),*sibling->getMMRData());MMREntry parent(sibling->getParentRow(),sibling->getParentEntry());sibling=getEntry(parent.getRow(),parent.getSibling());
    }
    return proof;
}
MMRProof MMR::getPeakToRoot(const MMRData& peak) const {
    MMRProof total(mBlockTime);auto peaks=getPeaks();auto currentPeak=peak;
    while(peaks.size()>1) {
        MMR tree;Entry keeper;
        for(const auto& p:peaks) {auto current=tree.addEntry(p->getMMRData());if(p->getMMRData()->getData().isEqual(currentPeak.getData())) keeper=current;}
        if(!keeper) throw std::logic_error("MMR peak not found");
        auto proof=tree.getProofToPeak(keeper->getEntryNumber());for(int i=0;i<proof.getProofLength();++i) total.addProofChunk(proof.getProofChunk(i));
        currentPeak=total.calculateProof(peak);peaks=tree.getPeaks();
    }
    return total;
}
MMRProof MMR::getProof(const MMREntryNumber& number) const {
    auto entry=getEntry(0,number);if(entry->isEmpty()) throw std::logic_error("Missing MMR leaf");
    auto proof=getProofToPeak(number);auto peak=proof.calculateProof(*entry->getMMRData());auto rootProof=getPeakToRoot(peak);
    for(int i=0;i<rootProof.getProofLength();++i) proof.addProofChunk(rootProof.getProofChunk(i));return proof;
}
std::vector<MMR::Entry> MMR::getPeaks() const {
    if(mFinalized) return mFinalizedPeaks;
    std::vector<Entry> peaks;for(int row=mMaxRow;row>=0;--row) if(mMaxEntries[row] && mMaxEntries[row]->isLeft()) peaks.push_back(mMaxEntries[row]);return peaks;
}
MMR::Data MMR::getRoot() const {
    if(mFinalized) return mFinalizedRoot;
    auto peaks=getPeaks();if(peaks.empty()) return nullptr;
    while(peaks.size()>1) {MMR tree;for(const auto& peak:peaks) tree.addEntry(MMRData(peak->getMMRData()->getData(),peak->getMMRData()->getValue()));peaks=tree.getPeaks();}
    return peaks.front()->getMMRData();
}
bool MMR::checkProof(const MMRData& data,const MMRProof& proof) const {
    auto value=proof.calculateProof(data);auto root=getRoot();if(!root) throw std::logic_error("Cannot check proof against empty MMR");if(value.isEqual(*root)) return true;
    for(const auto& peak:getPeaks()) if(value.isEqual(*peak->getMMRData())) return true;return false;
}
const MMR* MMR::getParentAtTime(const MiniNumber& time) const {const MMR* current=this;while(current) {if(current->mBlockTime.isEqual(time)) return current;if(current->mBlockTime.isLess(time)) return nullptr;current=current->mParent.get();}return nullptr;}
bool MMR::checkProofTimeValid(const MMREntryNumber& number,const MMRData& data,const MMRProof& proof) const {
    auto mmr=getParentAtTime(proof.getBlockTime());if(!mmr||!mmr->checkProof(data,proof)) return false;auto checker=getEntry(0,number,proof.getBlockTime());return checker->isEmpty()||checker->getMMRData()->isEqual(data);
}
std::vector<uint8_t> MMR::serialise() const {auto out=wire::join(mBlockTime,mEntryNumber,MiniNumber(getTotalEntries()));for(const auto& entry:getAllEntries()) wire::append(out,entry->serialise());return out;}
MMR MMR::deepCopy() const {auto data=serialise();size_t offset=0;return deserialise(data,offset);}
MMR MMR::deserialise(std::span<const uint8_t> data,size_t& offset) {
    MMR mmr;mmr.readDataStream(data,offset);return mmr;
}
void MMR::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mBlockTime=MiniNumber::deserialise(data,offset);
    mEntryNumber=MMREntryNumber::deserialise(data,offset);
    mEntries=JavaHashtable<Entry>();mMaxEntries={};mMaxRow=0;
    int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i) {auto entry=MMREntry::deserialise(data,offset);setEntry(entry.getRow(),entry.getEntryNumber(),entry.getMMRData());}
    finalizeSet();
}
void MMR::prune(const Entry& entry) {
    if(entry->isEmpty()||entry->getChildRow()<0) return;
    auto left=getEntry(entry->getChildRow(),entry->getLeftChildEntry()),right=getEntry(entry->getChildRow(),entry->getRightChildEntry());prune(left);prune(right);
    if(entry->getMMRData()->getValue().isZero()) {remove(left);remove(right);}
}
void MMR::pruneTree() {for(const auto& peak:getPeaks()) prune(peak);}
bool MMR::scanUnspendable(const Entry& entry) {
    if(entry->isEmpty()) return true;if(entry->getChildRow()<0) return entry->getMMRData()->isUnspendable();
    auto left=getEntry(entry->getChildRow(),entry->getLeftChildEntry()),right=getEntry(entry->getChildRow(),entry->getRightChildEntry());
    bool leftUnspendable=scanUnspendable(left),rightUnspendable=scanUnspendable(right);
    bool leftZero=left->isEmpty()||left->getMMRData()->getValue().isZero(),rightZero=right->isEmpty()||right->getMMRData()->getValue().isZero();
    bool unspendable=(leftUnspendable||leftZero)&&(rightUnspendable||rightZero);entry->getMMRData()->setUnspendable(unspendable);
    if(unspendable) {remove(left);remove(right);if(entry->getChildRow()==0) {if(!left->isEmpty()) mPrunedCoins.insert(left->getEntryNumber().toString());if(!right->isEmpty()) mPrunedCoins.insert(right->getEntryNumber().toString());}}
    return unspendable;
}
void MMR::scanUnspendableTree() {mPrunedCoins.clear();for(const auto& peak:getPeaks()) scanUnspendable(peak);}
}
