#pragma once
#include "MinimaException.hpp"
namespace minima::kissvm {
class ExecutionException : public MinimaException {
public:
    explicit ExecutionException(const std::string& error);
};
}
