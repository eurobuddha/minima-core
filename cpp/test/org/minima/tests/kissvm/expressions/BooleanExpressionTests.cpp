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

    	{
            auto cet = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true));
            auto cef = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false));

            std::shared_ptr<BooleanExpression> be;

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

            be = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_AND);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cet, cef, BooleanExpression::BOOLEAN_AND);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cef, cet, BooleanExpression::BOOLEAN_AND);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cef, cef, BooleanExpression::BOOLEAN_AND);
            require(be->getBooleanValue(*ctr)->isFalse());

            be = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NAND);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cet, cef, BooleanExpression::BOOLEAN_NAND);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cef, cet, BooleanExpression::BOOLEAN_NAND);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cef, cef, BooleanExpression::BOOLEAN_NAND);
            require(be->getBooleanValue(*ctr)->isTrue());

            be = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_OR);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cet, cef, BooleanExpression::BOOLEAN_OR);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cef, cet, BooleanExpression::BOOLEAN_OR);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cef, cef, BooleanExpression::BOOLEAN_OR);
            require(be->getBooleanValue(*ctr)->isFalse());

            be = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NOR);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cet, cef, BooleanExpression::BOOLEAN_NOR);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cef, cet, BooleanExpression::BOOLEAN_NOR);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cef, cef, BooleanExpression::BOOLEAN_NOR);
            require(be->getBooleanValue(*ctr)->isTrue());

            be = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_XOR);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cet, cef, BooleanExpression::BOOLEAN_XOR);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cef, cet, BooleanExpression::BOOLEAN_XOR);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cef, cef, BooleanExpression::BOOLEAN_XOR);
            require(be->getBooleanValue(*ctr)->isFalse());

            be = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NXOR);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(cet, cef, BooleanExpression::BOOLEAN_NXOR);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cef, cet, BooleanExpression::BOOLEAN_NXOR);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(cef, cef, BooleanExpression::BOOLEAN_NXOR);
            require(be->getBooleanValue(*ctr)->isTrue());

            be = std::make_shared<BooleanExpression>(cet, BooleanExpression::BOOLEAN_NOT);
            require(be->getBooleanValue(*ctr)->isFalse());

            be = std::make_shared<BooleanExpression>(cef, BooleanExpression::BOOLEAN_NOT);
            require(be->getBooleanValue(*ctr)->isTrue());
        }

        {
            auto cet = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x01"));
            auto cef = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x00"));

            std::shared_ptr<BooleanExpression> be;

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_AND);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NAND);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_OR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_XOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NXOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, BooleanExpression::BOOLEAN_NOT);
	            be1->getValue(*ctr);
	        });
        }

        {
            auto cet = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1));
            auto cef = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0));

            std::shared_ptr<BooleanExpression> be;

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_AND);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NAND);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_OR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_XOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NXOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, BooleanExpression::BOOLEAN_NOT);
	            be1->getValue(*ctr);
	        });
        }

        {
            auto cet = std::make_shared<ConstantExpression>(std::make_shared<StringValue>("hello world"));
            auto cef = std::make_shared<ConstantExpression>(std::make_shared<StringValue>(""));

            std::shared_ptr<BooleanExpression> be;

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_AND);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NAND);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_OR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_XOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, cet, BooleanExpression::BOOLEAN_NXOR);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(cet, BooleanExpression::BOOLEAN_NOT);
	            be1->getValue(*ctr);
	        });
        }

        {
            auto ce1 = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false));
            auto ce2 = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true));

            std::shared_ptr<BooleanExpression> be;

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

            be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce2, ce2, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isFalse());

            be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isTrue());

            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LT);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LTE);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GT);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GTE);
	            be1->getValue(*ctr);
	        });

            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, INT32_MIN);
  		  	rejectOriginal<ExecutionException>([&] {
  		  		be1->getValue(*ctr);
  		  	});
        }

        {
            auto ce1 = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x01"));
            auto ce2 = std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x02"));

            std::shared_ptr<BooleanExpression> be;

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

            be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce2, ce2, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isFalse());

            be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isTrue());

            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LT);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LTE);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GT);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GTE);
	            be1->getValue(*ctr);
	        });

            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, INT32_MIN);
  		  	rejectOriginal<ExecutionException>([&] {
  		  		be1->getValue(*ctr);
  		  	});
        }

        {
            auto ce1 = std::make_shared<ConstantExpression>(std::make_shared<StringValue>("hello world"));
            auto ce2 = std::make_shared<ConstantExpression>(std::make_shared<StringValue>(""));

            std::shared_ptr<BooleanExpression> be;

            auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

            be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce2, ce2, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_EQ);
            require(be->getBooleanValue(*ctr)->isFalse());

            be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isFalse());
            be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isTrue());
            be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_NEQ);
            require(be->getBooleanValue(*ctr)->isTrue());

            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LT);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LTE);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GT);
	            be1->getValue(*ctr);
	        });
            rejectOriginal<std::invalid_argument>([&] {
	            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GTE);
	            be1->getValue(*ctr);
	        });

            auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, INT32_MIN);
  		  	rejectOriginal<ExecutionException>([&] {
  		  		be1->getValue(*ctr);
  		  	});
        }

	    {
		  auto ce1 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1));
		  auto ce2 = std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(2));

		  std::shared_ptr<BooleanExpression> be;

		  auto ctr = std::make_shared<Contract>("", std::string{}, std::make_shared<Witness>(), std::make_shared<Transaction>(), std::vector<StateVariable>{});

		  be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_EQ);
		  require(be->getBooleanValue(*ctr)->isTrue());
		  be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_EQ);
		  require(be->getBooleanValue(*ctr)->isFalse());
		  be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_EQ);
		  require(be->getBooleanValue(*ctr)->isFalse());

		  be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_NEQ);
		  require(be->getBooleanValue(*ctr)->isFalse());
		  be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NEQ);
		  require(be->getBooleanValue(*ctr)->isTrue());
		  be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_NEQ);
		  require(be->getBooleanValue(*ctr)->isTrue());

		  be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_LT);
		  require(be->getBooleanValue(*ctr)->isFalse());
		  be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LT);
		  require(be->getBooleanValue(*ctr)->isTrue());
		  be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_LT);
		  require(be->getBooleanValue(*ctr)->isFalse());

		  be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_LTE);
		  require(be->getBooleanValue(*ctr)->isTrue());
		  be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LTE);
		  require(be->getBooleanValue(*ctr)->isTrue());
		  be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_LTE);
		  require(be->getBooleanValue(*ctr)->isFalse());

		  be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_GT);
		  require(be->getBooleanValue(*ctr)->isFalse());
		  be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GT);
		  require(be->getBooleanValue(*ctr)->isFalse());
		  be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_GT);
		  require(be->getBooleanValue(*ctr)->isTrue());

		  be = std::make_shared<BooleanExpression>(ce1, ce1, BooleanExpression::BOOLEAN_GTE);
		  require(be->getBooleanValue(*ctr)->isTrue());
		  be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GTE);
		  require(be->getBooleanValue(*ctr)->isFalse());
		  be = std::make_shared<BooleanExpression>(ce2, ce1, BooleanExpression::BOOLEAN_GTE);
		  require(be->getBooleanValue(*ctr)->isTrue());

		  auto be1 = std::make_shared<BooleanExpression>(ce1, ce2, INT32_MIN);
		  rejectOriginal<ExecutionException>([&] {
		      be1->getValue(*ctr);
		  });

	  }
    }

    static void testToString() {
        auto ce1 = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true));
        auto ce2 = std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(false));

        std::shared_ptr<BooleanExpression> be;

        std::string exp_s;
        std::string obj_s;

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_AND);
        exp_s = "( " + ce1->toString() + " AND " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NAND);
        exp_s = "( " + ce1->toString() + " NAND " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_OR);
        exp_s = "( " + ce1->toString() + " OR " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NOR);
        exp_s = "( " + ce1->toString() + " NOR " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_XOR);
        exp_s = "( " + ce1->toString() + " XOR " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NXOR);
        exp_s = "( " + ce1->toString() + " NXOR " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_EQ);
        exp_s = "( " + ce1->toString() + " EQ " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NEQ);
        exp_s = "( " + ce1->toString() + " NEQ " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LT);
        exp_s = "( " + ce1->toString() + " LT " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_LTE);
        exp_s = "( " + ce1->toString() + " LTE " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GT);
        exp_s = "( " + ce1->toString() + " GT " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_GTE);
        exp_s = "( " + ce1->toString() + " GTE " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, BooleanExpression::BOOLEAN_NOT);
        exp_s = "NOT ( " + ce1->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);

        be = std::make_shared<BooleanExpression>(ce1, ce2, INT32_MIN);
        exp_s = "( " + ce1->toString() + " ERROR " + ce2->toString() + " )";
        obj_s = be->toString();
        equalOriginal("should be equal ", exp_s, obj_s);
    }

int main() {
    testConstructors();testToString();
    if(assertions!=116)throw std::runtime_error("Original assertion count changed");
    std::cout<<"ORIGINAL_EXPRESSIONS_RESULT {\"class\":\"BooleanExpressionTests\",\"tests\":2,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
