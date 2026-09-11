// Complete active pinned source fixtures, helper and assertions.
#include "../../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/txn/input/GETINAMT.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static void testConstructors() {GETINAMT prototype;constructors(prototype,"GETINAMT");}
static Address newSimpleAddress() {
    auto pubkey=MiniData::getRandomData(32);
    return Address("RETURN SIGNEDBY("+pubkey.to0xString()+")");
}
static void testValidParams() {
    Address addr1 = newSimpleAddress();
    Address addr2 = newSimpleAddress();
    Address addr3 = newSimpleAddress();
    Address addr4 = newSimpleAddress();
    Address addr5 = newSimpleAddress();
    Address addr6 = newSimpleAddress();
    auto w=std::make_shared<Witness>();
    Token tmp(MiniData::getRandomData(16),
    MiniNumber::TEN,
    MiniNumber::MILLION,
    MiniString("TestToken"),
    MiniString("Hello from TestToken"));
    auto trx=std::make_shared<Transaction>();
    Coin in1(MiniData::getRandomData(16), addr1.getAddressData(), MiniNumber("50"), Token::TOKENID_MINIMA);
    trx->addInput(in1);
    Coin in2(MiniData::getRandomData(16), addr2.getAddressData(), MiniNumber("75"), *tmp.getTokenID());
    in2.setToken(tmp);
    trx->addInput(in2);
    Coin in3(MiniData::getRandomData(16), addr3.getAddressData(), MiniNumber("1"), MiniData::getRandomData(16));
    trx->addInput(in3);
    Coin out1(MiniData::getRandomData(16), addr4.getAddressData(), MiniNumber("40"), Token::TOKENID_MINIMA);
    trx->addOutput(out1);
    Coin out2(MiniData::getRandomData(16), addr5.getAddressData(), MiniNumber("30"), *tmp.getTokenID());
    out2.setToken(tmp);
    trx->addOutput(out2);
    Coin out3(MiniData::getRandomData(16), addr6.getAddressData(), MiniNumber("1"), in3.getTokenID());
    out2.setToken(in3.getToken());
    trx->addOutput(out3);
    w->addScript(ScriptProof(addr1.getScript()));
    w->addScript(ScriptProof(addr2.getScript()));
    Contract ctr("", std::string{}, w, trx);
    GETINAMT prototype;
    auto result=prepared(prototype,{std::make_shared<NumberValue>(0)})->runFunction(ctr);
    require(result->getValueType()==Value::VALUE_NUMBER);
    require(in1.getAmount().getAsInt()==dynamic_cast<NumberValue&>(*result).getNumber().getAsInt());
    result=prepared(prototype,{std::make_shared<NumberValue>(1)})->runFunction(ctr);
    require(result->getValueType()==Value::VALUE_NUMBER);
    require(tmp.getScaledTokenAmount(in2.getAmount()).getAsLong()==dynamic_cast<NumberValue&>(*result).getNumber().getAsLong());
    rejected(prototype,ctr,{std::make_shared<NumberValue>(2)});
}
static void testInvalidParams() {
    Address addr1 = newSimpleAddress();
    Address addr2 = newSimpleAddress();
    Address addr3 = newSimpleAddress();
    Address addr4 = newSimpleAddress();
    Address addr5 = newSimpleAddress();
    Address addr6 = newSimpleAddress();
    auto w=std::make_shared<Witness>();
    Token tmp(MiniData::getRandomData(16),
    MiniNumber::TEN,
    MiniNumber::MILLION,
    MiniString("TestToken"),
    MiniString("Hello from TestToken"));
    auto trx=std::make_shared<Transaction>();
    Coin in1(MiniData::getRandomData(16), addr1.getAddressData(), MiniNumber("50"), Token::TOKENID_MINIMA);
    trx->addInput(in1);
    Coin in2(MiniData::getRandomData(16), addr2.getAddressData(), MiniNumber("75"), *tmp.getTokenID());
    in2.setToken(tmp);
    trx->addInput(in2);
    Coin in3(MiniData::getRandomData(16), addr3.getAddressData(), MiniNumber("1"), MiniData::getRandomData(16));
    trx->addInput(in3);
    Coin out1(MiniData::getRandomData(16), addr4.getAddressData(), MiniNumber("40"), Token::TOKENID_MINIMA);
    trx->addOutput(out1);
    Coin out2(MiniData::getRandomData(16), addr5.getAddressData(), MiniNumber("30"), *tmp.getTokenID());
    out2.setToken(tmp);
    trx->addOutput(out2);
    Coin out3(MiniData::getRandomData(16), addr6.getAddressData(), MiniNumber("1"), in3.getTokenID());
    trx->addOutput(out3);
    w->addScript(ScriptProof(addr1.getScript()));
    w->addScript(ScriptProof(addr2.getScript()));
    Contract ctr("", std::string{}, w, trx);
    GETINAMT prototype;
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
int main() {testConstructors();testValidParams();testInvalidParams();report("GETINAMTTests",16,"ORIGINAL_GETTER_RESULT");}
