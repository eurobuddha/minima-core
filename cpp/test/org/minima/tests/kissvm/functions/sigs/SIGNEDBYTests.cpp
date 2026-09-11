// All active source methods, random sizes and assertions retained.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/sigs/SIGNEDBY.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static void testConstructors() {SIGNEDBY prototype;constructors(prototype,"SIGNEDBY");}
static void testValidParams() {
    auto single1=MiniData::getRandomData(20);auto single2=MiniData::getRandomData(20);
    std::vector<MiniData> signatures1{single1};std::vector<MiniData> signatures2{single2};
    SIGNEDBY prototype;
    {
        Contract contract("",signatures1,std::make_shared<Witness>(),std::make_shared<Transaction>());
        auto result=prepared(prototype,{std::make_shared<HexValue>(single1)})->runFunction(contract);
        require(result->getValueType()==Value::VALUE_BOOLEAN);require(dynamic_cast<BooleanValue&>(*result).toString()=="TRUE");
    }
    {
        Contract contract("",signatures1,std::make_shared<Witness>(),std::make_shared<Transaction>());
        auto result=prepared(prototype,{std::make_shared<HexValue>(single2)})->runFunction(contract);
        require(result->getValueType()==Value::VALUE_BOOLEAN);require(dynamic_cast<BooleanValue&>(*result).toString()=="FALSE");
    }
}
static void testInvalidParams() {
    auto contract=context();SIGNEDBY prototype;
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(1)},
        {std::make_shared<StringValue>("Hello World")}
    };
    for(const auto& values:invalid)rejected(prototype,contract,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("SIGNEDBYTests",13,"ORIGINAL_SIGS_RESULT");}
