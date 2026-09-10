#pragma once
#include "JavaConcurrentMap.hpp"
namespace minima {
// The existing SocketAddressSet composition, keyed directly by Java strings.
// Each operation is synchronized; separate contains/add calls remain separate.
class JavaStringSet {
    JavaHashMap<bool> mValues;
    mutable std::mutex mMutex;
public:
    bool contains(const std::optional<std::string>& value) const {std::lock_guard lock(mMutex);return value?mValues.containsKey(*value):mValues.containsKey(nullptr);}
    bool add(const std::optional<std::string>& value){std::lock_guard lock(mMutex);if(value){if(mValues.containsKey(*value))return false;mValues.put(*value,true);}else{if(mValues.containsKey(nullptr))return false;mValues.put(nullptr,true);}return true;}
    void clear(){std::lock_guard lock(mMutex);mValues.clear();}
    size_t size() const {std::lock_guard lock(mMutex);return mValues.size();}
    auto elements() const {std::lock_guard lock(mMutex);return mValues.nullableKeys();}
};
}
