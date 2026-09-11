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
        auto Empty = std::make_shared<ConstantExpression>(std::make_shared<HexValue>(""));
        auto Random = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0xBC05B9280323A07AE6B2C6FA183F20991D38D01BC91F02C439CB5A33113E106C"));

        auto ms1 = std::make_shared<MASTstatement>(Empty);
        equalOriginal("MAST ", ms1->toString());

        auto ms2 = std::make_shared<MASTstatement>(Random);
        equalOriginal("MAST 0xBC05B9280323A07AE6B2C6FA183F20991D38D01BC91F02C439CB5A33113E106C", ms2->toString());
    }

    static void testExecution() {
        {
            {
                auto sp = std::make_shared<ScriptProof>("");
                auto ms = std::make_shared<MASTstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>(sp->getAddress().getAddressData())));

                auto w = std::make_shared<Witness>();
                w->addScript(*sp);
                auto ctr = std::make_shared<Contract>("", std::string{}, w, std::make_shared<Transaction>(), std::vector<StateVariable>{});
                {
                    ms->execute(*ctr);
                }
                equalOriginal(false, ctr->isSuccessSet());
                equalOriginal(false, ctr->isSuccess());
                equalOriginal(1, ctr->getNumberOfInstructions());
            }
        }
        {
            {
                auto sp = std::make_shared<ScriptProof>("RETURN TRUE");
                auto ms = std::make_shared<MASTstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>(sp->getAddress().getAddressData())));

                auto w = std::make_shared<Witness>();
                w->addScript(*sp);
                auto ctr = std::make_shared<Contract>("", std::string{}, w, std::make_shared<Transaction>(), std::vector<StateVariable>{});
                {
                    ms->execute(*ctr);
                }
                equalOriginal(true, ctr->isSuccessSet());
                equalOriginal(true, ctr->isSuccess());
                equalOriginal(3, ctr->getNumberOfInstructions());
            }
        }
        {
            {
                auto sp = std::make_shared<ScriptProof>("RETURN FALSE");
                auto ms = std::make_shared<MASTstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>(sp->getAddress().getAddressData())));

                auto w = std::make_shared<Witness>();
                w->addScript(*sp);
                auto ctr = std::make_shared<Contract>("", std::string{}, w, std::make_shared<Transaction>(), std::vector<StateVariable>{});
                {
                    ms->execute(*ctr);
                }
                equalOriginal(true, ctr->isSuccessSet());
                equalOriginal(false, ctr->isSuccess());
                equalOriginal(3, ctr->getNumberOfInstructions());
            }
        }
        {
            {
                auto sp = std::make_shared<ScriptProof>("Hello World");
                auto ms = std::make_shared<MASTstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>(sp->getAddress().getAddressData())));

                auto w = std::make_shared<Witness>();
                w->addScript(*sp);
                auto ctr = std::make_shared<Contract>("", std::string{}, w, std::make_shared<Transaction>(), std::vector<StateVariable>{});
                rejectStatement([&] {
                    ms->execute(*ctr);
                });
                equalOriginal(false, ctr->isSuccessSet());
                equalOriginal(false, ctr->isSuccess());
                equalOriginal(1, ctr->getNumberOfInstructions());
            }
        }
        {
            {
                auto sp = std::make_shared<ScriptProof>("RETURN TRUE");
                auto ms = std::make_shared<MASTstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>(sp->getAddress().getAddressData())));

                auto w = std::make_shared<Witness>();
                auto ctr = std::make_shared<Contract>("", std::string{}, w, std::make_shared<Transaction>(), std::vector<StateVariable>{});
                rejectStatement([&] {
                    ms->execute(*ctr);
                });
                equalOriginal(false, ctr->isSuccessSet());
                equalOriginal(false, ctr->isSuccess());
                equalOriginal(1, ctr->getNumberOfInstructions());
            }
        }
    }
int main() {
    testConstructors();testExecution();
    if(assertions!=19)throw std::runtime_error("Original assertion count changed");
    std::cout<<"ORIGINAL_STATEMENTS_RESULT {\"class\":\"MASTstatementTests\",\"tests\":2,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
