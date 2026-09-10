#include "txnsign.hpp"
#include "txnpost.hpp"
#include "../backup/vault.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
namespace minima::commands {
txnsign::txnsign():Command("txnsign","[id:] [publickey:0x..|auto] (txnpostauto:) (txnpostburn:) (txnpostmine:) (txndelete:) - Sign a transaction"){}
std::optional<std::string> txnsign::getFullHelp() const {return std::string(
"\ntxnsign\n"
"\n"
"Sign a transaction.\n"
"\n"
"Specify the public key or use 'auto' if the coin inputs are simple.\n"
"\n"
"id:\n"
"    The id of the transaction to sign.\n"
"\n"
"publickey:\n"
"    The public key specified in a custom script, or 'auto' for transactions with simple inputs.\n"
"\n"
"txnpostauto: (optional)\n"
"    Do you want to post this transaction. Use the same values as you would for txnpost auto(sort MMR and Scripts)\n"
"\n"
"txnpostburn: (optional)\n"
"    If you also post this transaction, do you want to add a burn transaction.\n"
"\n"
"txnpostmine: (optional)\n"
"    If you also post this transaction, do you want to mine it immediately.\n"
"\n"
"txndelete: (optional)\n"
"    true or false - delete this txn after signing AND posting.\n"
"\n"
"Examples:\n"
"\n"
"txnsign id:simpletxn publickey:auto\n"
"\n"
"txnsign id:simpletxn publickey:auto password:your_password\n"
"\n"
"txnsign id:multisig publickey:0xFD8B..\n"
"\n"
"txnsign id:simpletxn publickey:auto txnpostauto:true\n");}
std::vector<std::string> txnsign::getValidParams() const {return {"id","publickey","txndelete","txnpostauto","txnpostburn","txnpostmine","password","privatekey","keyuses"};}
std::shared_ptr<Command> txnsign::getFunction() const {return std::make_shared<txnsign>();}

json::JSONValue txnsign::runCommand(){
    auto result=getJSONReply();auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();auto id=getParam("id");auto publicKey=getParam("publickey");
    auto row=sqlRequired(db).getTransactionRow(getParam("id"));if(!row)throw CommandException("Transaction not found : "+id);
    auto transaction=row->getTransaction();auto witness=row->getWitness();TxPoWGenerator::precomputeTransactionCoinID(sqlRequired(transaction));transaction->calculateTransactionID();
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();json::JSONArray missing,found,nonsimple;json::JSONObject response;bool passwordLock=false;
    if(existsParam("password")&&!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable()){vault::passowrdUnlockDB(getParam("password"));passwordLock=true;}
    if(publicKey=="auto"){
        for(const auto& coin:transaction->getAllInputs()){
            auto script=sqlRequired(wallet).getScriptFromAddress(coin.getAddress().to0xString());
            if(!script){missing.add(coin.getAddress().to0xString());continue;}
            if(!script->isSimple()){nonsimple.add(script->getAddress()?json::JSONValue(*script->getAddress()):json::JSONValue());continue;}
            if(!sqlRequired(witness).isSignedBy(script->getPublicKey())){
                found.add(script->getPublicKey()?json::JSONValue(*script->getPublicKey()):json::JSONValue());
                auto signature=wallet->signData(script->getPublicKey(),transaction->getTransactionID());witness->addSignature(signature);
            }
        }
    }else if(publicKey=="custom"){
        auto privateKey=getDataParam("privatekey");auto uses=getNumberParam("keyuses");auto tree=TreeKey::createDefault(privateKey);tree.setUses(uses.getAsInt());
        sqlRequired(witness).addSignature(tree.sign(transaction->getTransactionID()));
    }else{
        auto key=sqlRequired(wallet).getKeyFromPublic(publicKey);
        if(!key){if(passwordLock)vault::passwordLockDB(getParam("password"));throw CommandException("Public Key not found : "+publicKey);}
        found.add(key->getPublicKey()?json::JSONValue(*key->getPublicKey()):json::JSONValue());auto signature=wallet->signData(key->getPublicKey(),transaction->getTransactionID());sqlRequired(witness).addSignature(signature);
    }
    if(passwordLock)vault::passwordLockDB(getParam("password"));
    response.put("keys",found);if(nonsimple.size()>0)response.put("nonsimple",nonsimple);if(missing.size()>0)response.put("notfound",missing);
    if(existsParam("txnpostauto")){
        auto automatic=getBooleanParam("txnpostauto");auto burn=getNumberParam("txnpostburn",MiniNumber::ZERO);auto mine=getBooleanParam("txnpostmine",false);
        auto pow=txnpost::postTxn(id,burn,automatic,mine);response.put("txnpost",true);response.put("txnpostauto",automatic);response.put("txnpostburn",burn.toString());response.put("txnpostmine",mine);response.put("txpow",pow->toJSON());
        auto remove=getBooleanParam("txndelete",false);if(remove){db->deleteTransaction(id);response.put("delete",true);}else response.put("delete",false);
    }else response.put("txnpost",false);
    result.put("response",response);return result;
}
}
