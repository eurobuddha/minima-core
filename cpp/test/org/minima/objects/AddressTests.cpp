// Active assertions and inputs from the pinned AddressTests.
#include "org/minima/objects/Address.hpp"
#include "support/ByteIO.hpp"
#include <iostream>
using namespace minima;
namespace {
int assertions=0;
void require(bool value) { ++assertions; if(!value) throw std::runtime_error("AddressTests assertion "+std::to_string(assertions)); }
void testAddress() {
    MiniData c,j=MiniData::fromHex("0xFFFF"),n=MiniData::fromHex("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    Address ad; auto a=std::make_shared<Address>("0xf0f0"); Address abc("0xf0f0");
    require(bool(a)); (void)a->getAddressData(); Address adr(j),adrTwo(n),adrThree(c);
    require(!a->isEqual(adrThree)); require(a->isEqual(abc));
}
void testWriteAndReadDataStream() {
    try {
        auto i=std::make_shared<MiniData>(MiniData::fromHex("0xfff0f0")); Address a(*i);
        auto bytes=a.serialise(); size_t offset=0; a.readDataStream(bytes,offset);
        require(bool(i)); require(a.toString()=="0xFFF0F0");
    } catch(const wire::IOException&) { require(false); }
}
void testMakeMinimaAddress() {
    auto i=MiniData::fromHex("0xffffffffffffffffffffffffffffffffffffffff");
    auto mxAddress=Address::makeMinimaAddress(i); auto j=Address::convertMinimaAddress(mxAddress);
    require(i==j); std::cout<<"should be equal to - "<<i.to0xString()<<'\n';
    auto q=MiniData::fromHex("0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    auto mxAddress1=Address::makeMinimaAddress(q); auto p=Address::convertMinimaAddress(mxAddress1); require(q==p);
    auto l=MiniData::fromHex("0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    auto o=MiniData::fromHex("0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    auto v=MiniData::fromHex("0xffffffffffffffffffffffffffffffffffffffff");
    auto mxAddress3=Address::makeMinimaAddress(l),mxAddress4=Address::makeMinimaAddress(v),mxAddress5=Address::makeMinimaAddress(o);
    auto m=Address::convertMinimaAddress(mxAddress3),m2=Address::convertMinimaAddress(mxAddress4),m3=Address::convertMinimaAddress(mxAddress5);
    require(l==m); require(v==m2); require(o==m3);
}
}
int main() {
    testAddress(); testWriteAndReadDataStream(); testMakeMinimaAddress();
    // Supplemental cross-runtime wire/content observation, separate from the 10 source assertions.
    Address value(MiniData::fromHex("0xfff0f0")); auto bytes=value.serialise(); size_t offset=0;
    auto decoded=Address::deserialise(bytes,offset); json::JSONObject report;
    report.put("class","AddressTests"); report.put("tests",3); report.put("assertions",assertions); report.put("passed",true);
    report.put("wire",MiniData(bytes).to0xString()); report.put("decoded",decoded.toJSON());
    std::cout<<"ORIGINAL_OBJECT_RESULT "<<report.toJSONString()<<'\n';
}
