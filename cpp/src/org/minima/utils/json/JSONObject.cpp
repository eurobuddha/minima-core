#include "JSONObject.hpp"
#include "JSONWriter.hpp"
#include "support/JavaUnicode.hpp"
namespace minima::json {
void JSONObject::put(std::string key, JSONValue value) {put(unicode::decodeUTF8(key),std::move(value));}
void JSONObject::put(std::u16string key, JSONValue value) {
    values.put({key,std::move(value)},[&](const auto& entry){return entry.first==key;});
}
JSONValue JSONObject::get(const std::string& key) const {
    const auto decoded=unicode::decodeUTF8(key);
    auto entry=values.find([&](const auto& item){return item.first==decoded;});
    return entry?entry->second:JSONValue();
}
bool JSONObject::containsKey(const std::string& key) const {
    const auto decoded=unicode::decodeUTF8(key);
    return values.find([&](const auto& item){return item.first==decoded;}).has_value();
}
std::string JSONObject::getString(const std::string& key) const { return get(key).asString(); }
std::string JSONObject::getString(const std::string& key, const std::string& fallback) const {
    return containsKey(key) ? getString(key) : fallback;
}
bool JSONObject::getBoolean(const std::string& key) const { return get(key).asBool(); }
void JSONObject::writeJSONString(JSONWriter& out) const {
    out.write('{'); bool first = true;
    for (const auto& [key, value] : entries()) {
        if (!first) out.write(',');
        first = false;
        out.write('"'); out.write(JSONValue::escape(key)); out.write('"'); out.write(':');
        JSONValue::writeJSONString(value, out);
    }
    out.write('}');
}
std::string JSONObject::toJSONString() const { JSONWriter out; writeJSONString(out); return out.toString(); }
}
