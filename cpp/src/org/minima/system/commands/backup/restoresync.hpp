#pragma once
#include "../Command.hpp"
#include "org/minima/database/txpowdb/sql/TxPoWList.hpp"
#include <filesystem>
namespace minima {class GzipInput;}
namespace minima::commands {
class restoresync:public Command {
public:
    restoresync();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"file","password","host"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<restoresync>();}
    json::JSONObject performResync(const std::optional<std::string>& host,int32_t keyUses,const MiniNumber* startBlock,bool incrementKeys);
    json::JSONObject performResync(const std::optional<std::string>& host,int32_t keyUses,const MiniNumber& startBlock,bool incrementKeys){return performResync(host,keyUses,&startBlock,incrementKeys);}
private:
    int64_t readNextBackup(const std::filesystem::path& output,GzipInput& input);
    std::shared_ptr<TxPoWList> readNextTxPoWList(GzipInput& input);
};
}
