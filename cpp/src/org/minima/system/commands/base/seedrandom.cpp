#include "seedrandom.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "support/JavaUnicode.hpp"
namespace minima::commands {
seedrandom::seedrandom():Command("seedrandom","[modifier:] - Generate a random value, based on your SEED and a modifier"){}
std::optional<std::string> seedrandom::getFullHelp() const {return std::string(
"\nseedrandom\n"
"\n"
"Generate a random value, based on your SEED and a modifier.\n"
"\n"
"modifier: \n"
"    The modifier - added to seed before hash.\n"
"\n"
"Examples:\n"
"\n"
"seedrandom modifier:\"Hello you\"\n"
"\n");}
std::vector<std::string> seedrandom::getValidParams() const {return {"modifier"};}
std::shared_ptr<Command> seedrandom::getFunction() const {return std::make_shared<seedrandom>();}
json::JSONValue seedrandom::runCommand(){
    auto ret=getJSONReply();
    if(!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable())throw CommandException("DB locked!");
    auto modifier=getParamUTF16("modifier");
    MiniData moddata(MiniString(unicode::encodeUTF8(modifier)).serialise());
    auto hashmod=Crypto::getInstance().hashObjects(moddata,MiniData::fromHex("0xDEADDEAD"));
    auto seed=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getBaseSeed();
    const auto& text=sqlRequired(seed).getSeed();if(!text)throw JavaNullPointerException("null base seed");
    auto hash=Crypto::getInstance().hashObjects(hashmod,MiniData::fromHex(*text));
    json::JSONObject response;response.put("modifier",modifier);response.put("seedrandom",hash.to0xString());
    ret.put("response",response);return ret;
}
}
