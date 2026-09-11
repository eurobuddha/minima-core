// Active assertions, original observations and inputs from the pinned TokenTests.
#include "org/minima/objects/Token.hpp"
#include "support/ByteIO.hpp"
#include <iostream>
using namespace minima;
namespace {
int assertions=0;
void require(bool value) { ++assertions; if(!value) throw std::runtime_error("TokenTests assertion "+std::to_string(assertions)); }
void testTokenProof() {
    MiniString name("token-name"),script("FFFF"); auto coinId=MiniData::fromHex("0xffff"); auto twelve=MiniNumber::TWELVE;
    auto t=std::make_shared<Token>(coinId,twelve,twelve,name,script);
    require(bool(t)); require(twelve==t->getAmount()); std::cout<<"amount  value "<<t->getAmount().toString()<<'\n';
    require(name==t->getName()); std::cout<<"name  value "<<t->getName().toString()<<'\n';
    std::cout<<"script value "<<script.toString()<<" "<<t->getTokenScript().toString()<<'\n';
    std::cout<<"tokenscript  value "<<t->getTokenScript().toString()<<'\n';
    std::cout<<"coin id  value "<<t->getCoinID().to0xString()<<'\n';
    std::cout<<"token id  value "<<(t->getTokenID()?t->getTokenID()->to0xString():"null")<<'\n';
    require(twelve==t->getScale()); std::cout<<"scale value "<<t->getScale().toString()<<'\n';
    std::cout<<"Json  value "<<t->toJSON().toJSONString()<<'\n';
}
Token streamToken() { return Token(MiniData::fromHex("0xffff"),MiniNumber::TWELVE,MiniNumber::TWELVE,MiniString("token-name"),MiniString("FFFF")); }
void testStaticReadAndWriteDataStream() {
    try {
        auto t=std::make_shared<Token>(streamToken()); auto bytes=t->serialise(); size_t offset=0;
        (void)Token::deserialise(bytes,offset);
        std::cout<<"Proof json values in read and write stream - "<<t->toJSON().toJSONString()<<'\n'; require(bool(t));
    } catch(const wire::IOException&) { require(false); }
}
}
int main() {
    testTokenProof(); testStaticReadAndWriteDataStream();
    auto value=streamToken(); auto bytes=value.serialise(); size_t offset=0; auto decoded=Token::deserialise(bytes,offset);
    json::JSONObject report; report.put("class","TokenTests"); report.put("tests",2); report.put("assertions",assertions); report.put("passed",true);
    report.put("wire",MiniData(bytes).to0xString()); report.put("decoded",decoded.toJSON());
    std::cout<<"ORIGINAL_OBJECT_RESULT "<<report.toJSONString()<<'\n';
}
