// Complete active pinned source fixtures, helper and assertions.
#include "../../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/txn/output/GETOUTTOK.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static void testConstructors() {GETOUTTOK prototype;constructors(prototype,"GETOUTTOK");}
static Address newSimpleAddress() {
    auto pubkey=MiniData::getRandomData(32);
    return Address("RETURN SIGNEDBY("+pubkey.to0xString()+")");
}
static void testValidParams() {
    Address addr1 = newSimpleAddress();
    Address addr2 = newSimpleAddress();
    Address addr3 = newSimpleAddress();
    Address addr4 = newSimpleAddress();
    auto trx=std::make_shared<Transaction>();
    Coin in1(Coin::COINID_OUTPUT, addr1.getAddressData(), MiniNumber("25"), Token::TOKENID_MINIMA);
    trx->addInput(in1);
    Coin in2(Coin::COINID_OUTPUT, addr2.getAddressData(), MiniNumber("75"), Token::TOKENID_MINIMA);
    trx->addInput(in2);
    Coin out1(Coin::COINID_OUTPUT, addr3.getAddressData(), MiniNumber("40"), Token::TOKENID_MINIMA);
    trx->addOutput(out1);
    Coin out2(Coin::COINID_OUTPUT, addr4.getAddressData(), MiniNumber("60"), Token::TOKENID_MINIMA);
    trx->addOutput(out2);
    auto w=std::make_shared<Witness>();
    w->addScript(ScriptProof(addr1.getScript()));
    w->addScript(ScriptProof(addr2.getScript()));
    Contract ctr("", std::string{}, w, trx);
    GETOUTTOK prototype;
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(0)})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_HEX);
        require(out1.getTokenID().isEqual(dynamic_cast<HexValue&>(*result).getMiniData()));
    }
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(1)})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_HEX);
        require(out2.getTokenID().isEqual(dynamic_cast<HexValue&>(*result).getMiniData()));
    }
}
static void testInvalidParams() {
    Address addr1 = newSimpleAddress();
    Address addr2 = newSimpleAddress();
    Address addr3 = newSimpleAddress();
    Address addr4 = newSimpleAddress();
    auto trx=std::make_shared<Transaction>();
    Coin in1(Coin::COINID_OUTPUT, addr1.getAddressData(), MiniNumber("25"), Token::TOKENID_MINIMA);
    trx->addInput(in1);
    Coin in2(Coin::COINID_OUTPUT, addr2.getAddressData(), MiniNumber("75"), Token::TOKENID_MINIMA);
    trx->addInput(in2);
    Coin out1(Coin::COINID_OUTPUT, addr3.getAddressData(), MiniNumber("40"), Token::TOKENID_MINIMA);
    trx->addOutput(out1);
    Coin out2(Coin::COINID_OUTPUT, addr4.getAddressData(), MiniNumber("60"), Token::TOKENID_MINIMA);
    trx->addOutput(out2);
    auto w=std::make_shared<Witness>();
    w->addScript(ScriptProof(addr1.getScript()));
    w->addScript(ScriptProof(addr2.getScript()));
    Contract ctr("", std::string{}, w, trx);
    GETOUTTOK prototype;
    const std::vector<std::vector<ValuePtr>> invalid{
        {},
        {std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0)},
        {std::make_shared<NumberValue>(-1)},
        {std::make_shared<NumberValue>(35)},
        {std::make_shared<BooleanValue>(true)},
        {std::make_shared<HexValue>("0x12345678")},
        {std::make_shared<StringValue>("Hello World")}
    };
    for(const auto& values:invalid)rejected(prototype,ctr,values);
}
int main() {testConstructors();testValidParams();testInvalidParams();report("GETOUTTOKTests",15,"ORIGINAL_GETTER_RESULT");}
