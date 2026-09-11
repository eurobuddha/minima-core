// All active original fixtures, registry loops, random sizes and assertions.
#include "../../../../../../support/OriginalFunctionTestSupport.hpp"
#include "org/minima/kissvm/tokens/ScriptTokenizer.hpp"
using namespace minima;using namespace minima::kissvm;using namespace minima::original_function_tests;
template<class A,class B> static void equalOriginal(const A& expected,const B& actual) {require(expected==actual);}

    static void testConstructors() {
        for (const auto& s : ScriptTokenizer::TOKENS_COMMAND) {
            ScriptToken t(ScriptToken::TOKEN_COMMAND, s);
            equalOriginal(ScriptToken::TOKEN_COMMAND, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("COMMAND", t.getTokenTypeString());
        }

        std::vector<std::string> AllFunctions;
        for (const auto& f : MinimaFunction::allFunctions()) {
            AllFunctions.push_back(f->getName());
        }
        for (const auto& s : AllFunctions) {
            ScriptToken t(ScriptToken::TOKEN_FUNCTIION, s);
            equalOriginal(ScriptToken::TOKEN_FUNCTIION, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("FUNCTION", t.getTokenTypeString());
        }

        for (const auto& s : ScriptTokenizer::TOKENS_NUMBER_OPERATOR) {
            ScriptToken t(ScriptToken::TOKEN_OPERATOR, s);
            equalOriginal(ScriptToken::TOKEN_OPERATOR, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("OPERATOR", t.getTokenTypeString());
        }

        for (const auto& s : ScriptTokenizer::TOKENS_BOOLEAN_OPERATOR) {
            ScriptToken t(ScriptToken::TOKEN_OPERATOR, s);
            equalOriginal(ScriptToken::TOKEN_OPERATOR, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("OPERATOR", t.getTokenTypeString());
        }

        {
            std::string s = "0x12345678";
            ScriptToken t(ScriptToken::TOKEN_VALUE, s);
            equalOriginal(ScriptToken::TOKEN_VALUE, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("VALUE", t.getTokenTypeString());
        }

        {
            std::string s = ":123456";
            ScriptToken t(ScriptToken::TOKEN_VALUE, s);
            equalOriginal(ScriptToken::TOKEN_VALUE, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("VALUE", t.getTokenTypeString());
        }

        {
            std::string s = "abcdefgh";
            ScriptToken t(ScriptToken::TOKEN_VARIABLE, s);
            equalOriginal(ScriptToken::TOKEN_VARIABLE, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("VARIABLE", t.getTokenTypeString());
        }

        {
            std::string s = "ijklmnop";
            ScriptToken t(ScriptToken::TOKEN_VARIABLE, s);
            equalOriginal(ScriptToken::TOKEN_VARIABLE, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("VARIABLE", t.getTokenTypeString());
        }

        {
            std::string s = "@BLKNUM";
            ScriptToken t(ScriptToken::TOKEN_GLOBAL, s);
            equalOriginal(ScriptToken::TOKEN_GLOBAL, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("GLOBAL", t.getTokenTypeString());
        }

        {
            std::string s = "@TOKENSCRIPT";
            ScriptToken t(ScriptToken::TOKEN_GLOBAL, s);
            equalOriginal(ScriptToken::TOKEN_GLOBAL, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("GLOBAL", t.getTokenTypeString());
        }

        {
            std::string s = "(";
            ScriptToken t(ScriptToken::TOKEN_OPENBRACKET, s);
            equalOriginal(ScriptToken::TOKEN_OPENBRACKET, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("OPENBRACKET", t.getTokenTypeString());
        }

        {
            std::string s = ")";
            ScriptToken t(ScriptToken::TOKEN_CLOSEBRACKET, s);
            equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("CLOSEBRACKET", t.getTokenTypeString());
        }

        {
            std::string s = "TRUE";
            ScriptToken t(ScriptToken::TOKEN_TRUE, s);
            equalOriginal(ScriptToken::TOKEN_TRUE, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("TRUE", t.getTokenTypeString());
        }

        {
            std::string s = "FALSE";
            ScriptToken t(ScriptToken::TOKEN_FALSE, s);
            equalOriginal(ScriptToken::TOKEN_FALSE, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("FALSE", t.getTokenTypeString());
        }
    }

    static void testInvalidConstructors() {
        {
            std::string s = "whatever";
            ScriptToken t(99, s);
            equalOriginal(99, t.getTokenType());
            equalOriginal(s, t.getToken());
            equalOriginal("null", t.getTokenTypeString());
        }
    }

    static void testCheckers() {
        require(ScriptTokenizer::isVariable("a"));
        require(ScriptTokenizer::isVariable("b"));
        require(ScriptTokenizer::isVariable("c"));
        require(ScriptTokenizer::isVariable("ab"));
        require(ScriptTokenizer::isVariable("abc"));
        require(ScriptTokenizer::isVariable("abcd"));
        require(ScriptTokenizer::isVariable("abcde"));
        require(ScriptTokenizer::isVariable("abcdefghijklmnopqrstuvwxyz"));

        require(!(ScriptTokenizer::isNumeric("a")));
        require(!(ScriptTokenizer::isNumeric("b")));
        require(!(ScriptTokenizer::isNumeric("c")));
        require(!(ScriptTokenizer::isNumeric("ab")));
        require(!(ScriptTokenizer::isNumeric("abc")));
        require(!(ScriptTokenizer::isNumeric("abcd")));
        require(!(ScriptTokenizer::isNumeric("abcde")));
        require(!(ScriptTokenizer::isNumeric("abcdefghijklmnopqrstuvwxyz")));

        require(!(ScriptTokenizer::isVariable("0")));
        require(!(ScriptTokenizer::isVariable("-0")));
        require(!(ScriptTokenizer::isVariable("1")));
        require(!(ScriptTokenizer::isVariable("-1")));
        require(!(ScriptTokenizer::isVariable("0.0")));
        require(!(ScriptTokenizer::isVariable("123.456")));

        require(ScriptTokenizer::isNumeric("0"));

        require(ScriptTokenizer::isNumeric("1"));

        require(ScriptTokenizer::isNumeric("0.0"));
        require(ScriptTokenizer::isNumeric("123.456"));

        require(!(ScriptTokenizer::isVariable("-123.-456")));
        require(!(ScriptTokenizer::isVariable("A1")));
        require(!(ScriptTokenizer::isVariable("A2")));
        require(!(ScriptTokenizer::isVariable("A3")));
        require(!(ScriptTokenizer::isVariable("1$#@45")));
        require(!(ScriptTokenizer::isVariable("{}[]()")));

        require(!(ScriptTokenizer::isNumeric("-123.-456")));
        require(!(ScriptTokenizer::isNumeric("A1")));
        require(!(ScriptTokenizer::isNumeric("A2")));
        require(!(ScriptTokenizer::isNumeric("A3")));
        require(!(ScriptTokenizer::isNumeric("1$#@45")));
        require(!(ScriptTokenizer::isNumeric("{}[]()")));

        require(!(ScriptTokenizer::isVariable("+123")));
        require(!(ScriptTokenizer::isVariable("+123.456")));
        require(!(ScriptTokenizer::isVariable(".456")));
        require(!(ScriptTokenizer::isVariable("-.456")));
        require(!(ScriptTokenizer::isVariable("+.456")));

        require(!(ScriptTokenizer::isNumeric("+123")));
        require(!(ScriptTokenizer::isNumeric("+123.456")));
        require(!(ScriptTokenizer::isNumeric(".456")));
        require(!(ScriptTokenizer::isNumeric("-.456")));
        require(!(ScriptTokenizer::isNumeric("+.456")));

        require(!(ScriptTokenizer::isVariable("A")));
        require(!(ScriptTokenizer::isVariable("B")));
        require(!(ScriptTokenizer::isVariable("C")));
        require(!(ScriptTokenizer::isVariable("AB")));
        require(!(ScriptTokenizer::isVariable("ABC")));
        require(!(ScriptTokenizer::isVariable("ABCD")));
        require(!(ScriptTokenizer::isVariable("ABCDE")));
        require(!(ScriptTokenizer::isVariable("ABCDEFGHIJKLMNOPQRSTUVWXYZ")));
        require(!(ScriptTokenizer::isVariable("A1")));
        require(!(ScriptTokenizer::isVariable("A2")));
        require(!(ScriptTokenizer::isVariable("A3")));
        require(!(ScriptTokenizer::isVariable("1$#@45")));
        require(!(ScriptTokenizer::isVariable("{}[]()")));

        require(!(ScriptTokenizer::isNumeric("A")));
        require(!(ScriptTokenizer::isNumeric("B")));
        require(!(ScriptTokenizer::isNumeric("C")));
        require(!(ScriptTokenizer::isNumeric("AB")));
        require(!(ScriptTokenizer::isNumeric("ABC")));
        require(!(ScriptTokenizer::isNumeric("ABCD")));
        require(!(ScriptTokenizer::isNumeric("ABCDE")));
        require(!(ScriptTokenizer::isNumeric("ABCDEFGHIJKLMNOPQRSTUVWXYZ")));
        require(!(ScriptTokenizer::isNumeric("A1")));
        require(!(ScriptTokenizer::isNumeric("A2")));
        require(!(ScriptTokenizer::isNumeric("A3")));
        require(!(ScriptTokenizer::isNumeric("1$#@45")));
        require(!(ScriptTokenizer::isNumeric("{}[]()")));
    }

    static void testTokenize() {
        {
            std::string Script = "";
            for (const auto& s : ScriptTokenizer::TOKENS_COMMAND) {
                Script = Script + s + " ";
            }
            {
                auto tokens = ScriptToken::tokenize(Script);
                for (int i = 0; i < ScriptTokenizer::TOKENS_COMMAND.size(); i++) {
                    equalOriginal(ScriptToken::TOKEN_COMMAND, tokens.at(i).getTokenType());
                    equalOriginal(ScriptTokenizer::TOKENS_COMMAND[i], tokens.at(i).getToken());
                }
            }
        }

        {
            std::string Script = "";
            for (const auto& f : MinimaFunction::allFunctions()) {
                Script = Script + f->getName() + " ";
            }
            {
                auto tokens = ScriptToken::tokenize(Script);
                for (int i = 0; i < MinimaFunction::allFunctions().size(); i++) {
                    equalOriginal(ScriptToken::TOKEN_FUNCTIION, tokens.at(i).getTokenType());
                    equalOriginal(MinimaFunction::allFunctions()[i]->getName(), tokens.at(i).getToken());
                }
            }
        }

        {
            std::string Script = "";
            for (const auto& s : ScriptTokenizer::TOKENS_NUMBER_OPERATOR) {
                Script = Script + s + " ";
            }
            {
                auto tokens = ScriptToken::tokenize(Script);
                for (int i = 0; i < ScriptTokenizer::TOKENS_NUMBER_OPERATOR.size(); i++) {
                    equalOriginal(ScriptToken::TOKEN_OPERATOR, tokens.at(i).getTokenType());
                    equalOriginal(ScriptTokenizer::TOKENS_NUMBER_OPERATOR[i], tokens.at(i).getToken());
                }
            }
        }

        {
            std::string Script = "";
            std::vector<std::string> Values = {"[ abc ]", "[ def ]", "[ ghi ]", "[ abc [ def ] ghi ]"};
            for (const auto& s : Values) {
                Script = Script + s + " ";
            }
            {
                auto tokens = ScriptToken::tokenize(Script);
                for (int i = 0; i < Values.size(); i++) {
                    equalOriginal(ScriptToken::TOKEN_VALUE, tokens.at(i).getTokenType());
                    equalOriginal(Values[i], tokens.at(i).getToken());
                }
            }
        }

        {
            std::string Script = "";
            std::vector<std::string> Values = {"0x1", "0x12", "0x123", "0x1234", "0x12345", "0x123456", "0x1234567", "0x12345678", "0x1234567890123456789"};
            for (const auto& s : Values) {
                Script = Script + s + " ";
            }
            {
                auto tokens = ScriptToken::tokenize(Script);
                for (int i = 0; i < Values.size(); i++) {
                    equalOriginal(ScriptToken::TOKEN_VALUE, tokens.at(i).getTokenType());
                    equalOriginal(Values[i], tokens.at(i).getToken());
                }

            }
        }

        {
            std::string Script = "";
            std::vector<std::string> Values = {"0xA", "0xAB", "0x1AB", "0x1A2B",
                MiniData::getRandomData(4).to0xString(),
                MiniData::getRandomData(8).to0xString(),
                MiniData::getRandomData(16).to0xString(),
                MiniData::getRandomData(32).to0xString(),
                MiniData::getRandomData(64).to0xString()};
            for (const auto& s : Values) {
                Script = Script + s + " ";
            }
            {
                auto tokens = ScriptToken::tokenize(Script);
                for (int i = 0; i < Values.size(); i++) {
                    equalOriginal(ScriptToken::TOKEN_VALUE, tokens.at(i).getTokenType());
                    equalOriginal(Values[i], tokens.at(i).getToken());
                }

            }
        }

        {
            std::string Script = "( ) ) ( ( ) ) ( ( ) ) ( ( ) ) ( ( )";
            {
                auto tokens = ScriptToken::tokenize(Script);
                int i = 0;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_OPENBRACKET, tokens.at(i).getTokenType());
                equalOriginal("(", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_CLOSEBRACKET, tokens.at(i).getTokenType());
                equalOriginal(")", tokens.at(i).getToken());
                i++;
            }
        }

        {
            std::string Script = "TRUE FALSE FALSE TRUE TRUE FALSE FALSE TRUE TRUE FALSE FALSE TRUE TRUE FALSE FALSE TRUE TRUE FALSE";
            {
                auto tokens = ScriptToken::tokenize(Script);
                int i = 0;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_TRUE, tokens.at(i).getTokenType());
                equalOriginal("TRUE", tokens.at(i).getToken());
                i++;
                equalOriginal(ScriptToken::TOKEN_FALSE, tokens.at(i).getTokenType());
                equalOriginal("FALSE", tokens.at(i).getToken());
                i++;
            }
        }

        {
            std::string Script = "";
            std::vector<std::string> Values = {"@ADDRESS", "@AMOUNT", "@COINID", "@INBLKNUM", "@INPUT", "@BLKNUM", "@BLKTIME", "@SCRIPT", "@TOKENID", "@TOKENSCRIPT", "@TOKENTOTAL", "@TOTIN", "@TOTOUT"};
            for (const auto& s : Values) {
                Script = Script + s + " ";
            }
            {
                auto tokens = ScriptToken::tokenize(Script);
                for (int i = 0; i < Values.size(); i++) {
                    equalOriginal(ScriptToken::TOKEN_GLOBAL, tokens.at(i).getTokenType());
                    equalOriginal(Values[i], tokens.at(i).getToken());
                }

            }
        }

        {
            std::string Script = "";
            std::vector<std::string> Values = {"a", "bc", "def", "ghij", "klmno", "pqrstu", "vwxyzab", "cdefghij", "klmnopqrs", "tuvwxyzabc", "defghijklmno", "pqrstuvwxyzab", "cdefghijklmnop", "qrstuvwxyzabcdef"};
            for (const auto& s : Values) {
                Script = Script + s + " ";
            }
            {
                auto tokens = ScriptToken::tokenize(Script);
                for (int i = 0; i < Values.size(); i++) {
                    equalOriginal(ScriptToken::TOKEN_VARIABLE, tokens.at(i).getTokenType());
                    equalOriginal(Values[i], tokens.at(i).getToken());
                }

            }
        }

    }
int main() {
    testConstructors();testInvalidConstructors();testCheckers();testTokenize();
    if(assertions!=699)throw std::runtime_error("Original token assertion count changed: "+std::to_string(assertions));
    std::cout<<"ORIGINAL_TOKENS_RESULT {\"class\":\"TokenTests\",\"tests\":4,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
