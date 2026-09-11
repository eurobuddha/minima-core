#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
namespace minima {
// Native counterpart of the two-method Java interface. The existing byte-vector
// output and span/offset input preserve append and in-place read semantics.
// Concrete classes own their wire format and failure/partial-state behavior.
class Streamable {
public:
    virtual ~Streamable();
    virtual void writeDataStream(std::vector<uint8_t>& output) const=0;
    virtual void readDataStream(std::span<const uint8_t> data,size_t& offset)=0;
    // Bridge for the existing native generic serialization callers.
    std::vector<uint8_t> serialise() const;
};
}
