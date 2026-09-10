#include "LexicalTokenizer.hpp"
#include <bit>
#include <cstdint>
namespace minima::kissvm {
// Java's constructor ignores the supplied depth; preserve the zero initializer.
LexicalTokenizer::LexicalTokenizer(std::vector<ScriptToken> tokens,int):mTokens(std::move(tokens)){}
const ScriptToken& LexicalTokenizer::getNextToken() {if(size_t(mPos)>=mTokens.size()) throw MinimaParseException("Run out of tokens!..");return mTokens.at(size_t(mPos++));}
void LexicalTokenizer::goBackToken() {if(mPos==0) throw MinimaParseException("LexicalTokenizer cannot go back as at 0 position");--mPos;}
void LexicalTokenizer::incrementStackDepth() {mStackDepth=std::bit_cast<int32_t>(uint32_t(mStackDepth)+1u);}
void LexicalTokenizer::decrementStackDepth() {mStackDepth=std::bit_cast<int32_t>(uint32_t(mStackDepth)-1u);}
}
