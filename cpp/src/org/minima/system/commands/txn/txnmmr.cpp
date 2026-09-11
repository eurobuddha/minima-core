#include "org/minima/system/params/GlobalParams.hpp"
#include "txnmmr.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
txnmmr::txnmmr():Command("txnmmr","[id:] - Add MMR proofs to a transaction leaving those already present"){}
std::vector<std::string> txnmmr::getValidParams() const {return {"id"};}
json::JSONValue txnmmr::runCommand(){
    auto result=getJSONReply();auto db=TxnDB::getRuntimeDB();auto id=getParam("id");
    auto row=db->getTransactionRow(getParam("id"));if(!row)throw CommandException("Transaction not found : "+id);
    auto transaction=row->getTransaction();auto witness=row->getWitness();auto proofs=sqlRequired(witness).getAllCoinProofs()->size();
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto current=sqlRequired(tip).getBlockNumber();auto minimum=current.sub(GlobalParams::MINIMA_MMR_PROOF_HISTORY);
    const auto& coins=sqlRequired(transaction).getAllInputs();
    for(const auto& input:coins)if(input.getBlockCreated().isMore(minimum))minimum=input.getBlockCreated();
    auto node=tip->getPastNode(minimum);size_t counter=0;
    for(const auto& input:coins){
        if(++counter<=proofs)continue;
        auto proof=sqlRequired(sqlRequired(node).getMMR()).getProofToPeak(input.getMMREntryNumber());
        witness->addCoinProof(CoinProof(input,std::move(proof)));
    }
    result.put("response",sqlRequired(db->getTransactionRow(id)).toJSON());return result;
}
std::shared_ptr<Command> txnmmr::getFunction() const {return std::make_shared<txnmmr>();}
}
