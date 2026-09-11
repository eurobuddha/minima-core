// Active assertions and inputs from the pinned CoinTests.
#include "org/minima/objects/Coin.hpp"
#include <iostream>
using namespace minima;
namespace {
int assertions=0;
void require(bool value) { ++assertions; if(!value) throw std::runtime_error("CoinTests assertion "+std::to_string(assertions)); }
void testCoin() {
    auto md=Coin::COINID_OUTPUT,md3=Token::TOKENID_CREATE,md2=Token::TOKENID_MINIMA;
    auto coinId=MiniData::fromHex("0xfff"),coinId2=MiniData::fromHex("0xf0f0"),coinAddress=MiniData::fromHex("0xffffffff"),tokenId=MiniData::fromHex("123");
    auto two=MiniNumber::TWO,twelve=MiniNumber::TWELVE; Coin c(coinId,coinAddress,two,tokenId);
    require(c.getTokenID()==tokenId); require(c.getAmount()==two); require(c.getAddress()==coinAddress);
}
Coin streamCoin() { return Coin(MiniData::fromHex("0xfff"),MiniData::fromHex("0xffffffff"),MiniNumber::TWELVE,MiniData::fromHex("123")); }
void testReadAndWriteDataStream() {
    try { auto c=streamCoin(); auto bytes=c.serialise(); size_t offset=0; c.readDataStream(bytes,offset); require(!c.isNull()); }
    catch(const wire::IOException&) { require(false); }
}
void testStaticReadAndWriteDataStream() {
    try {
        auto c=streamCoin(); auto bytes=c.serialise(); size_t offset=0;
        // The original static read result is deliberately discarded.
        (void)Coin::deserialise(bytes,offset); require(!c.isNull());
    } catch(const wire::IOException&) { require(false); }
}
}
int main() {
    testCoin(); testReadAndWriteDataStream(); testStaticReadAndWriteDataStream();
    auto value=streamCoin(); auto bytes=value.serialise(); size_t offset=0; auto decoded=Coin::deserialise(bytes,offset);
    json::JSONObject report; report.put("class","CoinTests"); report.put("tests",3); report.put("assertions",assertions); report.put("passed",true);
    report.put("wire",MiniData(bytes).to0xString()); report.put("decoded",decoded.toJSON());
    std::cout<<"ORIGINAL_OBJECT_RESULT "<<report.toJSONString()<<'\n';
}
