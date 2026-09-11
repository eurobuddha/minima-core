#pragma once
#include "org/minima/utils/Streamable.hpp"
/**
 * MiniString — UTF-8 string type used in KISS VM scripts and state variables.
 *
 * Wrapper around std::string with Minima-specific serialisation.
 */

#include <string>
#include <vector>
#include <cstdint>
#include <span>

namespace minima {

class MiniString:public Streamable {
public:
    MiniString() = default;
    explicit MiniString(const std::string& s);
    explicit MiniString(std::span<const uint8_t> bytes);
    static std::string normalizeUTF8(std::span<const uint8_t> bytes);

    const std::string& str() const { return m_value; }
    const std::string& toString() const { return m_value; }
    std::vector<uint8_t> getData() const { return {m_value.begin(), m_value.end()}; }
    bool isEqual(const std::string& value) const { return m_value == value; }
    size_t length() const { return m_value.size(); }
    bool   isEmpty() const { return m_value.empty(); }
    bool   empty()   const { return m_value.empty(); }

    bool operator==(const MiniString& rhs) const { return m_value == rhs.m_value; }
    bool operator!=(const MiniString& rhs) const { return !(*this == rhs); }

    // KISS VM string operations
    MiniString concat(const MiniString& rhs) const;
    MiniString subset(size_t start, size_t len) const;

    void writeDataStream(std::vector<uint8_t>& output) const override;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    std::vector<uint8_t> serialise() const;
    static MiniString    deserialise(std::span<const uint8_t> data, size_t& offset);

private:
    std::string m_value;
};

} // namespace minima
