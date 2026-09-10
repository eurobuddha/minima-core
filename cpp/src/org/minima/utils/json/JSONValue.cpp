#include "JSONValue.hpp"
#include "support/JavaFloating.hpp"
#include "JSONObject.hpp"
#include "JSONArray.hpp"
#include "JSONWriter.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "support/JavaUnicode.hpp"
#include "parser/JSONParser.hpp"
#include <cmath>
#include <algorithm>
#include <charconv>
#include <stdexcept>
namespace minima::json {
JSONValue::JSONValue(const JSONObject& v) : value(std::make_shared<JSONObject>(v)) {}
JSONValue::JSONValue(const JSONArray& v) : value(std::make_shared<JSONArray>(v)) {}
JSONValue JSONValue::raw(std::string text) { JSONValue result; result.value = RawJSON{std::move(text)}; return result; }
std::string JSONValue::asString() const {if(auto text=std::get_if<std::string>(&value)) return *text;return unicode::encodeUTF8(std::get<std::u16string>(value));}
std::u16string JSONValue::asUTF16() const {if(auto text=std::get_if<std::u16string>(&value)) return *text;return unicode::decodeUTF8(std::get<std::string>(value));}
std::string JSONValue::escape(const std::string& text) {return escape(unicode::decodeUTF8(text));}
std::string JSONValue::escape(const std::u16string& text) {
    std::u16string out;
    for(auto code:text) switch(code) {
        case '"':out+=u"\\\"";break;case '\\':out+=u"\\\\";break;
        case '\b':out+=u"\\b";break;case '\f':out+=u"\\f";break;case '\n':out+=u"\\n";break;case '\r':out+=u"\\r";break;case '\t':out+=u"\\t";break;case '/':out+=u"\\/";break;
        default:if(code<=0x1f||(code>=0x7f&&code<=0x9f)||(code>=0x2000&&code<=0x20ff)) {out+=u"\\u";for(int shift=12;shift>=0;shift-=4) out+=u"0123456789ABCDEF"[(code>>shift)&15];}else out+=code;
    }
    return unicode::encodeUTF8(out);
}
JSONValue JSONValue::parse(const std::string& text) {try {return parseWithException(text);}catch(const std::exception&) {return nullptr;}}
JSONValue JSONValue::parseWithException(const std::string& text) {return parser::JSONParser().parse(text);}
void JSONValue::writeJSONString(const JSONValue& v, JSONWriter& out) { out.write(v.toJSONString()); }
std::string JSONValue::toJSONString(const JSONValue& value) { return value.toJSONString(); }
std::string JSONValue::toJSONString() const {
    return std::visit([](const auto& item) -> std::string {
        using T = std::decay_t<decltype(item)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) return "null";
        else if constexpr (std::is_same_v<T, bool>) return item ? "true" : "false";
        else if constexpr (std::is_same_v<T, int64_t>) return std::to_string(item);
        else if constexpr (std::is_same_v<T, double>) {
            if (!std::isfinite(item)) return "null";
            return javaFloating(item);
        } else if constexpr ((std::is_same_v<T, std::string>||std::is_same_v<T, std::u16string>)) return "\"" + escape(item) + "\"";
        else if constexpr (std::is_same_v<T, RawJSON>) return item.value;
        else return item ? item->toJSONString() : "null";
    }, value);
}
}
