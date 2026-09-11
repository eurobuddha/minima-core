#include "SqlRecordingFixture.hpp"
#include "ArchiveFixture.hpp"
#include "CoinFixture.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/commands/backup/mysqlcoins.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/mysql/MySQLConnect.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;using namespace minima::sql_fixture;
static MinimaDBServices services(){
    return {{[](const auto&,const auto&,const auto&)->std::shared_ptr<SqlConnection>{return std::make_shared<Connection>();},[]{return false;},[](const auto&,bool){}},
        []{return false;},[](auto){},[](auto){},[](const auto&,const auto&){},[](bool,const auto&){},[](int){throw std::runtime_error("fixture halt");},[]{return "fixture";}};
}
static JSONObject run(const JSONObject& q){
    config=&object(q.get("sql"));events=JSONArray();logs=JSONArray();calls=connectionID=preparedID=0;
    GeneralParams::IS_MOBILE=q.getBoolean("mobile");MinimaDB::createDB(services());auto prefs=MinimaDB::getDB()->getUserDB();*prefs->getAllData()=object(q.get("prefs"));
    MySQLConnect::runtimeServices={[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user?JSONValue(*user):JSONValue(),password?JSONValue(*password):JSONValue()});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();},[](const auto& message){logs.add(message);},TimerMessage::currentTimeMillis};
    MinimaLogger::Runtime logger;logger.println=[](const std::u16string& text){auto line=unicode::encodeUTF8(text);auto at=line.find("] : ");if(at!=std::string::npos)logs.add(line.substr(at+4));};MinimaLogger::setRuntime(logger);
    JSONObject out;JSONArray blocks;
    for(int i=0;i<2;++i){TxBlock block(support::archiveFixtureTxPoW(i+10,0));block.getInputCoinProofs().emplace_back(support::coinFixture(i),MMRProof());block.getOutputCoins().push_back(support::coinFixture(i+1));block.getOutputCoins().push_back(support::coinFixture(i+2));blocks.add(MiniData(block.serialise()).to0xString());}
    out.put("blocks",blocks);commands::mysqlcoins command;*command.getParams()=object(q.get("params"));out.put("before",TimerMessage::currentTimeMillis());
    try{JSONValue value;auto op=q.getString("op");
        if(op=="meta"){JSONObject meta;meta.put("name",command.getName().value());meta.put("help",command.getHelp().value());meta.put("fullhelp",command.getFullHelp().value());JSONArray valid;for(auto name:command.getValidParams())valid.add(name);meta.put("valid",valid);meta.put("fresh",command.getFunction()->getJSONReply());value=meta;}
        else if(op=="runner")value=CommandRunner::getRunner().runMultiCommand(q.getString("caller"),q.getString("text"));
        else if(op!="bootstrap")value=command.runCommand();
        out.put("value",value);
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}
    catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    out.put("after",TimerMessage::currentTimeMillis());out.put("events",events);out.put("logs",logs);out.put("prefs",*prefs->getAllData());return out;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
