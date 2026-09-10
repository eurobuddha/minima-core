#pragma once
#include "../../Command.hpp"
#include "org/minima/objects/Coin.hpp"
namespace minima::commands {
class consolidatefrom:public Command {
public:
    consolidatefrom();
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    virtual json::JSONObject runCommand(const std::string& command);
    virtual std::shared_ptr<json::JSONObject> createConsolidate(const std::vector<std::shared_ptr<Coin>>* coins,const MiniNumber* burn,
        const std::optional<std::string>& from,const std::optional<std::string>& token,const std::optional<std::string>& script,
        const std::optional<std::string>& privatekey,const MiniNumber* keyuses);
    std::shared_ptr<Command> getFunction() const override;
};
}
