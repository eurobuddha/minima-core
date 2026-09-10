#pragma once
#include "JSONValue.hpp"
#include "JSONStreamAware.hpp"
#include <vector>
namespace minima::json {
class JSONArray : public JSONAware, public JSONStreamAware {
    std::vector<JSONValue> values;
public:
    JSONArray() = default;
    explicit JSONArray(std::vector<JSONValue> input) : values(std::move(input)) {}
    void add(JSONValue value) { values.push_back(std::move(value)); }
    const JSONValue& get(size_t index) const { return values.at(index); }
    bool removeString(const std::string& value);
    bool containsString(const std::string& value) const;
    size_t size() const { return values.size(); }
    const auto& elements() const { return values; }
    std::string toJSONString() const override;
    std::string toString() const { return toJSONString(); }
    void writeJSONString(JSONWriter& out) const override;
};
}
