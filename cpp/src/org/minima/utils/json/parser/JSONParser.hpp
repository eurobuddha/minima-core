#pragma once
#include "Yylex.hpp"
#include "ContainerFactory.hpp"
#include "ContentHandler.hpp"
#include <istream>
namespace minima::json::parser {
class JSONParser {
    Yylex lexer;
    Yytoken token{Yytoken::TYPE_EOF};
    int status=0;
    std::optional<std::vector<int>> handlerStatusStack;
    void nextToken();
    [[noreturn]] void unexpected();
public:
    static constexpr int S_INIT=0,S_IN_FINISHED_VALUE=1,S_IN_OBJECT=2,S_IN_ARRAY=3,S_PASSED_PAIR_KEY=4,S_IN_PAIR_VALUE=5,S_END=6,S_IN_ERROR=-1;
    void reset();
    void reset(const std::u16string& text);
    int getPosition() const {return lexer.getPosition();}
    JSONValue parse(const std::string& text,ContainerFactory* factory=nullptr) {return parse(unicode::decodeUTF8(text),factory);}
    JSONValue parse(const std::u16string& text,ContainerFactory* factory=nullptr);
    JSONValue parse(std::istream& input,ContainerFactory* factory=nullptr);
    void parse(const std::string& text,ContentHandler& handler,bool resume=false) {parse(unicode::decodeUTF8(text),handler,resume);}
    void parse(const std::u16string& text,ContentHandler& handler,bool resume=false);
    void parse(std::istream& input,ContentHandler& handler,bool resume=false);
};
}
