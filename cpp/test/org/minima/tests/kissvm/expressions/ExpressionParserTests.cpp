// All active pinned original cases, operator loops and exact type assertions.
#include "../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/expressions/ExpressionParser.hpp"
#include "org/minima/kissvm/expressions/BooleanExpression.hpp"
#include "org/minima/kissvm/expressions/OperatorExpression.hpp"
#include "org/minima/kissvm/statements/StatementParser.hpp"
#include <typeinfo>
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
template<class T> static bool exactType(const ExpressionPtr& expression) {const auto* value=expression.get();return value && typeid(*value)==typeid(T);}
static int parseCalls=0;
static ExpressionPtr parseExpressionOriginal(const std::vector<ScriptToken>& tokens,int depth) {++parseCalls;return ExpressionParser::getExpression(tokens,depth);}
static std::shared_ptr<StatementBlock> parseStatementsOriginal(const std::vector<ScriptToken>& tokens,int depth) {++parseCalls;return StatementParser::parseTokens(tokens,depth);}
template<class A,class B> static void equalOriginal(const A& expected,const B& actual) {require(expected==actual);}
template<class F> static void rejectOriginal(F action) {bool caught=false;try {action();}catch(const MinimaParseException&) {caught=true;}require(caught);}

    static void testBooleanExpressions() {

        std::vector<std::string> Ops = {"AND", "NAND", "OR", "NOR", "XOR", "NXOR", "EQ", "NEQ", "LT", "LTE", "GT", "GTE"};
        for (const auto& Op : Ops) {
            {
                {
                    std::string Script = "TRUE " + Op + " TRUE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( TRUE " + Op + " TRUE )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "TRUE " + Op + " FALSE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( TRUE " + Op + " FALSE )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "FALSE " + Op + " TRUE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( FALSE " + Op + " TRUE )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "FALSE " + Op + " FALSE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( FALSE " + Op + " FALSE )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0x1 " + Op + " 0x1";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 0x01 " + Op + " 0x01 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0x1 " + Op + " 0x0";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 0x01 " + Op + " 0x00 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0x0 " + Op + " 0x1";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 0x00 " + Op + " 0x01 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0x0 " + Op + " 0x0";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 0x00 " + Op + " 0x00 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "1 " + Op + " 1";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 1 " + Op + " 1 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "1 " + Op + " 0";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 1 " + Op + " 0 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0 " + Op + " 1";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 0 " + Op + " 1 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0 " + Op + " 0";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 0 " + Op + " 0 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "1.5 " + Op + " 1.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 1.5 " + Op + " 1.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "1.5 " + Op + " 0.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 1.5 " + Op + " 0.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0.5 " + Op + " 1.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 0.5 " + Op + " 1.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0.5 " + Op + " 0.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( 0.5 " + Op + " 0.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "-1.5 " + Op + " -1.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( -1.5 " + Op + " -1.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "-1.5 " + Op + " -0.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( -1.5 " + Op + " -0.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "-0.5 " + Op + " -1.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( -0.5 " + Op + " -1.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "-0.5 " + Op + " -0.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( -0.5 " + Op + " -0.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "a " + Op + " b";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( variable:a " + Op + " variable:b )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "[Hello World] " + Op + " [Hello World]";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( Hello World " + Op + " Hello World )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "LEN(0x12345678) " + Op + " LEN(0x12345678)";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( function:LEN, params:[0x12345678] " + Op + " function:LEN, params:[0x12345678] )", expr->toString());
                }
            }

            {
                rejectOriginal([&] {
                    std::string Script = "LEN 0x12345678 " + Op + " LEN 0x12345678";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                });
            }

            {
                {
                    std::string Script = "@BLKNUM " + Op + " @BLKTIME";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( global:@BLKNUM " + Op + " global:@BLKTIME )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "((a) " + Op + " (((b))))";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( variable:a " + Op + " variable:b )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "((a) " + Op + " (((b))))";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<BooleanExpression>(expr));
                    equalOriginal("( variable:a " + Op + " variable:b )", expr->toString());
                }
            }
            {
                rejectOriginal([&] {
                    std::string Script = "(A " + Op + " B B";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                });
            }

        }
    }

    static void testBooleanExpressionsUnary() {

        {
            {
                std::string Script = "NOT TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( TRUE )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT FALSE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( FALSE )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT 0x1";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( 0x01 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT 0x0";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( 0x00 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT 1";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( 1 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT 0";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( 0 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT 1.5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( 1.5 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT 0.5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( 0.5 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT -1.5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( -1.5 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT -0.5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( -0.5 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT a";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( variable:a )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT [Hello World]";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( Hello World )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NOT LEN(0x12345678)";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( function:LEN, params:[0x12345678] )", expr->toString());
            }
        }

        {
            rejectOriginal([&] {
                std::string Script = "NOT LEN 0x12345678";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
            });
        }

        {
            {
                std::string Script = "NOT @BLKNUM";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<BooleanExpression>(expr));
                equalOriginal("NOT ( global:@BLKNUM )", expr->toString());
            }
        }

        {
            rejectOriginal([&] {
                std::string Script = "NOT ((A)";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
            });
        }
        {
            rejectOriginal([&] {
                std::string Script = "NOT (A";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
            });
        }

        {
            rejectOriginal([&] {
                std::string Script = "NOT (A";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
            });
        }
    }

    static void testOperatorExpressions() {

        std::vector<std::string> Ops = {"+", "-", "*", "/", "<<", ">>", "%", "&", "|", "^"};
        for (const auto& Op : Ops) {
            {
                {
                    std::string Script = "TRUE " + Op + " TRUE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( TRUE " + Op + " TRUE )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "TRUE " + Op + " FALSE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( TRUE " + Op + " FALSE )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "FALSE " + Op + " TRUE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( FALSE " + Op + " TRUE )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "FALSE " + Op + " FALSE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( FALSE " + Op + " FALSE )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0x1 " + Op + " 0x1";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 0x01 " + Op + " 0x01 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0x1 " + Op + " 0x0";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 0x01 " + Op + " 0x00 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0x0 " + Op + " 0x1";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 0x00 " + Op + " 0x01 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0x0 " + Op + " 0x0";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 0x00 " + Op + " 0x00 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "1 " + Op + " 1";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 1 " + Op + " 1 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "1 " + Op + " 0";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 1 " + Op + " 0 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0 " + Op + " 1";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 0 " + Op + " 1 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0 " + Op + " 0";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 0 " + Op + " 0 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "1.5 " + Op + " 1.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 1.5 " + Op + " 1.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "1.5 " + Op + " 0.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 1.5 " + Op + " 0.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0.5 " + Op + " 1.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 0.5 " + Op + " 1.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "0.5 " + Op + " 0.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( 0.5 " + Op + " 0.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "-1.5 " + Op + " -1.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( -1.5 " + Op + " -1.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "-1.5 " + Op + " -0.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( -1.5 " + Op + " -0.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "-0.5 " + Op + " -1.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( -0.5 " + Op + " -1.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "-0.5 " + Op + " -0.5";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( -0.5 " + Op + " -0.5 )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "a " + Op + " b";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( variable:a " + Op + " variable:b )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "[Hello World] " + Op + " [Hello World]";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( Hello World " + Op + " Hello World )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "LEN(0x12345678) " + Op + " LEN(0x12345678)";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( function:LEN, params:[0x12345678] " + Op + " function:LEN, params:[0x12345678] )", expr->toString());
                }
            }

            {
                rejectOriginal([&] {
                    std::string Script = "LEN 0x12345678 " + Op + " LEN 0x12345678";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                });
            }

            {
                {
                    std::string Script = "@BLKNUM " + Op + " @BLKTIME";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( global:@BLKNUM " + Op + " global:@BLKTIME )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "((a) " + Op + " (((b))))";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( variable:a " + Op + " variable:b )", expr->toString());
                }
            }

            {
                {
                    std::string Script = "((a) " + Op + " (((b))))";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                    require(exactType<OperatorExpression>(expr));
                    equalOriginal("( variable:a " + Op + " variable:b )", expr->toString());
                }
            }
            {
                rejectOriginal([&] {
                    std::string Script = "(A " + Op + " B B";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto expr = parseExpressionOriginal(tokens,0);
                });
            }

        }
    }

    static void testOperatorExpressionsUnary() {

        {
            {
                std::string Script = "NEG TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( TRUE )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG FALSE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( FALSE )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG 0x1";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( 0x01 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG 0x0";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( 0x00 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG 1";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( 1 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG 0";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( 0 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG 1.5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( 1.5 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG 0.5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( 0.5 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG -1.5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( -1.5 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG -0.5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( -0.5 )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG a";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( variable:a )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG [Hello World]";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( Hello World )", expr->toString());
            }
        }

        {
            {
                std::string Script = "NEG LEN(0x12345678)";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( function:LEN, params:[0x12345678] )", expr->toString());
            }
        }

        {
            rejectOriginal([&] {
                std::string Script = "NEG LEN 0x12345678";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
            });
        }

        {
            {
                std::string Script = "NEG @BLKNUM";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
                require(exactType<OperatorExpression>(expr));
                equalOriginal(" - ( global:@BLKNUM )", expr->toString());
            }
        }

        {
            rejectOriginal([&] {
                std::string Script = "NEG ((A)";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
            });
        }
        {
            rejectOriginal([&] {
                std::string Script = "NEG (A";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
            });
        }

        {
            rejectOriginal([&] {
                std::string Script = "NEG (A";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto expr = parseExpressionOriginal(tokens,0);
            });
        }
    }

    static void testOperatorExpressionsExceptions() {
        rejectOriginal([&] {
            std::string Script = "A + B";
            auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
            tokens.emplace_back(-1, "@#$");
            auto expr = parseExpressionOriginal(tokens,0);
        });

        rejectOriginal([&] {
            std::vector<ScriptToken> tokens;
            tokens.emplace_back(ScriptToken::TOKEN_OPENBRACKET, "(");
            tokens.emplace_back(-1, "@#$");
            tokens.emplace_back(ScriptToken::TOKEN_CLOSEBRACKET, ")");
            auto expr = parseExpressionOriginal(tokens,0);
        });
    }
int main() {
    testBooleanExpressions();testBooleanExpressionsUnary();testOperatorExpressions();testOperatorExpressionsUnary();testOperatorExpressionsExceptions();
    if(assertions!=1254||parseCalls!=654)throw std::runtime_error("Original parser accounting changed: "+std::to_string(assertions)+" / "+std::to_string(parseCalls));
    std::cout<<"ORIGINAL_PARSERS_RESULT {\"class\":\"ExpressionParserTests\",\"tests\":5,\"assertions\":"<<assertions<<",\"parse_calls\":"<<parseCalls<<",\"passed\":true}\n";
}
