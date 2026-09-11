#pragma once
#include "../Command.hpp"
#include "org/minima/database/txpowdb/sql/TxPoWList.hpp"
#include <filesystem>
namespace minima::commands {
class decryptbackup:public Command {
public:
    decryptbackup();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"file","password","output"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<decryptbackup>();}
private:
    int64_t readNextBackup(const std::filesystem::path& output,std::span<const uint8_t> data,size_t& offset);
    std::shared_ptr<TxPoWList> readNextTxPoWList(std::span<const uint8_t> data,size_t& offset);
};
}
