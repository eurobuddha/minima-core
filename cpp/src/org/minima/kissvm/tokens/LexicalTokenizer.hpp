#pragma once
#include "ScriptToken.hpp"
#include "../exceptions/MinimaParseException.hpp"
namespace minima::kissvm {
class LexicalTokenizer {
    std::vector<ScriptToken> mTokens;
    int mPos=0,mStackDepth=0;
public:
    LexicalTokenizer(std::vector<ScriptToken> tokens,int stackDepth);
    const ScriptToken& getNextToken();
    int getCurrentPosition() const {return mPos;}
    void goBackToken();
    bool checkAllTokensUsed() const {return size_t(mPos)==mTokens.size();}
    bool hasMoreElements() const {return size_t(mPos)<mTokens.size();}
    int getStackDepth() const {return mStackDepth;}
    void incrementStackDepth();
    void decrementStackDepth();
};
}
