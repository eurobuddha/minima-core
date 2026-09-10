#pragma once
#include "../JSONObject.hpp"
#include "../JSONArray.hpp"
namespace minima::json::parser {
class ContainerFactory {
public:
    virtual ~ContainerFactory();
    virtual std::shared_ptr<JSONObject> createObjectContainer()=0;
    virtual std::shared_ptr<JSONArray> creatArrayContainer()=0;
};
}
