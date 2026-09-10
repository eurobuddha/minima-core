#include "ScriptTokenizer.hpp"
#include "../exceptions/MinimaParseException.hpp"
#include <algorithm>

namespace minima::kissvm {
const std::vector<std::string> ScriptTokenizer::TOKENS_COMMAND={"LET","IF","THEN","ELSEIF","ELSE","ENDIF","RETURN","ASSERT","WHILE","DO","ENDWHILE","EXEC","MAST"};
const std::vector<std::string> ScriptTokenizer::TOKENS_NUMBER_OPERATOR={"+","-","/","*","%","&","|","^","~","="};
const std::vector<std::string> ScriptTokenizer::TOKENS_BOOLEAN_OPERATOR={"LT","LTE","GT","GTE","EQ","NEQ","XOR","AND","OR","NXOR","NAND","NOR","NOT","NEG"};
const std::vector<std::string> ScriptTokenizer::TOKENS_ENDOFWORD={"+","-","/","*","%","&","|","^","~","=","(",")","[","]","<",">","<<",">>"};
const std::vector<std::string> ScriptTokenizer::TOKENS_AFTERBRACKET={"+","-","/","*","%","&","|","^","=","<",">","<<",">>",")"};
const std::vector<std::string> ScriptTokenizer::FUNCTION_NAMES={"CONCAT","LEN","REV","SUBSET","GET","EXISTS","ADDRESS","BOOL","HEX","NUMBER","STRING","ASCII","UTF8","ABS","CEIL","FLOOR","MAX","MIN","DEC","INC","SIGDIG","POW","SQRT","FUNCTION","SUMINPUTS","SUMOUTPUTS","SETLEN","REPLACE","REPLACEFIRST","SUBSTR","OVERWRITE","SHA2","SHA3","PROOF","BITSET","BITGET","BITCOUNT","SIGNEDBY","MULTISIG","CHECKSIG","GETINADDR","GETINAMT","GETINID","GETINTOK","VERIFYIN","GETOUTADDR","GETOUTAMT","GETOUTTOK","GETOUTKEEPSTATE","VERIFYOUT","STATE","PREVSTATE","SAMESTATE"};
namespace {
bool contains(const std::vector<std::string>& values,const std::string& value) {return std::find(values.begin(),values.end(),value)!=values.end();}
// Only ASCII identifiers are accepted. These are the additional Unicode case
// mappings that can produce an entirely ASCII identifier in Java's root locale.
std::string identifierCase(const std::string& value,bool upper) {
    std::u16string result;
    for(auto c:unicode::decodeUTF8(value)) {
        if(upper) {
            if(c>='a'&&c<='z') {result+=char16_t(c-32);continue;}
            switch(c) {
                case 0x00df:result+=u"SS";continue;
                case 0x0131:result+=u'I';continue;
                case 0x017f:result+=u'S';continue;
                case 0xfb00:result+=u"FF";continue;
                case 0xfb01:result+=u"FI";continue;
                case 0xfb02:result+=u"FL";continue;
                case 0xfb03:result+=u"FFI";continue;
                case 0xfb04:result+=u"FFL";continue;
                case 0xfb05:case 0xfb06:result+=u"ST";continue;
            }
        } else {
            if(c>='A'&&c<='Z') {result+=char16_t(c+32);continue;}
            if(c==0x212a) {result+=u'k';continue;}
        }
        result+=c;
    }
    return unicode::encodeUTF8(result);
}
}
std::string ScriptTokenizer::getNextWord() {
    std::u16string word;
    while(mPos<mScript.size()) {auto symbol=unicode::encodeUTF8(std::u16string(1,mScript[mPos]));if(contains(TOKENS_ENDOFWORD,symbol)||isWhiteSpace(symbol)) break;word+=mScript[mPos++];}
    return unicode::encodeUTF8(word);
}
bool ScriptTokenizer::isNumeric(const std::string& word) {
    if(word.empty()) return false;size_t pos=0;while(pos<word.size()&&word[pos]>='0'&&word[pos]<='9') ++pos;
    if(pos==0) return false;if(pos==word.size()) return true;if(word[pos++]!='.') return false;size_t fraction=pos;
    while(pos<word.size()&&word[pos]>='0'&&word[pos]<='9') ++pos;return pos>fraction&&pos==word.size();
}
bool ScriptTokenizer::isHex(const std::string& word) {return word.size()>2&&word.starts_with("0x")&&std::all_of(word.begin()+2,word.end(),[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F');});}
bool ScriptTokenizer::isVariable(const std::string& word) {return !word.empty()&&std::all_of(word.begin(),word.end(),[](char c){return c>='a'&&c<='z';});}
bool ScriptTokenizer::isGlobal(const std::string& word) {return word.size()>1&&word[0]=='@'&&std::all_of(word.begin()+1,word.end(),[](char c){return c>='A'&&c<='Z';});}
bool ScriptTokenizer::isWhiteSpace(const std::string& word) {return !word.empty()&&std::all_of(word.begin(),word.end(),[](char c){return c==' '||c=='\t'||c=='\r'||c=='\n'||c=='\v'||c=='\f';});}
std::vector<ScriptToken> ScriptTokenizer::tokenize() {
    std::vector<ScriptToken> tokens;mPos=0;bool lastSpace=true;
    while(mPos<mScript.size()) {
        auto ch=mScript[mPos];auto symbol=unicode::encodeUTF8(std::u16string(1,ch));bool space=false;
        if(isWhiteSpace(symbol)) {++mPos;space=true;}
        else if(contains(TOKENS_NUMBER_OPERATOR,symbol)) {tokens.emplace_back(ScriptToken::TOKEN_OPERATOR,symbol);++mPos;}
        else if(ch=='<'||ch=='>') {
            auto next=mScript.at(mPos+1);if(next!=ch) throw MinimaParseException("Incorrect Token found @ "+std::to_string(mPos)+" "+symbol+unicode::encodeUTF8(std::u16string(1,next)));
            tokens.emplace_back(ScriptToken::TOKEN_OPERATOR,symbol+symbol);mPos+=2;
        } else if(ch=='('||ch==')') {tokens.emplace_back(ch=='('?ScriptToken::TOKEN_OPENBRACKET:ScriptToken::TOKEN_CLOSEBRACKET,symbol);++mPos;}
        else if(ch=='[') {
            std::u16string value;int depth=0;
            while(mPos<mScript.size()) {ch=mScript[mPos++];value+=ch;if(ch=='[') ++depth;else if(ch==']'&&--depth==0) break;}
            tokens.emplace_back(ScriptToken::TOKEN_VALUE,unicode::encodeUTF8(value));
        } else if(ch=='$') tokens.emplace_back(ScriptToken::TOKEN_FUNCTIIONPARAM,getNextWord());
        else {
            auto word=getNextWord();auto upper=mCaseInsensitive?identifierCase(word,true):word;auto lower=mCaseInsensitive?identifierCase(word,false):word;
            if(contains(TOKENS_COMMAND,upper)) {
                if(!lastSpace) throw MinimaParseException("Missing space before Command @ "+std::to_string(mPos)+" "+upper);
                tokens.emplace_back(ScriptToken::TOKEN_COMMAND,upper);
            } else if(contains(FUNCTION_NAMES,upper)) tokens.emplace_back(ScriptToken::TOKEN_FUNCTIION,upper);
            else if(contains(TOKENS_BOOLEAN_OPERATOR,upper)) tokens.emplace_back(ScriptToken::TOKEN_OPERATOR,upper);
            else if(isNumeric(word)||isHex(word)) tokens.emplace_back(ScriptToken::TOKEN_VALUE,word);
            else if(upper=="TRUE"||upper=="FALSE") tokens.emplace_back(upper=="TRUE"?ScriptToken::TOKEN_TRUE:ScriptToken::TOKEN_FALSE,upper);
            else if(isGlobal(upper)) tokens.emplace_back(ScriptToken::TOKEN_GLOBAL,upper);
            else if(isVariable(lower)) {if(lower.size()>32) throw MinimaParseException("MAX Variable length is 32 @ "+std::to_string(mPos)+" "+word);tokens.emplace_back(ScriptToken::TOKEN_VARIABLE,lower);}
            else throw MinimaParseException("Incorrect Token found @ "+std::to_string(mPos)+" "+word);
        }
        lastSpace=space;
    }
    return tokens;
}
}
