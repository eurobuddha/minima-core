#include "SqlRecordingFixture.hpp"
#include "MainFixture.hpp"
#include "org/minima/system/commands/backup/vault.hpp"
#include "ArchiveFixture.hpp"
#include "CoinFixture.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/commands/backup/mysql.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/mysql/MySQLConnect.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;using namespace minima::sql_fixture;
class CommandMain:public Main{public:CommandMain():Main(MainServices{}){}};
static MinimaDBServices services(){
    return {{[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user,password});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();},[]{return false;},[](const auto& text,bool){logs.add(text);}},
        []{return false;},[](auto){},[](auto){},[](const auto&,const auto&){},[](bool,const auto&){},[](int){throw std::runtime_error("fixture halt");},[]{return "fixture";}};
}
static JSONObject run(const JSONObject& q){
    config=&object(q.get("sql"));events=JSONArray();logs=JSONArray();calls=connectionID=preparedID=0;queryBatches.clear();
    auto main=std::make_shared<CommandMain>();MainFixture::install(main);
    commands::vault::getMain=[main]{return std::static_pointer_cast<commands::vault::MainReceiver>(main);};
    GeneralParams::DATA_FOLDER=q.getString("folder");GeneralParams::BASE_FILE_FOLDER=q.getString("folder");GeneralParams::MYSQL_STORE_ALLTXPOW=q.getBoolean("storeAll");
    GeneralParams::IS_MOBILE=q.getBoolean("mobile");MinimaDB::createDB(services());auto prefs=MinimaDB::getDB()->getUserDB();*prefs->getAllData()=object(q.get("prefs"));
    auto requested=config;JSONObject setup;setup.put("rows",JSONArray());setup.put("columns",JSONArray());config=&setup;
    MinimaDB::getDB()->getArchive()->loadDB(std::filesystem::path(q.getString("folder"))/"archive");config=requested;events=JSONArray();calls=0;queryBatches.clear();
    MySQLConnect::runtimeServices={[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user?JSONValue(*user):JSONValue(),password?JSONValue(*password):JSONValue()});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();},[](const auto& message){logs.add(message);},TimerMessage::currentTimeMillis};
    MinimaLogger::Runtime logger;logger.println=[](const std::u16string& text){auto line=unicode::encodeUTF8(text);auto at=line.find("] : ");if(at!=std::string::npos&&!line.substr(at+4).starts_with("     "))logs.add(line.substr(at+4));};MinimaLogger::setRuntime(logger);
    JSONObject out;JSONArray blocks;
    for(int i=0;i<2;++i){TxBlock block(support::archiveFixtureTxPoW(i+10,0));block.getInputCoinProofs().emplace_back(support::coinFixture(i),MMRProof());block.getOutputCoins().push_back(support::coinFixture(i+1));block.getOutputCoins().push_back(support::coinFixture(i+2));blocks.add(MiniData(block.serialise()).to0xString());}
    out.put("blocks",blocks);
    if(q.getString("op")=="bootstrap"){
        JSONObject chains,pows,raws;
        for(const auto* name:{"normal","gap","bad","zero"}){
            JSONArray wires;wire::Bytes raw=MiniByte::FALSE.serialise();wire::append(raw,MiniNumber(3).serialise());MiniData parent=MiniData::ZERO_TXPOWID;
            for(int i=0;i<3;++i){int number=std::string(name)=="zero"?i:i+1;if(std::string(name)=="gap"&&i>0)++number;auto pow=support::archiveFixtureTxPoW(number,0);if(std::string(name)!="bad"){pow->setSuperParent(0,parent);pow->calculateTXPOWID();}parent=pow->getTxPoWIDData();TxBlock block(pow);Coin coin(MiniData::fromHex("0x01"),MiniData::fromHex("0x01"),MiniNumber(number),MiniData::fromHex("0x00"));coin.setState({StateVariable(0,"0xAABB")});block.getOutputCoins().push_back(coin);block.getInputCoinProofs().emplace_back(coin,MMRProof());auto data=block.serialise();wire::append(raw,data);wires.add(MiniData(data).to0xString());if(i==0)pows.put(name,MiniData(pow->serialise()).to0xString());}
            chains.put(name,wires);raws.put(name,MiniData(raw).to0xString());
        }out.put("chains",chains);out.put("pows",pows);out.put("raws",raws);
    }
    commands::mysql command;*command.getParams()=object(q.get("params"));out.put("before",TimerMessage::currentTimeMillis());
    try{JSONValue value;auto op=q.getString("op");
        if(op=="meta"){JSONObject meta;meta.put("name",command.getName().value());meta.put("help",command.getHelp().value());meta.put("fullhelp",command.getFullHelp().value());JSONArray valid;for(auto name:command.getValidParams())valid.add(name);meta.put("valid",valid);meta.put("fresh",command.getFunction()->getJSONReply());value=meta;}
        else if(op=="runner")value=CommandRunner::getRunner().runMultiCommand(q.getString("caller"),q.getString("text"));
        else if(op=="configure"){GeneralParams::MYSQL_DB_DELAY=integer(q,"delay");GeneralParams::MYSQL_DB_COINS=q.getBoolean("coins");commands::mysql::convertMySQLParams(string(q.get("text")));}
        else if(op=="main"){std::ostringstream stdoutBuffer,stderrBuffer;auto previousOut=std::cout.rdbuf(stdoutBuffer.rdbuf());auto previousError=std::cerr.rdbuf(stderrBuffer.rdbuf());try{commands::mysql::main({});}catch(...){std::cout.rdbuf(previousOut);std::cerr.rdbuf(previousError);throw;}std::cout.rdbuf(previousOut);std::cerr.rdbuf(previousError);JSONObject output;output.put("stdout",stdoutBuffer.str());output.put("stderr",stderrBuffer.str());value=output;}
        else if(op!="bootstrap")value=command.runCommand();
        out.put("value",value);
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}
    catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    out.put("importing",main->MYSQL_IMPORTING_NO_ACTION.load());out.put("storeAll",GeneralParams::MYSQL_STORE_ALLTXPOW);out.put("after",TimerMessage::currentTimeMillis());out.put("events",events);out.put("logs",logs);out.put("prefs",*prefs->getAllData());return out;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
