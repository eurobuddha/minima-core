#include "magic.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
magic::magic():Command("magic","(kissvm:) (txpowsize:) (txnsperblock:) - Set the Magic numbers that define the Minima network overall capacity"){}
std::vector<std::string> magic::getValidParams() const {return {"kissvm","txpowsize","txnsperblock"};}
std::shared_ptr<Command> magic::getFunction() const {return std::make_shared<magic>();}
json::JSONValue magic::runCommand(){
    auto ret=getJSONReply();json::JSONObject response;auto db=sqlRequired(MinimaDB::getDB()).getUserDB();
    if(existsParam("kissvm"))sqlRequired(db).setMagicDesiredKISSVM(getNumberParam("kissvm"));
    if(existsParam("txpowsize"))sqlRequired(db).setMagicMaxTxPoWSize(getNumberParam("txpowsize"));
    if(existsParam("txnsperblock"))sqlRequired(db).setMagicMaxTxns(getNumberParam("txnsperblock"));
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();response.put("lastblock",sqlRequired(tip).getTxPoW().getMagic().toJSON());
    json::JSONObject desired;desired.put("kissvm",json::JSONValue::raw(sqlRequired(db).getMagicDesiredKISSVM().toString()));
    desired.put("txpowsize",json::JSONValue::raw(db->getMagicMaxTxPoWSize().toString()));desired.put("txnsperblock",json::JSONValue::raw(db->getMagicMaxTxns().toString()));
    response.put("desired",desired);ret.put("response",response);return ret;
}
}
