#include "txninput.hpp"
#include "txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
namespace minima::commands {
txninput::txninput():Command("txninput","[id:] (coinid:) (coindata:) (floating:) (address:) (amount:) (tokenid:) (scriptmmr:true)- Add a coin as an input to a transaction"){}
std::optional<std::string> txninput::getFullHelp() const {return std::string(
"\ntxninput\n"
"\n"
"Add a coin as an input to a transaction.\n"
"\n"
"Optionally specify address, amount and tokenid to use an unspecified coinid - a floating ELTOO coin.\n"
"\n"
"A floating coin can be attached to multiple different existing coins as long as it has the same\n"
"\n"
"address, amount and tokenid - but different coinid.\n"
"\n"
"id:\n"
"    The id of the transaction to add an input to.\n"
"\n"
"coinid: (optional)\n"
"    The id of the coin to add as an input.\n"
"\n"
"coindata: (optional)\n"
"    The data of the coin to add, instead of coinid.\n"
"    Can be from the 'coinexport' command or 'outputcoindata' from another transaction.\n"
"\n"
"floating: (optional)\n"
"    true or false, true will add an unspecified, floating ELTOO coin as an input.\n"
"    If true, also specify address, amount, tokenid.\n"
"    If false, specify a coinid or coindata.\n"
"\n"
"address: (optional)\n"
"    Coin address to use for the floating input. Can be 0x or Mx address.\n"
"    The coin that is used "
"\n"
"amount: (optional)\n"
"    Amount of a coin for the floating input.\n"
"\n"
"tokenid: (optional)\n"
"    tokenid of a coin for the floating input.\n"
"\n"
"scriptmmr: (optional)\n"
"    true or false, true will add the scripts and MMR proof for the coin.\n"
"\n"
"Examples:\n"
"\n"
"txninput id:simpletxn coinid:0xD0BF..\n"
"\n"
"txninput id:multisig coinid:0xD0BF.. scriptmmr:true\n"
"\n"
"txninput id:posttxn coindata:0x000..\n"
"\n"
"txninput id:eltootxn floating:true address:0xFED5.. amount:10 tokenid:0x00\n");}
std::vector<std::string> txninput::getValidParams() const {return {"id","coinid","coindata","floating","address","amount","tokenid","scriptmmr"};}
json::JSONValue txninput::runCommand() {
    auto result=getJSONReply();auto id=getParam("id");
    auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();
    auto row=sqlRequired(db).getTransactionRow(getParam("id"));
    if(!row) throw CommandException("Transaction not found : "+id);
    bool floating=getBooleanParam("floating",false);
    std::optional<Coin> coin;
    if(existsParam("coinid")) {
        auto coinid=getParam("coinid");
        coin=TxPoWSearcher::searchCoin(MiniData::fromHex(coinid));
        if(!coin) throw CommandException("CoinID not found : "+coinid);
        if(floating) coin->resetCoinID(Coin::COINID_ELTOO);
    } else if(existsParam("coindata")) {
        coin=Coin::convertMiniDataVersion(MiniData::fromHex(getParam("coindata")));
        if(!coin) throw CommandException("ERROR importing coin data");
        if(floating) coin->resetCoinID(Coin::COINID_ELTOO);
    } else {
        auto address=getAddressParam("address");auto amount=getParam("amount");
        auto tokenid=MiniData::fromHex(getParam("tokenid","0x00"));
        std::shared_ptr<Token> token;MiniNumber scaled(amount);
        if(!tokenid.isEqual(Token::TOKENID_MINIMA)) {
            token=TxPoWSearcher::getToken(tokenid);
            if(!token) throw CommandException("Could not find token : "+tokenid.to0xString());
            scaled=token->getScaledMinimaAmount(scaled);
        }
        coin=Coin(Coin::COINID_ELTOO,MiniData::fromHex(address),scaled,tokenid);
        if(token) coin->setToken(*token);
    }
    auto transaction=row->getTransaction();
    sqlRequired(transaction).addInput(*coin);
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);
    transaction->calculateTransactionID();
    if(getBooleanParam("scriptmmr",false)) txnutils::setMMRandScripts(*coin,sqlRequired(row->getWitness()));
    result.put("response",sqlRequired(db->getTransactionRow(id)).toJSON());return result;
}
std::shared_ptr<Command> txninput::getFunction() const {return std::make_shared<txninput>();}
}
