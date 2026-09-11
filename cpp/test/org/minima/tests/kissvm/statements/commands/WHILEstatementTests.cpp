// Retains every active pinned original fixture and assertion.
#include "../../../../../../../support/OriginalFunctionTestSupport.hpp"
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
        auto WhileCheck = std::make_shared<BooleanExpression>(
                std::make_shared<VariableExpression>("a"),
                std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                BooleanExpression::BOOLEAN_LT);

        auto oe1 = std::make_shared<OperatorExpression>(
                std::make_shared<VariableExpression>("a"),
                std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)),
                OperatorExpression::OPERATOR_ADD);

        auto ls2 = std::make_shared<LETstatement>("a", oe1);

        std::vector<StatementPtr> WhileCodeBlockAL;
        WhileCodeBlockAL.push_back(ls2);

        auto WhileCodeBlock = std::make_shared<StatementBlock>(WhileCodeBlockAL);

        auto ws = std::make_shared<WHILEstatement>(WhileCheck, WhileCodeBlock);

        equalOriginal("WHILE ( variable:a LT 5 )", ws->toString());

    }

    static void testExecution() {
        {
            std::vector<StatementPtr> CodeBlockAL;

            auto ls1 = std::make_shared<LETstatement>("a", std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0)));

            CodeBlockAL.push_back(ls1);

            auto WhileCheck = std::make_shared<BooleanExpression>(
                    std::make_shared<VariableExpression>("a"),
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                    BooleanExpression::BOOLEAN_LT);

            auto oe1 = std::make_shared<OperatorExpression>(
                    std::make_shared<VariableExpression>("a"),
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)),
                    OperatorExpression::OPERATOR_ADD);

            auto ls2 = std::make_shared<LETstatement>("a", oe1);

            std::vector<StatementPtr> WhileCodeBlockAL;
            WhileCodeBlockAL.push_back(ls2);

            auto WhileCodeBlock = std::make_shared<StatementBlock>(WhileCodeBlockAL);

            auto ws = std::make_shared<WHILEstatement>(WhileCheck, WhileCodeBlock);

            CodeBlockAL.push_back(ws);

            auto CodeBlock = std::make_shared<StatementBlock>(CodeBlockAL);

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                CodeBlock->run(*ctr);
            }
        }
        {
            std::vector<StatementPtr> CodeBlockAL;

            auto ls1 = std::make_shared<LETstatement>("a", std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0)));

            CodeBlockAL.push_back(ls1);

            auto WhileCheck = std::make_shared<BooleanExpression>(
                    std::make_shared<VariableExpression>("a"),
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(5)),
                    BooleanExpression::BOOLEAN_LT);

            auto oe1 = std::make_shared<OperatorExpression>(
                    std::make_shared<VariableExpression>("a"),
                    std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)),
                    OperatorExpression::OPERATOR_ADD);

            auto ls2 = std::make_shared<LETstatement>("a", oe1);

            std::vector<StatementPtr> WhileCodeBlockAL;
            WhileCodeBlockAL.push_back(ls2);

            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)));
            WhileCodeBlockAL.push_back(rs);

            auto WhileCodeBlock = std::make_shared<StatementBlock>(WhileCodeBlockAL);

            auto ws = std::make_shared<WHILEstatement>(WhileCheck, WhileCodeBlock);

            CodeBlockAL.push_back(ws);

            auto CodeBlock = std::make_shared<StatementBlock>(CodeBlockAL);

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                CodeBlock->run(*ctr);
            }
        }
    }

int main() {
    testConstructors();testExecution();
    if(assertions!=1)throw std::runtime_error("Original assertion count changed");
    std::cout<<"ORIGINAL_STATEMENTS_RESULT {\"class\":\"WHILEstatementTests\",\"tests\":2,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
