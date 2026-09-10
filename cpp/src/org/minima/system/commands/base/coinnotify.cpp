#include "coinnotify.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
coinnotify::coinnotify():Command("coinnotify","[action:] [address:] - listen for a specific coin address and send a NOTIFYCOIN message when found in chain"){}
std::optional<std::string> coinnotify::getFullHelp() const {return "\ncoinnotify\n\nListen for a specific coin address - without adding it to scripts.\nYou need to do this every startup.. from your Minidapp service.js for example\n\naction: \n    add : Add to the list - only added once if already added.\n    remove : Remove from the list\n    check : Check if in the list.\n\naddress:\n    The address to look out for.\n\nExamples:\n\ncoinnotify action:add address:0xFFEEDD..\n\ncoinnotify action:remove address:0xFFEEDD..\n\ncoinnotify action:check address:Mx12ABGF56..\n";}
std::vector<std::string> coinnotify::getValidParams() const {return {"action","address"};}
json::JSONValue coinnotify::runCommand(){
    auto result=getJSONReply();auto action=getParam("action");auto address=getAddressParam("address");
    json::JSONObject response;response.put("address",address);
    if(action=="add")sqlRequired(MinimaDB::getDB()).addCoinNotify(address);
    else if(action=="remove")response.put("found",sqlRequired(MinimaDB::getDB()).removeCoinNotify(address));
    else if(action=="check")response.put("found",sqlRequired(MinimaDB::getDB()).checkCoinNotify(address));
    else throw CommandException("Invalid action : "+action);
    result.put("response",response);return result;
}
std::shared_ptr<Command> coinnotify::getFunction() const {return std::make_shared<coinnotify>();}
}
