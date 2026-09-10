#pragma once
#include "../Command.hpp"
#include <functional>
namespace minima::commands {
class rpc:public Command {
public:
    static std::function<void(int64_t)> sleep;
    rpc();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"enable","ssl","password","action","username","password","mode"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<rpc>();}
};
}
