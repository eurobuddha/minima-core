#pragma once
#include <string>
namespace minima::json {
class JSONWriter {
    std::string buffer;
public:
    void write(const std::string& value);
    void write(int codeUnit);
    const std::string& getFullString() const;
    const std::string& toString() const;
};
}
