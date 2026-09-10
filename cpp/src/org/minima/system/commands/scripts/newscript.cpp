#include "newscript.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "support/JavaLowerCase.hpp"
namespace minima::commands {
newscript::newscript():Command("newscript","[script:] [trackall:false|true] (clean:false|true) - Add a new custom script. Track ALL addresses or just ones with relevant state variables."){}
std::optional<std::string> newscript::getFullHelp() const {return "\nnewscript\n\nAdd a new custom script.\n\nTrack ALL coins with this script address or just ones with state variables relevant to you.\n\nscript:\n    The script to add to your node.\n    Your node will then know about coins with this script address.\n\ntrackall:\n    true or false, true will track all coins with this script address.\n    false will only track coins with this script address that are relevant to you.\n\nclean: (optional)\n    true or false, true will clean the script to its minimal correct representation.\n    Default is false.\n\nExamples:\n\nnewscript trackall:true script:\"RETURN SIGNEDBY(0x1539..) AND SIGNEDBY(0xAD25..)\"\n\nnewscript trackall:false script:\"RETURN (@BLOCK GTE PREVSTATE(1) OR @COINAGE GTE PREVSTATE(4)) AND VERIFYOUT(@INPUT PREVSTATE(2) @AMOUNT @TOKENID FALSE)\"\n";}
std::vector<std::string> newscript::getValidParams() const {return {"script","trackall","clean"};}
json::JSONValue newscript::runCommand(){
    auto result=getJSONReply();auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    auto script=getParam("script");auto track=getBooleanParam("trackall");auto clean=getBooleanParam("clean",false);
    if(clean)script=kissvm::Contract::cleanScript(script);
    auto address=Address(script).getAddressData().to0xString();
    auto existing=sqlRequired(wallet).getScriptFromAddress(address);
    if(existing)wallet->removeScript(address);
    auto row=wallet->addScript(script,false,false,"0x00",track);
    result.put("response",sqlRequired(row).toJSON());return result;
}
std::shared_ptr<Command> newscript::getFunction() const {return std::make_shared<newscript>();}
}
