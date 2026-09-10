#pragma once
#include "JSONValue.hpp"
#include "JSONStreamAware.hpp"
#include <vector>
#include "support/JSONSequence.hpp"
namespace minima::json {
class JSONObject : public JSONAware, public JSONStreamAware {
    JSONSequence<std::pair<std::u16string, JSONValue>,false> values;
public:
    void put(std::string key, JSONValue value);
    void put(std::u16string key, JSONValue value);
    JSONValue get(const std::string& key) const;
    bool containsKey(const std::string& key) const;
    std::string getString(const std::string& key) const;
    std::string getString(const std::string& key, const std::string& fallback) const;
    bool getBoolean(const std::string& key) const;
    size_t size() const { return values.size(); }
    auto entries() const { return values.range(); }
    auto iterator() const { return values.iterator(); }
    std::string toJSONString() const override;
    std::string toString() const { return toJSONString(); }
    void writeJSONString(JSONWriter& out) const override;
    static std::string escape(const std::string& value) { return JSONValue::escape(value); }
};
}
