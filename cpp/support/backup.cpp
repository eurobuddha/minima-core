#include "MainFixture.hpp"
#include "ArchiveFixture.hpp"
#include "org/minima/system/commands/backup/backup.hpp"
#include "org/minima/system/commands/backup/decryptbackup.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeH2Connection.hpp"
#include "support/NativeThrowable.hpp"
#include "support/GzipOutput.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
class ManualBackupMain:public Main {public:explicit ManualBackupMain(MainServices services):Main(std::move(services)){} };
JSONObject run(const JSONObject& request,NativeH2Runtime& engine){
    JSONObject out;auto op=request.getString("op","");GeneralParams::resetDefaults();
    GeneralParams::DATA_FOLDER=request.getString("folder");GeneralParams::BASE_FILE_FOLDER=request.getString("folder");GeneralParams::USE_SQL_COINDB=false;
    if(op=="stream"){
        JSONArray calls;wire::Bytes bytes;int64_t fail=std::get<int64_t>(request.get("fail").value);
        try{GzipOutput gzip([&](auto part){calls.add(int64_t(part.size()));if(int64_t(calls.size())==fail)throw wire::IOException("fixture write failure");wire::append(bytes,part);});
            for(const auto& part:std::get<std::shared_ptr<JSONArray>>(request.get("parts").value)->elements())gzip.write(MiniData::fromHex(part.asString()).bytes());
            gzip.finish();gzip.finish();
            if(request.getBoolean("after"))gzip.write({});
        }catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
        out.put("calls",calls);out.put("data",MiniData(bytes).to0xString());return out;
    }
    if(op=="decode"){
        commands::decryptbackup command;command.getParams()->put("file",request.getString("file"));command.getParams()->put("output",request.getString("output"));command.getParams()->put("password",request.getString("password"));
        command.runCommand();out.put("data",MiniData(MiniFile::readCompleteFile(request.getString("output"))).to0xString());return out;
    }
    MainServices services;services.sql.connect=engine.connector();services.sql.log=[](const auto& text,bool){std::clog<<text<<'\n';};services.halt=[](int){throw std::runtime_error("fixture startup failed");};
    auto main=std::make_shared<ManualBackupMain>(std::move(services));MainFixture::install(main);MainFixture::initialize(main);
    auto db=MinimaDB::getDB();db->loadAllDB();JSONObject flags;flags.put("keys",!request.containsKey("keys")||request.get("keys").asBool());MainFixture::fields(*main,flags);
    try{
        if(op=="seed"){
            auto sql=db->getTxPoWDB()->getSQLDB();
            for(int i=0;i<10;++i)if(!sql->addTxPoW(support::archiveFixtureTxPoW(i,0).get(),i%3!=0))throw std::runtime_error("fixture insert failed");
            auto result=sql->executeGenericSQL("UPDATE txpow SET timemilli=id*1000");out.put("sql",result);db->saveAllDB(false);out.put("seeded",true);return out;
        }
        if(request.containsKey("tip"))db->getTxPoWTree()->setRoot(std::make_shared<TxPoWTreeNode>(std::make_shared<TxBlock>(support::archiveFixtureTxPoW(12,0)),false));
        if(request.containsKey("payload"))db->getUserDB()->getAllData()->put("fixture_payload",request.get("payload"));
        if(request.containsKey("autoInitial"))db->getUserDB()->setAutoBackup(request.get("autoInitial").asBool());
        commands::backup command;
        if(op=="meta"){
            out.put("name",*command.getName());out.put("help",*command.getHelp());out.put("fullHelp",*command.getFullHelp());JSONArray params;for(const auto& name:command.getValidParams())params.add(name);out.put("params",params);
            command.getParams()->put("file","fixture");out.put("freshParams",*command.getFunction()->getParams());JSONArray dates;for(int64_t time:{int64_t(0),int64_t(-1),int64_t(1700000000000)})dates.add(commands::backup::DATEFORMAT.format(time));out.put("dates",dates);
        }else if(op=="runner")out.put("result",CommandRunner::getRunner().runMultiCommand(request.getString("caller","0x00"),request.getString("command")));
        else{
            for(const auto& [key,value]:std::get<std::shared_ptr<JSONObject>>(request.get("params").value)->entries())command.getParams()->put(key,value);
            out.put("result",command.runCommand());
        }
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}
    catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    out.put("auto",db->getUserDB()->isAutoBackup());auto lock=db->getRWLockInfo();out.put("lock",lock.substr(lock.find('[')));db->saveSQL(false);return out;
}
int main(){NativeH2Runtime engine;std::string line;while(std::getline(std::cin,line)){
    JSONObject out;try{out=run(*std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value),engine);}catch(const std::exception& error){out.put("fixtureError",nativeExceptionText(error));}
    std::cout<<out.toJSONString()<<std::endl;
}}
