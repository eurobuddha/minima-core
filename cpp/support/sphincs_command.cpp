#include "org/minima/system/commands/base/sphincs.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
#include <sstream>
using namespace minima;using namespace minima::json;
namespace minima {struct WalletCommandsFixture {static void init(){auto db=MinimaDB::getDB();db->mTxnDB=std::make_shared<TxnDB>(db->getUserDB());}};}
static const JSONObject* config;static JSONArray calls;static std::shared_ptr<TxnDB> db;
static const JSONObject& object(const JSONValue& v){return *std::get<std::shared_ptr<JSONObject>>(v.value);}
static JSONArray rows(){JSONArray r;for(auto row:*db->listTxns()){JSONObject item;item.put("id",row->getID().value());item.put("wire",MiniData(row->getTransaction()->serialise()).to0xString());item.put("transaction",row->getTransaction()->toJSON());r.add(item);}return r;}
class Child:public Command {
    std::shared_ptr<Command> original;
public:
    explicit Child(std::shared_ptr<Command> cmd):Command(cmd->getName(),cmd->getHelp()),original(cmd){}
    std::vector<std::string> getValidParams() const override{return original->getValidParams();}
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<Child>(original->getFunction());}
    JSONValue runCommand()override{
        JSONObject call;call.put("text",getCompleteCommand().value());call.put("rows",rows());calls.add(call);
        if(config->containsKey("fail")&&getName()==config->getString("fail"))return config->get("reply");
        auto id=getParam("id");
        if(getName()=="txncreate"||getName()=="txndelete"){*original->getParams()=*getParams();return original->runCommand();}
        if(getName()=="txnaddamount"){
            auto txn=db->getTransactionRow(id)->getTransaction();int n=int(std::get<int64_t>(config->get("inputs").value));
            for(int i=0;i<n;++i)txn->addInput(Coin(MiniData(std::vector<uint8_t>{uint8_t(i+2)}),MiniData::fromHex("0x01"),MiniNumber::ONE,MiniData::fromHex("0x00"),true));
            txn->addOutput(Coin(MiniData::fromHex("0x00"),MiniData::fromHex("0x02"),MiniNumber::ONE,MiniData::fromHex("0x00"),true));
        }
        auto ret=getJSONReply();JSONObject response;response.put("child",getName().value());ret.put("response",response);return ret;
    }
};
static JSONObject run(const JSONObject& input){
    config=&input;calls=JSONArray();MinimaDBServices services;
    services.sql.connect=[](const auto&,const auto&,const auto&)->std::shared_ptr<SqlConnection>{throw JavaNullPointerException("offline SQL connection");};services.sql.log=[](const auto&,bool){};
    services.isShuttingDown=[]{return false;};services.postMainMessage=[](auto){};services.postMainTimer=[](auto){};services.postNotifyEvent=[](const auto&,const auto&){};services.setStartupError=[](bool,const auto&){};services.halt=[](int){throw std::runtime_error("unexpected halt");};services.currentThreadName=[]{return "main";};
    MinimaDB::createDB(services);WalletCommandsFixture::init();db=MinimaDB::getDB()->getCustomTxnDB();GeneralParams::BASE_FILE_FOLDER=input.getString("base");
    if(input.containsKey("write")){auto file=MiniFile::createBaseFile(input.getString("path"));MiniFile::writeDataToFile(file,MiniData::fromHex(input.getString("write")).bytes());}
    auto saved=CommandRunner::ALL_COMMANDS();struct Restore{decltype(saved) value;~Restore(){CommandRunner::ALL_COMMANDS()=std::move(value);}}restore{saved};JSONObject out;
    try{
        if(input.containsKey("inputs"))for(auto& cmd:CommandRunner::ALL_COMMANDS())if(cmd->getName()=="txncreate"||cmd->getName()=="txnaddamount"||cmd->getName()=="txnscript"||cmd->getName()=="txnmmr"||cmd->getName()=="txnpost"||cmd->getName()=="txndelete")cmd=std::make_shared<Child>(cmd);
        commands::sphincs cmd;*cmd.getParams()=object(input.get("params"));
        if(input.containsKey("meta")&&input.get("meta").asBool()){JSONObject m;m.put("help",cmd.getHelp().value());m.put("fullhelp",cmd.getFullHelp().value());JSONArray params;for(auto p:cmd.getValidParams())params.add(p);m.put("params",params);m.put("fresh",cmd.getFunction()->getJSONReply());out.put("value",m);}
        else if(input.containsKey("runner"))out.put("value",CommandRunner::getRunner().runMultiCommand(input.getString("runner")));
        else out.put("value",cmd.runCommand());
    }catch(const CommandException& e){out.put("exception",std::string("org.minima.system.commands.CommandException")+(e.getMessage()?": "+*e.getMessage():""));}
    catch(const std::exception& e){out.put("exception",nativeExceptionText(e));}
    out.put("calls",calls);out.put("rows",rows());return out;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line)){
    auto q=object(JSONValue::parseWithException(line));std::ostringstream capture;auto old=std::cout.rdbuf(capture.rdbuf());JSONObject out;
    try{out=run(q);}catch(const std::exception& e){std::cout.rdbuf(old);std::cerr<<e.what()<<'\n';return 1;}std::cout.rdbuf(old);
    JSONArray logs;std::istringstream stream(capture.str());std::string text;while(std::getline(stream,text)){if(text.empty())continue;auto pos=text.find("] : ");if(pos!=text.npos)text=text.substr(pos+4);if(!text.starts_with("     "))logs.add(text);}out.put("logs",logs);std::cout<<out.toJSONString()<<'\n';
}}
