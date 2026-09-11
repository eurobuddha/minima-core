// All active source methods, random sizes and assertions retained.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/sigs/CHECKSIG.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static void testConstructors() {CHECKSIG prototype;constructors(prototype,"CHECKSIG");}
static void testValidParams() {
    auto contract=context();CHECKSIG prototype;
    auto seed1=MiniData::getRandomData(32);auto seed2=MiniData::getRandomData(64);
    TreeKey key1(seed1,16,2);TreeKey key2(seed2,16,2);
    auto data1=MiniData::getRandomData(32);auto data2=MiniData::getRandomData(64);
    MiniData sig1(key1.sign(data1).serialise());MiniData sig2(key2.sign(data2).serialise());
    struct Case {MiniData pubkey,data,signature;std::string expected;};
    const std::vector<Case> cases{
        {*key1.getPublicKey(),data1,sig1,"TRUE"},
        {*key2.getPublicKey(),data2,sig2,"TRUE"},
        {*key2.getPublicKey(),data1,sig1,"FALSE"},
        {*key1.getPublicKey(),data2,sig1,"FALSE"},
        {*key1.getPublicKey(),data1,sig2,"FALSE"},
        {*key2.getPublicKey(),data2,sig1,"FALSE"},
        {*key2.getPublicKey(),data1,sig2,"FALSE"},
        {*key1.getPublicKey(),data2,sig2,"FALSE"}
    };
    for(const auto& row:cases) {
        auto result=prepared(prototype,{std::make_shared<HexValue>(row.pubkey),std::make_shared<HexValue>(row.data),std::make_shared<HexValue>(row.signature)})->runFunction(contract);
        require(result->getValueType()==Value::VALUE_BOOLEAN);require(dynamic_cast<BooleanValue&>(*result).toString()==row.expected);
    }
}
static void testInvalidParams() {
    auto contract=context();CHECKSIG prototype;
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>(""),std::make_shared<HexValue>(""),std::make_shared<HexValue>("")},
        {std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(0)},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<BooleanValue>(true),std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true),std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true),std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(0),std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(0),std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(0)},
        {std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(0)},
        {std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(0)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<BooleanValue>(true),std::make_shared<BooleanValue>(true),std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<StringValue>("Hello World"),std::make_shared<StringValue>("Hello World")}
    };
    for(const auto& values:invalid)rejected(prototype,contract,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("CHECKSIGTests",55,"ORIGINAL_SIGS_RESULT");}
