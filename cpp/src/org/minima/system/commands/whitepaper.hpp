#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class whitepaper:public Command {
public:
    static const std::string WP;
    whitepaper();
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<whitepaper>();}
};
}
