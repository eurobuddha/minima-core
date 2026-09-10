#include "Coin.hpp"
#include "Address.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/ByteIO.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
const MiniData Coin::COINID_OUTPUT=MiniData::fromHex("0x00"),Coin::COINID_ELTOO=MiniData::fromHex("0x01");
Coin::Coin(MiniData address,MiniNumber amount,MiniData tokenID,bool storeState)
:Coin(MiniData::fromHex("0x00"),std::move(address),std::move(amount),std::move(tokenID),storeState){}
Coin::Coin(MiniData coinID,MiniData address,MiniNumber amount,MiniData tokenID,bool storeState)
{
    fields().mCoinID=std::move(coinID);fields().mAddress=std::move(address);fields().mAmount=std::move(amount);
    fields().mTokenID=std::move(tokenID);fields().mStoreState=storeState;
}
Coin Coin::getSameCoinWithCoinID(MiniData coinID) const {auto coin=deepCopy();coin.resetCoinID(std::move(coinID));return coin;}
MiniNumber Coin::getTokenAmount() const {return fields().mToken?fields().mToken->getScaledTokenAmount(fields().mAmount):fields().mAmount;}
bool Coin::checkForStateVariable(const std::string& value,bool wildcard) const {
    for(const auto& state:states()) {auto text=state.toString();if(wildcard?text.find(value)!=std::string::npos:text==value) return true;}return false;
}
json::JSONObject Coin::toJSON(bool simpleState) const {
    json::JSONObject obj;obj.put("coinid",fields().mCoinID.to0xString());obj.put("amount",fields().mAmount.toString());obj.put("address",fields().mAddress.to0xString());
    obj.put("miniaddress",Address::makeMinimaAddress(fields().mAddress));obj.put("tokenid",fields().mTokenID.to0xString());
    if(fields().mToken) {obj.put("token",fields().mToken->toJSON());obj.put("tokenamount",getTokenAmount().toString());}else obj.put("token",nullptr);
    obj.put("storestate",fields().mStoreState);
    if(simpleState) obj.put("state",getStateAsJSON());else {json::JSONArray arr;for(const auto& state:states()) arr.add(state.toJSON());obj.put("state",arr);}
    obj.put("spent",getSpent());obj.put("mmrentry",fields().mMMREntryNumber.toString());obj.put("created",fields().mBlockCreated.toString());return obj;
}
json::JSONObject Coin::convertStateListToJSON(const std::vector<StateVariable>& states) {json::JSONObject obj;for(const auto& state:states) obj.put(std::to_string(state.getPort()),state.toString());return obj;}
json::JSONObject Coin::getStateAsJSON() const {return convertStateListToJSON(states());}
std::optional<Coin> Coin::convertMiniDataVersion(const MiniData& data) {
    return convertMiniDataVersion(data,[](const wire::IOException& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));});
}
std::optional<Coin> Coin::convertMiniDataVersion(const MiniData& data,
    const std::function<void(const wire::IOException&)>& onError) {
    try {size_t offset=0;return deserialise(data.bytes(),offset);}
    catch(const wire::IOException& error) {onError(error);return std::nullopt;}
}
Coin Coin::deepCopy() const {
    try {auto bytes=serialise();size_t offset=0;return deserialise(bytes,offset);}
    catch(const wire::IOException& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));return Coin(nullptr);}
}
std::vector<uint8_t> Coin::serialise() const {
    auto out=fields().mCoinID.serialiseHash();wire::append(out,fields().mAddress.serialiseHash());wire::append(out,fields().mAmount.serialise());wire::append(out,fields().mTokenID.serialiseHash());
    wire::append(out,wire::join(MiniByte(fields().mStoreState),fields().mMMREntryNumber,fields().mSpent,fields().mBlockCreated,MiniNumber(int64_t(states().size()))));
    for(const auto& state:states()) wire::append(out,state.serialise());
    wire::append(out,MiniByte(fields().mToken.has_value()).serialise());if(fields().mToken) wire::append(out,fields().mToken->serialise());return out;
}
void Coin::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    (void)fields(); // A null receiver fails before consuming input.
    fields().mCoinID=MiniData::deserialiseHash(data,offset);
    fields().mAddress=MiniData::deserialiseHash(data,offset);
    fields().mAmount=MiniNumber::deserialise(data,offset);
    fields().mTokenID=MiniData::deserialiseHash(data,offset);
    fields().mStoreState=MiniByte::deserialise(data,offset).isTrue();
    fields().mMMREntryNumber=MMREntryNumber::deserialise(data,offset);
    fields().mSpent=MiniByte::deserialise(data,offset);
    fields().mBlockCreated=MiniNumber::deserialise(data,offset);
    fields().mState=std::make_shared<States>();
    int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i)states().push_back(StateVariable::deserialise(data,offset));
    fields().mToken.reset();
    if(MiniByte::deserialise(data,offset).isTrue())fields().mToken=Token::deserialise(data,offset);
}
Coin Coin::deserialise(std::span<const uint8_t> data,size_t& offset) {
    Coin coin;coin.readDataStream(data,offset);return coin;
}
}
