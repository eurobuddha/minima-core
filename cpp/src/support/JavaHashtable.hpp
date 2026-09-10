#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "JavaUnicode.hpp"
namespace minima {
// Extracted from the Java-tested MMR bucket implementation. String hashing
// uses Java UTF-16 code units, including for network traffic labels.
template<class T> class JavaHashtable {
    struct Slot {std::string key;uint32_t hash;T value;};
    std::vector<std::vector<Slot>> mBuckets{11};
    size_t mSize=0;
    static uint32_t hash(const std::string& text) {
        uint32_t value=0;
        for(char16_t c:unicode::decodeUTF8(text))value=value*31+c;
        return value;
    }
public:
    size_t size() const {return mSize;}
    bool empty() const {return mSize==0;}
    T* get(const std::string& key) {
        auto h=hash(key);for(auto& slot:mBuckets[(h&0x7fffffffu)%mBuckets.size()]) if(slot.key==key) return &slot.value;return nullptr;
    }
    const T* get(const std::string& key) const {
        auto h=hash(key);for(const auto& slot:mBuckets[(h&0x7fffffffu)%mBuckets.size()]) if(slot.key==key) return &slot.value;return nullptr;
    }
    void put(std::string key,T value) {
        auto h=hash(key);auto index=(h&0x7fffffffu)%mBuckets.size();
        for(auto& slot:mBuckets[index]) if(slot.key==key) {slot.value=std::move(value);return;}
        if(mSize>=size_t(float(mBuckets.size())*0.75f)) {
            auto old=std::move(mBuckets);mBuckets=std::vector<std::vector<Slot>>(old.size()*2+1);
            for(auto bucket=old.rbegin();bucket!=old.rend();++bucket) for(auto& slot:*bucket) {
                auto& destination=mBuckets[(slot.hash&0x7fffffffu)%mBuckets.size()];destination.insert(destination.begin(),std::move(slot));
            }
            index=(h&0x7fffffffu)%mBuckets.size();
        }
        mBuckets[index].insert(mBuckets[index].begin(),Slot{std::move(key),h,std::move(value)});++mSize;
    }
    void remove(const std::string& key) {
        auto& bucket=mBuckets[(hash(key)&0x7fffffffu)%mBuckets.size()];
        for(auto it=bucket.begin();it!=bucket.end();++it) if(it->key==key) {bucket.erase(it);--mSize;return;}
    }
    std::vector<T> elements() const {
        std::vector<T> result;result.reserve(mSize);
        for(auto bucket=mBuckets.rbegin();bucket!=mBuckets.rend();++bucket) for(const auto& slot:*bucket) result.push_back(slot.value);
        return result;
    }
    std::vector<std::string> keys() const {
        std::vector<std::string> result;result.reserve(mSize);
        for(auto bucket=mBuckets.rbegin();bucket!=mBuckets.rend();++bucket) for(const auto& slot:*bucket) result.push_back(slot.key);
        return result;
    }
};
}
