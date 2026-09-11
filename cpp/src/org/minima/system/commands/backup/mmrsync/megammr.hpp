#pragma once
#include "../../Command.hpp"
#include "MegaMMRBackup.hpp"
namespace minima::commands {
class megammr:public Command {
public:
    megammr();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"action","file"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<megammr>();}
    static BigInt checkMegaMMR(const std::filesystem::path& file);
    static BigInt checkMegaMMR(MegaMMRBackup& backup);
    static void main(const std::vector<std::string>& args);
};
}
