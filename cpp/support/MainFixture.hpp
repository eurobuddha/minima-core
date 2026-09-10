#pragma once
#include "org/minima/system/Main.hpp"
namespace minima {class MainFixture {public:
    static void install(const std::shared_ptr<Main>& main){std::lock_guard lock(Main::mStaticLock);Main::mMainInstance=main;}
    static void initialize(const std::shared_ptr<Main>& main){main->mUptimeMilli=0;main->bindRuntime();MinimaDB::createDB(main->databaseServices());}
    static void database(const std::shared_ptr<Main>& main){MinimaDB::createDB(main->databaseServices());}
    static void network(Main& main,std::shared_ptr<NetworkManager> network){main.replace(main.mNetwork,std::move(network));}
    static void dispatch(Main& main,const std::shared_ptr<Message>& message){main.processMessage(message);}
    static void fields(Main& main,const json::JSONObject& values){for(const auto& [key,value]:values.entries()){auto name=unicode::encodeUTF8(key);if(name=="restoring")main.mRestoring=value.asBool();else if(name=="shutdown")main.mShuttingdown=value.asBool();else if(name=="keys")main.mInitKeysCreated=value.asBool();else if(name=="complete")main.STARTUP_COMPLETE=value.asBool();else if(name=="importing")main.MYSQL_IMPORTING_NO_ACTION=value.asBool();else if(name=="uptime")main.mUptimeMilli=std::get<int64_t>(value.value);}}
};}
