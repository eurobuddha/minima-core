// Adapted from Java LexicalTokenizerTests, plus the constructor depth behavior.
#include "org/minima/kissvm/tokens/LexicalTokenizer.hpp"
#include <stdexcept>
using namespace minima::kissvm;
static void require(bool ok) {if(!ok) throw std::runtime_error("LexicalTokenizerTests assertion");}
template<class F> static void rejected(F fn) {bool threw=false;try {fn();}catch(const MinimaParseException&) {threw=true;}require(threw);}
int main() {
    LexicalTokenizer empty({},17);rejected([&]{empty.getNextToken();});rejected([&]{empty.goBackToken();});require(empty.getCurrentPosition()==0&&empty.checkAllTokensUsed()&&!empty.hasMoreElements());require(empty.getStackDepth()==0);
    const auto tokens=ScriptToken::tokenize("a b c d e f g h i j k l m n o p q r s t u v w x y z");LexicalTokenizer lexical(tokens,0);
    for(int i=0;i<26;++i) {require(lexical.getCurrentPosition()==i&&lexical.hasMoreElements()&&!lexical.checkAllTokensUsed());require(lexical.getNextToken().getToken()==tokens[size_t(i)].getToken());}
    require(lexical.getCurrentPosition()==26&&!lexical.hasMoreElements()&&lexical.checkAllTokensUsed());
    for(int i=25;i>=20;--i) {lexical.goBackToken();require(lexical.getCurrentPosition()==i);}
}
