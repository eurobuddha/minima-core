#include "txnoutput.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
namespace minima::commands {
txnoutput::txnoutput():Command("txnoutput","[id:] [amount:] [address:] (tokenid:) (storestate:) - Create a transaction output"){}
std::optional<std::string> txnoutput::getFullHelp() const {return std::string(
"\ntxnoutput\n"
"\n"
"Create a transaction output.\n"
"\n"
"This will create a new coin (UTxO).\n"
"\n"
"If the sum of inputs > outputs, the difference will be burned unless change to the sender is defined as an output.\n"
"\n"
"Optionally store the transaction state variables in the new output coin.\n"
"\n"
"id:\n"
"    The id of the transaction to add an output to.\n"
"\n"
"amount:\n"
"    The amount for the output. To send to the specified address.\n"
"\n"
"address:\n"
"    Address of the recipient/script to send the output to. Can be 0x or Mx address.\n"
"\n"
"tokenid: (optional)\n"
"    tokenid of the output. Default is Minima (0x00).\n"
"\n"
"storestate: (optional)\n"
"    true or false, true will keep the state variables of the transaction in the newly created output coin.\n"
"    Default is true.\n"
"\n"
"Examples:\n"
"\n"
"txnoutput id:simpletxn amount:10 address:0xFED5..\n"
"\n"
"txnoutput id:multisig amount:10 address:0xFED5.. tokenid:0xCEF5.. storestate:false\n"
"\n"
"txnoutput id:eltootxn amount:10 address:0xFED5..\n");}
std::vector<std::string> txnoutput::getValidParams() const {return {"id","amount","address","tokenid","storestate"};}
json::JSONValue txnoutput::runCommand(){
    auto result=getJSONReply();auto owner=TxnDB::getRuntimeDB();auto& db=*owner;auto id=getParam("id");auto amount=getNumberParam("amount");auto address=MiniData::fromHex(getAddressParam("address"));bool store=getBooleanParam("storestate",true);auto tokenid=Token::TOKENID_MINIMA;std::shared_ptr<Token> token;
    if(existsParam("tokenid")){tokenid=getDataParam("tokenid");if(!tokenid.isEqual(Token::TOKENID_MINIMA)){token=TxPoWSearcher::getToken(tokenid);if(!token)throw CommandException("Token not found : "+tokenid.to0xString());}}
    auto minimaAmount=token?token->getScaledMinimaAmount(amount):amount;Coin output(Coin::COINID_OUTPUT,address,minimaAmount,tokenid,store);if(token)output.setToken(*token);
    auto row=db.getTransactionRow(getParam("id"));if(!row)throw CommandException("Transaction not found : "+id);auto transaction=row->getTransaction();if(!transaction)throw std::runtime_error("Null transaction");transaction->addOutput(output);TxPoWGenerator::precomputeTransactionCoinID(*transaction);transaction->calculateTransactionID();result.put("response",db.getTransactionRow(id)->toJSON());return result;
}
std::shared_ptr<Command> txnoutput::getFunction() const {return std::make_shared<txnoutput>();}
}
