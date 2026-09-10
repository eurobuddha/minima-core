#pragma once
#include "support/JavaConcurrentMap.hpp"
#include "support/JavaFloating.hpp"
#include "support/JavaExceptions.hpp"
#include <any>
#include <functional>
namespace minima {
// Java Object adapter. Shared pointers preserve identity for object payloads;
// primitive boxes retain their exact type so an incorrect cast fails.
class MessageObject {
    std::any mValue;
    std::function<std::string()> mRender;
public:
    MessageObject()=default;
    MessageObject(std::nullptr_t) {}
    MessageObject(bool value):mValue(value),mRender([value]{return value?"true":"false";}){}
    MessageObject(int32_t value):mValue(value),mRender([value]{return std::to_string(value);}){}
    MessageObject(int64_t value):mValue(value),mRender([value]{return std::to_string(value);}){}
    MessageObject(float value):mValue(value),mRender([value]{return javaFloating(value);}){}
    MessageObject(double value):mValue(value),mRender([value]{return javaFloating(value);}){}
    MessageObject(std::string value):mValue(value),mRender([value]{return value;}){}
    MessageObject(const char* value):MessageObject(value?MessageObject(std::string(value)):MessageObject()){}
    template<class T> MessageObject(std::shared_ptr<T> value,std::function<std::string(const T&)> render) {
        if(value){mValue=value;mRender=[value,render=std::move(render)]{return render(*value);};}
    }
    template<class T> explicit MessageObject(std::shared_ptr<T> value) requires requires(const T& v){v.toString();}
        :MessageObject(value,std::function<std::string(const T&)>([](const T& v){return v.toString();})){}
    bool isNull() const {return !mValue.has_value();}
    template<class T> T as() const {return std::any_cast<T>(mValue);}
    std::string toString() const {return isNull()?"null":mRender();}
};
class Message {
public:
    using Key=std::optional<std::string>;
    using Contents=JavaHashMap<MessageObject>;
protected:
    Key mMessageType=std::string();
    std::shared_ptr<Contents> mContents=std::make_shared<Contents>();
public:
    Message()=default;
    explicit Message(Key type):mMessageType(std::move(type)){}
    virtual ~Message()=default;
    void setMessageType(Key type){mMessageType=std::move(type);}
    const Key& getMessageType() const {return mMessageType;}
    bool isMessageType(const Key& type) const;
    std::shared_ptr<Contents> getAllContents() const{return mContents;}
    Message& addObject(const Key& name,MessageObject value);
    Message& addFloat(const Key& name,float value){return addObject(name,value);}
    Message& addInteger(const Key& name,int32_t value){return addObject(name,value);}
    Message& addString(const Key& name,const Key& value){return addObject(name,value.value());}
    Message& addBoolean(const Key& name,bool value){return addObject(name,value);}
    MessageObject getObject(const Key& name) const;
    bool exists(const Key& name) const {return !getObject(name).isNull();}
    bool getBoolean(const Key& name) const;
    bool getBoolean(const Key& name,bool fallback) const {return exists(name)?getBoolean(name):fallback;}
    int32_t getInteger(const Key& name) const;
    float getFloat(const Key& name) const;
    Key getString(const Key& name) const;
    virtual std::string toString() const;
};
}
