// 1:1 active methods from pinned test/org/minima/tests/kissvm/functions/sha/SHA2Tests.java.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/sha/SHA2.hpp"
#include "org/minima/utils/Crypto.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static void testConstructors() {SHA2 prototype;constructors(prototype,"SHA2");}
static void testValidParams() {
    auto contract=context();SHA2 prototype;
    for(int i=0;i<100;++i) {
        auto param=std::make_shared<HexValue>(MiniData::getRandomData(64).to0xString());
        HexValue expected(Crypto::getInstance().hashSHA2(param->getRawData()));
        auto result=prepared(prototype,{param})->runFunction(contract);
        require(result->getValueType()==Value::VALUE_HEX);
        require(dynamic_cast<HexValue&>(*result).toString()==expected.toString());
    }
    auto param=std::make_shared<HexValue>(std::string{});
    HexValue expected(Crypto::getInstance().hashSHA2(param->getRawData()));
    auto result=prepared(prototype,{param})->runFunction(contract);
    require(result->getValueType()==Value::VALUE_HEX);
    require(dynamic_cast<HexValue&>(*result).toString()==expected.toString());
    result=prepared(prototype,{std::make_shared<StringValue>(std::string{})})->runFunction(contract);
    require(result->getValueType()==Value::VALUE_HEX);
    require(dynamic_cast<HexValue&>(*result).toString()=="0xE3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855");
}
static void testInvalidParams() {
    auto contract=context();SHA2 prototype;
    // Exact active source fixtures in source order; no commented cases revived.
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(0)}
    };
    for(const auto& values:invalid)rejected(prototype,contract,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("SHA2Tests",212);}
