#pragma once
#include "../Command.hpp"
#include "org/minima/utils/MiniUtil.hpp"
namespace minima::commands {
class backup:public Command {
public:
    inline static const MiniUtil::DateFormat DATEFORMAT{};
    backup();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"debug","password","file","auto","confirm","maxhistory"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<backup>();}
};
}
