#include "sendnosign.hpp"
#include "send.hpp"
#include "../txn/txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaLowerCase.hpp"
#include "support/JavaInteger.hpp"
#include "support/SendRecipients.hpp"
#include <chrono>
namespace minima::commands {
using send_detail::nextRecipientToken;using send_detail::requiredToken;
sendnosign::sendnosign():Command("sendnosign","(address:Mx..|0x..) (amount:) (multi:[address:amount,..]) (tokenid:) (state:{}) (burn:) (split:) (file:) (debug:) - Create a txn but don't sign it"){}
std::optional<std::string> sendnosign::getFullHelp() const {return std::string(
"\nsendnosign\n"
"\n"
"Create a txn but don't sign it.\n"
"\n"
"Constructs and outputs an unsigned transaction to a file in the base folder\n"
"\n"
"The output .txn file can then be imported to an offline node for signing.\n"
"\n"
"Must be done from an online node as the MMR proofs for the input coins are added.\n"
"\n"
"Useful when the keys on an online node are wiped or password locked.\n"
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
"split: (optional)\n"
"    Set the number of coins the recipient will receive, between 1 and 20. Default is 1.\n"
"    The amount being sent will be split into multiple coins of equal value.\n"
"    You can split your own coins by sending to your own address.\n"
"    Useful if you want to send multiple transactions without waiting for change to be confirmed.\n"
"\n"
"file: (optional)\n"
"    Specify the file to output otherwise default chosen\n"
"\n"
"debug: (optional)\n"
"    true or false, true will print more detailed logs.\n"
"\n"
"Examples:\n"
"\n"
"sendnosign address:0xFF.. amount:10\n"
"\n"
"sendnosign address:0xFF.. amount:10 tokenid:0xFED5.. burn:0.1\n"
"\n"
"sendnosign address:0xFF.. amount:10 split:5 burn:0.1\n"
"\n"
"sendnosign multi:[\"0xFF..:10\",\"0xEE..:10\",\"0xDD..:10\"] split:20\n"
"\n"
"sendnosign amount:1 address:0xFF.. state:{\"0\":\"0xEE..\",\"1\":\"0xDD..\"}\n"
"\n");}
std::vector<std::string> sendnosign::getValidParams() const {return {"address","amount","multi",
				"tokenid","state","burn","split","debug","dryrun","file"};}
std::shared_ptr<Command> sendnosign::getFunction() const {return std::make_shared<sendnosign>();}
json::JSONValue sendnosign::runCommand() {
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
    auto tokenid=getParam("tokenid","0x00");bool debug=getBooleanParam("debug",false);
    auto burn=getNumberParam("burn",MiniNumber::ZERO);
    if(burn.isLess(MiniNumber::ZERO))throw CommandException("Cannot have negative burn "+burn.toString());
    auto split=getNumberParam("split",MiniNumber::ONE);
    if(split.isLess(MiniNumber::ONE)||split.isMore(MiniNumber::TWENTY))throw CommandException("Split outputs from 1 to 20");
    if(tokenid=="0x00")total=total.add(burn);
    auto state=std::make_shared<json::JSONObject>();if(existsParam("state"))state=getJSONObjectParam("state");
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto confirmationDepth=GlobalParams::MINIMA_CONFIRM_DEPTH.getAsInt();
    for(int32_t i=0;i<confirmationDepth;++i){
        tip=sqlRequired(tip).getParent();
        if(!tip){result.put("status",false);result.put("message","Insufficient blocks..");return result;}
    }
    auto txpdb=sqlRequired(MinimaDB::getDB()).getTxPoWDB();auto miner=txnutils::getMiner();
    auto coinage=getNumberParam("coinage",MiniNumber::ZERO);
    auto found=TxPoWSearcher::getRelevantUnspentCoins(tip,tokenid,true);
    auto minimum=sqlRequired(tip).getBlockNumber().sub(coinage);std::vector<Coin> coins;
    for(const auto& coin:found)if(coin.getBlockCreated().isLessEqual(minimum))coins.push_back(coin);
    if(coins.empty())throw CommandException("No Coins of tokenid:"+tokenid+" available!");
    auto findAmount=total;if(tokenid!="0x00")findAmount=requiredToken(coins.front().getToken()).getScaledMinimaAmount(total);
    coins=send::selectCoins(coins,findAmount,debug);auto current=MiniNumber::ZERO;std::vector<Coin> selected;std::optional<Token> token;
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
        transaction->addInput(coin);
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
    if(tokenid!="0x00"){
        auto amount=requiredToken(token).getScaledMinimaAmount(total);
        if(!token->getScaledTokenAmount(amount).isEqual(total))throw CommandException("Invalid Token amount to send.. "+total.toString());total=amount;
    }else if(!total.isValidMinimaValue())throw CommandException("Invalid Minima amount to send.. "+total.toString());
    for(const auto& user:recipients){
        auto splitamount=user.getAmount().div(split);
        if(tokenid!="0x00")splitamount=requiredToken(token).getScaledMinimaAmount(splitamount);
        for(int32_t i=0;i<split.getAsInt();++i){
            Coin output(Coin::COINID_OUTPUT,user.getAddress(),splitamount,Token::TOKENID_MINIMA,true);
            if(tokenid!="0x00"){output.resetTokenID(MiniData::fromHex(tokenid));output.setToken(token);}
            transaction->addOutput(output);
        }
    }
    if(debug)MinimaLogger::log("Change amount : "+change.toString());
    if(change.isMore(MiniNumber::ZERO)){
        auto row=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getDefaultAddress();
        auto address=sqlRequired(row).getAddress();if(!address)throw JavaNullPointerException("null change address");
        auto changeAddress=MiniData::fromHex(*address);
        auto amount=change;if(tokenid!="0x00")amount=requiredToken(token).getScaledMinimaAmount(change);
        Coin output(Coin::COINID_OUTPUT,changeAddress,amount,Token::TOKENID_MINIMA,false);
        if(tokenid!="0x00"){output.resetTokenID(MiniData::fromHex(tokenid));output.setToken(token);}
        transaction->addOutput(output);
    }
    for(const auto& [port,value]:sqlRequired(state).entries()){
        auto number=javaParseInt(unicode::encodeUTF8(port));
        if(!value.isNull()&&!value.isString())throw JavaClassCastException("Cannot cast state value to String");
        if(value.isNull()){
            // StateVariable validates the port before dereferencing its string.
            if(number<0||number>255)(void)StateVariable(number,std::string{});
            throw JavaNullPointerException("null state data");
        }
        transaction->addStateVariable(StateVariable(number,value.asString()));
    }
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);transaction->calculateTransactionID();
    if(debug)MinimaLogger::log("Total signatures required : "+std::to_string(signatures.size()));
    std::shared_ptr<TxPoW> pow;
    if(tokenid!="0x00"&&burn.isMore(MiniNumber::ZERO)){
        auto burned=txnutils::createBurnTransaction(added,transaction->getTransactionID(),burn,false);
        pow=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(transaction,witness,burned->getTransaction(),burned->getWitness()));
    }else pow=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(transaction,witness));
    pow->calculateTXPOWID();std::filesystem::path file;
    if(existsParam("file"))file=MiniFile::createBaseFile(getParam("file"));
    else{
        auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        file=MiniFile::createBaseFile("unsignedtransaction-"+std::to_string(now)+".txn");
    }
    MiniFile::writeObjectToFile(file,*pow);
    json::JSONObject response;response.put("txpow",std::filesystem::absolute(file).string());result.put("response",response);
    return result;
}
}
