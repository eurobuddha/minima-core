#pragma once
#include <vector>
#include <stdexcept>
#include "JavaExceptions.hpp"
#include <bit>
namespace minima {
// List storage with Arrays.asList's fixed-size structural mutation checks.
// Iteration exposes const elements; set remains available for fixed-size lists.
template<class T> class JavaList {
    std::vector<T> mValues;
    bool mFixed=false;
public:
    explicit JavaList(std::vector<T> values={},bool fixed=false):mValues(std::move(values)),mFixed(fixed){}
    virtual ~JavaList()=default;
    size_t size() const {return mValues.size();}
    bool isEmpty() const {return mValues.empty();}
    const T& get(size_t i) const {return mValues.at(i);}
    T set(size_t i,T value) {T old=mValues.at(i);mValues.at(i)=std::move(value);return old;}
    virtual void add(T value) {if(mFixed)throw std::runtime_error("UnsupportedOperationException");mValues.push_back(std::move(value));}
    void add(size_t index,T value) {
        if(mFixed)throw JavaTypedException("java.lang.UnsupportedOperationException","");
        if(index>size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index: "+std::to_string(std::bit_cast<int64_t>(uint64_t(index)))+", Size: "+std::to_string(size()));
        mValues.insert(mValues.begin()+index,std::move(value));
    }
    T remove(size_t i) {if(i>=size())throw std::out_of_range("IndexOutOfBoundsException");if(mFixed)throw std::runtime_error("UnsupportedOperationException");T old=mValues[i];mValues.erase(mValues.begin()+i);return old;}
    void clear() {if(mFixed&&!mValues.empty())throw std::runtime_error("UnsupportedOperationException");mValues.clear();}
    auto begin() const {return mValues.begin();}
    auto end() const {return mValues.end();}
};
}
