#include "MainFixture.hpp"
#include "ArchiveFixture.hpp"
#include "org/minima/system/commands/backup/restore.hpp"
#include "org/minima/system/commands/backup/restoresync.hpp"
#include "org/minima/system/commands/backup/reset.hpp"
#include "org/minima/system/commands/backup/backup.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeH2Connection.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima {struct WalletCommandsFixture {static bool allow(){return MinimaDB::getDB()->mAllowSaveState.load();}};}
static JSONArray events;static const JSONObject* request;
class ManualRestoreMain:public Main {
public:
    explicit ManualRestoreMain(MainServices services):Main(std::move(services)){}
    void restoreReady(bool shutdown) override {
        JSONArray event;event.add("ready");event.add(shutdown);events.add(event);
        if(request->containsKey("readyFail")&&request->getBoolean("readyFail"))throw JavaTypedException("java.lang.RuntimeException","fixture ready failure");
        if(request->containsKey("realReady")&&request->getBoolean("realReady")){Main::restoreReady(shutdown);return;}
        JSONObject flags;flags.put("restoring",true);MainFixture::fields(*this,flags);
    }
    void restoreReadyForSync() override {
        events.add("readyForSync");
        if(request->containsKey("syncFail")&&request->getBoolean("syncFail"))throw JavaTypedException("java.lang.RuntimeException","fixture sync failure");
        if(request->containsKey("realSync")&&request->getBoolean("realSync"))Main::restoreReadyForSync();
    }
    void setHasShutDown() override{events.add("shutdown");Main::setHasShutDown();}
    void stopMessageProcessor() override{events.add("stop");Main::stopMessageProcessor();}
};
// Scoped prototypes retain the real CommandRunner parser and status handling.
class ResetChild:public Command {
    std::string mChild;
public:
    explicit ResetChild(std::string name):Command(name,"fixture child"),mChild(std::move(name)){}
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<ResetChild>(mChild);}
    std::vector<std::string> getValidParams() const override{
        if(mChild=="restore")return commands::restore().getValidParams();
        return {"action","host","phrase","anyphrase","keys","keyuses","file","address","statecheck","logs","maxexport"};
    }
    JSONValue runCommand() override{
        JSONObject event;event.put("child",mChild);event.put("text",getCompleteCommand().value_or(""));event.put("params",*getParams());event.put("caller",getMiniDAPPID().value_or(""));events.add(event);
        if(mChild=="restore"&&request->containsKey("realRestore")&&request->getBoolean("realRestore")){
            commands::restore command;*command.getParams()=*getParams();command.setCompleteCommand(getCompleteCommand());command.setMiniDAPPID(getMiniDAPPID());return command.runCommand();
        }
        if(request->containsKey(mChild+"Throw"))throw JavaTypedException("java.lang.RuntimeException",request->getString(mChild+"Throw"));
        if(request->containsKey(mChild+"Reply"))return request->get(mChild+"Reply");
        auto result=getJSONReply();result.put("response","fixture "+mChild);return result;
    }
};
struct ResetChildrenScope {
    std::vector<std::shared_ptr<Command>> saved;
    explicit ResetChildrenScope(bool active):saved(CommandRunner::ALL_COMMANDS){
        if(active){for(auto& command:CommandRunner::ALL_COMMANDS)if(command->getName()=="restore"||command->getName()=="archive")command=std::make_shared<ResetChild>(*command->getName());
            if(!CommandRunner::getRunner().getCommandOnly("archive"))CommandRunner::ALL_COMMANDS.push_back(std::make_shared<ResetChild>("archive"));}
    }
    ~ResetChildrenScope(){CommandRunner::ALL_COMMANDS=std::move(saved);}
};
class RestoreListener:public MessageListener {public:void processMessage(std::shared_ptr<Message> message) override{events.add(message->getMessageType().value_or("null"));}};
JSONObject run(const JSONObject& input,NativeH2Runtime& engine){
    request=&input;events=JSONArray();JSONObject out;GeneralParams::resetDefaults();auto folder=input.getString("folder"),op=input.getString("op","");GeneralParams::DATA_FOLDER=folder;GeneralParams::BASE_FILE_FOLDER=folder;GeneralParams::USE_SQL_COINDB=false;
    MainServices services;services.sql.connect=engine.connector();services.sql.log=[](const auto& text,bool){std::clog<<text<<'\n';};services.halt=[](int){throw std::runtime_error("fixture load failed");};
    auto main=std::make_shared<ManualRestoreMain>(std::move(services));MainFixture::install(main);MainFixture::initialize(main);Main::setMinimaListener(std::make_shared<RestoreListener>());
    auto db=MinimaDB::getDB();db->loadAllDB();JSONObject flags;flags.put("keys",!input.containsKey("keys")||input.getBoolean("keys"));MainFixture::fields(*main,flags);
    if(op=="seed"){
        auto source=input.getString("role")=="source";auto marker=source?"source":"target";db->getUserDB()->setString("fixture_marker",marker);auto uninstalled=std::make_shared<JSONArray>();uninstalled->add("fixture-dapp");db->getUserDB()->setJSONArray("minidapps_uninstalled",uninstalled);db->getP2PDB()->setString("fixture_marker",marker);
        auto sql=db->getTxPoWDB()->getSQLDB();for(int i=0;i<(source?10:2);++i)sql->addTxPoW(support::archiveFixtureTxPoW(source?i:i+20,0).get(),source?i%3!=0:true);sql->executeGenericSQL("UPDATE txpow SET timemilli=id*1000");
        auto query=source?"INSERT INTO keys(size,depth,uses,maxuses,modifier,privatekey,publickey) VALUES(2,2,5,1000,'0x01','0xABCD','0x01'),(2,2,100,1000,'0x02','0x1234','0x02')":"INSERT INTO keys(size,depth,uses,maxuses,modifier,privatekey,publickey) VALUES(2,2,999,1000,'0x03','0x5678','0x03')";
        auto result=db->getWallet()->executeGenericSQL(query);if(!result.getBoolean("status"))throw std::runtime_error("fixture wallet insert failed");
        if(input.containsKey("treeTime")){auto pow=support::archiveFixtureTxPoW(1,0);pow->setTimeMilli(MiniNumber(input.getString("treeTime")));pow->calculateTXPOWID();db->getTxPoWTree()->setRoot(std::make_shared<TxPoWTreeNode>(*pow));}
        if(source){commands::backup command;command.getParams()->put("file","source.bak");out.put("backup",command.runCommand());}
        db->saveAllDB(false);out.put("seeded",true);Main::setMinimaListener(nullptr);return out;
    }
    auto oldWallet=db->getWallet();auto oldTxPoWDB=db->getTxPoWDB();auto oldArchive=db->getArchive();
    bool resetCommand=input.getString("name","")=="reset",restoreSync=input.getString("name","")=="restoresync";
    if(restoreSync){MainFixture::finalMDSStopped(*main);GeneralParams::SEED_PHRASE="restore sync deterministic fixture seed";GeneralParams::ANYSEED_PHRASE=true;}
    ResetChildrenScope children(resetCommand);
    try{
        std::shared_ptr<Command> commandOwner=resetCommand?std::shared_ptr<Command>(std::make_shared<commands::reset>()):std::make_shared<commands::restore>();if(restoreSync)commandOwner=std::make_shared<commands::restoresync>();auto& command=*commandOwner;
        if(op=="meta"){
            out.put("name",*command.getName());out.put("help",*command.getHelp());out.put("fullHelp",*command.getFullHelp());JSONArray params;for(auto& name:command.getValidParams())params.add(name);out.put("params",params);command.getParams()->put("file","fixture");out.put("fresh",*command.getFunction()->getParams());
        }else if(op=="performResync"){
            auto host=input.get("host");std::optional<MiniNumber> start;if(!input.get("start").isNull())start=MiniNumber(input.getString("start"));
            out.put("result",dynamic_cast<commands::restoresync&>(command).performResync(host.isNull()?std::optional<std::string>():std::optional(host.asString()),256,start?&*start:nullptr,true));
        }else if(op=="runner")out.put("result",CommandRunner::getRunner().runMultiCommand(input.getString("caller","0x00"),input.getString("command")));
        else{for(const auto& [key,value]:std::get<std::shared_ptr<JSONObject>>(input.get("params").value)->entries())command.getParams()->put(key,value);out.put("result",command.runCommand());}
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}
    catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    out.put("events",events);out.put("restoring",main->isRestoring());out.put("shutdown",main->isShuttingDown());out.put("published",Main::getInstance()==main);out.put("running",main->isRunning());out.put("allowSave",WalletCommandsFixture::allow());
    out.put("user",*db->getUserDB()->getAllData());out.put("p2p",*db->getP2PDB()->getAllData());out.put("cascade",MiniData(db->getCascade()->serialise()).to0xString());out.put("tree",MiniData(db->getTxPoWTree()->serialise()).to0xString());
    auto cached=db->getWallet()->getBaseSeed()->getSeed();out.put("cachedSeedHash",cached?MiniData(MiniString(*cached).getData()).sha3().to0xString():"null");
    out.put("walletOpen",db->getWallet()->isOpen());out.put("historyOpen",db->getTxPoWDB()->getSQLDB()->isOpen());out.put("archiveOpen",db->getArchive()->isOpen());out.put("archiveFile",std::filesystem::exists(*db->getArchive()->getSQLFile()));
    // Observations after capturing closure/cache state may reopen SQL connections.
    out.put("keys",db->getWallet()->executeGenericSQL("SELECT publickey,uses FROM keys ORDER BY publickey"));
    out.put("seed",db->getWallet()->executeGenericSQL("SELECT RAWTOHEX(HASH('SHA-256',seed)) AS seedhash FROM seed"));
    out.put("history",db->getTxPoWDB()->getSQLDB()->executeGenericSQL("SELECT txpowid,isrelevant FROM txpow ORDER BY txpowid"));
    if(restoreSync){auto tip=db->getTxPoWTree()->getTip();out.put("tipID",tip?JSONValue(tip->getTxPoW().getTxPoWID()):JSONValue());}
    if(resetCommand||restoreSync){out.put("processorCreated",bool(main->getTxPoWProcessor()));out.put("walletReplaced",oldWallet!=db->getWallet());}
    if(auto processor=main->getTxPoWProcessor()){processor->stopMessageProcessor();processor->waitToShutDown();}
    oldWallet->hardCloseDB();oldTxPoWDB->hardCloseSQLDB();oldArchive->hardCloseDB();
    db->getWallet()->hardCloseDB();db->getTxPoWDB()->hardCloseSQLDB();db->getArchive()->hardCloseDB();CoinDB::getTxPoWTreeCoinDB()->hardCloseDB();Main::setMinimaListener(nullptr);return out;
}
int main(){NativeH2Runtime engine;std::string line;while(std::getline(std::cin,line)){JSONObject out;try{out=run(*std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value),engine);}catch(const std::exception& error){out.put("fixtureError",nativeExceptionText(error));}std::cout<<out.toJSONString()<<std::endl;}}
