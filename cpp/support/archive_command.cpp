#include "MainFixture.hpp"
#include "ArchiveFixture.hpp"
#include "org/minima/system/commands/backup/archive.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeH2Connection.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima::commands {struct ArchiveCommandFixture {
    static JSONObject state(){JSONObject out;out.put("h2",archive::H2_TEMPARCHIVE.load());out.put("temp",bool(archive::source(archive::STATIC_TEMPARCHIVE)));out.put("raw",bool(archive::source(archive::STATIC_RAW)));return out;}
    static void clear(){if(auto temp=archive::source(archive::STATIC_TEMPARCHIVE))temp->hardCloseDB();if(auto raw=archive::source(archive::STATIC_RAW))try{raw->stop();}catch(...){}archive::replaceSource(archive::STATIC_TEMPARCHIVE,std::shared_ptr<ArchiveManager>());archive::replaceSource(archive::STATIC_RAW,std::shared_ptr<RawArchiveInput>());archive::H2_TEMPARCHIVE=true;}
};}
static JSONArray events;static const JSONObject* request;
class ArchiveMain:public Main{public:explicit ArchiveMain(MainServices services):Main(std::move(services)){} };
class ArchiveProbe:public commands::archive {
public:
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<ArchiveProbe>();}
    JSONValue runCommand() override{
        if(getParam("action","")=="resync"&&request->containsKey("recordSync")&&request->getBoolean("recordSync")){
            JSONObject event;event.put("command",getCompleteCommand().value_or(""));event.put("params",*getParams());event.put("static",commands::ArchiveCommandFixture::state());events.add(event);
            auto out=getJSONReply();out.put("status",false);out.put("error","fixture sync declined");return out;
        }return commands::archive::runCommand();
    }
};
class ArchiveListener:public MessageListener{public:void processMessage(std::shared_ptr<Message> message)override{
    JSONObject event;event.put("type",message->getMessageType().value());event.put("notify",*message->getObject("notify").as<std::shared_ptr<JSONObject>>());events.add(event);
    if(request->containsKey("listenerThrow")&&request->getBoolean("listenerThrow"))throw JavaTypedException("java.lang.RuntimeException","fixture listener failure");
}};
JSONObject run(const JSONObject& input,NativeH2Runtime& engine){
    commands::ArchiveCommandFixture::clear();request=&input;events=JSONArray();GeneralParams::resetDefaults();auto folder=input.getString("folder"),op=input.getString("op","");GeneralParams::DATA_FOLDER=folder;GeneralParams::BASE_FILE_FOLDER=folder;GeneralParams::USE_SQL_COINDB=false;
    MainServices services;services.sql.connect=engine.connector();services.sql.log=[](const auto& text,bool){std::clog<<text<<'\n';};services.halt=[](int){throw std::runtime_error("fixture startup failed");};auto main=std::make_shared<ArchiveMain>(std::move(services));MainFixture::install(main);MainFixture::initialize(main);auto db=MinimaDB::getDB();db->loadAllDB();JSONObject flags;flags.put("keys",!input.containsKey("keys")||input.getBoolean("keys"));MainFixture::fields(*main,flags);
    auto saved=CommandRunner::ALL_COMMANDS;struct Registry{std::vector<std::shared_ptr<Command>> saved;~Registry(){CommandRunner::ALL_COMMANDS=std::move(saved);}} registry{saved};for(auto& command:CommandRunner::ALL_COMMANDS)if(command->getName()=="archive")command=std::make_shared<ArchiveProbe>();
    JSONObject out;try{
        if(op=="seed"){
            JSONArray wires;MiniData parent=MiniData::ZERO_TXPOWID;
            for(const auto& item:std::get<std::shared_ptr<JSONArray>>(input.get("heights").value)->elements()){
                auto i=int(std::get<int64_t>(item.value));auto pow=support::archiveFixtureTxPoW(i,0);if(!input.containsKey("badParent")){pow->setSuperParent(0,parent);pow->calculateTXPOWID();}parent=pow->getTxPoWIDData();auto block=std::make_shared<TxBlock>(pow);
                if(input.containsKey("coins")){Coin coin(MiniData::fromHex("0x"+std::string(i%2?"01":"02")),MiniData::fromHex("0x01"),MiniNumber(i),MiniData::fromHex("0x00"));coin.setState({StateVariable(0,"0xAABB")});block->getOutputCoins().push_back(coin);block->getInputCoinProofs().push_back(CoinProof(coin,MMRProof()));}
                db->getArchive()->saveBlock(block.get());wires.add(MiniData(block->serialise()).to0xString());
            }
            db->getArchive()->executeGenericSQL("UPDATE syncblock SET timemilli=block*1000");out.put("blocks",wires);db->saveAllDB();out.put("seeded",true);return out;
        }
        if(input.containsKey("treeBlock")){auto bytes=MiniData::fromHex(input.getString("treeBlock"));size_t offset=0;auto block=std::make_shared<TxBlock>(TxBlock::deserialise(bytes.bytes(),offset));auto node=std::make_shared<TxPoWTreeNode>(std::make_shared<TxBlock>(block->getSharedTxPoW()),false);node->getTxBlock().getOutputCoins()=block->getOutputCoins();node->getTxBlock().getInputCoinProofs()=block->getInputCoinProofs();db->getTxPoWTree()->setRoot(node);}
        commands::archive command;
        if(op=="meta"){out.put("name",command.getName().value());out.put("help",command.getHelp().value());out.put("fullHelp",command.getFullHelp().value());JSONArray params;for(const auto& key:command.getValidParams())params.add(key);out.put("params",params);command.getParams()->put("file","fixture");out.put("fresh",*command.getFunction()->getParams());}
        else if(op=="notify"){std::shared_ptr<MessageListener> listener;if(!input.containsKey("nullListener"))listener=std::make_shared<ArchiveListener>();auto text=input.get("message");commands::archive::NotifyListener(listener,text.isNull()?std::nullopt:std::optional(text.asString()));out.put("notified",true);}
        else if(op=="runner")out.put("result",CommandRunner::getRunner().runMultiCommand(input.getString("caller","0x00"),input.getString("command")));
        else{*command.getParams()=*std::get<std::shared_ptr<JSONObject>>(input.get("params").value);out.put("result",command.runCommand());}
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    out.put("events",events);out.put("static",commands::ArchiveCommandFixture::state());out.put("archiveSize",db->getArchive()->getSize());out.put("shutdown",main->isShuttingDown());out.put("restoring",main->isRestoring());db->saveSQL(false);return out;
}
int main(){NativeH2Runtime engine;std::string line;while(std::getline(std::cin,line)){JSONObject out;try{out=run(*std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value),engine);}catch(const std::exception& error){out.put("fixtureError",nativeExceptionText(error));}std::cout<<out.toJSONString()<<std::endl;}commands::ArchiveCommandFixture::clear();}
