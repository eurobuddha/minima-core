#include "Yytoken.hpp"
namespace minima::json::parser {
std::string Yytoken::toString() const {
    switch(type) {
        case TYPE_VALUE:return "VALUE("+(value.isString()?value.asString():value.toJSONString())+")";
        case TYPE_LEFT_BRACE:return "LEFT BRACE({)";
        case TYPE_RIGHT_BRACE:return "RIGHT BRACE(})";
        case TYPE_LEFT_SQUARE:return "LEFT SQUARE([)";
        case TYPE_RIGHT_SQUARE:return "RIGHT SQUARE(])";
        case TYPE_COMMA:return "COMMA(,)";
        case TYPE_COLON:return "COLON(:)";
        case TYPE_EOF:return "END OF FILE";
        default:return "";
    }
}
}
