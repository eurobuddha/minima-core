#include "Token.hpp"
#include "org/minima/utils/Crypto.hpp"
#include <bit>
namespace minima {
const MiniData Token::TOKENID_CREATE=MiniData::fromHex("0xFF"),Token::TOKENID_MINIMA=MiniData::fromHex("0x00");
Token::Token(MiniData coinID,MiniNumber scale,MiniNumber amount,MiniString name,MiniString script,MiniNumber created)
:mCoinID(std::move(coinID)),mTokenScale(std::move(scale)),mTokenMinimaAmount(std::move(amount)),mTokenName(std::move(name)),mTokenScript(std::move(script)),mTokenCreated(std::move(created)) {calculateTokenID();}
void Token::calculateTokenID() {
    try {mTokenID=Crypto::getInstance().hashObject(MiniData(serialise()));}
    catch(const std::runtime_error&) {mTokenID.reset();}
}
MiniNumber Token::getScaledTokenAmount(MiniNumber amount) const {for(int i=0;i<mTokenScale.getAsInt();++i) amount=amount.mult(MiniNumber::TEN);return amount;}
MiniNumber Token::getScaledMinimaAmount(MiniNumber amount) const {for(int i=0;i<mTokenScale.getAsInt();++i) amount=amount.div(MiniNumber::TEN);return amount;}
MiniNumber Token::getDecimalPlaces() const {return MiniNumber(std::bit_cast<int32_t>(uint32_t(MiniNumber::MAX_DECIMAL_PLACES)-uint32_t(mTokenScale.getAsInt())));}
json::JSONObject Token::toJSON() const {
    json::JSONObject obj;auto name=mTokenName.toString();auto start=name.find_first_not_of(" \t\r\n\f\v");
    // Java stores a MiniString object verbatim when the name begins with '{'.
    if(start!=std::string::npos && name[start]=='{') obj.put("name",json::JSONValue::raw(name));else obj.put("name",name);
    obj.put("coinid",mCoinID.to0xString());obj.put("total",getTotalTokens().toString());
    obj.put("decimals",json::JSONValue::raw(getDecimalPlaces().toString()));obj.put("script",mTokenScript.toString());
    obj.put("totalamount",mTokenMinimaAmount.toString());obj.put("scale",mTokenScale.toString());obj.put("created",mTokenCreated.toString());
    obj.put("tokenid",mTokenID?json::JSONValue(mTokenID->to0xString()):json::JSONValue(nullptr));return obj;
}
std::vector<uint8_t> Token::serialise() const {auto out=mCoinID.serialiseHash();wire::append(out,wire::join(mTokenScript,mTokenScale,mTokenMinimaAmount,mTokenName,mTokenCreated));return out;}
Token Token::deserialise(std::span<const uint8_t> data,size_t& offset) {
    Token token;token.mCoinID=MiniData::deserialiseHash(data,offset);token.mTokenScript=MiniString::deserialise(data,offset);
    token.mTokenScale=MiniNumber::deserialise(data,offset);token.mTokenMinimaAmount=MiniNumber::deserialise(data,offset);
    token.mTokenName=MiniString::deserialise(data,offset);token.mTokenCreated=MiniNumber::deserialise(data,offset);token.calculateTokenID();return token;
}
std::optional<Token> Token::convertMiniDataVersion(const MiniData& data) {try {size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException&) {return std::nullopt;}}
}
