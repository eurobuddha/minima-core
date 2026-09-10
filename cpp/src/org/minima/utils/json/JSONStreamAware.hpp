#pragma once
namespace minima::json {
class JSONWriter;
class JSONStreamAware {
public:
    virtual ~JSONStreamAware();
    virtual void writeJSONString(JSONWriter& out) const = 0;
};
}
