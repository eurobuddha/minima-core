#include "ScriptToken.hpp"
#include "ScriptTokenizer.hpp"
namespace minima::kissvm {
std::string ScriptToken::getTokenTypeString() const {
    switch(mTokenType) {
        case TOKEN_COMMAND:return "COMMAND";
        case TOKEN_FUNCTIION:return "FUNCTION";
        case TOKEN_OPERATOR:return "OPERATOR";
        case TOKEN_VALUE:return "VALUE";
        case TOKEN_VARIABLE:return "VARIABLE";
        case TOKEN_GLOBAL:return "GLOBAL";
        case TOKEN_OPENBRACKET:return "OPENBRACKET";
        case TOKEN_CLOSEBRACKET:return "CLOSEBRACKET";
        case TOKEN_TRUE:return "TRUE";
        case TOKEN_FALSE:return "FALSE";
        case TOKEN_FUNCTIIONPARAM:return "FUNCTIONPARAM";
        default:return "null";
    }
}
std::vector<ScriptToken> ScriptToken::tokenize(const std::string& script) {return ScriptTokenizer(script).tokenize();}
}
