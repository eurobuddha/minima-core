#include "JSONArray.hpp"
#include "JSONWriter.hpp"
#include "support/JavaUnicode.hpp"
namespace minima::json {
bool JSONArray::removeString(const std::string& value) {auto text=unicode::decodeUTF8(value);return values.remove([&](const JSONValue& item){return item.isString()&&item.asUTF16()==text;});}
bool JSONArray::containsString(const std::string& value) const {auto text=unicode::decodeUTF8(value);return values.find([&](const JSONValue& item){return item.isString()&&item.asUTF16()==text;}).has_value();}
void JSONArray::writeJSONString(JSONWriter& out) const {
    out.write('['); bool first = true;
    for (const auto& value : elements()) {
        if (!first) out.write(',');
        first = false;
        JSONValue::writeJSONString(value, out);
    }
    out.write(']');
}
std::string JSONArray::toJSONString() const { JSONWriter out; writeJSONString(out); return out.toString(); }
}
