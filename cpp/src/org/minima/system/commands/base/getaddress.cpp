#include "getaddress.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
getaddress::getaddress():Command("getaddress","Get one of your default Minima addresses"){}
std::optional<std::string> getaddress::getFullHelp() const {return "\ngetaddress\n\nReturns an existing default Minima address to receive funds, use as a change address etc.\n\nEach address can be used securely 262144 (64^3) times.\n\nThen you can wipe the private keys from your online node using the 'vault' command.\n\nExamples:\n\ngetaddress\n";}
std::vector<std::string> getaddress::getValidParams() const {return {};}
json::JSONValue getaddress::runCommand(){
    auto result=getJSONReply();auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    if(!existsParam("createall")){
        auto type=getParam("type","single");(void)type;
        auto row=sqlRequired(wallet).getDefaultAddress();result.put("response",sqlRequired(row).toJSON());
    }
    return result;
}
std::shared_ptr<Command> getaddress::getFunction() const {return std::make_shared<getaddress>();}
}
