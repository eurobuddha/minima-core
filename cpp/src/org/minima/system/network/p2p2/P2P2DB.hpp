#pragma once
#include "org/minima/utils/JsonDB.hpp"
#include "org/minima/utils/MiniUtil.hpp"
namespace minima::p2p2 {
class P2P2DB : public JsonDB {
    std::shared_ptr<MiniUtil::StringList> mAllPeers=std::make_shared<MiniUtil::StringList>();
public:
    P2P2DB()=default;
    void loadDB(const std::filesystem::path& file) override;
    void saveDB(const std::filesystem::path& file) override;
    bool isFirstStartUp() const {return getBoolean("first_startup",true);}
    void setFirstStartUp(bool value){setBoolean("first_startup",value);}
    std::shared_ptr<MiniUtil::StringList> getAllKnownPeers() const {return mAllPeers;}
    bool addPeerToAllKnown(const std::optional<std::string>& peer);
};
}
