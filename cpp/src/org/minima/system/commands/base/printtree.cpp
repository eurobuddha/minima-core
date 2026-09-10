#include "printtree.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/JavaInteger.hpp"
namespace minima::commands {
printtree::printtree():Command("printtree","(depth:) (cascade:true|false) - Print a tree representation of the blockchain. Depth default 32, Cascade false."){}
std::optional<std::string> printtree::getFullHelp() const {return "\nprinttree\n\nPrint a tree representation of the blockchain.\n\nDefault depth 32 blocks, can be increased to see more of the txpow tree.\n\nOptionally show the cascading chain, default is false.\n\ndepth: (optional)\n    Number of blocks back from the tip to show in the txpow tree.\n\ncascade: (optional)\n    true or false, true shows the cascade.\n\nExamples:\n\nprinttree\n\nprinttree depth:500\n\nprinttree cascade:true\n";}
std::vector<std::string> printtree::getValidParams() const {return {"depth","cascade"};}
json::JSONValue printtree::runCommand(){
    auto reply=getJSONReply();json::JSONObject response;
    int32_t depth=javaParseInt(getParam("depth",std::string("32")));
    bool cascade=getParam("cascade",std::string("false"))=="true";
    if(cascade){auto owner=sqlRequired(MinimaDB::getDB()).getCascade();response.put("cascade","\n"+sqlRequired(owner).printCascade());}
    auto tree=sqlRequired(MinimaDB::getDB()).getTxPoWTree();response.put("chain","\n"+sqlRequired(tree).printTree(depth));
    reply.put("response",response);return reply;
}
std::shared_ptr<Command> printtree::getFunction() const {return std::make_shared<printtree>();}
}
