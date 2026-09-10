#include "txnaddamount.hpp"
#include "../send/send.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima::commands {
txnaddamount::txnaddamount():Command("txnaddamount","[id:] [amount:] (address) (onlychange:) (tokenid:) (split:) (burn:) - Add inputs and calculate change for a certain amount"){}
std::optional<std::string> txnaddamount::getFullHelp() const {return std::string(
"\txnaddamount\n"
"\n"
"Add a certain amount to a transaction.\n"
"\n"
"Use in conjunction with txncoinlock to create multiple offline transactions.\n"
"\n"
"Output amount to an address OR only the change - if you have already added an output\n"
"\n");}
std::vector<std::string> txnaddamount::getValidParams() const {return {"id","amount","address","onlychange","tokenid","fromaddress","burn","storestate","split"};}
std::shared_ptr<Command> txnaddamount::getFunction() const {return std::make_shared<txnaddamount>();}
std::vector<std::string> txnaddamount::LOCKED_COINS;
bool txnaddamount::LOCKED_COINS_ENABLED=false;
void txnaddamount::enableCoinLock(bool enabled){LOCKED_COINS_ENABLED=enabled;if(!enabled)clearCoinLocked();}
bool txnaddamount::isCoinLockEnabled(){return LOCKED_COINS_ENABLED;}
void txnaddamount::addCoinLock(const std::string& id){if(LOCKED_COINS_ENABLED&&!isCoinLocked(id))LOCKED_COINS.push_back(id);}
bool txnaddamount::isCoinLocked(const std::string& id){return LOCKED_COINS_ENABLED&&std::find(LOCKED_COINS.begin(),LOCKED_COINS.end(),id)!=LOCKED_COINS.end();}
void txnaddamount::clearCoinLocked(){LOCKED_COINS.clear();}
json::JSONValue txnaddamount::runCommand() {
    auto result=getJSONReply();auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();
    auto id=getParam("id");auto amount=getNumberParam("amount");
    auto tokenid=Token::TOKENID_MINIMA;std::shared_ptr<Token> token;
    if(existsParam("tokenid")){
        tokenid=getDataParam("tokenid");
        if(!tokenid.isEqual(Token::TOKENID_MINIMA)){
            token=TxPoWSearcher::getToken(tokenid);
            if(!token)throw CommandException("Token not found : "+tokenid.to0xString());
        }
    }
    auto burn=getNumberParam("burn",MiniNumber::ZERO);
    if(burn.isMore(MiniNumber::ZERO)&&!tokenid.isEqual(Token::TOKENID_MINIMA))
        throw CommandException("Currently BURN on precreated transactions only works for Minima.. tokenid:0x00.. not tokens.");
    auto miniamount=amount;if(token)miniamount=token->getScaledMinimaAmount(amount);
    auto row=sqlRequired(db).getTransactionRow(getParam("id"));
    if(!row)throw CommandException("Transaction not found : "+id);
    auto transaction=row->getTransaction();
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    if(!tip){result.put("response",json::JSONArray());return result;}
    auto tokenamount=amount;
    if(!tokenid.isEqual(Token::TOKENID_MINIMA))tokenamount=sqlRequired(token).getScaledMinimaAmount(amount);
    if(!tokenid.isEqual(Token::TOKENID_MINIMA)){
        if(!token->getScaledTokenAmount(tokenamount).isEqual(amount))throw CommandException("Invalid Token amount to send.. "+amount.toString());
    }else if(!tokenamount.isValidMinimaValue())throw CommandException("Invalid Minima amount to send.. "+tokenamount.toString());
    bool onlychange=getBooleanParam("onlychange",false),useaddress=false;auto fromaddress=MiniData::fromHex("0x00");
    if(existsParam("fromaddress")){useaddress=true;fromaddress=MiniData::fromHex(getAddressParam("fromaddress"));}
    std::vector<Coin> coins;
    if(!useaddress)coins=TxPoWSearcher::searchCoins(tip,true,false,MiniData::ZERO_TXPOWID,false,MiniNumber::ZERO,false,MiniData::ZERO_TXPOWID,true,tokenid,true);
    else coins=TxPoWSearcher::searchCoins(tip,false,false,MiniData::ZERO_TXPOWID,false,MiniNumber::ZERO,true,fromaddress,true,tokenid,false,"",true,false,INT32_MAX,GeneralParams::IS_MEGAMMR);
    if(isCoinLockEnabled()){
        std::vector<Coin> valid;for(const auto& coin:coins)if(!isCoinLocked(coin.getCoinID().to0xString()))valid.push_back(coin);coins=std::move(valid);
    }
    auto required=tokenamount.add(burn);auto selected=send::selectCoins(coins,required);
    auto total=MiniNumber::ZERO;for(const auto& coin:selected)total=total.add(coin.getAmount());
    auto change=total.sub(required);
    if(change.isLess(MiniNumber::ZERO)){
        if(!tokenid.isEqual(Token::TOKENID_MINIMA))total=token->getScaledTokenAmount(total);
        throw CommandException("Not enough funds! Current balance : "+total.toString());
    }
    for(const auto& coin:selected){sqlRequired(transaction).addInput(coin);if(isCoinLockEnabled())addCoinLock(coin.getCoinID().to0xString());}
    bool storestate=getBooleanParam("storestate",true);
    if(!onlychange){
        auto address=getAddressParam("address");auto split=getNumberParam("split",MiniNumber::ONE).getAsInt();
        auto splitamount=tokenamount.div(MiniNumber(split));auto totalSplit=MiniNumber::ZERO;
        for(int32_t index=0;index<split;++index){
            Coin output(MiniData::fromHex(address),splitamount,tokenid,storestate);
            if(!tokenid.isEqual(Token::TOKENID_MINIMA))output.setToken(*token);
            sqlRequired(transaction).addOutput(output);totalSplit=totalSplit.add(splitamount);
        }
        auto remainder=tokenamount.sub(totalSplit);
        if(remainder.isMore(MiniNumber::ZERO)){
            Coin output(MiniData::fromHex(address),remainder,tokenid,storestate);
            if(!tokenid.isEqual(Token::TOKENID_MINIMA))output.setToken(*token);
            sqlRequired(transaction).addOutput(output);
        }
    }
    if(change.isMore(MiniNumber::ZERO)){
        auto address=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getDefaultAddress();
        auto text=sqlRequired(address).getAddress();if(!text)throw JavaNullPointerException("null change address");
        auto changeAddress=MiniData::fromHex(*text);if(useaddress)changeAddress=fromaddress;
        Coin output(Coin::COINID_OUTPUT,changeAddress,change,tokenid,false);
        if(!tokenid.isEqual(Token::TOKENID_MINIMA))output.setToken(*token);
        sqlRequired(transaction).addOutput(output);
    }
    TxPoWGenerator::precomputeTransactionCoinID(sqlRequired(transaction));transaction->calculateTransactionID();
    result.put("response",sqlRequired(db->getTransactionRow(id)).toJSON());return result;
}
}
