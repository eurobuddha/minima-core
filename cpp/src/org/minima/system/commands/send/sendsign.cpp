#include "sendsign.hpp"
#include "../backup/vault.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include <chrono>
#include "org/minima/objects/TxPoW.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "support/SqlRow.hpp"
namespace minima::commands {
sendsign::sendsign():Command("sendsign","[file:] - Sign a previously created txn"){}
std::optional<std::string> sendsign::getFullHelp() const {return std::string(
"\nsendsign\n"
"\n"
"Sign a transaction previously created by the 'sendnosign' command, by specifying its .txn file.\n"
"\n"
"Optionally, if the node is Vault password locked, provide the Vault password to decrypt the keys for signing,\n"
"\n"
"the keys will be automatically re-encrypted after signing.\n"
"\n"
"Can be signed on an offline node, then posted from an online node.\n"
"\n"
"Outputs a new .txn file for the signed txn, to be posted with the 'sendpost' command.\n"
"\n"
"file:\n"
"    Name of the unsigned transaction (.txn) file to sign, located in the node's base folder.\n"
"    If not in the base folder, specify the full file path.\n"
"\n"
"password:\n"
"    The Vault password, if the node is password locked.\n"
"\n"
"Examples:\n"
"\n"
"sendsign file:unsignedtransaction-1674907380057.txn\n"
"\n"
"sendsign file:C:\\Users\\unsignedtransaction-1674907380057.txn password:your_vaultpassword\n"
"\n");}
std::vector<std::string> sendsign::getValidParams() const {return {"file","password"};}
std::shared_ptr<Command> sendsign::getFunction() const {return std::make_shared<sendsign>();}
void sendsign::main(const std::vector<std::optional<std::string>>*){
    for(int i=0;i<512;++i)std::cout<<MiniData(BigInt(std::to_string(i))).to0xString()<<std::endl;
}
json::JSONValue sendsign::runCommand(){
    auto result=getJSONReply();auto filename=getParam("file");
    auto bytes=MiniFile::readCompleteFile(MiniFile::createBaseFile(filename));
    auto pow=TxPoW::convertMiniDataVersion(MiniData(std::move(bytes)));
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    std::vector<Wallet::Text> requiredSignatures;bool passwordLock=false;
    if(existsParam("password")&&!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable()){
        vault::passowrdUnlockDB(getParam("password"));passwordLock=true;
    }
    if(!pow)throw JavaNullPointerException("null decoded TxPoW");
    // The source never appends to requiredSignatures. Preserve repeated signing.
    for(bool burn:{false,true}){
        auto& transaction=burn?pow->getBurnTransaction():pow->getTransaction();
        auto& witness=burn?pow->getBurnWitness():pow->getWitness();
        for(const auto& input:transaction.getAllInputs()){
            auto address=input.getAddress().to0xString();auto row=sqlRequired(wallet).getScriptFromAddress(address);
            if(!row)throw CommandException("SERIOUS ERROR script missing for simple address : "+address);
            if(!row->getScript())throw JavaNullPointerException("null script");witness.addScript(ScriptProof(*row->getScript()));
            auto key=row->getPublicKey();
            if(std::find(requiredSignatures.begin(),requiredSignatures.end(),key)==requiredSignatures.end())witness.addSignature(wallet->signData(key,transaction.getTransactionID()));
        }
    }
    if(passwordLock)vault::passwordLockDB(getParam("password"));
    pow->calculateTXPOWID();
    auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto file=MiniFile::createBaseFile("signedtransaction-"+std::to_string(now)+".txn");MiniFile::writeObjectToFile(file,*pow);
    json::JSONObject response;response.put("txpow",std::filesystem::absolute(file).string());result.put("response",response);return result;
}
}
