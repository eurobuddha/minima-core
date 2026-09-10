#pragma once
#include "JSONAware.hpp"
#include <memory>
#include <variant>
#include <cstdint>
namespace minima::json {
class JSONObject;
class JSONArray;
class JSONWriter;
struct RawJSON { std::string value; };
class JSONValue : public JSONAware {
public:
    using Storage = std::variant<std::nullptr_t, bool, int64_t, double, std::string, std::u16string, RawJSON,
                                 std::shared_ptr<JSONObject>, std::shared_ptr<JSONArray>>;
    Storage value = nullptr;
    JSONValue() = default;
    JSONValue(std::nullptr_t) {}
    JSONValue(bool v) : value(v) {}
    JSONValue(int v) : value(int64_t(v)) {}
    JSONValue(int64_t v) : value(v) {}
    JSONValue(double v) : value(v) {}
    JSONValue(const char* v) : value(std::string(v)) {}
    JSONValue(std::string v) : value(std::move(v)) {}
    JSONValue(std::u16string v) : value(std::move(v)) {}
    JSONValue(const JSONObject& v);
    JSONValue(const JSONArray& v);
    static JSONValue raw(std::string v);
    static std::string escape(const std::string& text);
    static std::string escape(const std::u16string& text);
    static JSONValue parse(const std::string& text);
    static JSONValue parseWithException(const std::string& text);
    static std::string toJSONString(const JSONValue& value);
    static void writeJSONString(const JSONValue& value, JSONWriter& out);
    std::string toJSONString() const override;
    bool isNull() const { return std::holds_alternative<std::nullptr_t>(value); }
    bool isString() const {return std::holds_alternative<std::string>(value)||std::holds_alternative<std::u16string>(value);}
    std::string asString() const;
    std::u16string asUTF16() const;
    bool asBool() const { return std::get<bool>(value); }
};
}
