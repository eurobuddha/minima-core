#include "SqlRecordingFixture.hpp"
#include "ArchiveFixture.hpp"
#include "MainNetworkFixture.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/MySQLConfiguration.hpp"
#include "support/NativeThread.hpp"
#include <iostream>
#include <sstream>
using namespace minima;using namespace minima::json;using namespace minima::sql_fixture;
namespace {JSONArray timers;std::vector<std::shared_ptr<JSONObject>> held;JSONArray listenerCalls;
MainServices services(){MainServices s;s.sql.connect=[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user,password});return std::make_shared<Connection>();};s.sql.log=[](const auto& text,bool){std::clog<<text<<'\n';};s.mysql.connect=[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user?JSONValue(*user):JSONValue(),password?JSONValue(*password):JSONValue()});return std::make_shared<Connection>();};s.mysql.log=[](const auto& text){std::clog<<text<<'\n';};s.sleep=[](int64_t time){if(time<0)throw JavaIllegalArgumentException("timeout value is negative");std::this_thread::sleep_for(std::chrono::milliseconds(time));};s.halt=[](int code){throw JavaTypedException("fixture.Halt",std::to_string(code));};s.exit=s.halt;return s;}
class ManualMain:public Main {public:ManualMain():Main(services()){}std::shared_ptr<Message> take(){return getNextMessage();}void PostTimerMessage(const std::shared_ptr<TimerMessage>& timer) override{JSONArray entry;entry.add(timer->getMessageType().value_or("null"));entry.add(timer->getTimer());timers.add(entry);if(flag("timerFail"))throw JavaTypedException("java.lang.RuntimeException","fixture timer failure");}};
class Listener:public MessageListener {public:void processMessage(std::shared_ptr<Message> message) override{auto data=message->getObject("notify").as<std::shared_ptr<JSONObject>>();held.push_back(data);JSONObject info;info.put("main",bool(Main::getInstance()));info.put("type",message->getMessageType().value_or("null"));listenerCalls.add(info);if(flag("listenerError"))throw JavaError("java.lang.AssertionError","fixture listener failure");if(flag("listenerFail"))throw JavaTypedException("java.lang.RuntimeException","fixture listener failure");}};
}
namespace minima {class MainFixture {public:
    static void install(const std::shared_ptr<Main>& main){std::lock_guard lock(Main::mStaticLock);Main::mMainInstance=main;}
    static void initialize(const std::shared_ptr<Main>& main){main->mUptimeMilli=0;main->bindRuntime();MinimaDB::createDB(main->databaseServices());}
    static void network(Main& main,std::shared_ptr<NetworkManager> network){main.replace(main.mNetwork,std::move(network));}
    static void dispatch(Main& main,const std::shared_ptr<Message>& message){main.processMessage(message);}
    static void fields(Main& main,const JSONObject& values){for(const auto& [key,value]:values.entries()){auto name=unicode::encodeUTF8(key);if(name=="restoring")main.mRestoring=value.asBool();else if(name=="shutdown")main.mShuttingdown=value.asBool();else if(name=="keys")main.mInitKeysCreated=value.asBool();else if(name=="complete")main.STARTUP_COMPLETE=value.asBool();else if(name=="importing")main.MYSQL_IMPORTING_NO_ACTION=value.asBool();else if(name=="uptime")main.mUptimeMilli=std::get<int64_t>(value.value);}}
};}
namespace {
JSONObject snapshot(const std::shared_ptr<Main>& main){JSONObject out;out.put("startup",main->isStartUpComplete());out.put("error",main->isStartupError());auto error=main->getStartupErrorMsg();out.put("errorMessage",error?JSONValue(*error):JSONValue());out.put("shutdown",main->isShuttingDown());out.put("restoring",main->isRestoring());out.put("either",main->isShuttongDownOrRestoring());out.put("sync",main->isSyncIBD());out.put("normal",main->isNormalMineMode());out.put("automine",main->AUTOMINE_TIMER.load());out.put("keys",main->getAllKeysCreated());out.put("instance",Main::getInstance()==main);out.put("listener",bool(Main::getMinimaListener()));out.put("queue",main->getSize());return out;}
JSONObject run(const JSONObject& input,const std::string& root){
    config=&input;events=JSONArray();GeneralParams::resetDefaults();GeneralParams::DATA_FOLDER=root;GeneralParams::USE_SQL_COINDB=false;GeneralParams::P2P_ENABLED=false;GeneralParams::P2P2_ENABLED=false;GeneralParams::MYSQL_DB_DELAY=0;Main::STARTUP_DEBUG_LOGS=false;Main::setMinimaListener(nullptr);held.clear();connectionID=preparedID=0;
    auto main=std::make_shared<ManualMain>();MainFixture::install(main);MainFixture::initialize(main);auto data=std::make_shared<JSONObject>();std::shared_ptr<support::MainNetworkFixture> network;JSONArray outputs;
    for(const auto& step:array(input.get("ops")).elements()){
        config=&object(step);events=JSONArray();calls=0;timers=JSONArray();listenerCalls=JSONArray();JSONObject out;std::ostringstream stream;auto previous=std::clog.rdbuf(stream.rdbuf());auto before=TimerMessage::currentTimeMillis();
        try{JSONValue value;auto cmd=config->getString("cmd");
            if(cmd=="fields")MainFixture::fields(*main,object(config->get("values")));
            else if(cmd=="diagnostics"){
                if(flag("reset")){MessageProcessor::setTrace(false,"");NIOClient::mTraceON=false;NIOServer::mTraceON=false;}
                JSONObject diagnostics;diagnostics.put("trace",main->isTrace());diagnostics.put("filter",main->getTraceFilter());diagnostics.put("client",NIOClient::mTraceON.load());diagnostics.put("server",NIOServer::mTraceON.load());if(config->containsKey("match"))diagnostics.put("matches",main->checkTraceFilter(config->getString("match")));value=diagnostics;
            }
            else if(cmd=="normal")main->setNormalAutoMineSpeed();else if(cmd=="low")main->setLowPowAutoMineSpeed();
            else if(cmd=="sync"){GeneralParams::IBDSYNC_LOGS=flag("logs");main->setSyncIBD(flag("value"));}
            else if(cmd=="error")main->setStartUpError(flag("value"),string(config->get("message")));
            else if(cmd=="uptime")value=main->getUptimeMilli();
            else if(cmd=="listener")Main::setMinimaListener(flag("enabled")?std::make_shared<Listener>():nullptr);
            else if(cmd=="notify1")Main::NotifyMainListenerOnly(string(config->get("event")));
            else if(cmd=="notify2")Main::NotifyMainListenerOnly(string(config->get("event")),flag("null")?nullptr:data);
            else if(cmd=="notifyTo")main->PostNotifyEvent(string(config->get("event")),flag("null")?nullptr:data,string(config->get("to")));
            else if(cmd=="mutate")data->put(config->getString("key","value"),config->get("value"));
            else if(cmd=="clear")Main::ClearMainInstance();else if(cmd=="publish")MainFixture::install(main);else if(cmd=="shutdownNotice")main->NotifyMainListenerOfShutDown();
            else if(cmd=="network"){network=support::MainNetworkFixture::create(*config,[&](auto value){MainFixture::network(*main,std::move(value));});MainFixture::network(*main,flag("nullNetwork")?nullptr:network->network());}
            else if(cmd=="networkSnapshot")value=sqlRequired(network).snapshot();
            else if(cmd=="rescueNode")GeneralParams::RESCUE_MEGAMMR_NODE=config->getString("host");
            else if(cmd=="drain"){JSONArray queued;while(auto message=main->take()){JSONObject item;auto type=message->getMessageType();item.put("type",type?JSONValue(*type):JSONValue());auto host=message->getString("host");item.put("host",host?JSONValue(*host):JSONValue());queued.add(item);}value=queued;}
            else if(cmd=="run")value=CommandRunner::getRunner().runMultiCommand(config->getString("caller","0x00"),config->getString("text"));
            else if(cmd=="direct"||cmd=="meta"){auto command=CommandRunner::getRunner().getCommandOnly(config->getString("name"));sqlRequired(command);if(cmd=="direct"){for(const auto& [key,item]:object(config->get("params")).entries())command->getParams()->put(key,item);value=command->runCommand();}else{JSONObject meta;meta.put("name",command->getName().value_or("null"));meta.put("help",command->getHelp().value_or("null"));meta.put("fullHelp",command->getFullHelp().value_or("null"));JSONArray valid;for(auto& parameter:command->getValidParams())valid.add(parameter);meta.put("valid",valid);auto clone=command->getFunction();meta.put("fresh",clone!=command&&clone->getParams()!=command->getParams());value=meta;}}
            else if(cmd=="command")value=main->runSingleMinimaCMD(string(config->get("text")));
            else if(cmd=="shutdown")main->shutdown(flag("compact"));else if(cmd=="restore")main->restoreReady(flag("mds"));else if(cmd=="final")main->shutdownFinalProcs(flag("mds"));
            else if(cmd=="keysSize")value=main->getAllDefaultKeysSize();
            else if(cmd=="prefs"){for(const auto& [key,v]:object(config->get("values")).entries())MinimaDB::getDB()->getUserDB()->getAllData()->put(key,v);}
            else if(cmd=="healthChain"){
                auto db=MinimaDB::getDB();db->resetCascadeAndTxPoWTree();std::vector<std::shared_ptr<TxPoWTreeNode>> nodes;
                const auto& blocks=array(config->get("blocks"));
                for(size_t i=0;i<blocks.size();++i){auto pow=support::archiveFixtureTxPoW(int(i),0);pow->setBlockNumber(MiniNumber(blocks.get(i).asString()));pow->calculateTXPOWID();auto node=std::make_shared<TxPoWTreeNode>(*pow);
                    if(i){auto parent=config->containsKey("parents")?std::get<int64_t>(array(config->get("parents")).get(i).value):int64_t(i)-1;nodes.at(size_t(parent))->addChildNode(node);}else db->getTxPoWTree()->setRoot(node);nodes.push_back(node);
                }db->getTxPoWTree()->recalculateTree();
                int i=0;for(const auto& number:array(config->get("cascade")).elements()){auto pow=support::archiveFixtureTxPoW(i++,0);pow->setBlockNumber(MiniNumber(number.asString()));pow->calculateTXPOWID();db->getCascade()->addToTip(*pow);}
                if(flag("nullCascade"))db->setIBDCascade(nullptr);
            }
            else if(cmd=="chain"){auto db=MinimaDB::getDB();db->resetCascadeAndTxPoWTree();std::shared_ptr<TxPoWTreeNode> previous;for(int i=0;i<integer(*config,"count",3);++i){auto pow=support::archiveFixtureTxPoW(i,0);auto node=std::make_shared<TxPoWTreeNode>(*pow);if(previous)previous->addChildNode(node);else db->getTxPoWTree()->setRoot(node);previous=node;}db->getTxPoWTree()->recalculateTree();}
            else if(cmd=="mysqlConfig"){GeneralParams::MYSQL_DB_DELAY=int32_t(integer(*config,"delay"));GeneralParams::MYSQL_DB_COINS=flag("coins");mysql_detail::configure(string(config->get("text")),services().sleep);}
            else if(cmd=="message"){
                GeneralParams::TXBLOCK_NODE=flag("slave");GeneralParams::MYSQL_STORE_ALLTXPOW=flag("storeTxPoW");auto message=std::make_shared<Message>(string(config->get("type")));
                if(config->containsKey("fields"))for(const auto& [key,v]:object(config->get("fields")).entries()){auto name=unicode::encodeUTF8(key);if(v.isNull())message->addObject(name,MessageObject());else if(v.isString())message->addObject(name,MessageObject(v.asString()));else if(auto b=std::get_if<bool>(&v.value))message->addBoolean(name,*b);else if(auto n=std::get_if<int64_t>(&v.value))message->addInteger(name,int32_t(*n));}
                if(config->containsKey("pow")){auto pow=support::archiveFixtureTxPoW(int(integer(*config,"pow")),0);message->addObject("txpow",MessageObject(pow));}
                MainFixture::dispatch(*main,message);
            }else if(cmd!="info")throw std::invalid_argument(cmd);out.put("value",value);
        }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException: ")+error.what());}catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
        auto after=TimerMessage::currentTimeMillis();std::clog.rdbuf(previous);JSONArray logs;std::string line;std::istringstream lines(stream.str());while(std::getline(lines,line))logs.add(line);
        out.put("before",before);out.put("after",after);out.put("logs",logs);out.put("timers",timers);out.put("events",events);out.put("listeners",listenerCalls);JSONArray retained;for(const auto& item:held)retained.add(*item);out.put("held",retained);out.put("state",snapshot(main));out.put("user",*MinimaDB::getDB()->getUserDB()->getAllData());outputs.add(out);
    }Main::setMinimaListener(nullptr);MainFixture::install(nullptr);JSONObject result;result.put("ops",outputs);return result;
}
}
int main(int argc,char** argv){MinimaLogger::Runtime logger;logger.println=[](const auto& text){auto full=unicode::encodeUTF8(text);auto at=full.find("] : ");if(at!=std::string::npos&&!full.substr(at+4).starts_with("     "))std::clog<<full.substr(at+4)<<'\n';};MinimaLogger::setRuntime(std::move(logger));std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<nativeExceptionText(e)<<'\n';std::cout<<"{\"fatal\":true}\n";}}
