#pragma once
#include <string>
namespace minima::json {
class JSONAware {
public:
    virtual ~JSONAware();
    virtual std::string toJSONString() const = 0;
};
}
