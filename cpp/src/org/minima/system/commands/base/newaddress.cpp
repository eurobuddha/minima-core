#include "newaddress.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
newaddress::newaddress():Command("newaddress","Create a new address that will not be not used for anything else (not a default change address)"){}
std::optional<std::string> newaddress::getFullHelp() const {return "\nnewaddress\n\nCreate a new address that will not be not used for anything else (not one of the 64 default change address).\n\nCan be used for a specific use case or for improved privacy.\n\nExamples:\n\nnewaddress\n";}
json::JSONValue newaddress::runCommand(){
    auto result=getJSONReply();auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    auto row=sqlRequired(wallet).createNewSimpleAddress(false);
    auto total=wallet->getAllKeys().size();
    auto response=sqlRequired(row).toJSON();response.put("total",int64_t(total));
    result.put("response",response);return result;
}
std::shared_ptr<Command> newaddress::getFunction() const {return std::make_shared<newaddress>();}
}
