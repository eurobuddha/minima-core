#include "tokencreate.hpp"
#include "../txn/txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/JavaInteger.hpp"
#include <algorithm>
#include <bit>
namespace minima::commands {
tokencreate::tokencreate():Command("tokencreate","[name:] [amount:] (decimals:) (script:) (state:{}) (signtoken:) (webvalidate:) (burn:) - Create a token. 'name' can be a JSON Object"){}
std::optional<std::string> tokencreate::getFullHelp() const {return std::string(
"\ntokencreate\n"
"\n"
"Create (mint) custom tokens or NFTs.\n"
"\n"
"You must have some sendable Minima in your wallet as tokens are 'colored coins', a fraction of 1 Minima.\n"
"\n"
"name:\n"
"    The name of the token. Can be a string or JSON Object.\n"
"\n"
"amount: \n"
"    The amount of total supply to create for the token. Between 1 and 1 Trillion.\n"
"\n"
"decimals: (optional)\n"
"    The number of decimal places for the token. Default is 8, maximum 16.\n"
"    To create NFTs, use 0.\n"
"\n"
"script: (optional)\n"
"    Add a custom script that must return 'TRUE' when spending any coin of this token.\n"
"    Both the token script and coin script must return 'TRUE' for a coin to be sendable.\n"
"\n"
"state: (optional)\n"
"    List of state variables, if adding a script. A JSON object in the format {\"port\":\"value\",..}\n"
"\n"
"signtoken: (optional)\n"
"    Provide a public key to sign the token with.\n"
"    Useful for proving you are the creator of the token/NFT.\n"
"\n"
"webvalidate: (optional)\n"
"    Provide a URL to a publicly viewable .txt file you are hosting which stores the tokenid for validation purposes.\n"
"    Create the file in advance and get the tokenid after the token has been minted.\n"
"\n"
"burn: (optional)\n"
"    Amount to burn with the tokencreate minting transaction.\n"
"\n"
"mine: (optional)\n"
"    Mine the TxPoW synchronously.\n"
"\n"
"Examples:\n"
"\n"
"tokencreate name:newtoken amount:1000000\n"
"\n"
"tokencreate amount:10 name:{\"name\":\"newcoin\",\"link\":\"http:mysite.com\",\"description\":\"A very cool token\"}\n"
"\n"
"tokencreate name:mynft amount:10 decimals:0 webvalidate:https://www.mysite.com/nftvalidation.txt signtoken:0xFF.. burn:0.1\n"
"\n"
"tokencreate name:charitycoin amount:1000 script:\"ASSERT VERIFYOUT(@TOTOUT-1 0xMyAddress 1 0x00 TRUE)\"\n");}
std::vector<std::string> tokencreate::getValidParams() const {return {"name","amount","decimals","script","state","signtoken","webvalidate","burn","mine","uselimits"};}
json::JSONValue tokencreate::runCommand(){
    auto result=getJSONReply();
    if(!existsParam("name")||!existsParam("amount"))throw CommandException("MUST specify name and amount");
    auto mineSync=getBooleanParam("mine",false);auto limits=getBooleanParam("uselimits",true);
    auto state=std::make_shared<json::JSONObject>();if(existsParam("state"))state=getJSONObjectParam("state");
    std::shared_ptr<json::JSONObject> name;
    if(isParamJSONObject("name")){
        name=getJSONObjectParam("name");if(!sqlRequired(name).containsKey("name"))throw CommandException("MUST specify a 'name' for the token in the JSON");
    }else{name=std::make_shared<json::JSONObject>();name->put("name",getParam("name"));}
    auto rawString=[](const json::JSONValue& value)->std::optional<std::string>{
        if(value.isNull())return {};if(!value.isString())throw JavaClassCastException("Cannot cast parameter to String");return value.asString();
    };
    auto amount=rawString(getParams()->get("amount"));auto burn=getNumberParam("burn",MiniNumber::ZERO);int32_t decimals=8;
    if(getParams()->containsKey("decimals")){
        auto value=rawString(getParams()->get("decimals"));if(!value)throw JavaNumberFormatException("Cannot parse null string");
        decimals=javaParseInt(*value);if(limits&&decimals>16)throw JavaCheckedException("MAX 16 decimal places");
    }
    std::optional<std::string> script="RETURN TRUE";if(getParams()->containsKey("script"))script=rawString(getParams()->get("script"));
    if(!name||!amount)throw CommandException("MUST specify name and amount");
    auto total=MiniNumber(*amount).floor();if(limits&&total.isMore(MiniNumber::TRILLION))throw CommandException("MAX 1 Trillion coins for a token");
    if(total.isLessEqual(MiniNumber::ZERO))throw CommandException("Cannot create less than 1 token");
    auto decimalsFactor=MiniNumber::TEN.pow(decimals);auto color=MiniNumber::MINI_UNIT.mult(decimalsFactor).mult(total);
    auto scale=std::bit_cast<int32_t>(uint32_t(MiniNumber::MAX_DECIMAL_PLACES)-uint32_t(decimals));auto sendAmount=color.add(burn);
    auto sendKey=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getDefaultAddress();auto address=sqlRequired(sendKey).getAddress();
    if(!address)throw JavaNullPointerException("null send address");auto sendAddress=MiniData::fromHex(*address);
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto found=TxPoWSearcher::getRelevantUnspentCoins(tip,"0x00",true);std::vector<Coin> coins;
    auto minCoinBlock=sqlRequired(tip).getBlockNumber().sub(GlobalParams::MINIMA_CONFIRM_DEPTH);
    for(const auto& coin:found)if(coin.getBlockCreated().isLessEqual(minCoinBlock))coins.push_back(coin);
    if(coins.empty())throw CommandException("No Minima Coins available!");
    auto current=MiniNumber::ZERO;std::vector<Coin> selected;
    auto txpdb=sqlRequired(MinimaDB::getDB()).getTxPoWDB();auto miner=txnutils::getMiner();
    for(const auto& coin:coins){
        if(sqlRequired(miner).checkForMiningCoin(coin.getCoinID().to0xString()))continue;
        if(sqlRequired(txpdb).checkMempoolCoins(coin.getCoinID()))continue;
        selected.push_back(coin);current=current.add(coin.getAmount());if(current.isMoreEqual(sendAmount))break;
    }
    if(current.isLess(sendAmount)){result.put("status",false);result.put("message","Insufficient funds.. you only have "+current.toString());return result;}
    auto change=current.sub(sendAmount);auto transaction=std::make_shared<Transaction>();auto witness=std::make_shared<Witness>();auto minBlock=MiniNumber::ZERO;
    for(const auto& coin:selected){transaction->addInput(coin);if(coin.getBlockCreated().isMore(minBlock))minBlock=coin.getBlockCreated();}
    auto difference=tip->getBlockNumber().sub(minBlock);if(difference.isMore(GlobalParams::MINIMA_MMR_PROOF_HISTORY))difference=GlobalParams::MINIMA_MMR_PROOF_HISTORY;
    auto node=tip->getPastNode(tip->getBlockNumber().sub(difference));if(!node)throw CommandException("Not enough blocks in chain to make valid MMR Proofs..");
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();std::vector<Wallet::Text> signatures;
    for(const auto& coin:selected){
        auto proof=sqlRequired(node->getMMR()).getProofToPeak(coin.getMMREntryNumber());witness->addCoinProof(CoinProof(coin,std::move(proof)));
        auto inputAddress=coin.getAddress().to0xString();auto row=sqlRequired(wallet).getScriptFromAddress(inputAddress);
        if(!row)throw CommandException("SERIOUS ERROR script missing for simple address : "+inputAddress);
        if(!row->getScript())throw JavaNullPointerException("null script");witness->addScript(ScriptProof(*row->getScript()));
        auto key=row->getPublicKey();if(std::find(signatures.begin(),signatures.end(),key)==signatures.end())signatures.push_back(key);
    }
    Coin recipient(Coin::COINID_OUTPUT,sendAddress,color,Token::TOKENID_CREATE,true);
    if(existsParam("webvalidate"))name->put("webvalidate",getParam("webvalidate"));
    if(existsParam("signtoken")){
        if(transaction->getAllInputs()->empty())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index 0 out of bounds for length 0");
        auto first=transaction->getAllInputs()->front().getCoinID();auto id=transaction->calculateCoinID(first,0);auto publicKey=getParam("signtoken");
        auto signature=sqlRequired(wallet).signData(publicKey,id);auto data=MiniData(sqlRequired(signature).serialise());
        name->put("signtype","minima");name->put("signedby",publicKey);name->put("signature",data.to0xString());
    }
    if(!script)throw JavaNullPointerException("null token script");
    Token token(Coin::COINID_OUTPUT,MiniNumber(scale),color,MiniString(name->toJSONString()),MiniString(*script));recipient.setToken(token);transaction->addOutput(recipient);
    if(change.isMore(MiniNumber::ZERO)){
        auto row=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getDefaultAddress();auto outputAddress=sqlRequired(row).getAddress();
        if(!outputAddress)throw JavaNullPointerException("null change address");transaction->addOutput(Coin(Coin::COINID_OUTPUT,MiniData::fromHex(*outputAddress),change,Token::TOKENID_MINIMA,false));
    }
    for(const auto& [port,value]:sqlRequired(state).entries()){
        auto number=javaParseInt(unicode::encodeUTF8(port));auto text=rawString(value);
        if(!text){if(number<0||number>255)(void)StateVariable(number,std::string{});throw JavaNullPointerException("null state data");}
        transaction->addStateVariable(StateVariable(number,*text));
    }
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);transaction->calculateTransactionID();
    for(const auto& key:signatures)witness->addSignature(sqlRequired(wallet).signData(key,transaction->getTransactionID().get()));
    auto pow=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(transaction,witness));pow->calculateTXPOWID();
    auto active=txnutils::getMiner();
    if(mineSync){if(!sqlRequired(active).MineMaxTxPoW(false,pow,120000))throw CommandException("FAILED TO MINE txn in 120 seconds !?");}
    else sqlRequired(active).mineTxPoWAsync(pow);
    result.put("response",pow->toJSON());return result;
}
}
