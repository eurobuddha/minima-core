#include "removescript.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "support/JavaLowerCase.hpp"
namespace minima::commands {
removescript::removescript():Command("removescript","[address:] - Remove a script from your DB"){}
std::optional<std::string> removescript::getFullHelp() const {return "\removescript\n\nRemove a custom script. BE CAREFUL not to remove a script you need.\n\naddress:\n    The address of the script.\n\nExamples:\n\nremovescript address:0xFFE678768CDE..\nremovescript address:MxFFE678768CDE..";}
std::vector<std::string> removescript::getValidParams() const {return {"address"};}
json::JSONValue removescript::runCommand(){
    auto result=getJSONReply();auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    auto address=getAddressParam("address");sqlRequired(wallet).removeScript(address);
    result.put("response","Script removed");return result;
}
std::shared_ptr<Command> removescript::getFunction() const {return std::make_shared<removescript>();}
}
