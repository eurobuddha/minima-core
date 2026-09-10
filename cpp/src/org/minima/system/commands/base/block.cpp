#include "block.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/JavaDateString.hpp"
namespace minima::commands {
block::block():Command("block","Simply return the current top block"){}
std::optional<std::string> block::getFullHelp() const {return "\nblock\n\nReturn the top block\n\nExamples:\n\nblock\n";}
json::JSONValue block::runCommand(){
    auto reply=getJSONReply();
    auto tree=sqlRequired(MinimaDB::getDB()).getTxPoWTree();auto tip=sqlRequired(tree).getTip();
    if(!tip)throw CommandException("NO Blocks yet..");
    const auto& top=tip->getTxPoW();json::JSONObject response;
    response.put("block",top.getBlockNumber().toString());response.put("hash",top.getTxPoWID());
    response.put("timemilli",top.getTimeMilli().toString());response.put("date",javaDateString(top.getTimeMilli().getAsLong()));
    reply.put("response",response);return reply;
}
std::shared_ptr<Command> block::getFunction() const {return std::make_shared<block>();}
}
