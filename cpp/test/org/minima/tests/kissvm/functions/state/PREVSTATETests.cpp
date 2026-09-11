// Active fixtures and assertions from pinned test/org/minima/tests/kissvm/functions/state/PREVSTATETests.java.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/state/PREVSTATE.hpp"
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

static void testConstructors() {PREVSTATE prototype;constructors(prototype,"PREVSTATE");}
static void testValidParams() {
    auto values=states();auto transaction=std::make_shared<Transaction>();
    Contract contract("",std::string{},std::make_shared<Witness>(),transaction,values);
    PREVSTATE prototype;
    for(int i=0;i<int(values.size());++i) {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(i)})->runFunction(contract);
        const int types[]={Value::VALUE_BOOLEAN,Value::VALUE_HEX,Value::VALUE_NUMBER,Value::VALUE_SCRIPT};
        require(result->getValueType()==types[i%4]);
        // Preserve source casts as well as value equality.
        std::string text;
        switch(i%4) {
        case 0:text=dynamic_cast<BooleanValue&>(*result).toString();break;
        case 1:text=dynamic_cast<HexValue&>(*result).toString();break;
        case 2:text=dynamic_cast<NumberValue&>(*result).toString();break;
        default:text=dynamic_cast<StringValue&>(*result).toString();break;
        }
        require(Value::getValue(values.at(i).toString())->toString()==text);
    }
}
static void testInvalidParams() {
    auto contract=context();PREVSTATE prototype;
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0)},
        {std::make_shared<NumberValue>(-1)},
        {std::make_shared<NumberValue>(0)},
        {std::make_shared<BooleanValue>(true)},
        {std::make_shared<HexValue>("0x12345678")},
        {std::make_shared<StringValue>("Hello World")}
    };
    for(const auto& values:invalid)rejected(prototype,contract,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("PREVSTATETests",139,"ORIGINAL_STATE_RESULT");}
