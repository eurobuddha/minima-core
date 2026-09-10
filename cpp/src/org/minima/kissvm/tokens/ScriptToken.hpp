#pragma once
#include <string>
#include <vector>
namespace minima::kissvm {
class ScriptToken {
    int mTokenType;
    std::string mToken;
public:
    static constexpr int TOKEN_COMMAND=0,TOKEN_FUNCTIION=1,TOKEN_OPERATOR=2,TOKEN_VALUE=3,TOKEN_VARIABLE=4,TOKEN_GLOBAL=6,TOKEN_OPENBRACKET=7,TOKEN_CLOSEBRACKET=8,TOKEN_TRUE=9,TOKEN_FALSE=10,TOKEN_FUNCTIIONPARAM=11;
    ScriptToken(int type,std::string token):mTokenType(type),mToken(std::move(token)){}
    int getTokenType() const {return mTokenType;}
    std::string getTokenTypeString() const;
    const std::string& getToken() const {return mToken;}
    std::string toString() const {return getTokenTypeString()+":"+mToken;}
    static std::vector<ScriptToken> tokenize(const std::string& script);
};
}
