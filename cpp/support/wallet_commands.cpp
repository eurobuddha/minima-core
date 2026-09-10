#include "org/minima/system/params/GlobalParams.hpp"
#include "SqlRecordingFixture.hpp"
#include "TxPoWSearcherFixture.hpp"
#include "org/minima/system/commands/txn/txnauto.hpp"
#include "org/minima/system/commands/base/burn.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/system/commands/txn/txnutils.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;using namespace minima::sql_fixture;
#include "WalletMiningFixture.hpp"
#include "WalletVaultFixture.hpp"
#include "org/minima/system/commands/send/send.hpp"
#include "org/minima/system/commands/search/history.hpp"
#include "org/minima/system/commands/base/scanchain.hpp"
#include "org/minima/system/commands/base/magic.hpp"
#include "org/minima/system/commands/send/sendsign.hpp"
#include <sstream>
#include "org/minima/system/commands/txn/txnaddamount.hpp"
namespace minima {
struct WalletCommandsFixture {
    static void initTxn() {
        auto db=MinimaDB::getDB();db->mTxnDB=std::make_shared<TxnDB>(db->getUserDB());
    }
};
}
class DormantMiner:public TxPoWMiner {
public:explicit DormantMiner(TxPoWMinerServices services):TxPoWMiner(std::move(services)){}
};
static JSONArray forwarded,historyCalls;
class RecordingHistory:public commands::history {
public:
    std::shared_ptr<JSONObject> getTxnDetails(TxPoW* txpow) override {
        if(config->containsKey("detailBlock")){txpow->setBlockNumber(MiniNumber(config->getString("detailBlock")));JSONObject change;change.put("id",txpow->getTxPoWID());change.put("block",txpow->getBlockNumber().toString());historyCalls.add(change);}
        else historyCalls.add(txpow->getTxPoWID());
        if(historyCalls.size()==size_t(integer(*config,"detailThrow",0)))throw JavaTypedException("java.lang.RuntimeException","fixture detail failure");
        if(config->containsKey("detailResults")){
            const auto& value=array(config->get("detailResults")).get(historyCalls.size()-1);
            if(value.isNull())return nullptr;
            auto object=std::get_if<std::shared_ptr<JSONObject>>(&value.value);
            if(!object)throw JavaClassCastException("invalid fixture detail");return *object;
        }
        return history::getTxnDetails(txpow);
    }
};
class RecordingSend:public commands::send {
public:
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<RecordingSend>();}
    JSONValue runCommand() override {
        JSONObject call;call.put("text",getCompleteCommand().value_or(""));call.put("params",*getParams());forwarded.add(call);
        if(flag("sendThrow"))throw JavaTypedException("java.lang.RuntimeException","fixture send failure");
        if(config->containsKey("sendResult"))return object(config->get("sendResult"));
        auto result=getJSONReply();JSONObject response;response.put("forwarded",true);result.put("response",response);return result;
    }
};
struct SendRecordingScope {
    bool enabled;std::vector<std::shared_ptr<Command>> saved;
    explicit SendRecordingScope(bool active):enabled(active){if(enabled){saved=CommandRunner::ALL_COMMANDS;for(auto& command:CommandRunner::ALL_COMMANDS)if(command->getName()=="send")command=std::make_shared<RecordingSend>();}}
    ~SendRecordingScope(){if(enabled)CommandRunner::ALL_COMMANDS=std::move(saved);}
};
class RecordingMysql:public Command {
public:
    RecordingMysql():Command("mysql","fixture MySQL delegation"){}
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<RecordingMysql>();}
    std::vector<std::string> getValidParams() const override{return {"action","txpowid"};}
    JSONValue runCommand() override {
        JSONObject call;call.put("text",getCompleteCommand().value_or(""));call.put("params",*getParams());forwarded.add(call);
        if(flag("mysqlThrow"))throw JavaTypedException("java.lang.RuntimeException","fixture mysql failure");
        return object(config->get("mysqlResult"));
    }
};
struct MysqlRecordingScope {
    bool enabled;std::vector<std::shared_ptr<Command>> saved;
    explicit MysqlRecordingScope(bool active):enabled(active){
        if(enabled){saved=CommandRunner::ALL_COMMANDS;for(auto& command:CommandRunner::ALL_COMMANDS)if(command->getName()=="mysql"){command=std::make_shared<RecordingMysql>();return;}CommandRunner::ALL_COMMANDS.push_back(std::make_shared<RecordingMysql>());}
    }
    ~MysqlRecordingScope(){if(enabled)CommandRunner::ALL_COMMANDS=std::move(saved);}
};
static JSONObject unsignedFileObservation(std::span<const uint8_t> data){
    JSONObject result;
    try{
        size_t offset=0;(void)TxHeader::deserialise(data,offset);
        if(MiniByte::deserialise(data,offset).isTrue()){result.put("prngLength",int32_t(wire::integer(data,offset)));result.put("prngOffset",int64_t(offset));}
        offset=0;auto pow=TxPoW::deserialise(data,offset);result.put("consumed",int64_t(offset));result.put("json",pow.toJSON());
    }catch(const std::exception& error){result.put("error",nativeExceptionText(error));}
    return result;
}
static JSONObject run(const JSONObject& input,const std::string& root){
    if(input.containsKey("nodeState"))std::filesystem::remove(std::filesystem::path(root)/"control-prefs");
    auto offlineRoot=std::filesystem::path(root)/"offline";bool offline=input.containsKey("offline");
    GeneralParams::BASE_FILE_FOLDER=offline?offlineRoot.string():"";
    if(offline){std::filesystem::remove_all(offlineRoot);std::filesystem::create_directories(offlineRoot);}
    auto signedFile=[&](){std::string name;for(const auto& file:std::filesystem::directory_iterator(offlineRoot))if(file.path().filename().string().starts_with("signedtransaction-"))name=file.path().filename().string();return name;};
    commands::txnaddamount::enableCoinLock(false);WalletMiningFixture::reset();WalletVaultFixture::reset(input,root);TxPoWSqlDB::MAX_RELEVANT_TXPOW=MiniNumber(100);GeneralParams::NUMBER_DAYS_SQLTXPOWDB=3;connectionID=preparedID=0;GlobalParams::MINIMA_MMR_PROOF_HISTORY=MiniNumber(256);GeneralParams::DATA_FOLDER=root;auto prefs=std::filesystem::path(root)/"databases/userprefs.db";std::filesystem::remove(prefs);MinimaDBServices services;
    services.sql.connect=[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user,password});return std::make_shared<Connection>();};
    services.sql.log=[](const auto& text,bool){logs.add(text);};
    services.isShuttingDown=[]{return false;};services.postMainMessage=[](auto){};services.postMainTimer=[](auto){};
    services.postNotifyEvent=[](const auto&,const auto&){};services.setStartupError=[](bool,const auto&){};
    services.halt=[](int){throw std::runtime_error("unexpected halt");};services.currentThreadName=[]{return "main";};
    MinimaDB::createDB(services);WalletCommandsFixture::initTxn();commands::txnutils::getMiner=[]()->std::shared_ptr<TxPoWMiner>{throw JavaNullPointerException("null Main receiver");};GeneralParams::USE_SQL_COINDB=false;GeneralParams::IS_MEGAMMR=false;auto wallet=MinimaDB::getDB()->getWallet();JSONArray outputs;std::optional<TxPoW> held;std::optional<TxBody> body;std::shared_ptr<commands::burn> burnMetrics;
    MinimaLogger::Runtime logger;logger.println=[](const auto& full){auto text=unicode::encodeUTF8(full);auto at=text.find("] : ");if(at!=std::string::npos){auto body=text.substr(at+4);if(!body.starts_with("     "))logs.add(body);}};
    MinimaLogger::setRuntime(logger);struct Reset{~Reset(){MinimaLogger::setRuntime({});}}reset;
    for(const auto& entry:array(input.get("ops")).elements()){
        config=&object(entry);forwarded=JSONArray();events=JSONArray();logs=JSONArray();calls=0;historyCalls=JSONArray();WalletMiningFixture::calls=JSONArray();WalletVaultFixture::mainCalls=JSONArray();JSONObject out;auto before=TimerMessage::currentTimeMillis();
        try{
            SendRecordingScope recording(flag("recordSend"));MysqlRecordingScope mysqlRecording(flag("recordMysql"));
            auto op=config->getString("op");JSONValue result;
            if(op=="filePut"){
                auto file=offlineRoot/config->getString("file");std::filesystem::create_directories(file.parent_path());auto bytes=MiniData::fromHex(config->getString("data")).bytes();
                std::ofstream stream(file,std::ios::binary);stream.write(reinterpret_cast<const char*>(bytes.data()),std::streamsize(bytes.size()));
            }
            else if(op=="signDemo"){
                std::ostringstream printed;auto previous=std::cout.rdbuf(printed.rdbuf());
                struct Restore {std::streambuf* previous;~Restore(){std::cout.rdbuf(previous);}}restore{previous};
                std::vector<std::optional<std::string>> args;commands::sendsign::main(flag("nullArgs")?nullptr:&args);result=printed.str();
            }
            else if(op=="fileWrite")MiniFile::writeDataToFile(MiniFile::createBaseFile(config->getString("file")),MiniData::fromHex(config->getString("data")).bytes(),flag("append"));
            else if(op=="fileBase")GeneralParams::BASE_FILE_FOLDER=config->getString("folder");
            else if(op=="fileDirectory")std::filesystem::create_directories(offlineRoot/config->getString("file"));
            else if(op=="verifyFile"){
                auto bytes=MiniFile::readCompleteFile(offlineRoot/signedFile());auto pow=TxPoW::convertMiniDataVersion(MiniData(bytes));if(!pow)throw JavaNullPointerException("null file TxPoW");
                JSONObject value;for(bool burn:{false,true}){JSONArray valid;auto& txn=burn?pow->getBurnTransaction():pow->getTransaction();auto& witness=burn?pow->getBurnWitness():pow->getWitness();for(const auto& signature:witness.getAllSignatures()){TreeKey key;key.setPublicKey(signature.getRootPublicKey());valid.add(key.verify(txn.getTransactionID(),signature));}value.put(burn?"burn":"main",valid);}result=value;
            }
            else if(op=="vaultMain"){WalletMiningFixture::init();WalletVaultFixture::init();}
            else if(op=="vaultStores")WalletVaultFixture::stores();
            else if(op=="vaultTarget")Wallet::NUMBER_GETADDRESS_KEYS=int32_t(integer(*config,"target",64));
            else if(op=="vaultSeed")wallet->updateSeedRow(string(config->get("phrase")),string(config->get("seed")));
            else if(op=="vaultEncrypted")MinimaDB::getDB()->getUserDB()->setEncryptedSeed(MiniData::fromHex(config->getString("data")));
            else if(op=="vaultDecrypt")result=MiniString(PasswordCrypto::decryptPassword(config->getString("password"),MiniData::fromHex(config->getString("data"))).bytes()).toString();
            else if(op=="vaultLock")commands::vault::passwordLockDB(string(config->get("password")));
            else if(op=="vaultUnlock")commands::vault::passowrdUnlockDB(string(config->get("password")));
            else if(op=="keysCheck")result=commands::keys::checkKey(string(config->get("publickey")));
            else if(op=="vaultCheck")commands::vault::checkAllKeysCreated();
            else if(op=="vaultStop")commands::vault::stopAllKeysCreated();
            else if(op=="treeIndex"){TreeKeyNode node(MiniData::fromHex("0x12"),2);auto index=int32_t(integer(*config,"index",0));result=config->getString("method")=="child"?node.getChild(index).getPublicKey().to0xString():node.getWOTSKey(index).getPublicKey()->to0xString();}
            else if(op=="verifyStored")result=WalletVaultFixture::verify();
            else if(op=="addNullSignature")MinimaDB::getDB()->getCustomTxnDB()->getTransactionRow("test")->getWitness()->addSignature(std::shared_ptr<Signature>{});
            else if(op=="signedBy")result=MinimaDB::getDB()->getCustomTxnDB()->getTransactionRow("test")->getWitness()->isSignedBy(string(config->get("publickey")));
            else if(op=="selection"){
                std::vector<Coin> coins;
                for(const auto& item:array(config->get("coins")).elements()){
                    const auto& spec=object(item);
                    if(spec.containsKey("alias"))coins.push_back(coins.at(size_t(integer(spec,"alias",0))));
                    else {Coin coin(MiniData::fromHex(spec.getString("id")),MiniData::fromHex(spec.getString("address")),MiniNumber(spec.getString("amount")),Token::TOKENID_MINIMA);coins.push_back(coin);}
                }
                JSONObject value;std::vector<Coin> selected;
                try{selected=flag("order")?commands::send::orderCoins(coins):commands::send::selectCoins(coins,MiniNumber(config->getString("amount")),flag("debug"));}
                catch(const std::exception& error){value.put("error",nativeExceptionText(error));}
                if(flag("mutate")&&!selected.empty())selected.front().setSpent(true);
                JSONArray sorted,returned;for(const auto& coin:coins)sorted.add(coin.toJSON());for(const auto& coin:selected)returned.add(coin.toJSON());
                value.put("sorted",sorted);value.put("selected",returned);result=value;
            }
            else if(op=="coinLock"){
                auto action=config->getString("action");
                if(action=="enable")commands::txnaddamount::enableCoinLock(flag("enabled"));
                else if(action=="add")commands::txnaddamount::addCoinLock(config->getString("id"));
                else if(action=="clear")commands::txnaddamount::clearCoinLocked();
                else result=commands::txnaddamount::isCoinLocked(config->getString("id"));
            }
            else if(op=="verifyMined"){
                JSONArray valid;for(const auto& pow:WalletMiningFixture::held)for(const auto& signature:pow->getWitness().getAllSignatures()){
                    TreeKey key;key.setPublicKey(signature.getRootPublicKey());valid.add(key.verify(pow->getTransaction().getTransactionID(),signature));
                }result=valid;
            }
            else if(op=="verifyMinted"){
                JSONArray valid;for(const auto& pow:WalletMiningFixture::held){
                    const auto& coin=pow->getTransaction().getAllOutputs().at(0);auto token=coin.getToken();if(!token)throw JavaNullPointerException("null minted token");
                    auto name=object(JSONValue::parse(token->getName().toString()));
                    if(name.containsKey("signature")){auto signature=Signature::convertMiniDataVersion(MiniData::fromHex(name.getString("signature")));if(!signature)throw JavaNullPointerException("null token signature");TreeKey key;key.setPublicKey(MiniData::fromHex(name.getString("signedby")));valid.add(key.verify(coin.getCoinID(),*signature));}
                }result=valid;
            }
            else if(op=="reserveMining"){
                auto pow=std::make_shared<TxPoW>();pow->setTimeMilli(MiniNumber::ZERO);
                for(const auto& coin:array(config->get("coins")).elements())pow->getTransaction().addInput(Coin::convertMiniDataVersion(MiniData::fromHex(coin.asString())).value());
                sqlRequired(WalletMiningFixture::miner).mineTxPoWAsync(pow);
            }
            else if(op=="mining")WalletMiningFixture::init();
            else if(op=="mineNext")sqlRequired(WalletMiningFixture::miner).processNext();
            else if(op=="mineState"){}
            else if(op=="postHeld"){auto command=CommandRunner::getRunner().getCommandOnly("txnminepost");command->getParams()->put("data",MiniData(WalletMiningFixture::held.back()->serialise()).to0xString());result=command->runCommand();}
            else if(op=="limitSize"){auto tip=MinimaDB::getDB()->getTxPoWTree()->getTip();tip->getTxPoW().getMagic().mCurrentMaxTxPoWSize=MiniNumber(config->getString("size"));}

            else if(op=="coinStore")CoinDB::createCoinDB(std::filesystem::path(root)/"control-coins",services.sql);
            else if(op=="coinSQL")GeneralParams::USE_SQL_COINDB=flag("enabled");
            else if(op=="relevantEntries"){
                auto node=MinimaDB::getDB()->getTxPoWTree()->getTip()->getParent(int32_t(integer(*config,"depth",0)));auto& entries=node->getRelevantCoinsEntries();entries.clear();
                for(const auto& value:array(config->get("entries")).elements())entries.emplace_back(BigInt(value.asString()));if(flag("recalculate"))node->calculateRelevantCoins();
            }
            else if(op=="poolEntries"){
                auto pool=MinimaDB::getDB()->getTxPoWDB()->getCompleteMemPool();if(flag("clear"))pool->clear();
                for(const auto& value:array(config->get("entries")).elements()){
                    const auto& row=object(value);auto key=row.getString("key");std::shared_ptr<TxPoW> pow;
                    if(!row.get("nullTxPoW").isNull()&&row.get("nullTxPoW").asBool()){}
                    else if(row.containsKey("wire")){auto decoded=TxPoW::convertMiniDataVersion(MiniData::fromHex(row.getString("wire")));pow=std::make_shared<TxPoW>(*decoded);}
                    else pow=std::make_shared<TxPoW>(row.getString("id"),0,1,row.get("block").asBool(),"0x00",row.get("transaction").asBool());
                    auto ram=std::make_shared<RamData>(pow);ram->setInCascade(row.get("cascade").asBool());ram->setOnMainChain(row.get("onchain").asBool());pool->put(key,ram);
                }
                if(config->containsKey("remove"))for(const auto& key:array(config->get("remove")).elements())pool->remove(key.asString());
            }
            else if(op=="preferencePatch"){auto prefs=MinimaDB::getDB()->getUserDB()->getAllData();for(const auto& [key,value]:object(config->get("values")).entries())prefs->put(key,value);}
            else if(op=="preferenceSave")MinimaDB::getDB()->getUserDB()->saveDB(std::filesystem::path(root)/"control-prefs");
            else if(op=="preferenceLoad")MinimaDB::getDB()->getUserDB()->loadDB(std::filesystem::path(root)/"control-prefs");
            else if(op=="generatedMagic")result=TxPoWGenerator::generateTxPoW(std::make_shared<Transaction>(),std::make_shared<Witness>()).getMagic().toJSON();
            else if(op=="txpowStores"){
                MinimaDB::getDB()->getTxPoWDB()->loadSQLDB(std::filesystem::path(root)/"txpow");MinimaDB::getDB()->getArchive()->loadDB(std::filesystem::path(root)/"archive");
            }
            else if(op=="txpowPrefs"){MinimaDB::getDB()->getUserDB()->setAutoLoginDetailsMySQL(flag("login"));GeneralParams::NUMBER_DAYS_SQLTXPOWDB=integer(*config,"days",3);}
            else if(op=="txpowRAM"){
                auto value=TxPoW::convertMiniDataVersion(MiniData::fromHex(config->getString("data")));auto owner=std::make_shared<TxPoW>(*value);
                MinimaDB::getDB()->getTxPoWDB()->getCompleteMemPool()->put(owner->getTxPoWID(),std::make_shared<RamData>(owner));result=owner->getTxPoWID();
            }
            else if(op=="historyLoad")MinimaDB::getDB()->getTxPoWDB()->getSQLDB()->loadDB(std::filesystem::path(root)/"txpow");
            else if(op=="historyLimit")TxPoWSqlDB::MAX_RELEVANT_TXPOW=MiniNumber(config->getString("limit"));
            else if(op=="tokenImports"){
                TxPoWSearcherFixture::reset();
                for(const auto& data:array(config->get("tokens")).elements()){
                    auto token=data.isNull()?std::optional<Token>():Token::convertMiniDataVersion(MiniData::fromHex(data.asString()));
                    TxPoWSearcher::importToken(token?std::make_shared<Token>(*token):nullptr);
                }
            }
            else if(op=="scanDetails"){
                std::optional<TxPoW> pow;
                if(!flag("nullTxPoW")){
                    pow=TxPoW::convertMiniDataVersion(MiniData::fromHex(config->getString("data")));
                    if(flag("bodyless"))pow->clearBody();
                    if(flag("nullTransaction"))pow->getTxBody()->mTransaction.reset();
                    if(flag("nullBurn"))pow->getTxBody()->mBurnTransaction.reset();
                }
                commands::scanchain command;auto detail=command.getTransactionDetails(pow?&*pow:nullptr);result=detail?JSONValue(*detail):JSONValue();
            }
            else if(op=="historyDetails"){
                std::optional<TxPoW> pow;
                if(!flag("nullTxPoW")){
                    pow=TxPoW::convertMiniDataVersion(MiniData::fromHex(config->getString("data")));
                    if(flag("bodyless"))pow->clearBody();
                    if(flag("nullTransaction"))pow->getTxBody()->mTransaction.reset();
                }
                commands::history command;auto detail=command.getTxnDetails(pow?&*pow:nullptr);result=detail?JSONValue(*detail):JSONValue();
            }
            else if(op=="historyOverride"){
                RecordingHistory command;for(const auto& [key,value]:object(config->get("params")).entries())command.getParams()->put(key,value);result=command.runCommand();
            }
            else if(op=="numberRange"){
                result=flag("decimal")?MiniNumber(BigInt(config->getString("coefficient")),int32_t(integer(*config,"scale",0))).toString():MiniNumber(config->getString("text")).toString();
            }
            else if(op=="load")wallet->loadDB(std::filesystem::path(root+"/wallet"));
            else if(op=="chain"){
                MinimaDB::getDB()->resetCascadeAndTxPoWTree();GlobalParams::MINIMA_MMR_PROOF_HISTORY=MiniNumber(config->getString("history"));
                std::shared_ptr<TxPoWTreeNode> first,previous;
                for(const auto& n:array(config->get("nodes")).elements()){
                    const auto& spec=object(n);auto pow=TxPoW::convertMiniDataVersion(MiniData::fromHex(spec.getString("pow")));auto node=std::make_shared<TxPoWTreeNode>(*pow);
                    auto bytes=MiniData::fromHex(spec.getString("mmr")).bytes();size_t offset=0;node->getMMR()->readDataStream(bytes,offset);
                    if(spec.containsKey("coins")) for(const auto& c:array(spec.get("coins")).elements()) node->addCoinToNode(Coin::convertMiniDataVersion(MiniData::fromHex(c.asString())).value());
                    if(spec.containsKey("relevant")) for(const auto& c:array(spec.get("relevant")).elements()) node->getRelevantCoins().push_back(Coin::convertMiniDataVersion(MiniData::fromHex(c.asString())).value());
                    if(previous)previous->addChildNode(node);else first=node;previous=node;
                }
                if(first)MinimaDB::getDB()->getTxPoWTree()->setRoot(first);
                if(config->containsKey("mega"))GeneralParams::IS_MEGAMMR=flag("mega");
                if(config->containsKey("megaCoins"))for(const auto& data:array(config->get("megaCoins")).elements()){
                    auto coin=std::make_shared<Coin>(Coin::convertMiniDataVersion(MiniData::fromHex(data.asString())).value());
                    MinimaDB::getDB()->getMegaMMR()->getAllCoins().put(coin->getCoinID().to0xString(),coin);
                }
            }else if(op=="txn"){
                auto transaction=std::make_shared<Transaction>();auto witness=std::make_shared<Witness>();
                for(const auto& c:array(config->get("coins")).elements()){
                    const auto& spec=object(c);auto coin=Coin::convertMiniDataVersion(MiniData::fromHex(spec.getString("data"))).value();
                    if(spec.containsKey("created"))coin.setBlockCreated(MiniNumber(spec.getString("created")));
                    if(spec.containsKey("entry"))coin.setMMREntryNumber(MMREntryNumber(BigInt(spec.getString("entry"))));
                    transaction->addInput(coin);
                }
                for(const auto& proof:array(config->get("proofs")).elements())witness->addCoinProof(CoinProof::convertMiniDataVersion(MiniData::fromHex(proof.asString())).value());
                for(const auto& script:array(config->get("scripts")).elements())witness->addScript(ScriptProof(script.asString()));
                transaction->mHaveCheckedMonotonic=true;transaction->mIsMonotonic=true;
                TxnDB::getRuntimeDB()->addCompleteTransaction(std::make_shared<TxnRow>(config->getString("id"),transaction,witness));
            }
            else if(op=="validationFixture") {
                auto bytes=MiniData::fromHex(config->getString("mmr")).bytes();size_t offset=0;auto mmr=MMR::deserialise(bytes,offset);
                auto pow=TxPoW::convertMiniDataVersion(MiniData::fromHex(config->getString("pow"))).value();
                auto parent=TxPoW::convertMiniDataVersion(MiniData::fromHex(config->getString("block"))).value();parent.setBlockNumber(mmr.getBlockTime());parent.calculateTXPOWID();
                auto node=std::make_shared<TxPoWTreeNode>(parent);*node->getMMR()=mmr;MinimaDB::getDB()->getTxPoWTree()->setRoot(node);
                auto txn=pow.getSharedTransaction();txn->mHaveCheckedMonotonic=true;txn->mIsMonotonic=true;txn->mIsValid=false;
                auto row=std::make_shared<TxnRow>("test",txn,pow.getSharedWitness());MinimaDB::getDB()->getCustomTxnDB()->addCompleteTransaction(row);
                if(config->containsKey("hashrate"))MinimaDB::getDB()->getUserDB()->setHashRate(MiniNumber(config->getString("hashrate")));
                if(flag("proofCoins"))for(const auto& proof:pow.getWitness().getAllCoinProofs()){node->addCoinToNode(proof.getCoin());node->getRelevantCoins().push_back(proof.getCoin());}
            }else if(op=="generate"||op=="held") {
                auto row=MinimaDB::getDB()->getCustomTxnDB()->getTransactionRow("test");
                if(op=="generate")held=TxPoWGenerator::generateTxPoW(row->getTransaction(),row->getWitness());
                JSONObject value;value.put("sameTransaction",row&&held->getSharedTransaction()==row->getTransaction());
                value.put("sameWitness",row&&held->getSharedWitness()==row->getWitness());
                value.put("transaction",MiniData(held->getTransaction().serialise()).to0xString());
                value.put("witness",MiniData(held->getWitness().serialise()).to0xString());result=value;
            }else if(op=="body") {
                body.emplace();auto row=MinimaDB::getDB()->getCustomTxnDB()->getTransactionRow("test");
                body->mTransaction=row->getTransaction();body->mWitness=row->getWitness();
                if(flag("alias")){body->mBurnTransaction=body->mTransaction;body->mBurnWitness=body->mWitness;}
                body->mPRNG=MiniData::fromHex("0x12");body->mTxPowIDList.push_back(MiniData::fromHex("0x99"));
                for(const auto& field:array(config->get("nulls")).elements()) {
                    auto key=field.asString();if(key=="transaction")body->mTransaction.reset();else if(key=="witness")body->mWitness.reset();
                    else if(key=="burnTransaction")body->mBurnTransaction.reset();else body->mBurnWitness.reset();
                }
            }else if(op=="bodyread") {
                auto txn=body->mTransaction;auto wit=body->mWitness;auto burn=body->mBurnTransaction;auto bw=body->mBurnWitness;
                auto bytes=MiniData::fromHex(config->getString("data")).bytes();size_t offset=0;
                try {body->readDataStream(bytes,offset);}catch(const std::exception& e){out.put("readError",nativeExceptionText(e));}
                JSONObject value;value.put("offset",int64_t(offset));value.put("prng",body->mPRNG.to0xString());value.put("difficulty",body->mTxnDifficulty.to0xString());
                value.put("sameTransaction",txn==body->mTransaction);value.put("sameWitness",wit==body->mWitness);value.put("sameBurn",burn==body->mBurnTransaction);value.put("sameBurnWitness",bw==body->mBurnWitness);
                JSONArray ids;for(const auto& id:body->mTxPowIDList)ids.add(id.to0xString());value.put("ids",ids);
                try{value.put("wire",MiniData(body->serialise()).to0xString());}catch(const std::exception& e){value.put("wireError",nativeExceptionText(e));}
                result=value;
            }else if(op=="utilities") {
                auto row=sqlRequired(MinimaDB::getDB()->getCustomTxnDB()).getTransactionRow(config->getString("id"));
                if(flag("single")) commands::txnutils::setMMRandScripts(row->getTransaction()->getAllInputs().at(0),*row->getWitness());
                else commands::txnutils::setMMRandScripts(*row->getTransaction(),*row->getWitness(),!config->containsKey("exit")||flag("exit"));
                result=row->toJSON();
            }else if(op=="miner") {
                std::shared_ptr<TxPoWMiner> miner;
                if(!flag("null")) {
                    TxPoWMinerServices ms;ms.releaseLocks=[]{};ms.postMain=[](auto){};ms.automineTimer=[]{return int64_t(1000);};
                    miner=std::make_shared<DormantMiner>(ms);
                    auto pow=std::make_shared<TxPoW>();
                    for(const auto& c:array(config->get("coins")).elements()) pow->getTransaction().addInput(Coin::convertMiniDataVersion(MiniData::fromHex(c.asString())).value());
                    miner->mineTxPoWAsync(pow);
                }
                commands::txnutils::getMiner=[miner]{return miner;};
            }else if(op=="pool") {
                auto pow=std::make_shared<TxPoW>();pow->getTxBody()->mPRNG=MiniData(std::vector<uint8_t>(32,0x12));pow->setTimeMilli(MiniNumber(0));pow->setTxDifficulty(MiniData::fromHex("0x"+std::string(64,'F')));
                for(const auto& data:array(config->get("proofs")).elements()) {
                    auto proof=CoinProof::convertMiniDataVersion(MiniData::fromHex(data.asString())).value();
                    pow->getTransaction().addInput(proof.getCoin());
                    if(flag("burnWitness"))pow->getBurnWitness().addCoinProof(proof);else pow->getWitness().addCoinProof(proof);
                }
                pow->setHeaderBodyHash();pow->calculateTXPOWID();auto data=std::make_shared<RamData>(pow);
                data->setInCascade(flag("cascade"));data->setOnMainChain(flag("onChain"));
                MinimaDB::getDB()->getTxPoWDB()->getCompleteMemPool()->put(pow->getTxPoWID(),data);
                result=pow->isTransaction();
            }else if(op=="burnNew"){burnMetrics=std::make_shared<commands::burn>();
            }else if(op=="burnRun"){
                if(flag("fresh"))burnMetrics=std::dynamic_pointer_cast<commands::burn>(sqlRequired(burnMetrics).getFunction());
                auto& metric=sqlRequired(burnMetrics);*metric.getParams()=object(config->get("params"));result=metric.runCommand();
            }else if(op=="burnCheck"){
                std::optional<TxPoW> pow;if(!flag("nullTxPoW")){pow=TxPoW::convertMiniDataVersion(MiniData::fromHex(config->getString("data")));if(flag("bodyless"))pow->clearBody();}
                sqlRequired(burnMetrics).checkBurn(pow?&*pow:nullptr,int32_t(integer(*config,"counter")));
            }else if(op=="burnMedian"){
                commands::burn::Values values;for(const auto& item:array(config->get("values")).elements()){
                    if(item.isNull())values.push_back(nullptr);
                    else if(item.isString())values.push_back(std::make_shared<MiniNumber>(item.asString()));
                    else values.push_back(values.at(size_t(integer(object(item),"alias"))));
                }auto original=values;JSONObject value;
                try{auto median=sqlRequired(burnMetrics).getMediaValue(flag("nullValues")?nullptr:&values);value.put("median",median?JSONValue(median->toString()):JSONValue());if(values.empty())value.put("emptyIdentity",median==sqlRequired(burnMetrics).getMediaValue(values));auto it=std::find(original.begin(),original.end(),median);value.put("identity",it==original.end()?-1:int64_t(it-original.begin()));}
                catch(const std::exception& error){value.put("exception",nativeExceptionText(error));}
                JSONArray sorted;for(const auto& item:values)sorted.add(item?JSONValue(item->toString()):JSONValue());value.put("values",sorted);result=value;
            }else if(op=="automaticHelper") {
                std::optional<MiniNumber> amount;if(!config->get("amount").isNull())amount.emplace(config->getString("amount"));
                auto row=commands::txnauto::createTransaction(string(config->get("address")),amount?&*amount:nullptr);
                JSONObject value;value.put("row",row->toJSON());value.put("wire",MiniData(row->serialise()).to0xString());JSONArray verified;
                for(const auto& signature:row->getWitness()->getAllSignatures()){TreeKey key;key.setPublicKey(signature.getRootPublicKey());verified.add(key.verify(row->getTransaction()->getTransactionID(),signature));}
                value.put("verified",verified);result=value;
            }else if(op=="burn") {
                std::vector<std::string> excluded;for(const auto& id:array(config->get("exclude")).elements()) excluded.push_back(id.asString());
                auto row=commands::txnutils::createBurnTransaction(excluded,MiniData::fromHex(config->getString("link")),MiniNumber(config->getString("amount")),!config->containsKey("sign")||flag("sign"));
                JSONObject value;value.put("row",row->toJSON());value.put("wire",MiniData(row->serialise()).to0xString());result=value;
            }else if(op=="clean")result=kissvm::Contract::cleanScript(config->getString("script"),flag("log"));
            else if(op=="allowSave")MinimaDB::getDB()->setAllowSaveState(flag("allow"));
            else if(op=="reloadPrefs")MinimaDB::getDB()->getUserDB()->loadDB(prefs);
            else if(op=="restart"){MinimaDB::createDB(services);WalletCommandsFixture::initTxn();wallet=MinimaDB::getDB()->getWallet();}
            else if(op=="locked")wallet->getBaseSeed()->mSeed="0x00";
            else if(op=="meta"){
                std::shared_ptr<Command> command=flag("classOnly")?std::make_shared<commands::magic>():CommandRunner::getRunner().getCommandOnly(config->getString("name"));sqlRequired(command);JSONObject meta;
                meta.put("name",command->getName().value());meta.put("help",command->getHelp().value());meta.put("fullhelp",command->getFullHelp().value());JSONArray valid;for(const auto& name:command->getValidParams())valid.add(name);meta.put("valid",valid);meta.put("fresh",command->getFunction()->getJSONReply());result=meta;
            }else if(op=="direct"){
                std::shared_ptr<Command> command=flag("classOnly")?std::make_shared<commands::magic>():CommandRunner::getRunner().getCommandOnly(config->getString("name"));sqlRequired(command);for(const auto& [key,value]:object(config->get("params")).entries())command->getParams()->put(key,value.isString()&&value.asString()=="$SIGNED"?JSONValue(signedFile()):value);result=command->runCommand();
            }else result=CommandRunner::getRunner().runMultiCommand(config->getString("caller"),config->getString("text"));
            out.put("value",flag("replyText")?JSONValue(result.toJSONString()):result);
        }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException: ")+error.what());}
        catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
        if(input.containsKey("nodeState")){
            auto savedPreferences=std::filesystem::path(root)/"control-prefs";out.put("savedPreferences",std::filesystem::exists(savedPreferences)?JSONValue(MiniData(MiniFile::readCompleteFile(savedPreferences)).to0xString()):JSONValue());
            try{out.put("preferenceWire",MiniData(MinimaDB::getDB()->getUserDB()->serialise()).to0xString());}catch(const std::exception& error){out.put("preferenceError",nativeExceptionText(error));}
            try{JSONArray nodes;for(auto node=MinimaDB::getDB()->getTxPoWTree()->getTip();node;node=node->getParent()){
                JSONObject value;JSONArray entries,coins;for(const auto& entry:node->getRelevantCoinsEntries())entries.add(entry.toString());for(const auto& coin:node->getRelevantCoins())coins.add(MiniData(coin.serialise()).to0xString());
                value.put("entries",entries);value.put("coins",coins);value.put("mmr",MiniData(node->getMMR()->serialise()).to0xString());
                if(input.containsKey("coinImport")){JSONArray all;for(const auto& coin:node->getAllCoins())all.add(MiniData(coin.serialise()).to0xString());value.put("allCoins",all);auto root=node->getMMR()->getRoot();value.put("root",root?JSONValue(MiniData(root->serialise()).to0xString()):JSONValue());value.put("finalized",node->getMMR()->isFinalized());}
                nodes.add(value);
            }out.put("nodeState",nodes);}catch(const std::exception& error){out.put("nodeStateError",nativeExceptionText(error));}
        }
        if(flag("echoParams"))out.put("parameterState",config->get("params"));
        if(input.containsKey("history")){out.put("before",before);out.put("after",TimerMessage::currentTimeMillis());out.put("historyCalls",historyCalls);}
        try{out.put("txnwire",MiniData(TxnDB::getRuntimeDB()->serialise()).to0xString());}catch(const std::exception& e){out.put("txnwireError",nativeExceptionText(e));}
        JSONArray txnflags;for(const auto& row:*TxnDB::getRuntimeDB()->listTxns()){JSONArray flags;auto txn=row->getTransaction();flags.add(txn->mHaveCheckedMonotonic);flags.add(txn->mIsMonotonic);flags.add(txn->mIsValid);txnflags.add(flags);}out.put("txnflags",txnflags);
        JSONArray checked;for(const auto& row:*TxnDB::getRuntimeDB()->listTxns())checked.add(row->getTransaction()->isCheckedMonotonic());out.put("checked",checked);out.put("mining",WalletMiningFixture::snapshot());
        if(WalletVaultFixture::enabled)out.put("vault",WalletVaultFixture::snapshot());
        if(input.containsKey("consolidation")||input.containsKey("txpowSearch"))out.put("forwarded",forwarded);
        if(input.containsKey("funding")){JSONArray ids;for(const auto& id:commands::txnaddamount::LOCKED_COINS)ids.add(id);out.put("coinLocks",ids);out.put("coinLockEnabled",commands::txnaddamount::isCoinLockEnabled());}
        if(offline){
            JSONObject files,structures;std::vector<std::string> directories;
            for(const auto& file:std::filesystem::recursive_directory_iterator(offlineRoot)){
                auto relative=file.path().lexically_relative(offlineRoot).string();
                if(file.is_directory())directories.push_back(relative);
                else if(file.is_regular_file()){auto bytes=MiniFile::readCompleteFile(file.path());files.put(relative,MiniData(bytes).to0xString());if(input.containsKey("unsigned"))structures.put(relative,unsignedFileObservation(bytes));}
            }
            std::sort(directories.begin(),directories.end());JSONArray dirs;for(const auto& directory:directories)dirs.add(directory);
            out.put("files",files);out.put("directories",dirs);if(input.containsKey("unsigned"))out.put("fileTxPoW",structures);
        }
        out.put("events",events);out.put("logs",logs);JSONArray cache;
        auto probes=array(config->get("probe")).snapshot();probes.emplace_back(Address::TRUE_ADDRESS.getAddressData().to0xString());
        for(const auto& a:probes){auto address=a.asString();JSONObject row;row.put("address",address);row.put("tracked",wallet->isAddressRelevant(address));row.put("simple",wallet->isAddressSimple(address));row.put("notify",MinimaDB::getDB()->checkCoinNotify(address));cache.add(row);}out.put("cache",cache);
        out.put("userEnabled",MinimaDB::getDB()->getUserDB()->isSlaveNode());out.put("userHost",MinimaDB::getDB()->getUserDB()->getSlaveNodeHost());
        out.put("prefs",std::filesystem::is_regular_file(prefs)?JSONValue(MiniData(MiniFile::readCompleteFile(prefs)).to0xString()):JSONValue());outputs.add(out);
    }
    JSONObject result;result.put("ops",outputs);return result;
}
int main(int argc,char** argv){if(argc!=2)return 2;GeneralParams::SEED_PHRASE="fixture seed";GeneralParams::ANYSEED_PHRASE=true;std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
