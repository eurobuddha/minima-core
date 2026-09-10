#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands::multisig_detail {
json::JSONValue run(Command& command,bool readOnly);
}
