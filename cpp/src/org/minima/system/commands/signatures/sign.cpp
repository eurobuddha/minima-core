#include "sign.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "support/JavaLowerCase.hpp"
namespace minima::commands {
sign::sign():Command("sign","[publickey:] [data:] - Sign the data with the publickey"){}
std::optional<std::string> sign::getFullHelp() const {return "\nsign\n\nSign the data with the publickey.\n\nReturns the signature of the data, signed with the corresponding private key.\n\ndata:\n    The 0x HEX data to sign.\n\nExamples:\n\nsign data:0xCD34..\n";}
std::vector<std::string> sign::getValidParams() const {return {"publickey","data"};}
json::JSONValue sign::runCommand(){
    auto result=getJSONReply();auto data=getDataParam("data");auto publickey=getDataParam("publickey");
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();auto signature=sqlRequired(wallet).signData(publickey.to0xString(),data);
    result.put("response",MiniData(sqlRequired(signature).serialise()).to0xString());return result;
}
std::shared_ptr<Command> sign::getFunction() const {return std::make_shared<sign>();}
}
