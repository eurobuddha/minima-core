#pragma once
#include "../Command.hpp"
#include "org/minima/objects/mmr/MMRData.hpp"
namespace minima::commands {
class mmrproof:public Command {
public:
    struct mmrleafnode {int32_t mEntry=0;std::optional<std::string> mData;std::shared_ptr<MiniData> mHash;};
    mmrproof();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
