#include "txnscript.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
txnscript::txnscript():Command("txnscript","[id:] (auto:false) (scripts:{}) - Add scripts to a txn"){}
std::optional<std::string> txnscript::getFullHelp() const {return std::string(
"\ntxnscript\n"
"\n"
"Add scripts to a transaction.\n"
"\n"
"id:\n"
"    The id of the transaction.\n"
"\n"
"auto:\n"
"    Automatically add scripts you know. Useful for multi user transactions.\n"
"\n"
"scripts:\n"
"    JSON holds the script and the proof in the format {script:proof}\n"
"    If it is a single script, and not one created with mmrcreate, leave the proof blank.\n"
"    If it is an mmrcreate script, include the proof.\n"
"\n"
"Examples:\n"
"\n"
"txnscript id:txnmast scripts:{\"RETURN TRUE\":\"\"}\n"
"\n"
"txnscript id:txnmast scripts:{\"RETURN TRUE\":\"0x000..\"}\n");}
std::vector<std::string> txnscript::getValidParams() const {return {"id","scripts","auto"};}
json::JSONValue txnscript::runCommand(){
    auto result=getJSONReply();auto db=TxnDB::getRuntimeDB();
    auto id=getParam("id");auto scripts=getJSONObjectParam("scripts",std::make_shared<json::JSONObject>());auto automatic=getBooleanParam("auto",false);
    auto row=db->getTransactionRow(getParam("id"));if(!row)throw CommandException("Transaction not found : "+id);
    auto witness=row->getWitness();std::vector<std::string> addedScripts;
    if(automatic){
        auto transaction=row->getTransaction();auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
        for(const auto& input:sqlRequired(transaction).getAllInputs()){
            if(!sqlRequired(witness).getScript(input.getAddress())){
                auto scriptRow=sqlRequired(wallet).getScriptFromAddress(input.getAddress().to0xString());
                if(scriptRow){
                    if(!scriptRow->getScript())throw JavaNullPointerException("null script");
                    ScriptProof proof(*scriptRow->getScript());witness->addScript(proof);addedScripts.push_back(proof.getScript().toString());
                }
            }
        }
    }else{
        for(const auto& [key,value]:sqlRequired(scripts).entries()){
            if(value.isNull())throw JavaNullPointerException("null script proof");
            if(!value.isString())throw JavaClassCastException("Cannot cast script proof to String");
            auto text=value.asString();auto script=unicode::encodeUTF8(key);
            auto proof=text.empty()?ScriptProof(script):ScriptProof(script,MMRProof::convertMiniDataVersion(MiniData::fromHex(text)));
            sqlRequired(witness).addScript(proof);addedScripts.push_back(proof.getScript().toString());
        }
    }
    result.put("response",sqlRequired(db->getTransactionRow(id)).toJSON());return result;
}
std::shared_ptr<Command> txnscript::getFunction() const {return std::make_shared<txnscript>();}
}
