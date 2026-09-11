// Active fixtures and assertions from pinned test/org/minima/tests/kissvm/functions/state/SAMESTATETests.java.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/state/SAMESTATE.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static std::vector<StateVariable> states() {
    std::vector<StateVariable> result;
    for(int i=0;i<16;++i) {
        result.emplace_back(4*i,BooleanValue(true).toString());
        result.emplace_back(4*i+1,HexValue("0x12345678").toString());
        result.emplace_back(4*i+2,NumberValue(i).toString());
        result.emplace_back(4*i+3,StringValue("[ Hello World "+std::to_string(4*i+3)).toString()+" ]");
    }
    return result;
}

static void testConstructors() {SAMESTATE prototype;constructors(prototype,"SAMESTATE");}
static void testValidParams() {
    auto previous=states();auto same=states();std::vector<StateVariable> different;
    for(int i=0;i<16;++i) {
        different.emplace_back(4*i,StringValue("[ Hello World "+std::to_string(4*i)).toString()+" ]");
        different.emplace_back(4*i+1,NumberValue(i).toString());
        different.emplace_back(4*i+2,HexValue("0x12345678").toString());
        different.emplace_back(4*i+3,BooleanValue(true).toString());
    }
    SAMESTATE prototype;
    auto transaction1=std::make_shared<Transaction>();
    for(const auto& state:same)transaction1->addStateVariable(state);
    Contract contract1("",std::string{},std::make_shared<Witness>(),transaction1,previous);
    for(int i=0;i<int(previous.size());++i) {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(i),std::make_shared<NumberValue>(i)})->runFunction(contract1);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isTrue());
    }
    auto transaction2=std::make_shared<Transaction>();
    for(const auto& state:different)transaction2->addStateVariable(state);
    Contract contract2("",std::string{},std::make_shared<Witness>(),transaction2,previous);
    for(int i=0;i<int(different.size());++i) {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(i),std::make_shared<NumberValue>(i)})->runFunction(contract2);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isFalse());
    }
}
static void testInvalidParams() {
    auto contract=context();SAMESTATE prototype;
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<NumberValue>(0)},
        {std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0)},
        {std::make_shared<NumberValue>(-1),std::make_shared<NumberValue>(-1)},
        {std::make_shared<NumberValue>(-1),std::make_shared<NumberValue>(-5)},
        {std::make_shared<NumberValue>(100),std::make_shared<NumberValue>(200)},
        {std::make_shared<BooleanValue>(true),std::make_shared<BooleanValue>(true)},
        {std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true),std::make_shared<NumberValue>(0)},
        {std::make_shared<BooleanValue>(true),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(0)},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<NumberValue>(0),std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(0),std::make_shared<StringValue>("Hello World")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<NumberValue>(0)},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<StringValue>("Hello World")}
    };
    for(const auto& values:invalid)rejected(prototype,contract,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("SAMESTATETests",281,"ORIGINAL_STATE_RESULT");}
