// Retains every active pinned original fixture and assertion.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/expressions/FunctionExpression.hpp"
#include "org/minima/kissvm/expressions/GlobalExpression.hpp"
#include "org/minima/kissvm/functions/general/GET.hpp"
#include "org/minima/kissvm/functions/number/DEC.hpp"
#include "org/minima/kissvm/functions/sigs/CHECKSIG.hpp"
#include "org/minima/kissvm/expressions/BooleanExpression.hpp"
#include "org/minima/kissvm/expressions/OperatorExpression.hpp"
#include "org/minima/kissvm/expressions/VariableExpression.hpp"
#include "org/minima/kissvm/statements/StatementBlock.hpp"
#include "org/minima/kissvm/statements/commands/RETURNstatement.hpp"
#include "org/minima/kissvm/statements/commands/WHILEstatement.hpp"
#include "org/minima/kissvm/statements/commands/IFstatement.hpp"
#include "org/minima/kissvm/statements/commands/MASTstatement.hpp"
#include "org/minima/kissvm/statements/commands/LETstatement.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
template<class A,class B> static void equalOriginal(const A& expected,const B& actual) {require(expected==actual);}
template<class F> static void rejectStatement(F action) {bool caught=false;try {action();}catch(const ExecutionException&) {caught=true;}require(caught);}

    static void testConstructors() {
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<BooleanExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)),
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)),
                            BooleanExpression::BOOLEAN_AND));
            equalOriginal("LET a = ( TRUE AND TRUE )", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<BooleanExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false)),
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false)),
                            BooleanExpression::BOOLEAN_EQ));
            equalOriginal("LET a = ( FALSE EQ FALSE )", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<BooleanExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)),
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false)),
                            BooleanExpression::BOOLEAN_NEQ));
            equalOriginal("LET a = ( TRUE NEQ FALSE )", ls->toString());
        }

        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-1)));
            equalOriginal("LET a = -1", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0)));
            equalOriginal("LET a = 0", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)));
            equalOriginal("LET a = 1", ls->toString());
        }

        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<FunctionExpression>(std::make_shared<GET>()));
            equalOriginal("LET a = function:GET, params:[]", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<FunctionExpression>(std::make_shared<DEC>()));
            equalOriginal("LET a = function:DEC, params:[]", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<FunctionExpression>(std::make_shared<CHECKSIG>()));
            equalOriginal("LET a = function:CHECKSIG, params:[]", ls->toString());
        }

        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<GlobalExpression>("@SCRIPT"));
            equalOriginal("LET a = global:@SCRIPT", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<GlobalExpression>("@BLKNUM"));
            equalOriginal("LET a = global:@BLKNUM", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<GlobalExpression>("@ADDRESS"));
            equalOriginal("LET a = global:@ADDRESS", ls->toString());
        }

        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<OperatorExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                            OperatorExpression::OPERATOR_ADD));
            equalOriginal("LET a = ( 5 + 5 )", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<OperatorExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(3)),
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(7)),
                            OperatorExpression::OPERATOR_MUL));
            equalOriginal("LET a = ( 3 * 7 )", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<OperatorExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)),
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(9)),
                            OperatorExpression::OPERATOR_SUB));
            equalOriginal("LET a = ( 1 - 9 )", ls->toString());
        }

        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<VariableExpression>("b"));
            equalOriginal("LET a = variable:b", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<VariableExpression>("c"));
            equalOriginal("LET a = variable:c", ls->toString());
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<VariableExpression>("d"));
            equalOriginal("LET a = variable:d", ls->toString());
        }

        {
            std::vector<ExpressionPtr> arl;
            arl.push_back(std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)));
            arl.push_back(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)));
            arl.push_back(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x12345678")));
            arl.push_back(std::make_shared<ConstantExpression>(std::make_shared<StringValue>("RETURN TRUE")));
            auto ls = std::make_shared<LETstatement>(arl,
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(42)));
            equalOriginal("LET ( TRUE 1 0x12345678 RETURN TRUE ) = 42", ls->toString());
        }
        {
            std::vector<ExpressionPtr> arl;
            arl.push_back(std::make_shared<BooleanExpression>(
                    std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)),
                    std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)),
                    BooleanExpression::BOOLEAN_AND));
            arl.push_back(std::make_shared<FunctionExpression>(std::make_shared<CHECKSIG>()));
            arl.push_back(std::make_shared<GlobalExpression>("@ADDRESS"));
            arl.push_back(std::make_shared<OperatorExpression>(
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                    OperatorExpression::OPERATOR_ADD));
            arl.push_back(std::make_shared<VariableExpression>("b"));
            auto ls = std::make_shared<LETstatement>(arl,
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(42)));
            equalOriginal("LET ( ( TRUE AND TRUE ) function:CHECKSIG, params:[] global:@ADDRESS ( 5 + 5 ) variable:b ) = 42", ls->toString());
        }
    }

    static void testExecution() {
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<BooleanExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)),
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)),
                            BooleanExpression::BOOLEAN_AND));

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("TRUE", dynamic_cast<BooleanValue&>(*ctr->getVariable("a")).toString());
            }
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<BooleanExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false)),
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false)),
                            BooleanExpression::BOOLEAN_EQ));

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("TRUE", dynamic_cast<BooleanValue&>(*ctr->getVariable("a")).toString());
            }
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<BooleanExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)),
                            std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false)),
                            BooleanExpression::BOOLEAN_NEQ));

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("TRUE", dynamic_cast<BooleanValue&>(*ctr->getVariable("a")).toString());
            }
        }

        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-1)));
            equalOriginal("LET a = -1", ls->toString());

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("-1", dynamic_cast<NumberValue&>(*ctr->getVariable("a")).toString());
            }
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0)));

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("0", dynamic_cast<NumberValue&>(*ctr->getVariable("a")).toString());
            }
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)));

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("1", dynamic_cast<NumberValue&>(*ctr->getVariable("a")).toString());
            }
        }

        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<OperatorExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                            OperatorExpression::OPERATOR_ADD));

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("10", dynamic_cast<NumberValue&>(*ctr->getVariable("a")).toString());
            }
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<OperatorExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(3)),
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(7)),
                            OperatorExpression::OPERATOR_MUL));

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("21", dynamic_cast<NumberValue&>(*ctr->getVariable("a")).toString());
            }
        }
        {
            auto ls = std::make_shared<LETstatement>("a",
                    std::make_shared<OperatorExpression>(
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)),
                            std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(9)),
                            OperatorExpression::OPERATOR_SUB));

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ls->execute(*ctr);
                equalOriginal("-8", dynamic_cast<NumberValue&>(*ctr->getVariable("a")).toString());
            }
        }

    }

int main() {
    testConstructors();testExecution();
    if(assertions!=30)throw std::runtime_error("Original assertion count changed");
    std::cout<<"ORIGINAL_LET_RESULT {\"class\":\"LETstatementTests\",\"tests\":2,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
