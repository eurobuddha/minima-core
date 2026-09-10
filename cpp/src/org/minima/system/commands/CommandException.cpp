#include "CommandException.hpp"
namespace minima {CommandException::CommandException(const std::string& message):std::runtime_error(message),mMessage(message){}}
