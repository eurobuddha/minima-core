#include "Streamable.hpp"
namespace minima {
Streamable::~Streamable()=default;
std::vector<uint8_t> Streamable::serialise() const {
    std::vector<uint8_t> output;
    writeDataStream(output);
    return output;
}
}
