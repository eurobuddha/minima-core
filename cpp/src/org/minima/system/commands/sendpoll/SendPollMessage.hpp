#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include <optional>
namespace minima {
class SendPollMessage {
    std::optional<std::string> mUID,mCommand;
public:
    explicit SendPollMessage(std::optional<std::string> command);
    SendPollMessage(std::optional<std::string> uid,std::optional<std::string> command):mUID(std::move(uid)),mCommand(std::move(command)){}
    const std::optional<std::string>& getUID() const {return mUID;}
    const std::optional<std::string>& getCommand() const {return mCommand;}
    json::JSONObject toJSON() const;
    SendPollMessage copy() const;
};
}
