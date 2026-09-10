// Java counterpart: test/org/minima/objects/base/MiniByteTests.java
#include "org/minima/objects/base/MiniByte.hpp"
#include "support/ByteIO.hpp"
#include <stdexcept>
using minima::MiniByte;
static void require(bool condition) { if (!condition) throw std::runtime_error("MiniByte parity failure"); }
int main() {
    require(MiniByte().isFalse());
    require(MiniByte(true).isEqual(MiniByte::TRUE));
    require(MiniByte(false).isEqual(MiniByte::FALSE));
    for (int value = -512; value <= 512; ++value) {
        MiniByte byte(value);
        require(byte.getValue() == (value & 255));
        require(byte.isTrue() == ((value & 255) != 0));
        require(byte.toString() == std::to_string(value & 255));
        auto bytes = byte.serialise();
        std::size_t offset = 0;
        require(MiniByte::deserialise(bytes, offset).isEqual(byte));
        require(offset == 1);
        bool rejected = false;
        try { MiniByte::deserialise(bytes, offset); } catch (const minima::wire::EOFException&) { rejected = true; }
        require(rejected);
    }
}
