#pragma once
#include "Yytoken.hpp"
#include "ParseException.hpp"
#include "support/JavaUnicode.hpp"
#include <optional>
namespace minima::json::parser {
class Yylex {
    std::u16string input,buffer;
    size_t marked=0,start=0;
    int lexicalState=0;
public:
    static constexpr int YYEOF=-1,YYINITIAL=0,STRING_BEGIN=2;
    explicit Yylex(std::u16string text={}):input(std::move(text)){}
    void yyreset(std::u16string text) {input=std::move(text);marked=start=0;lexicalState=0;}
    void yyclose() {input.resize(start);marked=start;}
    int getPosition() const {return int(start);}
    int yystate() const {return lexicalState;}
    void yybegin(int state) {lexicalState=state;}
    std::u16string yytext() const {return input.substr(start,marked-start);}
    char16_t yycharat(int pos) const {return input.at(start+size_t(pos));}
    int yylength() const {return int(marked-start);}
    void yypushback(int count);
    std::optional<Yytoken> yylex();
};
}
