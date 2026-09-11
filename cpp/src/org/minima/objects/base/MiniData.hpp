#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <span>
#include <functional>

namespace minima {
class BigInt;

class MiniData {
public:
    static constexpr int MINIMA_MAX_HASH_LENGTH     = 64;
    static constexpr int MINIMA_MAX_MINIDATA_LENGTH  = 512 * 1024 * 1024;

    static const MiniData ZERO_TXPOWID;
    static const MiniData ONE_TXPOWID;

    MiniData() = default;
    explicit MiniData(const BigInt& value);
    BigInt getDataValue() const;
    explicit MiniData(const std::vector<uint8_t>& b) : m_bytes(b) {}
    explicit MiniData(std::vector<uint8_t>&& b) : m_bytes(std::move(b)) {}

    static MiniData fromHex(const std::string& hex);
    static MiniData fromBytes(const uint8_t* data, size_t len) {
        if (len == 0) return MiniData();
        if (data == nullptr) throw std::invalid_argument("MiniData: null byte source");
        return MiniData(std::vector<uint8_t>(data, data + len));
    }
    static MiniData withMinLength(const std::vector<uint8_t>& data, int minLen);

    // Java getRandomData(int len) — for PRNG initialisation
    static MiniData getRandomData(int len);

    const std::vector<uint8_t>& bytes() const { return m_bytes; }
    const uint8_t* data() const { return m_bytes.data(); }
    size_t size() const { return m_bytes.size(); }
    int    getLength() const { return (int)m_bytes.size(); }
    bool   empty() const { return m_bytes.empty(); }

    bool isEqual(const MiniData& o) const { return m_bytes == o.m_bytes; }
    bool operator==(const MiniData& o) const { return m_bytes == o.m_bytes; }
    bool operator!=(const MiniData& o) const { return m_bytes != o.m_bytes; }
    bool isLess(const MiniData& o) const;
    bool isMore(const MiniData& o) const { return o.isLess(*this); }
    MiniData shiftl(int bits) const;
    MiniData shiftr(int bits) const;

    std::string to0xString() const;
    std::string toHexString(bool prefix = true) const;

    // Java CONCAT / SUBSET operations
    MiniData concat(const MiniData& o) const {
        std::vector<uint8_t> r = m_bytes;
        r.insert(r.end(), o.m_bytes.begin(), o.m_bytes.end());
        return MiniData(r);
    }
    MiniData subset(int start, int len) const {
        if(start < 0 || len < 0 || static_cast<size_t>(start) > m_bytes.size() ||
           static_cast<size_t>(len) > m_bytes.size() - static_cast<size_t>(start))
            throw std::runtime_error("MiniData::subset out of range");
        return MiniData(std::vector<uint8_t>(m_bytes.begin()+start, m_bytes.begin()+start+len));
    }

    MiniData sha3() const;  // NIST SHA3-256
    MiniData sha2() const;  // SHA-256

    // Normal serialisation: [int32 BE length][bytes]
    std::vector<uint8_t> serialise() const;
    static MiniData readFromStream(const std::function<size_t(std::span<uint8_t>)>& reader,int expectedSize=-1);
    static MiniData deserialise(std::span<const uint8_t> data, size_t& offset);
    static MiniData deserialise(std::span<const uint8_t> data, size_t& offset, int expectedSize);
    static MiniData deserialise(const std::vector<uint8_t>& data, size_t& offset) {
        return deserialise(std::span<const uint8_t>(data), offset);
    }

    // Hash serialisation: same but max 64 bytes enforced
    std::vector<uint8_t> serialiseHash() const;
    static MiniData deserialiseHash(std::span<const uint8_t> data, size_t& offset);
    static MiniData deserialiseHash(const std::vector<uint8_t>& data, size_t& offset) {
        return deserialiseHash(std::span<const uint8_t>(data), offset);
    }

private:
    std::vector<uint8_t> m_bytes;
};

using MiniHash = MiniData;

/// std::unordered_map hasher/equal for MiniData (shared across modules)
struct MiniDataHash {
    size_t operator()(const MiniData& d) const {
        size_t h = 0;
        for (uint8_t b : d.bytes()) h = h * 31 + b;
        return h;
    }
};
struct MiniDataEqual {
    bool operator()(const MiniData& a, const MiniData& b) const {
        return a.bytes() == b.bytes();
    }
};

} // namespace minima
