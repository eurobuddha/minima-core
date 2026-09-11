// All active pinned source fixtures, helper and assertions are retained.
#include "../../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/functions/txn/input/VERIFYIN.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static Address newSimpleAddress() {
    auto pubkey=MiniData::getRandomData(32);
    return Address("RETURN SIGNEDBY("+pubkey.to0xString()+")");
}
static void testConstructors() {VERIFYIN prototype;constructors(prototype,"VERIFYIN");}
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
    out3.setToken(in3.getToken());
    trx->addOutput(out3);
    w->addScript(ScriptProof(addr1.getScript()));
    w->addScript(ScriptProof(addr2.getScript()));
    Contract ctr("", std::string{}, w, trx);
    VERIFYIN prototype;
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(0),std::make_shared<HexValue>(addr1.getAddressData()),std::make_shared<NumberValue>(in1.getAmount()),std::make_shared<HexValue>(in1.getTokenID())})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isTrue());
    }
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(0),std::make_shared<HexValue>(addr2.getAddressData()),std::make_shared<NumberValue>(in1.getAmount()),std::make_shared<HexValue>(in1.getTokenID())})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isFalse());
    }
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(0),std::make_shared<HexValue>(addr1.getAddressData()),std::make_shared<NumberValue>(in2.getAmount()),std::make_shared<HexValue>(in1.getTokenID())})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isFalse());
    }
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(0),std::make_shared<HexValue>(addr1.getAddressData()),std::make_shared<NumberValue>(in1.getAmount()),std::make_shared<HexValue>(MiniData::getRandomData(16))})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isFalse());
    }
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(1),std::make_shared<HexValue>(addr2.getAddressData()),std::make_shared<NumberValue>(tmp.getScaledTokenAmount(in2.getAmount())),std::make_shared<HexValue>(in2.getTokenID())})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isTrue());
    }
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(1),std::make_shared<HexValue>(addr1.getAddressData()),std::make_shared<NumberValue>(tmp.getScaledTokenAmount(in2.getAmount())),std::make_shared<HexValue>(in2.getTokenID())})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isFalse());
    }
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(1),std::make_shared<HexValue>(addr2.getAddressData()),std::make_shared<NumberValue>(in2.getAmount()),std::make_shared<HexValue>(in2.getTokenID())})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isFalse());
    }
    {
        auto result=prepared(prototype,{std::make_shared<NumberValue>(1),std::make_shared<HexValue>(addr2.getAddressData()),std::make_shared<NumberValue>(tmp.getScaledTokenAmount(in2.getAmount())),std::make_shared<HexValue>(MiniData::getRandomData(16))})->runFunction(ctr);
        require(result->getValueType()==Value::VALUE_BOOLEAN);
        require(dynamic_cast<BooleanValue&>(*result).isFalse());
    }
    rejected(prototype,ctr,{std::make_shared<NumberValue>(2),std::make_shared<HexValue>(addr3.getAddressData()),std::make_shared<NumberValue>(tmp.getScaledTokenAmount(in2.getAmount())),std::make_shared<HexValue>(in3.getTokenID())});
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
    out3.setToken(in3.getToken());
    trx->addOutput(out3);
    w->addScript(ScriptProof(addr1.getScript()));
    w->addScript(ScriptProof(addr2.getScript()));
    Contract ctr("", std::string{}, w, trx);
    VERIFYIN prototype;
    rejected(prototype,ctr,{});
    rejected(prototype,ctr,{std::make_shared<NumberValue>(0)});
    rejected(prototype,ctr,{std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x12345678")});
    rejected(prototype,ctr,{std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x12345678"),std::make_shared<NumberValue>(0)});
    rejected(prototype,ctr,{std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x12345678"),std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x12345678"),std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0)});
    rejected(prototype,ctr,{std::make_shared<NumberValue>(-1),std::make_shared<HexValue>("0x12345678"),std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x12345678")});
    rejected(prototype,ctr,{std::make_shared<NumberValue>(35),std::make_shared<HexValue>("0x12345678"),std::make_shared<NumberValue>(0),std::make_shared<HexValue>("0x12345678")});
}
int main() {testConstructors();testValidParams();testInvalidParams();report("VERIFYINTests",28,"ORIGINAL_VERIFY_RESULT");}
