#include "SqlRecordingFixture.hpp"
#include "ArchiveFixture.hpp"
#include "org/minima/objects/Pulse.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace minima;
using namespace minima::json;
using namespace minima::sql_fixture;
namespace minima {class PulseRuntimeFixture {public:static void install(std::shared_ptr<MinimaDB> db){std::lock_guard lock(MinimaDB::mInstanceLock);MinimaDB::mMinimaDB=std::move(db);}};}
namespace {
JSONArray mainEvents,pulseEvents;
int pulseCalls=0;
std::shared_ptr<MinimaDB> pulseBacking;
std::string threadName="fixture";
MinimaDBServices services(){
    return {{[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{
        event("connect",{url,user,password});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();
    },[]{return flag("debug");},[](const auto& text,bool){std::clog<<text<<'\n';}},[]{return false;},
    [](auto message){JSONArray row;row.add("message");row.add(message->getMessageType().value_or("null"));mainEvents.add(row);},
    [](auto timer){JSONArray row;row.add("timer");row.add(timer->getMessageType().value_or("null"));row.add(timer->getTimer());mainEvents.add(row);},
    [](const auto&,const auto&){},[](bool value,const auto& error){JSONArray row;row.add("startup");row.add(value);row.add(error);mainEvents.add(row);},
    [](int code){throw SqlRuntimeException("fixture.Halt",std::to_string(code));},[]{return threadName;}};
}
class PulseFaultDB:public MinimaDB {
    void call(const std::string& name){pulseEvents.add(name);if(++pulseCalls==integer(*config,"pulseFail",-1)){if(flag("pulseError"))throw JavaError("java.lang.AssertionError","fixture pulse failure");throw JavaTypedException("java.lang.RuntimeException","fixture pulse failure");}}
public:
    PulseFaultDB():MinimaDB(services()){}
    void readLock(bool lock) override{call(lock?"readLock(true)":"readLock(false)");pulseBacking->readLock(lock);}
    std::shared_ptr<TxPowTree> getTxPoWTree() override{call("getTxPoWTree");if(flag("pulseClearDB"))PulseRuntimeFixture::install(nullptr);return flag("pulseNullTree")?nullptr:pulseBacking->getTxPoWTree();}
};
struct Identities {
    std::vector<std::shared_ptr<void>> objects;
    template<class T> int id(std::shared_ptr<T> value){if(!value)return -1;auto it=std::find(objects.begin(),objects.end(),value);if(it!=objects.end())return int(it-objects.begin());objects.push_back(value);return int(objects.size()-1);}
    JSONArray snapshot(const std::shared_ptr<MinimaDB>& db){JSONArray out;
        out.add(id(db));out.add(id(db->getArchive()));out.add(id(db->getTxPoWDB()));out.add(id(db->getTxPoWTree()));out.add(id(db->getCascade()));out.add(id(db->getUserDB()));out.add(id(db->getCustomTxnDB()));out.add(id(db->getWallet()));out.add(id(db->getTxBlockDB()));out.add(id(db->getMegaMMR()));out.add(id(db->getP2PDB()));out.add(id(db->getP2P2DB()));return out;}
};
JSONObject run(const JSONObject& input,const std::string& root){
    JSONObject initial;initial.put("rows",JSONArray());initial.put("columns",JSONArray());config=&initial;
    connectionID=preparedID=0;GeneralParams::DATA_FOLDER=root;GeneralParams::DB_IGNORE_LOCKS=false;GeneralParams::IS_MEGAMMR=false;GeneralParams::IS_MOBILE=true;GeneralParams::IS_JNLP=false;GeneralParams::RESCUE_MEGAMMR_NODE="";GeneralParams::TEST_PARAMS=false;GeneralParams::USE_SQL_COINDB=false;
    MinimaDB::createDB(services());auto db=MinimaDB::getDB();db->writeLock(true);db->writeLock(false);
    CoinDB::createCoinDB(std::filesystem::path(root)/"bootstrap"/"coins",services().sql);connectionID=preparedID=0;
    Identities ids;auto original=ids.snapshot(db);auto held=db;std::shared_ptr<TxnDB> txn;JSONArray outputs;auto query=std::make_shared<MMR>();std::shared_ptr<MMR> heldMMR;
    auto pulse=std::make_shared<Pulse>();std::shared_ptr<std::vector<MiniData>> heldPulseList;Pulse::PULSE_VERSION=MiniNumber::ONE;
    for(const auto& item:array(input.get("ops")).elements()){
        config=&object(item);calls=0;events=JSONArray();mainEvents=JSONArray();pulseEvents=JSONArray();pulseCalls=0;JSONObject out;std::ostringstream log;auto previous=std::clog.rdbuf(log.rdbuf());
        GeneralParams::SEED_PHRASE="fixture seed";GeneralParams::ANYSEED_PHRASE=true;
        if(config->containsKey("ignore"))GeneralParams::DB_IGNORE_LOCKS=flag("ignore");
        if(config->containsKey("mega"))GeneralParams::IS_MEGAMMR=flag("mega");
        if(config->containsKey("encrypted"))GeneralParams::IS_MAIN_DBPASSWORD_SET=flag("encrypted");else GeneralParams::IS_MAIN_DBPASSWORD_SET=false;
        GeneralParams::MAIN_DBPASSWORD="fixture password";GeneralParams::RESCUE_MEGAMMR_NODE=string(config->get("rescue")).value_or("");
        out.put("before",TimerMessage::currentTimeMillis());try{JSONValue value;auto cmd=config->get("cmd").asString();
            auto target=[&]{auto name=string(config->get("target")).value_or("query");return name=="tip"?sqlRequired(db->getTxPoWTree()->getTip()).getMMR():name=="global"?sqlRequired(db->getMegaMMR()).getSharedMMR():name=="held"?heldMMR:query;};
            if(cmd=="read")db->readLock(flag("lock"));else if(cmd=="write")db->writeLock(flag("lock"));else if(cmd=="safe")db->safeReleaseReadWriteLock();
            else if(cmd=="create"){MinimaDB::createDB(services());db=MinimaDB::getDB();}
            else if(cmd=="reset")db->resetCascadeAndTxPoWTree();else if(cmd=="cascade")db->setIBDCascade(flag("null")?nullptr:held->getCascade());else if(cmd=="mega")db->hardSetMegaMMR(flag("null")?nullptr:held->getMegaMMR());
            else if(cmd=="hold")held=db;else if(cmd=="useHeld")db=held;
            else if(cmd=="path"){GeneralParams::DATA_FOLDER=config->containsKey("base")?config->get("base").asString():root;value=db->getDBFile(string(config->get("name"))).string();GeneralParams::DATA_FOLDER=root;}
            else if(cmd=="add")db->addCoinNotify(string(config->get("address")));else if(cmd=="remove")value=db->removeCoinNotify(string(config->get("address")));else if(cmd=="check")value=db->checkCoinNotify(string(config->get("address")));
            else if(cmd=="load")db->loadAllDB();else if(cmd=="reload")db->loadArchiveAndTxPoWDB(flag("wallet"));else if(cmd=="restore")db->loadDBsForRestoreSync();else if(cmd=="refresh")db->refreshSQLDB();else if(cmd=="restart")db->fullDBRestartMemFree();
            else if(cmd=="allow")db->setAllowSaveState(flag("allow"));else if(cmd=="save")db->saveAllDB(flag("compact"));else if(cmd=="saveSQL")db->saveSQL(flag("compact"));else if(cmd=="saveState")db->saveState();else if(cmd=="saveUser")db->saveUserDB();else if(cmd=="saveP2P")db->saveP2PDB();
            else if(cmd=="holdTxn")txn=db->getCustomTxnDB();else if(cmd=="saveTxn")sqlRequired(txn).saveDB();else if(cmd=="newTxn")sqlRequired(db->getCustomTxnDB()).createTransaction(string(config->get("id")));
            else if(cmd=="mmrHold")heldMMR=sqlRequired(db->getMegaMMR()).getSharedMMR();
            else if(cmd=="mmrClearMega")sqlRequired(db->getMegaMMR()).clear();
            else if(cmd=="mmrClear")sqlRequired(target()).clearParent();
            else if(cmd=="mmrChild")query=std::make_shared<MMR>(query);
            else if(cmd=="mmrTime")sqlRequired(target()).setBlockTime(MiniNumber(integer(*config,"time")));
            else if(cmd=="mmrSet"){auto entry=sqlRequired(target()).setEntry(int(integer(*config,"row")),MMREntryNumber(int(integer(*config,"entry"))),MMRData(MiniData::fromHex("0x01"),MiniNumber(integer(*config,"amount",1))));value=entry?JSONValue(entry->toJSON()):JSONValue();}
            else if(cmd=="mmrAdd")value=sqlRequired(target()).addEntry(MMRData(MiniData::fromHex("0x01"),MiniNumber(integer(*config,"amount",1))))->toJSON();
            else if(cmd=="mmrGet")value=sqlRequired(target()).getEntry(int(integer(*config,"row")),MMREntryNumber(int(integer(*config,"entry"))),MiniNumber(integer(*config,"back")))->toJSON();
            else if(cmd=="mmrWire")value=MiniData(sqlRequired(target()).serialise()).to0xString();
            else if(cmd=="truncate"){
                auto name=config->get("name").asString();if(name!="cascade.db"&&name!="chaintree.db"&&name!="megammr.mmr"&&name!="userprefs.db")throw std::invalid_argument("Unexpected fixture file");
                auto file=db->getDBFile(name);auto bytes=MiniFile::readCompleteFile(file);bytes.resize(std::min(bytes.size(),size_t(integer(*config,"length"))));MiniFile::writeDataToFile(file,bytes);
            }
            else if(cmd=="snapshot"){
                JSONObject state;state.put("cascade",MiniData(sqlRequired(db->getCascade()).serialise()).to0xString());state.put("tree",MiniData(db->getTxPoWTree()->serialise()).to0xString());state.put("mega",MiniData(sqlRequired(db->getMegaMMR()).serialise()).to0xString());value=state;
            }
            else if(cmd=="command"){
                auto runner=CommandRunner::getRunner();auto mode=config->getString("mode","direct");
                if(mode=="runner")value=runner.runMultiCommand(config->getString("caller","0x00"),config->getString("text"));
                else{auto command=runner.getCommandOnly(config->getString("name"));auto& active=sqlRequired(command);if(config->containsKey("params"))for(const auto& [key,item]:object(config->get("params")).entries())active.getParams()->put(key,item);
                    if(mode=="meta"){JSONObject meta;meta.put("name",active.getName().value());meta.put("help",active.getHelp().value());meta.put("fullhelp",active.getFullHelp().value());JSONArray valid;for(const auto& key:active.getValidParams())valid.add(key);meta.put("valid",valid);active.getParams()->put("fixture","old");meta.put("fresh",active.getFunction()->getJSONReply());value=meta;}
                    else value=active.runCommand();}
            }
            else if(cmd=="tipTime"){auto& pow=sqlRequired(db->getTxPoWTree()->getTip()).getTxPoW();pow.setTimeMilli(MiniNumber(config->getString("time")));pow.calculateTXPOWID();db->getTxPoWTree()->recalculateTree();}
            else if(cmd=="chain"){
                db->resetCascadeAndTxPoWTree();std::shared_ptr<TxPoWTreeNode> previousNode;
                std::vector<std::shared_ptr<TxPoWTreeNode>> nodes;int start=int(integer(*config,"start",1));
                for(int i=0;i<integer(*config,"count",3);++i){if(config->containsKey("parents")){int parent=int(std::get<int64_t>(array(config->get("parents")).get(i).value));previousNode=parent<0?nullptr:nodes.at(parent);}auto pow=support::archiveFixtureTxPoW(start+i,0);if(previousNode){pow->setSuperParent(0,previousNode->getTxPoW().getTxPoWIDData());pow->calculateTXPOWID();}auto node=std::make_shared<TxPoWTreeNode>(*pow);if(previousNode)previousNode->addChildNode(node);else db->getTxPoWTree()->setRoot(node);previousNode=node;nodes.push_back(node);}
                db->getTxPoWTree()->recalculateTree();if(!flag("emptyCascade"))db->getCascade()->addToTip(*support::archiveFixtureTxPoW(int(integer(*config,"cascadeHeight",start-1)),0));
            }
            else if(cmd=="pulseFault"){pulseBacking=db;PulseRuntimeFixture::install(std::make_shared<PulseFaultDB>());}
            else if(cmd=="pulseRestore")PulseRuntimeFixture::install(db);
            else if(cmd=="pulse")pulse=std::make_shared<Pulse>(Pulse::createPulse());
            else if(cmd=="pulseNew")pulse=std::make_shared<Pulse>();
            else if(cmd=="pulseVersion")Pulse::PULSE_VERSION=MiniNumber(config->getString("version"));
            else if(cmd=="pulseHold")heldPulseList=config->getString("target","pulse")=="tree"?db->getTxPoWTree()->getSharedPulseList():pulse->getSharedBlockList();
            else if(cmd=="pulseSet"){
                auto mode=config->getString("target","held");pulse->setSharedBlockList(mode=="null"?nullptr:mode=="tree"?db->getTxPoWTree()->getSharedPulseList():mode=="new"?std::make_shared<std::vector<MiniData>>():heldPulseList);
            }
            else if(cmd=="pulseMutate"){
                auto name=config->getString("target","pulse");auto list=name=="tree"?db->getTxPoWTree()->getSharedPulseList():name=="held"?heldPulseList:pulse->getSharedBlockList();
                if(config->getString("action","add")=="clear")sqlRequired(list).clear();else sqlRequired(list).push_back(MiniData::fromHex(config->getString("id","0x01")));
            }
            else if(cmd=="pulseRecalc")db->getTxPoWTree()->recalculateTree();
            else if(cmd=="pulseWire")value=MiniData(pulse->serialise()).to0xString();
            else if(cmd=="pulseDataRead"){
                auto bytes=MiniData::fromHex(config->getString("wire")).bytes();size_t offset=0;
                try{value=MiniData::deserialise(bytes,offset,int(integer(*config,"expected",-1))).to0xString();}catch(...){out.put("remaining",int64_t(bytes.size()-offset));throw;}
                out.put("remaining",int64_t(bytes.size()-offset));
            }
            else if(cmd=="pulseRead"){
                auto bytes=MiniData::fromHex(config->getString("wire")).bytes();size_t offset=0;
                try{pulse->readDataStream(bytes,offset);}catch(...){out.put("remaining",int64_t(bytes.size()-offset));throw;}
                out.put("remaining",int64_t(bytes.size()-offset));
            }
            else if(cmd=="pulseSnapshot"){
                auto tree=db->getTxPoWTree()->getSharedPulseList();auto current=pulse->getSharedBlockList();JSONObject state;
                auto describe=[](const std::shared_ptr<std::vector<MiniData>>& list)->JSONValue{if(!list)return {};JSONArray result;for(const auto& id:*list)result.add(id.to0xString());return result;};
                state.put("pulse",describe(current));state.put("held",describe(heldPulseList));state.put("tree",describe(tree));state.put("pulseTree",current==tree);state.put("pulseHeld",current==heldPulseList);state.put("heldTree",heldPulseList==tree);value=state;
            }
            else if(cmd=="info"){}else throw std::invalid_argument(cmd);out.put("value",value);
        }catch(const wire::IOException& error){out.put("exception",javaIOExceptionText(error));}catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException: ")+error.what());}catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
        if(config->getString("cmd").starts_with("pulse"))out.put("pulseCalls",pulseEvents);
        std::clog.rdbuf(previous);GeneralParams::DATA_FOLDER=root;JSONArray logs;std::string line;std::istringstream lines(log.str());while(std::getline(lines,line))logs.add(line);
        out.put("logs",logs);out.put("events",events);out.put("main",mainEvents);out.put("ids",ids.snapshot(db));out.put("global",ids.id(MinimaDB::getDB()));out.put("lock",db->getRWLockInfo());out.put("thread",MinimaDB::currentWriteLockThread());out.put("state",MinimaDB::mCurrentWriteLockState.load());
        out.put("user",*db->getUserDB()->getAllData());JSONArray sizes;sizes.add(db->getCascadeFileSize());sizes.add(db->getUserDBFileSize());sizes.add(db->getTxPowTreeFileSize());sizes.add(db->getP2PFileSize());out.put("sizes",sizes);out.put("after",TimerMessage::currentTimeMillis());outputs.add(out);
    }
    JSONObject out;out.put("original",original);out.put("ops",outputs);return out;
}
}
int main(int argc,char** argv){if(argc!=2)return 2;
    auto logger=MinimaLogger::getRuntime();logger.println=[](const std::u16string& text){auto full=unicode::encodeUTF8(text);auto at=full.find("] : ");if(at!=std::string::npos){auto body=full.substr(at+4);if(!body.starts_with("     "))std::clog<<body<<'\n';}};MinimaLogger::setRuntime(std::move(logger));std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
