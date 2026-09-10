#include "SendPollMessage.hpp"
namespace minima {
SendPollMessage::SendPollMessage(std::optional<std::string> command):mUID(MiniData::getRandomData(16).to0xString()),mCommand(std::move(command)){}
json::JSONObject SendPollMessage::toJSON() const {
    json::JSONObject result;result.put("uid",mUID?json::JSONValue(*mUID):json::JSONValue());
    result.put("command",mCommand?json::JSONValue(*mCommand):json::JSONValue());return result;
}
SendPollMessage SendPollMessage::copy() const {return SendPollMessage(mUID,mCommand);}
}
