#include "Transaction.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaHashtable.hpp"
#include <algorithm>
#include <set>
namespace minima {
namespace {
MiniNumber sum(const std::vector<Coin>& coins,const MiniData* token=nullptr) {
    MiniNumber total;for(const auto& coin:coins) if(!token||coin.getTokenID().isEqual(*token)) total=total.add(coin.getAmount());return total;
}
template<class T> void writeList(std::vector<uint8_t>& out,const std::vector<T>& list) {
    wire::append(out,MiniNumber(int64_t(list.size())).serialise());for(const auto& item:list) wire::append(out,item.serialise());
}
void sortState(std::vector<StateVariable>& state) {std::stable_sort(state.begin(),state.end(),[](const auto& a,const auto& b){return a.getPort()<b.getPort();});}
}
MiniNumber Transaction::sumInputs() const {return sum(mInputs);}
MiniNumber Transaction::sumInputs(const MiniData& tokenID) const {return sum(mInputs,&tokenID);}
MiniNumber Transaction::sumOutputs() const {return sum(mOutputs);}
MiniNumber Transaction::sumOutputs(const MiniData& tokenID) const {return sum(mOutputs,&tokenID);}
bool Transaction::checkValid() const {
    if(mInputs.empty()) return false;
    const auto totalin=sumInputs();const auto totalout=sumOutputs();
    if(totalout.isMore(totalin)) {MinimaLogger::log("Transaction error : Inputs LESS than Outputs "+totalin.toString()+"/"+totalout.toString());return false;}
    for(const auto& coin:mInputs) if(!coin.getAmount().isValidMinimaValue()) {MinimaLogger::log("Transaction error : Input is invalid Minima Amount "+coin.getAmount().toString());return false;}
    for(const auto& coin:mOutputs) if(!coin.getAmount().isValidMinimaValue()) {MinimaLogger::log("Transaction error : Output is invalid Minima Amount "+coin.getAmount().toString());return false;}
    std::vector<std::string> tokens;
    for(const auto& coin:mOutputs) {
        auto token=(coin.getTokenID().isEqual(Token::TOKENID_CREATE)?Token::TOKENID_MINIMA:coin.getTokenID()).to0xString();
        if(std::find(tokens.begin(),tokens.end(),token)==tokens.end())tokens.push_back(std::move(token));
    }
    JavaHashtable<MiniNumber> outamounts;
    for(const auto& token:tokens)outamounts.put(token,sumOutputs(MiniData::fromHex(token)));
    // Source Hashtable enumeration and CREATE-ID treatment are observable when validation fails.
    for(const auto& token:outamounts.keys()) {
        if(sumInputs(MiniData::fromHex(token)).isLess(*outamounts.get(token))) {MinimaLogger::log("Transaction error : Inputs LESS than Outputs");return false;}
    }
    for(size_t i=0;i<mInputs.size();++i) for(size_t j=i+1;j<mInputs.size();++j) if(mInputs[i].getCoinID().isEqual(mInputs[j].getCoinID())) return false;
    return true;
}
MiniNumber Transaction::getBurn() const {return sumInputs().sub(sumOutputs());}
void Transaction::addStateVariable(StateVariable state) {removeStateVariable(state.getPort());mState.push_back(std::move(state));sortState(mState);}
void Transaction::removeStateVariable(int port) {
    // Reuse the native erase/remove pattern; Java additionally sorts after a removal.
    auto start=std::remove_if(mState.begin(),mState.end(),[port](const auto& state){return state.getPort()==port;});
    if(start==mState.end()) return;
    mState.erase(start,mState.end());sortState(mState);
}
const StateVariable* Transaction::getStateValue(int port) const {for(const auto& state:mState) if(state.getPort()==port) return &state;return nullptr;}
void Transaction::calculateTransactionID() const {mTransactionID=Crypto::getInstance().hashObject(*this);}
MiniData Transaction::calculateCoinID(const MiniData& baseCoinID,int output) const {return Crypto::getInstance().hashObjects(baseCoinID,MiniNumber(output));}
json::JSONObject Transaction::toJSON() const {
    json::JSONObject obj;json::JSONArray ins,outs,states;
    for(const auto& coin:mInputs) ins.add(coin.toJSON());for(const auto& coin:mOutputs) outs.add(coin.toJSON());for(const auto& state:mState) states.add(state.toJSON());
    obj.put("inputs",ins);obj.put("outputs",outs);obj.put("state",states);obj.put("linkhash",mLinkHash.to0xString());calculateTransactionID();obj.put("transactionid",mTransactionID.to0xString());return obj;
}
int64_t Transaction::calculateStateSize() const {try {std::vector<uint8_t> out;writeList(out,mState);return int64_t(out.size());}catch(const std::exception&) {return 1000000;}}
std::vector<uint8_t> Transaction::serialise() const {std::vector<uint8_t> out;writeList(out,mInputs);writeList(out,mOutputs);writeList(out,mState);wire::append(out,mLinkHash.serialiseHash());return out;}
void Transaction::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mInputs.clear();mOutputs.clear();mState.clear();int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mInputs.push_back(Coin::deserialise(data,offset));
    count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mOutputs.push_back(Coin::deserialise(data,offset));
    count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mState.push_back(StateVariable::deserialise(data,offset));
    mLinkHash=MiniData::deserialiseHash(data,offset);calculateTransactionID();
}
Transaction Transaction::deserialise(std::span<const uint8_t> data,size_t& offset) {Transaction txn;txn.readDataStream(data,offset);return txn;}
}
