#pragma once
#include "InetSocketAddress.hpp"
#include "JavaConcurrentMap.hpp"
namespace minima {
// HashSet adapter over the existing Java HashMap bins. Equal addresses retain
// the first inserted object, including its hostname and scope spelling.
class SocketAddressSet {
    JavaHashMap<SocketAddressPtr> mValues;
    mutable std::mutex mMutex;
    static std::string key(const SocketAddressPtr& address) {return address?address->equalityKey():"null";}
    static uint32_t hash(const SocketAddressPtr& address) {return address?uint32_t(address->hashCode()):0;}
public:
    bool add(const SocketAddressPtr& address) {std::lock_guard lock(mMutex);auto k=key(address);auto h=hash(address);if(mValues.getHashedKey(k,h))return false;mValues.putHashedKey(k,h,address);return true;}
    bool contains(const SocketAddressPtr& address) const {std::lock_guard lock(mMutex);return mValues.getHashedKey(key(address),hash(address)).has_value();}
    bool remove(const SocketAddressPtr& address) {std::lock_guard lock(mMutex);return mValues.removeHashedKey(key(address),hash(address)).has_value();}
    void clear() {std::lock_guard lock(mMutex);mValues.clear();}
    size_t size() const {std::lock_guard lock(mMutex);return mValues.size();}
    bool isEmpty() const {return size()==0;}
    std::vector<SocketAddressPtr> elements() const {std::lock_guard lock(mMutex);return mValues.elements();}
};
}
