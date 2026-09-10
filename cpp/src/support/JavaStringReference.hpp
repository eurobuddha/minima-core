#pragma once
#include <mutex>
#include <optional>
#include <string>
#include <utility>
namespace minima {
// Extracted from RPCClient::UserAgent. Java assignment replaces an immutable
// String reference; native readers must own a snapshot, not mutable storage.
class JavaStringReference {
public:
    using String=std::optional<std::string>;
private:
    mutable std::mutex mLock;
    String mValue;
public:
    explicit JavaStringReference(String value):mValue(std::move(value)){}
    JavaStringReference& operator=(String value){std::lock_guard lock(mLock);mValue=std::move(value);return *this;}
    String load() const {std::lock_guard lock(mLock);return mValue;}
    operator String() const {return load();}
};
}
