#pragma once
#include "PeerHarness.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
namespace minima {
struct NIOClientFixture{static void uid(NIOClient& value,const std::string& uid){value.mUID=uid;}static JSONArray packets(NIOClient& value){JSONArray out;for(const auto& packet:*value.mMessages)out.add(packet->to0xString());return out;}};
class NIOManagerFixture : public NIOManager {
    NIOManagerFixture():NIOManager([]{NIOManagerServices s;s.releaseLocks=[]{};s.currentTimeMillis=[]{return int64_t(0);};return s;}()){}
public:
    static std::shared_ptr<NIOManagerFixture> create(){auto m=std::shared_ptr<NIOManagerFixture>(new NIOManagerFixture);m->mNIOServer=std::make_shared<NIOServer>(0,std::make_shared<MessageStack>());return m;}
    static int64_t timerDelay(const TimerMessage& value){return value.mDelay;}
    void add(const std::shared_ptr<NIOClient>& value,bool connected){(connected?mNIOServer->mClients:mConnectingClients).put(value->getUID().value(),value);}
    JSONArray drain(){JSONArray rows;while(auto message=getNextMessage()){JSONObject row;row.put("type",nullable(message->getMessageType()));for(const char* key:{"uid","host"})if(message->exists(key))row.put(key,nullable(message->getString(key)));if(message->exists("port"))row.put("port",message->getInteger("port"));rows.add(row);}return rows;}
};
}
