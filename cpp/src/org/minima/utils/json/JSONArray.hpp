#pragma once
#include "JSONValue.hpp"
#include "JSONStreamAware.hpp"
#include <vector>
#include "support/JSONSequence.hpp"
namespace minima::json {
class JSONArray : public JSONAware, public JSONStreamAware {
    JSONSequence<JSONValue,true> values;
public:
    JSONArray() = default;
    explicit JSONArray(std::vector<JSONValue> input) : values(std::move(input)) {}
    void add(JSONValue value) { values.add(std::move(value)); }
    JSONValue get(size_t index) const { return values.get(index); }
    bool removeString(const std::string& value);
    bool containsString(const std::string& value) const;
    size_t size() const { return values.size(); }
    auto elements() const { return values.range(); }
    auto iterator() const { return values.iterator(); }
    auto snapshot() const { return values.snapshot(); }
    std::string toJSONString() const override;
    std::string toString() const { return toJSONString(); }
    void writeJSONString(JSONWriter& out) const override;
};
}
