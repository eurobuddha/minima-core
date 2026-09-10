#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include <limits>
#include <stdexcept>
using namespace minima;
template<class F> void rejects(F call) {
    bool rejected = false;
    try { call(); } catch (const std::exception&) { rejected = true; }
    if (!rejected) throw std::runtime_error("Malformed input accepted");
}
int main() {
    for (std::vector<uint8_t> bytes : {std::vector<uint8_t>{}, {0}, {0, 0, 0},
                                      {0xFF, 0xFF, 0xFF, 0xFF}, {0, 0, 0, 2, 1}}) {
        rejects([&] { size_t offset = 0; MiniData::deserialise(bytes, offset); });
        rejects([&] { size_t offset = 0; MiniString::deserialise(bytes, offset); });
        rejects([&] { size_t offset = 0; MiniData::deserialiseHash(bytes, offset); });
    }
    for (std::vector<uint8_t> bytes : {std::vector<uint8_t>{}, {0}, {0, 0}, {0, 33}, {0, 2, 1}})
        rejects([&] { size_t offset = 0; MiniNumber::deserialise(bytes, offset); });
    std::vector<uint8_t> bytes{0};
    rejects([&] { size_t offset = std::numeric_limits<size_t>::max(); MiniData::deserialise(bytes, offset); });
    rejects([&] { size_t offset = std::numeric_limits<size_t>::max(); MiniNumber::deserialise(bytes, offset); });
    rejects([&] { size_t offset = std::numeric_limits<size_t>::max(); MiniString::deserialise(bytes, offset); });
    rejects([] { MiniData::getRandomData(-1); });
}
