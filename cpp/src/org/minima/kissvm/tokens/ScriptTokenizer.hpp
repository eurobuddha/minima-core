#pragma once
#include "ScriptToken.hpp"
#include "support/JavaUnicode.hpp"
namespace minima::kissvm {
class ScriptTokenizer {
    std::u16string mScript;
    size_t mPos=0;
    bool mCaseInsensitive=false;
public:
    static const std::vector<std::string> TOKENS_COMMAND,TOKENS_NUMBER_OPERATOR,TOKENS_BOOLEAN_OPERATOR,TOKENS_ENDOFWORD,TOKENS_AFTERBRACKET;
    // Names are the pinned MinimaFunction registry; recognition is separate
    // from the function implementations, which are tracked individually.
    static const std::vector<std::string> FUNCTION_NAMES;
    explicit ScriptTokenizer(const std::string& script,bool caseInsensitive=false):mScript(unicode::decodeUTF8(script)),mCaseInsensitive(caseInsensitive){}
    std::string getNextWord();
    static bool isNumeric(const std::string& word);
    static bool isHex(const std::string& word);
    static bool isVariable(const std::string& word);
    static bool isGlobal(const std::string& word);
    static bool isWhiteSpace(const std::string& word);
    std::vector<ScriptToken> tokenize();
};
}
