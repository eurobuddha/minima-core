#include "org/minima/system/commands/send/wallet/createfrom.hpp"
#include "org/minima/system/commands/send/wallet/sendfrom.hpp"
#include "org/minima/system/commands/send/wallet/consolidatefrom.hpp"
#include "org/minima/system/commands/send/wallet/createtokenfrom.hpp"
#include "org/minima/system/commands/send/wallet/rawfrom.hpp"
#include "org/minima/system/commands/send/wallet/rawtxnfrom.hpp"
#include "org/minima/system/commands/send/wallet/constructfrom.hpp"
#include "org/minima/system/commands/send/wallet/postfrom.hpp"
#include "org/minima/system/commands/send/wallet/signfrom.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima {struct WalletCommandsFixture {static void init(bool missing){get().mTxnDB=missing?nullptr:std::make_shared<TxnDB>(get().getUserDB());}static MinimaDB& get(){return *MinimaDB::getDB();}};}
static const JSONObject* config;static JSONArray calls,logs;static std::shared_ptr<TxnDB> db;
static const JSONObject& object(const JSONValue& value){return *std::get<std::shared_ptr<JSONObject>>(value.value);}
static JSONArray rows(){JSONArray result;for(const auto& row:*db->listTxns()){JSONObject item;item.put("id",row->getID().value());item.put("wire",MiniData(row->serialise()).to0xString());if(config->containsKey("materialize")&&config->get("materialize").asBool())item.put("txn",row->getTransaction()->toJSON());result.add(item);}return result;}
static bool flag(const std::string& key){return config->containsKey(key)&&config->get(key).asBool();}
static bool at(const std::string& key,int64_t index){return config->containsKey(key)&&std::get<int64_t>(config->get(key).value)==index;}
static JSONObject child(const std::string& text){
    JSONObject call;call.put("text",text);call.put("rows",rows());calls.add(call);auto index=int64_t(calls.size());
    if(at("throwAt",index))throw JavaTypedException("java.lang.RuntimeException","injected child failure");
    if(at("resultAt",index))return object(config->get("result"));
    if(flag("materialize")){
        if(text.starts_with("txncreate ")||text.starts_with("txndelete "))CommandRunner::getRunner().runMultiCommand(text);
        if(text.starts_with("txnaddamount ")){
            auto id=text.substr(16);id=id.substr(0,id.find(' '));
            for(const auto& value:std::get<std::shared_ptr<JSONArray>>(config->get("fundedOutputs").value)->elements()){
                const auto& output=object(value);
                CommandRunner::getRunner().runMultiCommand("txnoutput id:"+id+" address:"+output.getString("address")+" amount:"+output.getString("amount"));
            }
        }
    }
    bool execute=flag("real");
    if(config->containsKey("execute"))for(const auto& name:std::get<std::shared_ptr<JSONArray>>(config->get("execute").value)->elements())if(name.asString()==text.substr(0,text.find(' ')))execute=true;
    if(execute){auto results=CommandRunner::getRunner().runMultiCommand(text);return object(results.get(0));}
    JSONObject result,response;result.put("status",true);response.put("marker","child"+std::to_string(index));response.put("txpow","fixture-pow");result.put("response",response);return result;
}
template<class Base>class Recording:public Base {public:std::shared_ptr<Command> getFunction() const override{return std::make_shared<Recording<Base>>();}JSONObject runCommand(const std::string& text)override{return child(text);}};
static std::shared_ptr<Command> command(const std::string& name){
    if(name=="consolidatefrom")return std::make_shared<Recording<commands::consolidatefrom>>();
    if(name=="sendfrom")return std::make_shared<Recording<commands::sendfrom>>();
    if(name=="createtokenfrom")return std::make_shared<Recording<commands::createtokenfrom>>();
    if(name=="rawfrom")return std::make_shared<Recording<commands::rawfrom>>();
    if(name=="rawtxnfrom")return std::make_shared<Recording<commands::rawtxnfrom>>();
    if(name=="createfrom")return std::make_shared<Recording<commands::createfrom>>();
    if(name=="constructfrom")return std::make_shared<Recording<commands::constructfrom>>();
    if(name=="postfrom")return std::make_shared<Recording<commands::postfrom>>();
    return std::make_shared<Recording<commands::signfrom>>();
}
static std::shared_ptr<Coin> fixtureCoin(const JSONValue& value){
    if(value.isNull())return nullptr;const auto& spec=object(value);
    auto coin=std::make_shared<Coin>(MiniData::fromHex(spec.getString("id")),MiniData::fromHex(spec.getString("address")),MiniNumber(spec.getString("amount")),MiniData::fromHex(spec.containsKey("token")?spec.getString("token"):"0x00"));
    coin->setBlockCreated(MiniNumber(spec.containsKey("created")?spec.getString("created"):"0"));
    if(spec.containsKey("spent"))coin->setSpent(spec.get("spent").asBool());
    if(spec.containsKey("scale"))coin->setToken(Token(Coin::COINID_OUTPUT,MiniNumber(spec.getString("scale")),MiniNumber(1),MiniString("fixture"),MiniString("RETURN TRUE")));
    return coin;
}
static void fixtureChain(){
    GeneralParams::USE_SQL_COINDB=false;GeneralParams::IS_MEGAMMR=flag("mega");
    if(config->containsKey("chainCoins")){
        TxPoW pow("0x10",config->containsKey("height")?int(std::get<int64_t>(config->get("height").value)):20,1);
        auto node=std::make_shared<TxPoWTreeNode>(pow);
        for(const auto& value:std::get<std::shared_ptr<JSONArray>>(config->get("chainCoins").value)->elements())node->addCoinToNode(*fixtureCoin(value));
        MinimaDB::getDB()->getTxPoWTree()->setRoot(node);
    }
    if(config->containsKey("megaCoins"))for(const auto& value:std::get<std::shared_ptr<JSONArray>>(config->get("megaCoins").value)->elements()){
        auto coin=fixtureCoin(value);MinimaDB::getDB()->getMegaMMR()->getAllCoins().put(coin->getCoinID().to0xString(),coin);
    }
    if(config->containsKey("mempool")){
        auto pow=std::make_shared<TxPoW>();
        for(const auto& value:std::get<std::shared_ptr<JSONArray>>(config->get("mempool").value)->elements()){
            auto coin=fixtureCoin(value);pow->getTransaction().addInput(*coin);pow->getWitness().addCoinProof(CoinProof(*coin,MMRProof()));
        }
        auto data=std::make_shared<RamData>(pow);data->setInCascade(flag("poolCascade"));data->setOnMainChain(flag("poolOnChain"));
        MinimaDB::getDB()->getTxPoWDB()->getCompleteMemPool()->put("fixture",data);
    }
}
static JSONObject run(const JSONObject& input){
    config=&input;calls=JSONArray();logs=JSONArray();MinimaDBServices services;
    services.sql.connect=[](const auto&,const auto&,const auto&)->std::shared_ptr<SqlConnection>{throw JavaNullPointerException("offline SQL connection");};services.sql.log=[](const auto& text,bool){logs.add(text);};
    services.isShuttingDown=[]{return false;};services.postMainMessage=[](auto){};services.postMainTimer=[](auto){};services.postNotifyEvent=[](const auto&,const auto&){};services.setStartupError=[](bool,const auto&){};services.halt=[](int){throw std::runtime_error("unexpected halt");};services.currentThreadName=[]{return "main";};
    MinimaDB::createDB(services);WalletCommandsFixture::init(flag("nullDB"));db=MinimaDB::getDB()->getCustomTxnDB();if(!db)db=std::make_shared<TxnDB>();
    MinimaLogger::Runtime logger;logger.println=[](const auto& full){auto text=unicode::encodeUTF8(full);auto at=text.find("] : ");if(at!=std::string::npos&&!text.substr(at+4).starts_with("     "))logs.add(text.substr(at+4));};MinimaLogger::setRuntime(logger);struct Reset{~Reset(){MinimaLogger::setRuntime({});}}reset;
    fixtureChain();
    if(input.containsKey("existing"))db->createTransaction(input.getString("existing"));JSONObject out;
    try{
        auto name=input.getString("name");auto cmd=command(name);*cmd->getParams()=object(input.get("params"));
        if(cmd->getParams()->get("data").isString()&&cmd->getParams()->get("data").asString()=="$ROW")cmd->getParams()->put("data",MiniData(TxnRow("imported",std::make_shared<Transaction>(),std::make_shared<Witness>()).serialise()).to0xString());
        if(input.containsKey("consolidateHelper")){
            std::vector<std::shared_ptr<Coin>> coins;const auto& all=input.get("consolidateHelper");
            if(!all.isNull())for(const auto& value:std::get<std::shared_ptr<JSONArray>>(all.value)->elements())coins.push_back(fixtureCoin(value));
            auto nullable=[&](const std::string& key)->std::optional<std::string>{auto value=cmd->getParams()->get(key);return value.isNull()?std::nullopt:std::optional(value.asString());};
            std::optional<MiniNumber> burn,keyuses;if(auto value=nullable("burn"))burn.emplace(*value);if(auto value=nullable("keyuses"))keyuses.emplace(*value);
            auto helper=std::dynamic_pointer_cast<Recording<commands::consolidatefrom>>(cmd);
            auto result=helper->createConsolidate(all.isNull()?nullptr:&coins,burn?&*burn:nullptr,nullable("fromaddress"),nullable("tokenid"),nullable("script"),nullable("privatekey"),keyuses?&*keyuses:nullptr);
            out.put("value",result?JSONValue(*result):JSONValue());
        }
        else if(flag("meta")){JSONObject meta;meta.put("help",cmd->getHelp().value());meta.put("fullhelp",cmd->getFullHelp().value());JSONArray params;for(const auto& p:cmd->getValidParams())params.add(p);meta.put("params",params);meta.put("fresh",cmd->getFunction()->getJSONReply());out.put("value",meta);}
        else if(input.containsKey("runner")){
            auto saved=CommandRunner::ALL_COMMANDS;
            struct Restore {std::vector<std::shared_ptr<Command>> saved;~Restore(){CommandRunner::ALL_COMMANDS=std::move(saved);}}restore{saved};
            if(flag("recordRunner"))CommandRunner::ALL_COMMANDS.insert(CommandRunner::ALL_COMMANDS.begin(),command(name));
            out.put("value",CommandRunner::getRunner().runMultiCommand("0x00",input.getString("runner")));
        }
        else if(input.containsKey("helper")){
            auto text=input.getString("helper");JSONObject value;
            if(name=="consolidatefrom")value=commands::consolidatefrom().runCommand(text);else if(name=="sendfrom")value=commands::sendfrom().runCommand(text);else if(name=="createtokenfrom")value=commands::createtokenfrom().runCommand(text);else if(name=="rawfrom")value=commands::rawfrom().runCommand(text);else if(name=="rawtxnfrom")value=commands::rawtxnfrom().runCommand(text);else if(name=="createfrom")value=commands::createfrom().runCommand(text);else if(name=="constructfrom")value=commands::constructfrom().runCommand(text);else if(name=="postfrom")value=commands::postfrom().runCommand(text);else value=commands::signfrom().runCommand(text);out.put("value",value);
        }else out.put("value",cmd->runCommand());
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}
    catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    const auto& value=out.get("value");
    if(auto result=std::get_if<std::shared_ptr<JSONObject>>(&value.value)){
        const auto& response=(*result)->get("response");
        if(auto object=std::get_if<std::shared_ptr<JSONObject>>(&response.value)){
            const auto& data=(*object)->get("data");
            if(flag("real")&&data.isString()){
                auto bytes=MiniData::fromHex(data.asString());size_t offset=0;auto row=TxnRow::deserialise(bytes.bytes(),offset);
                JSONObject observation;observation.put("consumed",int64_t(offset));observation.put("wire",MiniData(row->serialise()).to0xString());
                auto transaction=row->getTransaction();transaction->calculateTransactionID();JSONArray valid;
                for(const auto& signature:row->getWitness()->getAllSignatures()){TreeKey key;key.setPublicKey(signature.getRootPublicKey());valid.add(key.verify(transaction->getTransactionID(),signature));}
                observation.put("signatures",valid);out.put("export",observation);
            }
        }
    }
    out.put("calls",calls);out.put("rows",rows());out.put("logs",logs);return out;
}
int main(){std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
