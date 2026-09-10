#pragma once
#include <stdexcept>
#include <string>
#include <optional>
namespace minima {
class CommandException:public std::runtime_error {
    std::optional<std::string> mMessage;
public:
    explicit CommandException(const std::string& message);
    explicit CommandException(std::nullptr_t):std::runtime_error(""),mMessage(std::nullopt){}
    const std::optional<std::string>& getMessage() const {return mMessage;}
};
}
