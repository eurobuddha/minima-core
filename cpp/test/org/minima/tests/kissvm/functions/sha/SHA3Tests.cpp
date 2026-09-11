// 1:1 active methods from pinned test/org/minima/tests/kissvm/functions/sha/SHA3Tests.java.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/sha/SHA3.hpp"
#include "org/minima/utils/Crypto.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static void testConstructors() {SHA3 prototype;constructors(prototype,"SHA3");}
static void testValidParams() {
    auto contract=context();SHA3 prototype; // Remaining Java method body is commented out.
}
static void testInvalidParams() {
    auto contract=context();SHA3 prototype;
    // Exact active source fixtures in source order; no commented cases revived.
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<NumberValue>(160)},
        {std::make_shared<NumberValue>(160),std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(159),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(513),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(159),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<NumberValue>(513),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<NumberValue>(193),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<NumberValue>(353),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<BooleanValue>(true),std::make_shared<BooleanValue>(true)},
        {std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true),std::make_shared<NumberValue>(1)},
        {std::make_shared<BooleanValue>(true),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(1)},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<NumberValue>(1),std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(1),std::make_shared<NumberValue>(1)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<NumberValue>(1)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<StringValue>("Hello World")}
    };
    for(const auto& values:invalid)rejected(prototype,contract,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("SHA3Tests",27);}
