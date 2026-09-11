#include "send.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "../txn/txnutils.hpp"
#include "../backup/vault.hpp"
#include "../search/keys.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/JavaLowerCase.hpp"
#include "support/JavaInteger.hpp"
#include "support/JavaFloating.hpp"
#include "support/NativeThrowable.hpp"
#include "support/SendRecipients.hpp"
#include "support/StateText.hpp"
namespace minima::commands {
using send_detail::stateText;using send_detail::nextRecipientToken;using send_detail::requiredToken;
send::send():Command("send","(address:Mx..|0x..) (amount:) (multi:[address:amount,..]) (tokenid:) (state:{}) (password:) (burn:) (split:) (coinage:) (mine:) (debug:) (dryrun:) - Send Minima or Tokens to an address"){}
std::optional<std::string> send::getFullHelp() const {return std::string(
"\nsend\n"
"\n"
"Send Minima or custom tokens to a wallet or custom script address.\n"
"\n"
"Optionally, send to multiple addresses in one transaction; split UTxOs; add state variables or include a burn.\n"
"\n"
"address: (optional)\n"
"    A Minima 0x or Mx wallet address or custom script address. Must also specify amount.\n"
"\n"
"amount: (optional)\n"
"    The amount of Minima or custom tokens to send to the specified address.\n"
"\n"
"multi: (optional)\n"
"    JSON Array listing addresses and amounts to send in one transaction.\n"
"    Takes the format [address:amount,address2:amount2,..], with each set in double quotes.\n"
"\n"
"tokenid: (optional)\n"
"    If sending a custom token, you must specify its tokenid. Defaults to Minima (0x00).\n"
"\n"
"state: (optional)\n"
"    List of state variables, if sending coins to a script. A JSON object in the format {\"port\":\"value\",..}\n"
"\n"
"burn: (optional)\n"
"    The amount of Minima to burn with this transaction.\n"
"\n"
"password: (optional)\n"
"    If your Wallet is password locked you can unlock it for this one transaction - then relock it.\n"
"\n"
"split: (optional)\n"
"    You can set the number of coins the recipient will receive, between 1 and 20. Default is 1.\n"
"    The amount being sent will be split into multiple coins of equal value.\n"
"    You can split your own coins by sending to your own address.\n"
"    Useful if you want to send multiple transactions without waiting for change to be confirmed.\n"
"\n"
"coinage: (optional)\n"
"    How old must the coins be in blocks.\n"
"\n"
"debug: (optional)\n"
"    true or false, true will print more detailed logs.\n"
"\n"
"dryrun: (optional)\n"
"    true or false, true will simulate the send transaction but not execute it.\n"
"\n"
"mine: (optional)\n"
"    true or false - should you mine the transaction immediately.\n"
"\n"
"fromaddress: (optional)\n"
"    Only use this address for input coins.\n"
"\n"
"signkey: (optional)\n"
"    Sign the txn with only this key (use with fromaddress).\n"
"\n"
"storestate: (optional)\n"
"    true or false - defaults to true. Should the output coins store the state (will still appear in NOTIFYCOIN messages).\n"
"\n"
"Examples:\n"
"\n"
"send address:0xFF.. amount:10\n"
"\n"
"send address:0xFF.. amount:10 tokenid:0xFED5.. burn:0.1\n"
"\n"
"send address:0xFF.. amount:10 split:5 burn:0.1\n"
"\n"
"send multi:[\"0xFF..:10\",\"0xEE..:10\",\"0xDD..:10\"] split:20\n"
"\n"
"send amount:1 address:0xFF.. state:{\"0\":\"0xEE..\",\"1\":\"0xDD..\"}\n");}
std::vector<std::string> send::getValidParams() const {return {"action","uid",
				"address","amount","multi","tokenid","state","burn","coinage",
				"split","debug","dryrun","mine","password","storestate",
				"fromaddress","signkey"};}
std::shared_ptr<Command> send::getFunction() const {return std::make_shared<send>();}
std::vector<Coin> send::orderCoins(std::vector<Coin>& coins) {
    std::stable_sort(coins.begin(),coins.end(),[](const Coin& left,const Coin& right){return left.getAmount().isMore(right.getAmount());});
    std::vector<std::string> addresses;
    for(const auto& coin:coins){auto address=coin.getAddress().to0xString();if(std::find(addresses.begin(),addresses.end(),address)==addresses.end())addresses.push_back(address);}
    std::vector<Coin> result;
    for(const auto& address:addresses)for(const auto& coin:coins)if(coin.getAddress().to0xString()==address)result.push_back(coin);
    return result;
}
std::vector<Coin> send::selectCoins(std::vector<Coin>& coins,const MiniNumber& required,bool debug) {
    auto db=sqlRequired(MinimaDB::getDB()).getTxPoWDB();auto miner=txnutils::getMiner();
    auto ordered=orderCoins(coins);
    if(debug){
        MinimaLogger::log("All Selection coins");
        for(const auto& coin:ordered)MinimaLogger::log("Coin found : "+coin.getAmount().toString()+" "+coin.getCoinID().to0xString()+" @ "+coin.getAddress().to0xString());
        MinimaLogger::log("Now checking coins");
    }
    std::optional<Coin> current;
    for(const auto& coin:ordered){
        auto description=coin.getAmount().toString()+" "+coin.getCoinID().to0xString();
        if(sqlRequired(miner).checkForMiningCoin(coin.getCoinID().to0xString())){if(debug)MinimaLogger::log("Coin being mined : "+description);continue;}
        if(sqlRequired(db).checkMempoolCoins(coin.getCoinID())){if(debug)MinimaLogger::log("Coin in mempool : "+description);continue;}
        if(coin.getAmount().isMoreEqual(required)){
            if(debug)MinimaLogger::log("Valid Coin found : "+description);current=coin;
        }else{if(debug)MinimaLogger::log("Coin too small - no more checking : "+description);break;}
    }
    if(current){if(debug)MinimaLogger::log("Single coin returned : "+current->getAmount().toString()+" "+current->getCoinID().to0xString());return {*current};}
    if(debug)MinimaLogger::log("Returning all coins..");return ordered;
}

json::JSONValue send::runCommand() {
    auto result=getJSONReply();std::vector<AddressAmount> recipients;auto total=MiniNumber::ZERO;
    if(existsParam("multi")){
        auto all=getJSONArrayParam("multi");
        for(const auto& value:sqlRequired(all).elements()){
            if(value.isNull())throw JavaNullPointerException("null recipient");
            if(!value.isString())throw JavaClassCastException("Cannot cast recipient to String");
            auto text=value.asString();size_t offset=0;auto address=nextRecipientToken(text,offset);MiniData data;
            if(javaLowerCase(address).starts_with("mx")){
                try{data=Address::convertMinimaAddress(address);}catch(const std::invalid_argument& error){throw CommandException(javaExceptionText(error));}
            }else data=MiniData::fromHex(address);
            auto amount=MiniNumber(nextRecipientToken(text,offset));total=total.add(amount);recipients.emplace_back(data,amount);
        }
    }else{
        auto address=MiniData::fromHex(getAddressParam("address"));auto amount=getNumberParam("amount");total=amount;recipients.emplace_back(address,amount);
    }
    auto tokenid=getParam("tokenid","0x00");bool debug=getBooleanParam("debug",false),dryrun=getBooleanParam("dryrun",false);if(dryrun)debug=true;
    bool storestate=getBooleanParam("storestate",true);auto burn=getNumberParam("burn",MiniNumber::ZERO);
    if(burn.isLess(MiniNumber::ZERO))throw CommandException("Cannot have negative burn "+burn.toString());
    auto split=getNumberParam("split",MiniNumber::ONE).floor();
    if(split.isLess(MiniNumber::ONE)||split.isMore(GeneralParams::MAX_SPLIT_COINS))throw CommandException("Split must be whole number from 1 to "+GeneralParams::MAX_SPLIT_COINS.toString());
    if(tokenid=="0x00")total=total.add(burn);
    auto state=std::make_shared<json::JSONObject>();if(existsParam("state"))state=getJSONObjectParam("state");
    bool mineSync=getBooleanParam("mine",false);
    auto txpdb=sqlRequired(MinimaDB::getDB()).getTxPoWDB();auto miner=txnutils::getMiner();
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto coinage=getNumberParam("coinage",GlobalParams::MINIMA_CONFIRM_DEPTH);
    if(coinage.isLess(GlobalParams::MINIMA_CONFIRM_DEPTH))throw CommandException("Coinage MUST be >= "+GlobalParams::MINIMA_CONFIRM_DEPTH.toString());
    auto publicKey=getAddressParam("signkey","");auto from=getAddressParam("fromaddress","");bool useFrom=!from.empty();
    if(useFrom&&debug)MinimaLogger::log("Search only coins with address : "+from);
    auto found=useFrom?TxPoWSearcher::searchCoins(tip,true,false,MiniData::ZERO_TXPOWID,false,MiniNumber::ZERO,true,MiniData::fromHex(from),true,MiniData::fromHex(tokenid),false):TxPoWSearcher::getRelevantUnspentCoins(tip,tokenid,true);
    auto minimum=sqlRequired(tip).getBlockNumber().sub(coinage);std::vector<Coin> coins;
    for(const auto& coin:found)if(coin.getBlockCreated().isLessEqual(minimum))coins.push_back(coin);
    if(coins.empty())throw CommandException("No Coins of tokenid:"+tokenid+" available!");
    auto findAmount=total;if(tokenid!="0x00")findAmount=requiredToken(coins.front().getToken()).getScaledMinimaAmount(total);
    coins=selectCoins(coins,findAmount,debug);auto current=MiniNumber::ZERO;std::vector<Coin> selected;std::optional<Token> token;
    if(debug){MinimaLogger::log("Coins that will be checked for transaction");for(const auto& coin:coins)MinimaLogger::log("Coin : "+coin.getAmount().toString()+" "+coin.getCoinID().to0xString());}
    for(const auto& coin:coins){
        auto description=coin.getAmount().toString()+" "+coin.getCoinID().to0xString();
        if(sqlRequired(miner).checkForMiningCoin(coin.getCoinID().to0xString())){if(debug)MinimaLogger::log("Coin being mined : "+description);continue;}
        if(sqlRequired(txpdb).checkMempoolCoins(coin.getCoinID())){if(debug)MinimaLogger::log("Coin in mempool : "+description);continue;}
        selected.push_back(coin);
        if(tokenid=="0x00")current=current.add(coin.getAmount());
        else{if(!token)token=coin.getToken();current=current.add(requiredToken(coin.getToken()).getScaledTokenAmount(coin.getAmount()));}
        if(debug)MinimaLogger::log("Coin added : "+description+" total:"+current.toString());
        if(current.isMoreEqual(total))break;
    }
    if(token&&token->getTokenScript().toString()!="RETURN TRUE"){
        result.put("status",false);result.put("message","Token script is not simple : "+token->getTokenScript().toString());return result;
    }
    if(current.isLess(total)){result.put("status",false);result.put("message","Insufficient funds.. you only have "+current.toString()+" require:"+total.toString());return result;}
    if(debug)MinimaLogger::log("Total Coins used : "+std::to_string(selected.size()));
    auto change=current.sub(total);auto transaction=std::make_shared<Transaction>();auto witness=std::make_shared<Witness>();auto minblock=MiniNumber::ZERO;
    for(const auto& coin:selected){
        transaction->addInput(coin);if(debug)MinimaLogger::log("Input : "+coin.toJSON().toJSONString());
        if(coin.getBlockCreated().isMore(minblock))minblock=coin.getBlockCreated();
    }
    auto difference=tip->getBlockNumber().sub(minblock);if(difference.isMore(GlobalParams::MINIMA_MMR_PROOF_HISTORY))difference=GlobalParams::MINIMA_MMR_PROOF_HISTORY;
    auto node=tip->getPastNode(tip->getBlockNumber().sub(difference));
    if(!node)throw CommandException("Not enough blocks in chain to make valid MMR Proofs..");
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();std::vector<Wallet::Text> signatures;std::vector<std::string> added;
    for(const auto& coin:selected){
        added.push_back(coin.getCoinID().to0xString());auto proof=sqlRequired(node->getMMR()).getProofToPeak(coin.getMMREntryNumber());witness->addCoinProof(CoinProof(coin,std::move(proof)));
        auto address=coin.getAddress().to0xString();auto script=sqlRequired(wallet).getScriptFromAddress(address);
        if(!script)throw CommandException("SERIOUS ERROR script missing for simple address : "+address);
        if(!script->getScript())throw JavaNullPointerException("null script");witness->addScript(ScriptProof(*script->getScript()));
        auto key=script->getPublicKey();if(std::find(signatures.begin(),signatures.end(),key)==signatures.end())signatures.push_back(key);
    }
    // The stock Command helper returns the literal empty string for absent/0x keys.
    if(!publicKey.empty()){signatures.clear();signatures.push_back(publicKey);}
    if(tokenid!="0x00"){
        auto amount=requiredToken(token).getScaledMinimaAmount(total);
        if(!token->getScaledTokenAmount(amount).isEqual(total))throw CommandException("Invalid Token amount to send.. "+total.toString());total=amount;
    }else if(!total.isValidMinimaValue())throw CommandException("Invalid Minima amount to send.. "+total.toString());
    for(const auto& user:recipients){
        auto userTotal=user.getAmount();if(tokenid!="0x00")userTotal=requiredToken(token).getScaledMinimaAmount(userTotal);
        auto currentTotal=MiniNumber::ZERO;auto splitamount=userTotal.div(split);
        if(splitamount.isLessEqual(MiniNumber::ZERO))throw CommandException("Cannot have ZERO output - output is too small for this user.. "+user.getAddress().to0xString());
        for(int32_t index=0;index<split.getAsInt();++index){
            Coin output(Coin::COINID_OUTPUT,user.getAddress(),splitamount,Token::TOKENID_MINIMA,storestate);currentTotal=currentTotal.add(splitamount);
            if(tokenid!="0x00"){output.resetTokenID(MiniData::fromHex(tokenid));output.setToken(token);}
            transaction->addOutput(output);if(debug)MinimaLogger::log("Output : "+output.toJSON().toJSONString());
        }
        auto remainder=userTotal.sub(currentTotal);
        if(remainder.isMore(MiniNumber::ZERO)){
            Coin output(Coin::COINID_OUTPUT,user.getAddress(),remainder,Token::TOKENID_MINIMA,storestate);
            if(tokenid!="0x00"){output.resetTokenID(MiniData::fromHex(tokenid));output.setToken(token);}
            transaction->addOutput(output);if(debug)MinimaLogger::log("Rounding Output from split : "+output.toJSON().toJSONString());
        }
    }
    if(debug)MinimaLogger::log("Change amount : "+change.toString());
    if(change.isMore(MiniNumber::ZERO)){
        auto row=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getDefaultAddress();
        if(sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable())
            if(!keys::checkKey(sqlRequired(row).getPublicKey()))throw CommandException("[!] SERIOUS ERROR - INCORRECT Public key : "+row->getPublicKey().value_or("null"));
        auto address=sqlRequired(row).getAddress();if(!address)throw JavaNullPointerException("null change address");
        auto changeAddress=MiniData::fromHex(*address);if(useFrom)changeAddress=MiniData::fromHex(from);
        auto amount=change;if(tokenid!="0x00")amount=requiredToken(token).getScaledMinimaAmount(change);
        Coin output(Coin::COINID_OUTPUT,changeAddress,amount,Token::TOKENID_MINIMA,false);
        if(tokenid!="0x00"){output.resetTokenID(MiniData::fromHex(tokenid));output.setToken(token);}
        transaction->addOutput(output);if(debug)MinimaLogger::log("Change Output : "+output.toJSON().toJSONString());
    }
    auto outputCount=transaction->getAllOutputs()->size();
    if(int64_t(outputCount)>GeneralParams::MAX_RELAY_OUTPUTCOINS)throw CommandException("Too many outputs "+std::to_string(outputCount)+" - will not be relayed by network");
    for(const auto& [port,value]:sqlRequired(state).entries())transaction->addStateVariable(StateVariable(javaParseInt(unicode::encodeUTF8(port)),stateText(value)));
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);transaction->calculateTransactionID();
    if(debug)MinimaLogger::log(std::string(publicKey.empty()?"Total signatures required : ":"(SIGNKEYS) Total signatures required : ")+std::to_string(signatures.size()));
    bool passwordLock=false;
    if(!dryrun){
        if(existsParam("password")){
            if(sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable())throw CommandException("WalletDB NOT Locked! Password Invalid");
            if(debug)MinimaLogger::log("Unlocking password DB");vault::passowrdUnlockDB(getParam("password"));passwordLock=true;
        }
    }else MinimaLogger::log("DRYRUN so NOT Unlocking password DB");
    for(const auto& key:signatures){
        if(debug)MinimaLogger::log("Signing transction with : "+key.value_or("null"));
        if(!dryrun){try{witness->addSignature(sqlRequired(wallet).signData(key,transaction->getTransactionID().get()));}
            catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
            catch(const std::exception& error){throw CommandException(nativeExceptionText(error));}}
        else MinimaLogger::log("DRY RUN - not signing");
    }
    std::shared_ptr<TxPoW> pow;
    if(tokenid!="0x00"&&burn.isMore(MiniNumber::ZERO)){
        auto burned=txnutils::createBurnTransaction(added,transaction->transactionIDValue(),burn);
        pow=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(transaction,witness,burned->getTransaction(),burned->getWitness()));
    }else pow=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(transaction,witness));
    pow->calculateTXPOWID();auto size=pow->getSizeinBytesWithoutBlockTxns();auto maximum=Magic::required(tip->getTxPoW().getMagic().getMaxTxPoWSize()).getAsLong();
    if(debug)MinimaLogger::log("TxPoW size "+std::to_string(size)+" max:"+std::to_string(maximum));
    if(size>maximum){if(!dryrun&&passwordLock)vault::passwordLockDB(getParam("password"));throw CommandException("TxPoW size too large.. "+std::to_string(size)+"/"+std::to_string(maximum));}
    if(!dryrun){
        if(passwordLock)vault::passwordLockDB(getParam("password"));
        auto active=txnutils::getMiner();
        if(mineSync){if(!sqlRequired(active).MineMaxTxPoW(false,pow,120000))throw CommandException("FAILED TO MINE txn in 120 seconds !?");}
        else sqlRequired(active).mineTxPoWAsync(pow);
    }else MinimaLogger::log("DRY RUN - not sending");
    result.put("dryrun",dryrun);result.put("response",pow->toJSON());
    if(dryrun){
        json::JSONObject sizes,inputcoins,outputcoins;sizes.put("txpow",pow->getSizeinBytes());
        // Searcher de-duplicates input IDs; precompute generates each output ID.
        // Their string representations are the keys emitted by Java's JSON writer.
        for(const auto txnItems=pow->getTransaction().getAllInputs();const auto& coin:*txnItems)inputcoins.put(coin.getCoinID().to0xString(),int64_t(coin.serialise().size()));
        for(const auto txnItems=pow->getTransaction().getAllOutputs();const auto& coin:*txnItems)outputcoins.put(coin.getCoinID().to0xString(),int64_t(coin.serialise().size()));
        sizes.put("inputcoins",inputcoins);sizes.put("outputcoins",outputcoins);sizes.put("witness",int64_t(pow->getWitness().serialise().size()));result.put("bytesize",sizes);
    }
    return result;
}
}
