#pragma once
#include "../JSONValue.hpp"
namespace minima::json::parser {
class Yytoken {
public:
    static constexpr int TYPE_VALUE=0,TYPE_LEFT_BRACE=1,TYPE_RIGHT_BRACE=2,TYPE_LEFT_SQUARE=3,TYPE_RIGHT_SQUARE=4,TYPE_COMMA=5,TYPE_COLON=6,TYPE_EOF=-1;
    int type;
    JSONValue value;
    Yytoken(int type,JSONValue value=nullptr):type(type),value(std::move(value)){}
    std::string toString() const;
};
}
