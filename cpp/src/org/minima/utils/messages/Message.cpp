#include "Message.hpp"
namespace minima {
namespace {
template<class T>T boxed(const MessageObject& value){if(value.isNull())throw JavaNullPointerException("null message box");try{return value.as<T>();}catch(const std::bad_any_cast&){throw JavaClassCastException("wrong message box type");}}
}
int32_t Message::getInteger(const Key& name) const{return boxed<int32_t>(getObject(name));}
float Message::getFloat(const Key& name) const{return boxed<float>(getObject(name));}
bool Message::isMessageType(const Key& type) const {auto expected=unicode::decodeUTF8(type.value());return mMessageType&&unicode::decodeUTF8(*mMessageType)==expected;}
Message& Message::addObject(const Key& name,MessageObject value){if(name)mContents->put(*name,std::move(value));else mContents->put(nullptr,std::move(value));return *this;}
MessageObject Message::getObject(const Key& name) const {auto value=name?mContents->get(*name):mContents->get(nullptr);return value.value_or(MessageObject());}
bool Message::getBoolean(const Key& name) const {auto value=getObject(name);return value.isNull()?false:boxed<bool>(value);}
Message::Key Message::getString(const Key& name) const {auto value=getObject(name);return value.isNull()?std::nullopt:Key(boxed<std::string>(value));}
std::string Message::toString() const {std::string result="[ "+mMessageType.value_or("null")+", ";for(const auto& key:mContents->nullableKeys())result+=key.value_or("null")+":"+getObject(key).toString()+", ";return result+" ]";}
}
