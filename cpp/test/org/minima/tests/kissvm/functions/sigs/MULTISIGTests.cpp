// All active source methods, random sizes and assertions retained.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/sigs/MULTISIG.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static void testConstructors() {MULTISIG prototype;constructors(prototype,"MULTISIG");}
static void testValidParams() {
    std::vector<MiniData> signatures;
    for(int i=20;i<=64;i+=4) {if(i==44)continue;signatures.push_back(MiniData::getRandomData(i));}
    auto untracked=MiniData::getRandomData(20);
    Contract contract("",signatures,std::make_shared<Witness>(),std::make_shared<Transaction>());
    MULTISIG prototype;
    for(int i=1;i<=int(signatures.size());++i) {
        {
            std::vector<ValuePtr> args{std::make_shared<NumberValue>(i)};
            for(int j=0;j<i;++j)args.push_back(std::make_shared<HexValue>(signatures.at(j)));
            auto result=prepared(prototype,args)->runFunction(contract);
            require(result->getValueType()==Value::VALUE_BOOLEAN);require(dynamic_cast<BooleanValue&>(*result).toString()=="TRUE");
        }
        {
            std::vector<ValuePtr> args{std::make_shared<NumberValue>(i)};
            for(int j=0;j<i-1;++j)args.push_back(std::make_shared<HexValue>(signatures.at(j)));
            args.push_back(std::make_shared<HexValue>(untracked));
            auto result=prepared(prototype,args)->runFunction(contract);
            require(result->getValueType()==Value::VALUE_BOOLEAN);require(dynamic_cast<BooleanValue&>(*result).toString()=="FALSE");
        }
        {
            std::vector<ValuePtr> args{std::make_shared<NumberValue>(i+1)};
            for(int j=0;j<i;++j)args.push_back(std::make_shared<HexValue>(signatures.at(j)));
            args.push_back(std::make_shared<HexValue>(untracked));
            auto result=prepared(prototype,args)->runFunction(contract);
            require(result->getValueType()==Value::VALUE_BOOLEAN);require(dynamic_cast<BooleanValue&>(*result).toString()=="FALSE");
        }
    }
}
static void testInvalidParams() {
    auto contract=context();MULTISIG prototype;
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<NumberValue>(0)},
        {std::make_shared<NumberValue>(-1),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(-1),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<HexValue>("0x01234567"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<StringValue>("Hello World"),std::make_shared<HexValue>("0x01234567")},
        {std::make_shared<NumberValue>(1),std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(1),std::make_shared<NumberValue>(1)},
        {std::make_shared<NumberValue>(1),std::make_shared<StringValue>("Hello World")}
    };
    for(const auto& values:invalid)rejected(prototype,contract,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("MULTISIGTests",80,"ORIGINAL_SIGS_RESULT");}
