#pragma once
#include "../Command.hpp"
#include "support/MultisigContract.hpp"
#include <filesystem>
namespace minima::commands {
class multisigread:public Command {
public:
    inline static const std::string& MULTISIG_CONTRACT=::minima::MULTISIG_CONTRACT;
    multisigread();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<multisigread>();}
private:
    std::filesystem::path getRequiredFile();
};
}
