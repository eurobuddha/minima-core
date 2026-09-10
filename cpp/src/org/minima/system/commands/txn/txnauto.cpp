#include "txnauto.hpp"
#include "txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/AutomaticTransaction.hpp"
#include "support/CommandResult.hpp"
namespace minima::commands {
txnauto::txnauto():Command("txnauto","[id:] [amount:] [address:] (tokenid:) (sign:) (burn:) (mmrscript:) - Create a transaction automatically"){}
json::JSONValue txnauto::runCommand(){
    auto result=getJSONReply();auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();
    auto id=getParam("id");auto address=getAddressParam("address");auto amount=getNumberParam("amount");
    auto tokenid=getAddressParam("tokenid","0x00");bool sign=getBooleanParam("sign",false),mmrscript=getBooleanParam("mmrscript",true);
    auto burn=getNumberParam("burn",MiniNumber::ZERO);
    if(burn.isMore(MiniNumber::ZERO)&&tokenid!="0x00")throw CommandException("Currently BURN on precreated transactions only works for Minima.. tokenid:0x00.. not tokens.");
    if(sqlRequired(db).getTransactionRow(id))throw CommandException("Txn with this ID already exists : "+id);
    db->createTransaction(id);
    auto command="txnaddamount id:"+id+" burn:"+burn.toString()+" address:"+address+" amount:"+amount.toString()+" tokenid:"+tokenid;
    auto child=CommandRunner::getRunner().runSingleCommand(command);
    if(!command_result::status(child)){db->deleteTransaction(id);command_result::reject(child);}
    auto row=db->getTransactionRow(id);auto transaction=sqlRequired(row).getTransaction();auto witness=row->getWitness();
    if(mmrscript)txnutils::setMMRandScripts(sqlRequired(transaction),sqlRequired(witness));
    if(sign){
        child=CommandRunner::getRunner().runSingleCommand("txnsign id:"+id+" publickey:auto");
        if(!command_result::status(child)){db->deleteTransaction(id);command_result::reject(child);}
    }
    result.put("response",sqlRequired(db->getTransactionRow(id)).toJSON());return result;
}
std::shared_ptr<TxnRow> txnauto::createTransaction(const std::optional<std::string>& address,const MiniNumber* amount){
    (void)address;return automatic_transaction::create({},nullptr,amount,true,true);
}
}
