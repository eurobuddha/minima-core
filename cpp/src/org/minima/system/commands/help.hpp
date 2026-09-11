#pragma once
#include "Command.hpp"
namespace minima::commands {
class help:public Command {
    void addCommand(json::JSONObject& details,const Command& command);
public:
    help();
    std::vector<std::string> getValidParams() const override{return {"command"};}
    json::JSONValue runCommand() override;
    std::u16string getStrOfLength(int32_t desired,const std::optional<std::u16string>& text) const;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<help>();}
};
}
