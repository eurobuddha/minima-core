#include "scripts.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "support/JavaLowerCase.hpp"
namespace minima::commands {
scripts::scripts():Command("scripts","(address:) - Search scripts / addresses"){}
std::optional<std::string> scripts::getFullHelp() const {return "\nscripts\n\nList all scripts or search for a script / basic address your node is tracking.\n\naddress: (optional)\n    Script address or basic address to search for. Can be 0x or Mx address.\n\nExamples:\n\nscripts\n\nscripts address:0xFED5..\n\nscripts address:MxG087..n";}
std::vector<std::string> scripts::getValidParams() const {return {"address"};}
json::JSONValue scripts::runCommand(){
    auto result=getJSONReply();auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    auto address=getParam("address","");
    if(javaLowerCase(address).starts_with("mx")){
        try{address=Address::convertMinimaAddress(address).to0xString();}
        catch(const std::invalid_argument& error){throw CommandException(javaExceptionText(error));}
    }
    if(address.empty()){
        auto rows=sqlRequired(wallet).getAllAddresses();json::JSONArray all;
        for(const auto& row:rows)all.add(sqlRequired(row).toJSON());result.put("response",all);
    }else{
        auto row=sqlRequired(wallet).getScriptFromAddress(address);
        if(!row)throw CommandException("Script with that address not found");
        result.put("response",row->toJSON());
    }
    return result;
}
std::shared_ptr<Command> scripts::getFunction() const {return std::make_shared<scripts>();}
}
