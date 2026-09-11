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
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true)));
            equalOriginal("RETURN TRUE", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false)));
            equalOriginal("RETURN FALSE", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("")));
            equalOriginal("RETURN ", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x00")));
            equalOriginal("RETURN 0x00", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x12345678")));
            equalOriginal("RETURN 0x12345678", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")));
            equalOriginal("RETURN 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-1)));
            equalOriginal("RETURN -1", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0)));
            equalOriginal("RETURN 0", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1)));
            equalOriginal("RETURN 1", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<StringValue>("")));
            equalOriginal("RETURN ", rs->toString());
        }
        {
            auto rs = std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(std::make_shared<StringValue>("Hello World")));
            equalOriginal("RETURN Hello World", rs->toString());
        }
    }

    static void testExecution() {
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                rs->execute(*ctr);
            }
            equalOriginal(true, ctr->isSuccessSet());
            equalOriginal(true, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            {
                rs->execute(*ctr);
            }
            equalOriginal(true, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<HexValue>(""));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x00"));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x12345678"));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-1));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<StringValue>(""));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
        {
            auto ce = std::make_shared<ConstantExpression>(std::make_shared<StringValue>("Hello World"));
            auto rs = std::make_shared<RETURNstatement>(ce);
            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});
            rejectStatement([&] {
            	rs->execute(*ctr);
            });
            equalOriginal(false, ctr->isSuccessSet());
            equalOriginal(false, ctr->isSuccess());
        }
    }

int main() {
    testConstructors();testExecution();
    if(assertions!=42)throw std::runtime_error("Original assertion count changed");
    std::cout<<"ORIGINAL_STATEMENTS_RESULT {\"class\":\"RETURNstatementTests\",\"tests\":2,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
