#pragma once
#include "../Command.hpp"
namespace minima::commands {
class vault:public Command {
public:
    struct MainReceiver {
        virtual ~MainReceiver()=default;
        virtual bool getAllKeysCreated()=0;
        virtual int32_t getAllDefaultKeysSize()=0;
        virtual void restoreReady()=0;
        virtual void setHasShutDown()=0;
        virtual void stopMessageProcessor()=0;
    };
    // Main binds its receiver during native startup. Absence remains a null receiver.
    static std::function<std::shared_ptr<MainReceiver>()> getMain;
    vault();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
    static void checkAllKeysCreated();
    static void stopAllKeysCreated();
    static void passwordLockDB(const std::optional<std::string>& password);
    static void passowrdUnlockDB(const std::optional<std::string>& password);
};
}
