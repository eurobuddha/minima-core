#include "JSONArray.hpp"
#include "JSONWriter.hpp"
#include "support/JavaUnicode.hpp"
namespace minima::json {
bool JSONArray::removeString(const std::string& value) {auto text=unicode::decodeUTF8(value);for(auto it=values.begin();it!=values.end();++it)if(it->isString()&&it->asUTF16()==text){values.erase(it);return true;}return false;}
bool JSONArray::containsString(const std::string& value) const {auto text=unicode::decodeUTF8(value);for(const auto& item:values)if(item.isString()&&item.asUTF16()==text)return true;return false;}
void JSONArray::writeJSONString(JSONWriter& out) const {
    out.write('['); bool first = true;
    for (const auto& value : values) {
        if (!first) out.write(',');
        first = false;
        JSONValue::writeJSONString(value, out);
    }
    out.write(']');
}
std::string JSONArray::toJSONString() const { JSONWriter out; writeJSONString(out); return out.toString(); }
}
