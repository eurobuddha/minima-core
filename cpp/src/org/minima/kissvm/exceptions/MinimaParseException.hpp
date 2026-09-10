#pragma once
#include "MinimaException.hpp"
namespace minima::kissvm {
class MinimaParseException : public MinimaException {
public:
    explicit MinimaParseException(const std::string& error);
};
}
