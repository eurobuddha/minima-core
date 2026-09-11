#include "Transaction.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaHashtable.hpp"
#include "support/NativeThrowable.hpp"
#include <algorithm>
#include <set>
namespace minima {
namespace {
MiniNumber sum(const std::vector<Coin>& coins,const MiniData* token=nullptr) {
    MiniNumber total;for(const auto& coin:coins) if(!token||coin.getTokenID().isEqual(*token)) total=total.add(coin.getAmount());return total;
}
template<class T> void writeList(std::vector<uint8_t>& out,const std::vector<T>& list) {
    wire::append(out,MiniNumber(int64_t(list.size())).serialise());for(const auto& item:list) item.writeDataStream(out);
}
void sortState(std::vector<StateVariable>& state) {std::stable_sort(state.begin(),state.end(),[](const auto& a,const auto& b){return a.getPort()<b.getPort();});}
}
MiniNumber Transaction::sumInputs() const {return sum((*mData->inputs));}
MiniNumber Transaction::sumInputs(const MiniData& tokenID) const {return sum((*mData->inputs),&tokenID);}
MiniNumber Transaction::sumOutputs() const {return sum((*mData->outputs));}
MiniNumber Transaction::sumOutputs(const MiniData& tokenID) const {return sum((*mData->outputs),&tokenID);}
bool Transaction::checkValid() const {
    if((*mData->inputs).empty()) return false;
    const auto totalin=sumInputs();const auto totalout=sumOutputs();
    if(totalout.isMore(totalin)) {MinimaLogger::log("Transaction error : Inputs LESS than Outputs "+totalin.toString()+"/"+totalout.toString());return false;}
    for(const auto& coin:(*mData->inputs)) if(!coin.getAmount().isValidMinimaValue()) {MinimaLogger::log("Transaction error : Input is invalid Minima Amount "+coin.getAmount().toString());return false;}
    for(const auto& coin:(*mData->outputs)) if(!coin.getAmount().isValidMinimaValue()) {MinimaLogger::log("Transaction error : Output is invalid Minima Amount "+coin.getAmount().toString());return false;}
    std::vector<std::string> tokens;
    for(const auto& coin:(*mData->outputs)) {
        auto token=(coin.getTokenID().isEqual(Token::TOKENID_CREATE)?Token::TOKENID_MINIMA:coin.getTokenID()).to0xString();
        if(std::find(tokens.begin(),tokens.end(),token)==tokens.end())tokens.push_back(std::move(token));
    }
    JavaHashtable<MiniNumber> outamounts;
    for(const auto& token:tokens)outamounts.put(token,sumOutputs(MiniData::fromHex(token)));
    // Source Hashtable enumeration and CREATE-ID treatment are observable when validation fails.
    for(const auto& token:outamounts.keys()) {
        if(sumInputs(MiniData::fromHex(token)).isLess(*outamounts.get(token))) {MinimaLogger::log("Transaction error : Inputs LESS than Outputs");return false;}
    }
    for(size_t i=0;i<(*mData->inputs).size();++i) for(size_t j=i+1;j<(*mData->inputs).size();++j) if((*mData->inputs)[i].getCoinID().isEqual((*mData->inputs)[j].getCoinID())) return false;
    return true;
}
MiniNumber Transaction::getBurn() const {return sumInputs().sub(sumOutputs());}
void Transaction::addStateVariable(StateVariable state) {removeStateVariable(state.getPort());(*mData->state).push_back(std::move(state));sortState((*mData->state));}
void Transaction::removeStateVariable(int port) {
    auto replacement=std::make_shared<State>();bool found=false;
    for(const auto& state:*mData->state){if(state.getPort()!=port)replacement->push_back(state);else found=true;}
    if(!found)return;
    mData->state=std::move(replacement);sortState(*mData->state);
}
const StateVariable* Transaction::getStateValue(int port) const {for(const auto& state:(*mData->state)) if(state.getPort()==port) return &state;return nullptr;}
void Transaction::calculateTransactionID() const {mData->transactionID=Crypto::getInstance().hashObjectOrNull(*this);if(!mData->transactionID)throw JavaIllegalArgumentException("NULL mTransactionID!");}
MiniData Transaction::calculateCoinID(const MiniData& baseCoinID,int output) const {return Crypto::getInstance().hashObjects(baseCoinID,MiniNumber(output));}
json::JSONObject Transaction::toJSON() const {
    json::JSONObject obj;json::JSONArray ins,outs,states;
    for(const auto& coin:(*mData->inputs)) ins.add(coin.toJSON());for(const auto& coin:(*mData->outputs)) outs.add(coin.toJSON());for(const auto& state:(*mData->state)) states.add(state.toJSON());
    obj.put("inputs",ins);obj.put("outputs",outs);obj.put("state",states);obj.put("linkhash",linkHashValue().to0xString());calculateTransactionID();obj.put("transactionid",mData->transactionID?mData->transactionID->to0xString():"null");return obj;
}
int64_t Transaction::calculateStateSize() const {try {std::vector<uint8_t> out;writeList(out,(*mData->state));return int64_t(out.size());}catch(const JavaError&) {throw;}catch(const std::bad_alloc&) {throw;}catch(const std::exception& error) {MinimaLogger::log("Calcualte state size error!");MinimaLogger::log(std::make_shared<NativeThrowable>(error));return 1000000;}}
void Transaction::writeDataStream(std::vector<uint8_t>& out) const {writeList(out,(*mData->inputs));writeList(out,(*mData->outputs));writeList(out,(*mData->state));const auto link=getLinkHash();if(!link)throw JavaNullPointerException("null Transaction link hash");link->writeHashToStream(out);}
std::vector<uint8_t> Transaction::serialise() const {return Streamable::serialise();}
void Transaction::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mData->inputs=std::make_shared<Coins>();mData->outputs=std::make_shared<Coins>();mData->state=std::make_shared<State>();int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)(*mData->inputs).push_back(Coin::deserialise(data,offset));
    count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)(*mData->outputs).push_back(Coin::deserialise(data,offset));
    count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)(*mData->state).push_back(StateVariable::deserialise(data,offset));
    mData->linkHash=std::make_shared<MiniData>(MiniData::deserialiseHash(data,offset));calculateTransactionID();
}
Transaction Transaction::deserialise(std::span<const uint8_t> data,size_t& offset) {Transaction txn;txn.readDataStream(data,offset);return txn;}
}
