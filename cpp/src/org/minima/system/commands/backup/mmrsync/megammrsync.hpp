#pragma once
#include "../../Command.hpp"
#include "MegaMMRSyncData.hpp"
#include "MegaMMRIBD.hpp"
namespace minima::commands {
class megammrsync:public Command {
public:
    megammrsync();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"action","host","phrase","anyphrase","keys","keyuses","file","password"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<megammrsync>();}
    void updateP2PDB(const std::optional<std::string>& peers);
    static MegaMMRSyncData getMyDetails();
    static std::shared_ptr<MegaMMRIBD> getCurrentMegaMMRIBD(const MegaMMRSyncData& data);
    static std::shared_ptr<MegaMMRIBD::Proofs> getAllCoinProofs(const MegaMMRSyncData& data);
    static std::vector<Coin> searchMegaCoins(const std::shared_ptr<MegaMMRSyncData::DataList>& addresses,const std::shared_ptr<MegaMMRSyncData::DataList>& keys);
    static std::shared_ptr<MegaMMRIBD> sendMegaMMRSyncReq(const std::optional<std::string>& host,int32_t port,const MegaMMRSyncData* data);
    static std::shared_ptr<MegaMMRIBD> sendMegaMMRSyncReq(const std::optional<std::string>& host,int32_t port,const MegaMMRSyncData& data){return sendMegaMMRSyncReq(host,port,&data);}
};
}
