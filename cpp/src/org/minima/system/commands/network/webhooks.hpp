#pragma once
#include "../Command.hpp"
#include "org/minima/system/network/webhooks/NotifyManager.hpp"
namespace minima::commands {
class webhooks : public Command {
public:
    // Bind to Main.getInstance().getNotifyManager() during runtime assembly.
    static std::function<std::shared_ptr<NotifyManager>()> runtimeLookup;
    webhooks();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"enable","action","hook","filter"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<webhooks>();}
};
}
