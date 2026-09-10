// Pinned source: 271 successful assertion calls.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/hex/BITSET.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("BITSETTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static ValuePtr hex(const std::string& text){return std::make_shared<HexValue>(text);}
static ValuePtr number(int64_t value){return std::make_shared<NumberValue>(value);}
static ValuePtr boolean(bool value){return std::make_shared<BooleanValue>(value);}
static ValuePtr text(){return std::make_shared<StringValue>("Hello World");}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
static std::shared_ptr<MinimaFunction> prepared(BITSET& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    BITSET prototype;auto function=prototype.getNewFunction();require(function->getName()=="BITSET");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("BITSET");require(function->getName()=="BITSET");require(function->getParameterNum()==0);
    auto contract=context();
    for(int i=0;i<63;++i){
        // Exact byte representation of the source BitSet fixtures, little-endian bits.
        std::vector<uint8_t> single(size_t(i/8+1),0);single[size_t(i/8)]=uint8_t(1u<<(i%8));
        std::vector<uint8_t> almostAll(8,255);almostAll[size_t(i/8)]&=uint8_t(~(1u<<(i%8)));
        function=prepared(prototype,{std::make_shared<HexValue>(single),number(i),boolean(false)});auto result=function->runFunction(contract);
        require(result->getValueType()==Value::VALUE_HEX);require(dynamic_cast<HexValue&>(*result).toString()=="");
        function=prepared(prototype,{std::make_shared<HexValue>(almostAll),number(i),boolean(true)});result=function->runFunction(contract);
        require(result->getValueType()==Value::VALUE_HEX);require(dynamic_cast<HexValue&>(*result).toString()=="0xFFFFFFFFFFFFFFFF");
    }
    auto invalidContract=context();
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {hex("0x00")},
        {hex("0x00"),number(0)},
        {hex("0x00"),number(0),boolean(false),boolean(false)},
        {hex(""),number(0),boolean(true)},
        {hex("0x00"),number(256),boolean(true)},
        {boolean(true),number(0),boolean(false)},
        {number(123456798),number(0),boolean(false)},
        {text(),number(0),boolean(false)},
        {hex("0x00"),boolean(true),boolean(false)},
        {hex("0x00"),hex("0x00"),boolean(false)},
        {hex("0x00"),text(),boolean(false)},
        {hex("0x00"),number(0),hex("0x00")},
        {hex("0x00"),number(0),number(0)},
        {hex("0x00"),number(0),text()}
    };
    for(const auto& values:invalid){function=prepared(prototype,values);rejected([&]{function->runFunction(invalidContract);});}
}
