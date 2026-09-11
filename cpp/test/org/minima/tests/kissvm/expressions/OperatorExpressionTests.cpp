// Complete active pinned fixtures, assertions and expected exception categories.
#include "../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/expressions/BooleanExpression.hpp"
#include "org/minima/kissvm/expressions/OperatorExpression.hpp"
#include "org/minima/objects/mmr/MMREntryNumber.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
template<class A,class B> static void equalOriginal(const A& expected,const B& actual) {require(expected==actual);}
template<class A,class B> static void equalOriginal(const char*,const A& expected,const B& actual) {require(expected==actual);}
template<class E,class F> static void rejectOriginal(F action) {bool caught=false;try {action();}catch(const E&) {caught=true;}require(caught);}

    static void testConstructors() {
        auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

        auto ceTrue = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true));
        auto ceFalse = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false));

        auto ceHexEmpty = std::make_shared<ConstantExpression>(std::make_shared<HexValue>(""));
        auto ceHex00 = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x00"));
        auto ceHex01 = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x01"));

        auto ceHexFF = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0xFF"));
        auto ceHexFFFF = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0xFFFF"));
        auto ceHexFFFF00 = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0xFFFF00"));
        auto ceHex00FFFF = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x00FFFF"));

        auto ceHex0100 = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x0100"));
        auto ceHexDefault = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x12345678"));
        auto ceHex01L16 = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x0101010101010101"));
        auto ceHex10L16 = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x1010101010101010"));

        auto ce0 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0));
        auto ce1 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1));
        auto ce_1 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-1));
        auto ce3 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(3));
        auto ce_3 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-3));
        auto ce5 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5));
        auto ce_5 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5));
        auto ce1000 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1000));
        auto ce_1000 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-1000));
        auto ce1M = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1000000));
        auto ce_1M = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-1000000));
        auto ceMaxInt = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(INT32_MAX));
        auto ceMinInt = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(INT32_MIN));
        auto ceMaxLong = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(INT64_MAX));
        auto ceMinLong = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(INT64_MIN));
        auto ceMaxMini = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(MiniNumber(MiniNumber::MAX_MININUMBER)));
        auto ceMinMini = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(MiniNumber(MiniNumber::MIN_MININUMBER)));

        auto ceR0_5 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(MiniNumber("0.5")));
        auto ceR_0_5 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(MiniNumber("-0.5")));
        auto ceRPI = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(MiniNumber("3.14")));
        auto ceR_PI = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(MiniNumber("-3.14")));

        auto ceScriptEmpty = std::make_shared<ConstantExpression>(std::make_shared<StringValue>(""));
        auto ceScriptDummy = std::make_shared<ConstantExpression>(std::make_shared<StringValue>("[Hello World]"));
        auto ceScriptMinima = std::make_shared<ConstantExpression>(std::make_shared<StringValue>("LET A = 1"));

        equalOriginal(5, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce0, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(5, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce0, ce5, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(6, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce1, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(6, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1, ce5, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(int64_t(INT32_MAX) + 1, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMaxInt, ce1, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal((BigInt(INT64_MAX)+BigInt::one()).toDecimalString(), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMaxLong, ce1, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).getNumber().getAsBigInteger().toDecimalString());

        equalOriginal(5, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce0, OperatorExpression::OPERATOR_SUB))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-5, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce0, ce5, OperatorExpression::OPERATOR_SUB))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(4, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce1, OperatorExpression::OPERATOR_SUB))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-4, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1, ce5, OperatorExpression::OPERATOR_SUB))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(int64_t(INT32_MIN) - 1, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMinInt, ce1, OperatorExpression::OPERATOR_SUB))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal((BigInt(INT64_MIN)-BigInt::one()).toDecimalString(), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMinLong, ce1, OperatorExpression::OPERATOR_SUB))->getValue(*ctr)).getNumber().getAsBigInteger().toDecimalString());

        equalOriginal(0, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce0, OperatorExpression::OPERATOR_MUL))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(0, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce0, ce5, OperatorExpression::OPERATOR_MUL))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(5, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce1, OperatorExpression::OPERATOR_MUL))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(5, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1, ce5, OperatorExpression::OPERATOR_MUL))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(15, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce3, OperatorExpression::OPERATOR_MUL))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(15, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce3, ce5, OperatorExpression::OPERATOR_MUL))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(25, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce5, OperatorExpression::OPERATOR_MUL))->getValue(*ctr)).getNumber().getAsLong());

        equalOriginal(5, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce1, OperatorExpression::OPERATOR_DIV))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(1, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce5, ce5, OperatorExpression::OPERATOR_DIV))->getValue(*ctr)).getNumber().getAsLong());
        rejectOriginal<ExecutionException>([&] {
            (std::make_shared<OperatorExpression>(ce1, ce0, OperatorExpression::OPERATOR_DIV))->getValue(*ctr);
        });

        equalOriginal("", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptEmpty, ceScriptEmpty, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());
        equalOriginal("[Hello World]", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptEmpty, ceScriptDummy, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());
        equalOriginal("[Hello World]", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptDummy, ceScriptEmpty, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());
        equalOriginal("LET A = 1", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptEmpty, ceScriptMinima, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());
        equalOriginal("LET A = 1", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptMinima, ceScriptEmpty, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());

        equalOriginal("[Hello World][Hello World]", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptDummy, ceScriptDummy, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());
        equalOriginal("[Hello World]LET A = 1", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptDummy, ceScriptMinima, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());
        equalOriginal("LET A = 1[Hello World]", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptMinima, ceScriptDummy, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());

        equalOriginal("LET A = 1LET A = 1", dynamic_cast<StringValue&>(*(std::make_shared<OperatorExpression>(ceScriptMinima, ceScriptMinima, OperatorExpression::OPERATOR_ADD))->getValue(*ctr)).toString());

        equalOriginal(0, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce0, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-1, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(1, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce_1, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-1000, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1000, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(1000, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce_1000, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-1000000, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1M, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(1000000, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce_1M, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-int64_t(INT32_MAX), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMaxInt, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-int64_t(INT32_MIN), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMinInt, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal((-BigInt(INT64_MAX)).toDecimalString(), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMaxLong, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsBigInteger().toDecimalString());
        equalOriginal((-BigInt(INT64_MIN)).toDecimalString(), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMinLong, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsBigInteger().toDecimalString());
        equalOriginal(MiniNumber::MIN_MININUMBER.toBigDecimalString(), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMaxMini, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsBigInteger().toDecimalString());
        equalOriginal(MiniNumber::MAX_MININUMBER.toBigDecimalString(), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMinMini, OperatorExpression::OPERATOR_NEG))->getValue(*ctr)).getNumber().getAsBigInteger().toDecimalString());

        equalOriginal(0 % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce0, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(1 % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-1 % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce_1, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(1000 % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1000, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-1000 % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce_1000, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(1000000 % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce1M, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(-1000000 % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ce_1M, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(INT32_MAX % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMaxInt, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal(INT32_MIN % 3, dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMinInt, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsLong());
        equalOriginal((BigInt(INT64_MAX)%BigInt(3)).toDecimalString(), dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceMaxLong, ce3, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsBigInteger().toDecimalString());

        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ce0, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr)).toString());
        equalOriginal("0x02", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ce1, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr)).toString());
        equalOriginal("0x08", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ce3, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr)).toString());
        equalOriginal("0x20", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ce5, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr)).toString());

        equalOriginal("0x0100", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex0100, ce0, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr)).toString());
        equalOriginal("0x80", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex0100, ce1, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr)).toString());
        equalOriginal("0x20", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex0100, ce3, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr)).toString());
        equalOriginal("0x08", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex0100, ce5, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr)).toString());

        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex01, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex00, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex00, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010101", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex01L16, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex10L16, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex01L16, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101010", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex10L16, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex01L16, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex10L16, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex01L16, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex10L16, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex01, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex00, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex01, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex00, OperatorExpression::OPERATOR_AND))->getValue(*ctr)).toString());

        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex01, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex00, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex00, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010101", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex01L16, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x1111111111111111", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex10L16, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x1111111111111111", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex01L16, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101010", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex10L16, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010101", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex01L16, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101011", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex10L16, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010101", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex01L16, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101010", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex10L16, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010101", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex01, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010101", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex00, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101011", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex01, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101010", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex00, OperatorExpression::OPERATOR_OR))->getValue(*ctr)).toString());

        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex01, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex00, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x01", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex00, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex01L16, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x1111111111111111", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex10L16, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x1111111111111111", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex01L16, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex10L16, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010100", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex01L16, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101011", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ceHex10L16, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010101", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex01L16, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101010", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, ceHex10L16, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010100", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex01, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x0101010101010101", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01L16, ceHex00, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101011", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex01, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());
        equalOriginal("0x1010101010101010", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex10L16, ceHex00, OperatorExpression::OPERATOR_XOR))->getValue(*ctr)).toString());

        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHexFF, OperatorExpression::OPERATOR_NOT))->getValue(*ctr)).toString());
        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHexFFFF, OperatorExpression::OPERATOR_NOT))->getValue(*ctr)).toString());
        equalOriginal("0xFF", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00, OperatorExpression::OPERATOR_NOT))->getValue(*ctr)).toString());
        equalOriginal("0xFF0000", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex00FFFF, OperatorExpression::OPERATOR_NOT))->getValue(*ctr)).toString());
        equalOriginal("0xFF", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHexFFFF00, OperatorExpression::OPERATOR_NOT))->getValue(*ctr)).toString());

        rejectOriginal<ExecutionException>([&] {
            (std::make_shared<OperatorExpression>(ceTrue, ceFalse, OperatorExpression::OPERATOR_ADD))->getValue(*ctr);
        });
        rejectOriginal<ExecutionException>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_ADD))->getValue(*ctr);
        });

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceTrue, ceFalse, OperatorExpression::OPERATOR_SUB))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_SUB))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, ceScriptMinima, OperatorExpression::OPERATOR_SUB))->getValue(*ctr);
        });

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceTrue, ceFalse, OperatorExpression::OPERATOR_MUL))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_MUL))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, ceScriptMinima, OperatorExpression::OPERATOR_MUL))->getValue(*ctr);
        });

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceTrue, ceFalse, OperatorExpression::OPERATOR_DIV))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_DIV))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, ceScriptMinima, OperatorExpression::OPERATOR_DIV))->getValue(*ctr);
        });

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceTrue, OperatorExpression::OPERATOR_NEG))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, OperatorExpression::OPERATOR_NEG))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, OperatorExpression::OPERATOR_NEG))->getValue(*ctr);
        });

        equalOriginal("0.5", dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceR0_5, ceRPI, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsBigDecimal().toBigDecimalString());

        equalOriginal("0.14", dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceRPI, ceR0_5, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsBigDecimal().toBigDecimalString());

        equalOriginal("-0.5", dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceR_0_5, ceR_PI, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsBigDecimal().toBigDecimalString());

        equalOriginal("-0.14", dynamic_cast<NumberValue&>(*(std::make_shared<OperatorExpression>(ceR_PI, ceR_0_5, OperatorExpression::OPERATOR_MODULO))->getValue(*ctr)).getNumber().getAsBigDecimal().toBigDecimalString());

        equalOriginal("0x00", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ce_3, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr)).toString());

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceFalse, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceScriptMinima, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceFalse, ce0, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ce1, ce0, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, ce0, OperatorExpression::OPERATOR_SHIFTL))->getValue(*ctr);
        });

        equalOriginal("0x08", dynamic_cast<HexValue&>(*(std::make_shared<OperatorExpression>(ceHex01, ce_3, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr)).toString());

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceFalse, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceHex01, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceScriptMinima, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceFalse, ce0, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ce1, ce0, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, ce0, OperatorExpression::OPERATOR_SHIFTR))->getValue(*ctr);
        });

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceFalse, OperatorExpression::OPERATOR_AND))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ce1, OperatorExpression::OPERATOR_AND))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceScriptMinima, OperatorExpression::OPERATOR_AND))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceFalse, ceHex00, OperatorExpression::OPERATOR_AND))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ce1, ceHex00, OperatorExpression::OPERATOR_AND))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, ceHex00, OperatorExpression::OPERATOR_AND))->getValue(*ctr);
        });

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceFalse, OperatorExpression::OPERATOR_OR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ce1, OperatorExpression::OPERATOR_OR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceScriptMinima, OperatorExpression::OPERATOR_OR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceFalse, ceHex00, OperatorExpression::OPERATOR_OR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ce1, ceHex00, OperatorExpression::OPERATOR_OR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, ceHex00, OperatorExpression::OPERATOR_OR))->getValue(*ctr);
        });

        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceFalse, OperatorExpression::OPERATOR_XOR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ce1, OperatorExpression::OPERATOR_XOR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceScriptMinima, OperatorExpression::OPERATOR_XOR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceFalse, ceHex00, OperatorExpression::OPERATOR_XOR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ce1, ceHex00, OperatorExpression::OPERATOR_XOR))->getValue(*ctr);
        });
        rejectOriginal<std::invalid_argument>([&] {
            (std::make_shared<OperatorExpression>(ceScriptDummy, ceHex00, OperatorExpression::OPERATOR_XOR))->getValue(*ctr);
        });

        rejectOriginal<ExecutionException>([&] {
            (std::make_shared<OperatorExpression>(ceHex00, ceHex01, -5))->getValue(*ctr);
        });
    }

    static void testToString() {
        auto ce1 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(123));
        auto ce2 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(456));

        std::shared_ptr<OperatorExpression> oe;

        std::string exp_s;
        std::string obj_s;

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_ADD);
        exp_s = "( " + ce1->toString() + " + " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_SUB);
        exp_s = "( " + ce1->toString() + " - " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_MUL);
        exp_s = "( " + ce1->toString() + " * " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_DIV);
        exp_s = "( " + ce1->toString() + " / " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, OperatorExpression::OPERATOR_NEG);
        exp_s = " - ( " + ce1->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_SHIFTL);
        exp_s = "( " + ce1->toString() + " << " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_SHIFTR);
        exp_s = "( " + ce1->toString() + " >> " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_MODULO);
        exp_s = "( " + ce1->toString() + " % " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_AND);
        exp_s = "( " + ce1->toString() + " & " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_OR);
        exp_s = "( " + ce1->toString() + " | " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, OperatorExpression::OPERATOR_XOR);
        exp_s = "( " + ce1->toString() + " ^ " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        oe = std::make_shared<OperatorExpression>(ce1, ce2, INT32_MIN);
        exp_s = "( " + ce1->toString() + " ERROR " + ce2->toString() + " )";
        obj_s = oe->toString();
        equalOriginal("should be equal ", exp_s, obj_s);
    }
int main() {
    testConstructors();testToString();
    if(assertions!=178)throw std::runtime_error("Original assertion count changed");
    std::cout<<"ORIGINAL_EXPRESSIONS_RESULT {\"class\":\"OperatorExpressionTests\",\"tests\":2,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
