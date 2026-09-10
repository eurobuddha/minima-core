#include "Yylex.hpp"
#include <charconv>
#include <cstdlib>
namespace minima::json::parser {
namespace {
// Exact DFA tables extracted from the pinned Yylex.java (JFlex 1.4.2).
constexpr int cmap[]={
    0,0,0,0,0,0,0,0,0,7,7,0,0,7,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,7,0,9,0,0,0,0,0,0,0,0,6,25,2,4,10,
    3,3,3,3,3,3,3,3,3,3,26,0,0,0,0,0,0,1,1,1,1,5,1,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,8,24,0,0,
    0,18,11,1,1,17,12,0,0,0,0,0,19,0,13,0,0,0,14,20,15,16,0,0,
    0,0,0,21,0,22,0,0,
};
constexpr int actions[]={
    0,0,1,1,2,3,4,1,1,1,5,6,7,8,9,10,11,12,13,0,0,0,0,0,
    12,14,15,16,17,18,19,20,0,21,0,21,0,0,0,0,22,23,0,0,24,
};
constexpr int attributes[]={
    0,0,9,1,1,1,9,1,1,1,9,9,9,9,9,9,1,1,9,0,0,0,0,0,
    9,9,9,9,9,9,9,9,0,1,0,1,0,0,0,0,9,9,0,0,9,
};
constexpr int rows[]={
    0,27,54,81,108,135,54,162,189,216,54,54,54,54,54,54,243,270,54,297,324,351,378,405,
    54,54,54,54,54,54,54,54,432,459,486,486,513,540,567,594,54,54,621,648,54,
};
constexpr int transitions[]={
    2,2,3,4,2,2,2,5,2,6,2,2,7,8,2,9,2,2,2,2,2,10,11,12,
    13,14,15,16,16,16,16,16,16,16,16,17,18,16,16,16,16,16,16,16,16,16,16,16,
    16,16,16,16,16,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,4,19,20,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,21,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,16,16,16,16,16,16,16,16,-1,-1,16,16,16,16,16,16,16,16,16,16,16,
    16,16,16,16,16,16,-1,-1,-1,-1,-1,-1,-1,-1,24,25,26,27,28,29,30,31,32,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,33,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,35,-1,-1,34,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,36,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,37,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,38,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,39,-1,39,-1,39,-1,-1,-1,-1,-1,39,39,-1,-1,-1,-1,39,39,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,33,-1,20,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,35,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,38,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,40,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,41,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,42,-1,42,-1,42,
    -1,-1,-1,-1,-1,42,42,-1,-1,-1,-1,42,42,-1,-1,-1,-1,-1,-1,-1,-1,-1,43,-1,
    43,-1,43,-1,-1,-1,-1,-1,43,43,-1,-1,-1,-1,43,43,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,44,-1,44,-1,44,-1,-1,-1,-1,-1,44,44,-1,-1,-1,-1,44,44,-1,-1,-1,-1,-1,
    -1,-1,-1,
};
}
void Yylex::yypushback(int count) {
    if(count<0||size_t(count)>marked-start) throw std::out_of_range("Error: pushback value was too large");marked-=size_t(count);
}
std::optional<Yytoken> Yylex::yylex() {
    while(true) {
        start=marked;size_t current=marked;int action=-1;
        if(lexicalState!=YYINITIAL&&lexicalState!=STRING_BEGIN) throw std::out_of_range("Invalid lexer state");
        int state=lexicalState==YYINITIAL?0:1;
        while(current<input.size()) {
            auto ch=input[current++];int next=transitions[rows[state]+(ch<128?cmap[ch]:0)];if(next==-1) break;state=next;
            if(attributes[state]&1) {action=state;marked=current;if(attributes[state]&8) break;}
        }
        switch(action<0?action:actions[action]) {
            case 11:buffer+=yytext();break;
            case 4:buffer.clear();yybegin(STRING_BEGIN);break;
            case 16:buffer+=u'\b';break;
            case 6:return Yytoken(Yytoken::TYPE_RIGHT_BRACE);
            case 23:return Yytoken(Yytoken::TYPE_VALUE,yytext()==u"true");
            case 22:return Yytoken(Yytoken::TYPE_VALUE,nullptr);
            case 13:yybegin(YYINITIAL);return Yytoken(Yytoken::TYPE_VALUE,buffer);
            case 12:buffer+=u'\\';break;
            case 21: {
                auto text=unicode::encodeUTF8(yytext());char* end=nullptr;double value=std::strtod(text.c_str(),&end);
                if(end!=text.c_str()+text.size()) throw std::invalid_argument("Invalid JSON double");return Yytoken(Yytoken::TYPE_VALUE,value);
            }
            case 1:throw ParseException(getPosition(),ParseException::ERROR_UNEXPECTED_CHAR,unicode::encodeUTF8(std::u16string(1,yycharat(0))));
            case 8:return Yytoken(Yytoken::TYPE_RIGHT_SQUARE);
            case 19:buffer+=u'\r';break;
            case 15:buffer+=u'/';break;
            case 10:return Yytoken(Yytoken::TYPE_COLON);
            case 14:buffer+=u'"';break;
            case 5:return Yytoken(Yytoken::TYPE_LEFT_BRACE);
            case 17:buffer+=u'\f';break;
            case 24: {auto text=unicode::encodeUTF8(yytext().substr(2));unsigned value=0;auto result=std::from_chars(text.data(),text.data()+text.size(),value,16);if(result.ec!=std::errc{}) throw ParseException(getPosition(),2,"Invalid Unicode escape");buffer+=char16_t(value);break;}
            case 20:buffer+=u'\t';break;
            case 7:return Yytoken(Yytoken::TYPE_LEFT_SQUARE);
            case 2: {auto text=unicode::encodeUTF8(yytext());int64_t value;auto result=std::from_chars(text.data(),text.data()+text.size(),value);if(result.ec!=std::errc{}) throw std::out_of_range("JSON integer outside signed 64-bit range");return Yytoken(Yytoken::TYPE_VALUE,value);}
            case 18:buffer+=u'\n';break;
            case 9:return Yytoken(Yytoken::TYPE_COMMA);
            case 3:break;
            default:if(start==input.size()) return std::nullopt;throw std::runtime_error("Error: could not match input");
        }
    }
}
}
