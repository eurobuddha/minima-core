#include "JSONObject.hpp"
#include "JSONWriter.hpp"
#include "support/JavaUnicode.hpp"
namespace minima::json {
void JSONObject::put(std::string key, JSONValue value) {put(unicode::decodeUTF8(key),std::move(value));}
void JSONObject::put(std::u16string key, JSONValue value) {
    for (auto& entry : values) if (entry.first == key) { entry.second = std::move(value); return; }
    values.emplace_back(std::move(key), std::move(value));
}
const JSONValue& JSONObject::get(const std::string& key) const {
    const auto decoded=unicode::decodeUTF8(key);
    for (auto& entry : values) if (entry.first == decoded) return entry.second;
    static const JSONValue empty;
    return empty;
}
bool JSONObject::containsKey(const std::string& key) const {
    const auto decoded=unicode::decodeUTF8(key);
    for (auto& entry : values) if (entry.first == decoded) return true;
    return false;
}
std::string JSONObject::getString(const std::string& key) const { return get(key).asString(); }
std::string JSONObject::getString(const std::string& key, const std::string& fallback) const {
    return containsKey(key) ? getString(key) : fallback;
}
bool JSONObject::getBoolean(const std::string& key) const { return get(key).asBool(); }
void JSONObject::writeJSONString(JSONWriter& out) const {
    out.write('{'); bool first = true;
    for (const auto& [key, value] : values) {
        if (!first) out.write(',');
        first = false;
        out.write('"'); out.write(JSONValue::escape(key)); out.write('"'); out.write(':');
        JSONValue::writeJSONString(value, out);
    }
    out.write('}');
}
std::string JSONObject::toJSONString() const { JSONWriter out; writeJSONString(out); return out.toString(); }
}
