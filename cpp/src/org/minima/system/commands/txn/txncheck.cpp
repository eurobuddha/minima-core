#include "txncheck.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/brains/TxPoWChecker.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
namespace minima::commands {
txncheck::txncheck():Command("txncheck","[id:] - Show details about the transaction"){}
std::optional<std::string> txncheck::getFullHelp() const {return std::string(
"\ntxncheck\n"
"\n"
"Show details about the transaction.\n"
"\n"
"Verify whether the inputs, outputs, signatures, proofs and scripts are valid.\n"
"\n"
"id: (optional)\n"
"    The id of the transaction to check.\n"
"\n"
"Examples:\n"
"\n"
"txncheck id:multisig\n");}
std::vector<std::string> txncheck::getValidParams() const {return {"id"};}
json::JSONValue txncheck::runCommand() {
    auto result=getJSONReply();auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();
    auto id=getParam("id");auto row=sqlRequired(db).getTransactionRow(getParam("id"));
    if(!row) throw CommandException("Transaction not found : "+id);
    auto transaction=row->getTransaction();auto witness=row->getWitness();
    const auto& inputs=sqlRequired(row->getTransaction()).getAllInputs();
    const auto& outputs=sqlRequired(row->getTransaction()).getAllOutputs();
    json::JSONObject details;std::vector<std::string> tokens;
    for(const auto* list:{&inputs,&outputs}) for(const auto& coin:*list) {
        auto token=coin.getTokenID().isEqual(Token::TOKENID_CREATE)?Token::TOKENID_MINIMA:coin.getTokenID();
        auto text=token.to0xString();
        if(std::find(tokens.begin(),tokens.end(),text)==tokens.end()) tokens.push_back(text);
    }
    json::JSONArray allTokens;
    for(const auto& text:tokens) {
        auto token=MiniData::fromHex(text);auto out=transaction->sumOutputs(token);auto in=transaction->sumInputs(token);
        json::JSONObject amounts;amounts.put("tokenid",text);amounts.put("input",in.toString());
        amounts.put("output",out.toString());amounts.put("difference",in.sub(out).toString());allTokens.add(amounts);
    }
    details.put("coins",allTokens);details.put("tokens",int64_t(tokens.size()));
    details.put("inputs",int64_t(inputs.size()));
    details.put("mmrproofs",int64_t(sqlRequired(witness).getAllCoinProofs().size()));
    details.put("scripts",int64_t(witness->getAllScripts().size()));
    bool correctProofCount=inputs.size()==witness->getAllCoinProofs().size();
    auto totalIn=MiniNumber::ZERO;for(const auto& coin:inputs)totalIn=totalIn.add(coin.getAmount());
    details.put("outputs",int64_t(outputs.size()));json::JSONArray allOutputs;auto totalOut=MiniNumber::ZERO;
    for(const auto& coin:outputs) {totalOut=totalOut.add(coin.getAmount());allOutputs.add(coin.toJSON());}
    details.put("alloutputs",allOutputs);details.put("burn",totalIn.sub(totalOut).toString());
    details.put("validamounts",sqlRequired(row->getTransaction()).checkValid());
    details.put("signatures",int64_t(sqlRequired(row->getWitness()).getAllSignatures().size()));
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto temporary=TxPoWGenerator::generateTxPoW(transaction,witness);
    transaction->clearIsMonotonic();
    bool basic=TxPoWChecker::checkTxPoWBasic(temporary);
    json::JSONArray allSignatures;auto idToVerify=transaction->getTransactionID();bool allValid=true;
    for(const auto& signature:witness->getAllSignatures()) {
        TreeKey key;key.setPublicKey(signature.getRootPublicKey());
        json::JSONObject item;item.put("publickey",signature.getRootPublicKey().to0xString());
        bool valid=key.verify(idToVerify,signature);if(!valid)allValid=false;
        item.put("valid",valid);allSignatures.add(item);
    }
    details.put("allsignaturesvalid",allValid);
    bool mmr=correctProofCount&&TxPoWChecker::checkMMR(sqlRequired(tip).getMMR().get(),temporary);
    bool scripts=TxPoWChecker::checkTxPoWScripts(sqlRequired(tip).getMMR().get(),temporary,tip->getTxPoW());
    json::JSONObject valid;valid.put("basic",basic);valid.put("signatures",allSignatures);
    valid.put("mmrproofs",mmr);valid.put("scripts",scripts);details.put("valid",valid);
    // Pinned txncheck does not include allValid in this final conjunction.
    details.put("validtransaction",basic&&mmr&&scripts);result.put("response",details);return result;
}
std::shared_ptr<Command> txncheck::getFunction() const {return std::make_shared<txncheck>();}
}
