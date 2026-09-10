#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include "support/ByteIO.hpp"
namespace minima {
class Crypto {
public:
    static const MiniData MAX_HASH;
    static const BigInt MAX_VAL;
    static Crypto& getInstance();
    std::vector<uint8_t> hashSHA2(std::span<const uint8_t> data) const;
    std::vector<uint8_t> hashData(std::span<const uint8_t> data) const;
    template<class T> MiniData hashObject(const T& object) const { return MiniData(object.serialise()).sha3(); }
    template<class A,class B> MiniData hashObjects(const A& a,const B& b) const { return hashAllObjects(a,b); }
    template<class... T> MiniData hashAllObjects(const T&... objects) const { return MiniData(wire::join(objects...)).sha3(); }
};
}
