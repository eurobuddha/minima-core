#pragma once
#include <stdexcept>
#include <string>
namespace minima::kissvm {
class MinimaException : public std::runtime_error {
public:
    explicit MinimaException(const std::string& error);
};
}
