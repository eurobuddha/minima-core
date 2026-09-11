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
        auto CheckTrue = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false));
        auto CheckFalse = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false));

        auto ReturnTrue = std::make_shared<RETURNstatement>(CheckTrue);
        auto ReturnFalse = std::make_shared<RETURNstatement>(CheckFalse);

        std::vector<StatementPtr> CodeBlockReturnTrueAL;
        CodeBlockReturnTrueAL.push_back(ReturnTrue);
        auto CodeBlockReturnTrue = std::make_shared<StatementBlock>(CodeBlockReturnTrueAL);

        std::vector<StatementPtr> CodeBlockReturnFalseAL;
        CodeBlockReturnFalseAL.push_back(ReturnFalse);
        auto CodeBlockReturnFalse = std::make_shared<StatementBlock>(CodeBlockReturnTrueAL);

        auto ifs = std::make_shared<IFstatement>();
        ifs->addCondition(CheckFalse, CodeBlockReturnFalse);
        ifs->addCondition(CheckFalse, CodeBlockReturnFalse);
        ifs->addCondition(CheckTrue, CodeBlockReturnTrue);

        equalOriginal("IF FALSE, ELSEIF FALSE, ELSEIF FALSE", ifs->toString());

    }

    static void testExecution() {
        auto CheckTrue = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true));
        auto CheckFalse = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false));

        auto ReturnTrue = std::make_shared<RETURNstatement>(CheckTrue);
        auto ReturnFalse = std::make_shared<RETURNstatement>(CheckFalse);

        std::vector<StatementPtr> CodeBlockReturnTrueAL;
        CodeBlockReturnTrueAL.push_back(ReturnTrue);
        auto CodeBlockReturnTrue = std::make_shared<StatementBlock>(CodeBlockReturnTrueAL);

        std::vector<StatementPtr> CodeBlockReturnFalseAL;
        CodeBlockReturnFalseAL.push_back(ReturnFalse);
        auto CodeBlockReturnFalse = std::make_shared<StatementBlock>(CodeBlockReturnFalseAL);

        {
            auto ifs = std::make_shared<IFstatement>();

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

            {
                ifs->execute(*ctr);
            }
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
            equalOriginal(0, ctr->getNumberOfInstructions());
        }
        {
            auto ifs = std::make_shared<IFstatement>();
            ifs->addCondition(CheckTrue, CodeBlockReturnTrue);

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ifs->execute(*ctr);
            }
            equalOriginal(true, ctr->isSuccessSet());
            equalOriginal(true, ctr->isSuccess());
            equalOriginal(3, ctr->getNumberOfInstructions());
        }
        {
            auto ifs = std::make_shared<IFstatement>();
            ifs->addCondition(CheckTrue, CodeBlockReturnTrue);
            ifs->addCondition(CheckFalse, CodeBlockReturnFalse);

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ifs->execute(*ctr);
            }
            equalOriginal(true, ctr->isSuccessSet());
            equalOriginal(true, ctr->isSuccess());
            equalOriginal(3, ctr->getNumberOfInstructions());
        }
        {
            auto ifs = std::make_shared<IFstatement>();
            ifs->addCondition(CheckFalse, CodeBlockReturnFalse);
            ifs->addCondition(CheckTrue, CodeBlockReturnTrue);

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ifs->execute(*ctr);
            }
            equalOriginal(true, ctr->isSuccessSet());
            equalOriginal(true, ctr->isSuccess());
            equalOriginal(4, ctr->getNumberOfInstructions());
        }
        {
            auto ifs = std::make_shared<IFstatement>();
            ifs->addCondition(CheckTrue, CodeBlockReturnTrue);
            ifs->addCondition(CheckFalse, CodeBlockReturnFalse);
            ifs->addCondition(CheckFalse, CodeBlockReturnFalse);

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ifs->execute(*ctr);
            }
            equalOriginal(true, ctr->isSuccessSet());
            equalOriginal(true, ctr->isSuccess());
            equalOriginal(3, ctr->getNumberOfInstructions());
        }
        {
            auto ifs = std::make_shared<IFstatement>();
            ifs->addCondition(CheckFalse, CodeBlockReturnFalse);
            ifs->addCondition(CheckTrue, CodeBlockReturnTrue);
            ifs->addCondition(CheckFalse, CodeBlockReturnFalse);

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ifs->execute(*ctr);
            }
            equalOriginal(true, ctr->isSuccessSet());
            equalOriginal(true, ctr->isSuccess());
            equalOriginal(4, ctr->getNumberOfInstructions());
        }
        {
            auto ifs = std::make_shared<IFstatement>();
            ifs->addCondition(CheckFalse, CodeBlockReturnFalse);
            ifs->addCondition(CheckFalse, CodeBlockReturnFalse);
            ifs->addCondition(CheckTrue, CodeBlockReturnTrue);

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                ifs->execute(*ctr);
            }
            equalOriginal(true, ctr->isSuccessSet());
            equalOriginal(true, ctr->isSuccess());
            equalOriginal(5, ctr->getNumberOfInstructions());
        }
    }
int main() {
    testConstructors();testExecution();
    if(assertions!=22)throw std::runtime_error("Original assertion count changed");
    std::cout<<"ORIGINAL_STATEMENTS_RESULT {\"class\":\"IFstatementTests\",\"tests\":2,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
