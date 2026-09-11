#pragma once
#include "JavaExceptions.hpp"
#include <bit>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>
namespace minima::json {
// Ordered JSON storage. Reuses the locked-copy access discipline of
// JavaConcurrentMap; unlike its snapshot enumeration, iteration checks changes.
template<class T,bool Array> class JSONSequence {
    struct State {
        mutable std::mutex lock;
        std::vector<T> values;
        uint64_t modification=0;
        explicit State(std::vector<T> input={}):values(std::move(input)){}
    };
    const std::shared_ptr<State> state;
public:
    JSONSequence():state(std::make_shared<State>()){}
    explicit JSONSequence(std::vector<T> input):state(std::make_shared<State>(std::move(input))){}
    JSONSequence(const JSONSequence& other):JSONSequence(other.snapshot()){}
    JSONSequence& operator=(const JSONSequence& other){if(this!=&other){auto copy=other.snapshot();std::lock_guard guard(state->lock);state->values=std::move(copy);++state->modification;}return *this;}
    std::vector<T> snapshot() const {std::lock_guard guard(state->lock);return state->values;}
    size_t size() const {std::lock_guard guard(state->lock);return state->values.size();}
    T get(size_t index) const {
        std::lock_guard guard(state->lock);
        if constexpr(Array)if(index>=state->values.size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(std::bit_cast<int64_t>(uint64_t(index)))+" out of bounds for length "+std::to_string(state->values.size()));
        return state->values.at(index);
    }
    void add(T value){std::lock_guard guard(state->lock);state->values.push_back(std::move(value));++state->modification;}
    T set(size_t index,T value) requires(Array) {
        std::lock_guard guard(state->lock);
        if(index>=state->values.size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(std::bit_cast<int64_t>(uint64_t(index)))+" out of bounds for length "+std::to_string(state->values.size()));
        T old=state->values[index];state->values[index]=std::move(value);return old;
    }
    // ArrayList.clear is structural even when the list is already empty.
    void clear(){std::lock_guard guard(state->lock);++state->modification;state->values.clear();}
    template<class Predicate> std::optional<T> find(Predicate matches) const {
        std::lock_guard guard(state->lock);for(const auto& value:state->values)if(matches(value))return value;return std::nullopt;
    }
    template<class Predicate> void put(T value,Predicate matches){
        std::lock_guard guard(state->lock);
        for(auto& entry:state->values)if(matches(entry)){entry=std::move(value);return;}
        state->values.push_back(std::move(value));++state->modification;
    }
    template<class Predicate> bool remove(Predicate matches){
        std::lock_guard guard(state->lock);
        for(auto it=state->values.begin();it!=state->values.end();++it)if(matches(*it)){state->values.erase(it);++state->modification;return true;}return false;
    }
    struct Sentinel {};
    class Iterator {
        std::shared_ptr<State> owner;
        uint64_t expected;
        size_t cursor=0,limit;
        T current() const {
            std::lock_guard guard(owner->lock);
            if(expected!=owner->modification)throw JavaTypedException("java.util.ConcurrentModificationException","");
            if(cursor>=owner->values.size())throw JavaTypedException("java.util.NoSuchElementException","");
            return owner->values[cursor];
        }
    public:
        explicit Iterator(std::shared_ptr<State> source):owner(std::move(source)){
            std::lock_guard guard(owner->lock);expected=owner->modification;limit=owner->values.size();
        }
        bool hasNext() const {std::lock_guard guard(owner->lock);if constexpr(Array)return cursor!=owner->values.size();else return cursor<limit;}
        T next(){auto value=current();++cursor;return value;}
        T operator*() const {return current();}
        Iterator& operator++(){++cursor;return *this;}
        bool operator!=(Sentinel) const {return hasNext();}
    };
    class Range {
        std::shared_ptr<State> owner;
    public:
        explicit Range(std::shared_ptr<State> source):owner(std::move(source)){}
        Iterator begin() const {return Iterator(owner);}
        Sentinel end() const {return {};}
        T at(size_t index) const {std::lock_guard guard(owner->lock);return owner->values.at(index);}
        size_t size() const {std::lock_guard guard(owner->lock);return owner->values.size();}
    };
    Iterator iterator() const {return Iterator(state);}
    Range range() const {return Range(state);}
};
}
