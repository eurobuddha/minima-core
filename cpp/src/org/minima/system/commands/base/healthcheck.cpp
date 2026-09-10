#include "healthcheck.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
healthcheck::healthcheck():Command("healthcheck","Run a system check to see everything adds up"){}
std::optional<std::string> healthcheck::getFullHelp() const {return "\nhealthcheck\n\nReturn information about your chain, cascade and maxima.\n\nChain - tip:current chain tip block, root:current chain root block, chainlength:number of blocks in the heaviest chain.\n\nCascade - tip:current cascade tip block, tipcorrect:returns true if the cascade tip meets the root of the txpow tree.\n\nMaxima - hosts:number of maxima hosts, contacts:number of maxima contacts.\n\nExamples:\n\nhealthcheck\n";}
json::JSONValue healthcheck::runCommand(){
    auto reply=getJSONReply();json::JSONObject response;
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    if(!tip)throw CommandException("No TIP block found.. ?");
    json::JSONObject chain;chain.put("tip",tip->getTxPoW().getBlockNumber().toString());
    auto root=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getRoot();
    auto rootNumber=sqlRequired(root).getTxPoW().getBlockNumber();
    chain.put("root",rootNumber.toString());
    chain.put("chainlength",tip->getTxPoW().getBlockNumber().sub(rootNumber).toString());
    response.put("chain",chain);
    auto cascade=sqlRequired(MinimaDB::getDB()).getCascade();auto cascadeTip=sqlRequired(cascade).getTip();
    if(cascadeTip){
        auto number=sqlRequired(cascade->getTip()).getTxPoW().getBlockNumber();
        json::JSONObject info;info.put("tip",number.toString());
        info.put("tipcorrect",number.isEqual(rootNumber.decrement()));
        info.put("cascadelength",cascade->getLength());response.put("cascade",info);
    }else response.put("cascade","nocacade");
    reply.put("response",response);return reply;
}
}
