#pragma once
#include "../JSONValue.hpp"
namespace minima::json::parser {
class ContentHandler {
public:
    virtual ~ContentHandler();
    virtual void startJSON()=0;
    virtual void endJSON()=0;
    virtual bool startObject()=0;
    virtual bool endObject()=0;
    virtual bool startObjectEntry(const std::u16string& key)=0;
    virtual bool endObjectEntry()=0;
    virtual bool startArray()=0;
    virtual bool endArray()=0;
    virtual bool primitive(const JSONValue& value)=0;
};
}
