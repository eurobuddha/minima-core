// 1:1 active methods from pinned test/org/minima/tests/kissvm/functions/sha/CHAINSHATests.java.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/sha/PROOF.hpp"
#include "org/minima/utils/Crypto.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static void testConstructors() {PROOF prototype;constructors(prototype,"PROOF");}
static void testValidParams() {
    // Entire pinned valid-case body is commented out: zero active assertions.
}
static void testInvalidParams() {
    auto contract=context();PROOF prototype;
    // Exact active source fixtures in source order; no commented cases revived.
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x04")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x11")},
        {std::make_shared<BooleanValue>(true),std::make_shared<BooleanValue>(true)},
        {std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true),std::make_shared<NumberValue>(0)},
        {std::make_shared<BooleanValue>(true),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(0)},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<NumberValue>(0),std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0)},
        {std::make_shared<NumberValue>(0),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<NumberValue>(0)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<StringValue>("Hello World")}
    };
    for(const auto& values:invalid)rejected(prototype,contract,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("CHAINSHATests",24);}
