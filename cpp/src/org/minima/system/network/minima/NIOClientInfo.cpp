#include "NIOClientInfo.hpp"
#include "support/JavaDateString.hpp"
namespace minima {
NIOClientInfo::NIOClientInfo(std::shared_ptr<NIOClient> value,bool connected):mNIOClient(std::move(value)) {mWelcome=client()->getWelcomeMessage();mValidGreeting=client()->isValidGreeting();mConnected=connected;mHost=client()->getHost();mPort=client()->getPort();mMinimaPort=client()->getMinimaPort();mUID=client()->getUID();mIncoming=client()->isIncoming();}
json::JSONObject NIOClientInfo::toJSON() const {auto nullable=[](const std::optional<std::string>& value){return value?json::JSONValue(*value):json::JSONValue{};};json::JSONObject out;out.put("welcome",nullable(mWelcome));out.put("uid",nullable(mUID));out.put("incoming",mIncoming);out.put("host",nullable(mHost));out.put("port",mPort);out.put("minimaport",mMinimaPort);out.put("isconnected",mConnected);out.put("valid",mValidGreeting);if(mNIOClient)out.put("connected",javaDateString(getTimeConnected()));return out;}
}
