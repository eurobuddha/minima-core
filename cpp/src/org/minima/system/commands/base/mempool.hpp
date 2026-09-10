#pragma once
#include "../Command.hpp"
namespace minima::commands {
class mempool:public Command {
public:
    mempool();
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
