// All active pinned original cases, operator loops and exact type assertions.
#include "../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/expressions/ExpressionParser.hpp"
#include "org/minima/kissvm/expressions/BooleanExpression.hpp"
#include "org/minima/kissvm/expressions/OperatorExpression.hpp"
#include "org/minima/kissvm/statements/StatementParser.hpp"
#include <typeinfo>
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
static int parseCalls=0;
static ExpressionPtr parseExpressionOriginal(const std::vector<ScriptToken>& tokens,int depth) {++parseCalls;return ExpressionParser::getExpression(tokens,depth);}
static std::shared_ptr<StatementBlock> parseStatementsOriginal(const std::vector<ScriptToken>& tokens,int depth) {++parseCalls;return StatementParser::parseTokens(tokens,depth);}
template<class A,class B> static void equalOriginal(const A& expected,const B& actual) {require(expected==actual);}
template<class F> static void rejectOriginal(F action) {bool caught=false;try {action();}catch(const MinimaParseException&) {caught=true;}require(caught);}

    static void testConstructors() {
        {
            {
                std::string Script = "";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }
    }

    static void testParsingLETArrays() {
        {
            {
                std::string Script = "LET (a) = TRUE LET (b) = FALSE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            rejectOriginal([&] {
                std::string Script = "LET (a = TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }

        {
            rejectOriginal([&] {
                std::string Script = "LET (a * ) = TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }

        {
            rejectOriginal([&] {
                std::string Script = "LET ( ) = 5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }

        {
            {
                std::string Script = "LET (a b c d e) = TRUE LET (b c d e f) = FALSE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            {
                std::string Script = "LET (TRUE 1 0x123456 d BOOL(1)) = TRUE LET (FALSE -1 0xABCDEF d SCRIPT(0xABCDEF)) = FALSE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }
    }

    static void testParsingLET() {
        {
            {
                std::string Script = "LET a = TRUE LET b = FALSE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            {
                std::string Script = "LET a = 0x12345678 LET b = 0x87654321";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            {
                std::string Script = "LET a = -1 LET b = 0 LET c = 1 LET d = 9999999999 LET e = -0.000005";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            rejectOriginal([&] {
                std::vector<ScriptToken> tokens;
                tokens.emplace_back(ScriptToken::TOKEN_COMMAND, "LET");
                tokens.emplace_back(ScriptToken::TOKEN_VARIABLE, "a");
                tokens.emplace_back(ScriptToken::TOKEN_VARIABLE, "+");
                tokens.emplace_back(ScriptToken::TOKEN_VALUE, "5");
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }

        {
            {
                std::vector<ScriptToken> tokens;
                tokens.emplace_back(ScriptToken::TOKEN_COMMAND, "LET");
                tokens.emplace_back(ScriptToken::TOKEN_VARIABLE, "a");
                tokens.emplace_back(ScriptToken::TOKEN_VARIABLE, "=");
                tokens.emplace_back(ScriptToken::TOKEN_VALUE, "5");
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
        	rejectOriginal([&] {
                std::vector<ScriptToken> tokens;
                tokens.emplace_back(ScriptToken::TOKEN_COMMAND, "LET");
                tokens.emplace_back(ScriptToken::TOKEN_VARIABLE, "a");
                tokens.emplace_back(ScriptToken::TOKEN_OPERATOR, "+");
                tokens.emplace_back(ScriptToken::TOKEN_VALUE, "5");
                auto sb = parseStatementsOriginal(tokens,0);
        	});
        }

        {
            {
                std::vector<ScriptToken> tokens;
                tokens.emplace_back(ScriptToken::TOKEN_COMMAND, "LET");
                tokens.emplace_back(ScriptToken::TOKEN_VARIABLE, "a");
                tokens.emplace_back(ScriptToken::TOKEN_OPERATOR, "=");
                tokens.emplace_back(ScriptToken::TOKEN_VALUE, "5");
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            rejectOriginal([&] {
                std::vector<ScriptToken> tokens;
                tokens.emplace_back(ScriptToken::TOKEN_COMMAND, "LET");
                tokens.emplace_back(ScriptToken::TOKEN_OPERATOR, "=");
                tokens.emplace_back(ScriptToken::TOKEN_VALUE, "5");
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }
    }

    static void testParsingEXEC() {
        {
            {
                std::string Script = "EXEC [RETURN TRUE]";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            {
                std::string Script = "EXEC [RETURN TRUE] + 543 RETURN TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
        	rejectOriginal([&] {
                std::string Script = "EXEC RETURN";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }
    }

    static void testParsingMAST() {
        {
            {
                std::string Script = "MAST 1 + 5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            {
                std::string Script = "MAST TRUE + 543";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
        	rejectOriginal([&] {
                std::string Script = "MAST RETURN TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }
    }

    static void testParsingIF() {
        {
            std::vector<std::string> Exps = {"TRUE", "FALSE", "0x01", "0x00", "1", "0", "1.5", "0.0", "[hello world]", "[]"};
            for (const auto& Exp : Exps) {
                {
                    std::string Script = "IF " + Exp + " THEN RETURN TRUE ENDIF";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto sb = parseStatementsOriginal(tokens,0);
                }

                {
                    std::string Script = "IF " + Exp + " THEN RETURN TRUE ELSE RETURN FALSE ENDIF";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto sb = parseStatementsOriginal(tokens,0);
                }

                {
                    std::string Script = "IF " + Exp + " THEN RETURN TRUE ELSEIF FALSE THEN RETURN FALSE ELSE RETURN FALSE ENDIF";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto sb = parseStatementsOriginal(tokens,0);
                }

                {
                    std::string Script = "IF " + Exp + " THEN RETURN TRUE ELSE IF FALSE THEN RETURN FALSE ELSE RETURN FALSE ENDIF ENDIF";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto sb = parseStatementsOriginal(tokens,0);
                }
            }
        }

        {
            rejectOriginal([&] {
                std::string Script = "IF ( ) THEN RETURN TRUE ENDIF";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }
        {
            rejectOriginal([&] {
                std::string Script = "IF ( A + ) THEN RETURN TRUE ENDIF";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }

        {
            rejectOriginal([&] {
                std::string Script = "IF FALSE THEN RETURN TRUE HELLO FALSE THEN RETURN FALSE ELSE RETURN FALSE ENDIF";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }

    }

    static void testParsingWHILE() {
        {
            std::vector<std::string> Exps = {"TRUE", "FALSE", "0x01", "0x00", "1", "0", "1.5", "0.0", "[hello world]", "[]"};
            for (const auto& Exp : Exps) {
                {
                    std::string Script = "WHILE " + Exp + " DO RETURN TRUE ENDWHILE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto sb = parseStatementsOriginal(tokens,0);
                }
            }
        }
        {
            std::vector<std::string> Exps = {"TRUE", "FALSE", "0x01", "0x00", "1", "0", "1.5", "0.0", "[hello world]", "[]"};
            for (const auto& Exp : Exps) {
                {
                    std::string Script = "WHILE " + Exp + " DO WHILE " + Exp + " DO RETURN TRUE ENDWHILE ENDWHILE";
                    auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                    auto sb = parseStatementsOriginal(tokens,0);
                }
            }
        }
    }

    static void testParsingASSERT() {
        {
            {
                std::string Script = "ASSERT TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            {
                std::string Script = "ASSERT FALSE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
        	rejectOriginal([&] {
                std::string Script = "ASSERT RETURN TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }

        {
        	rejectOriginal([&] {
                std::string Script = "ASSERT RETURN A + @BLKNUM";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }

        {
            {
                std::string Script = "ASSERT [RETURN TRUE] + 543 RETURN TRUE";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }
    }

    static void testParsingRETURN() {
        {
            {
                std::string Script = "RETURN 1 + 5";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
            {
                std::string Script = "RETURN TRUE + 543";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            }
        }

        {
        	rejectOriginal([&] {
                std::string Script = "RETURN MAST 1";
                auto tokens = ScriptToken::tokenize(Contract::cleanScript(Script));
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }
    }

    static void testParsingException() {
        {
            rejectOriginal([&] {
                std::vector<ScriptToken> tokens;
                tokens.emplace_back(ScriptToken::TOKEN_COMMAND, "HELLO");
                auto sb = parseStatementsOriginal(tokens,0);
            });
        }
    }
int main() {
    testConstructors();testParsingLETArrays();testParsingLET();testParsingEXEC();testParsingMAST();testParsingIF();testParsingWHILE();testParsingASSERT();testParsingRETURN();testParsingException();
    if(assertions!=15||parseCalls!=93)throw std::runtime_error("Original parser accounting changed: "+std::to_string(assertions)+" / "+std::to_string(parseCalls));
    std::cout<<"ORIGINAL_PARSERS_RESULT {\"class\":\"StatementParserTests\",\"tests\":10,\"assertions\":"<<assertions<<",\"parse_calls\":"<<parseCalls<<",\"passed\":true}\n";
}
