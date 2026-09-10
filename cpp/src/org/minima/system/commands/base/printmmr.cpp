#include "printmmr.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
printmmr::printmmr():Command("printmmr","Print the MMR set of the tip block"){}
std::optional<std::string> printmmr::getFullHelp() const {return "\nprintmmr\n\nPrint the MMR set of the tip block and the total number of entries in the MMR.\n\nReturns the tip block number, latest entrynumber and latest set of MMR entries.\nFor each entry, details of its row, entry number, data and value of all new and updated MMR entries for the tip block.\n\nRow 1 represents the leaf nodes, entry 0 represents the first entry on a row.\n\nExamples:\n\nprintmmr\n";}
json::JSONValue printmmr::runCommand(){
    auto reply=getJSONReply();
    auto tree=sqlRequired(MinimaDB::getDB()).getTxPoWTree();auto tip=sqlRequired(tree).getTip();
    auto mmr=sqlRequired(tip).getMMR();reply.put("response",sqlRequired(mmr).toJSON());return reply;
}
std::shared_ptr<Command> printmmr::getFunction() const {return std::make_shared<printmmr>();}
}
