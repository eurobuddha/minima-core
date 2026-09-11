#include "org/minima/database/archive/ArchiveManager.hpp"
#include "org/minima/database/txpowdb/onchain/TxPoWOnChainDB.hpp"
#include "org/minima/database/txpowdb/sql/TxPoWSqlDB.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/NativeH2Connection.hpp"
#include "support/NativeThrowable.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
namespace {
json::JSONArray read(const std::string& name,const std::filesystem::path& home,NativeH2Runtime& engine) {
    const bool archive=name=="ArchiveManager",onchain=name=="TxPoWOnChainDB";
    const auto file=std::filesystem::absolute(home/(archive?"testfolder/sqlsync":"testsql"));
    auto connection=engine.connector()("jdbc:h2:"+file.string()+";MODE=MySQL;DB_CLOSE_ON_EXIT=FALSE","SA","");
    auto statement=connection->createStatement();auto rows=statement->executeQuery(std::string("SELECT * FROM ")+(archive?"syncblock":onchain?"onchaintxpow":"txpow")+" ORDER BY id");
    json::JSONArray output;
    while(rows->next()) {
        json::JSONObject row;row.put("id",rows->getLong("id"));row.put("txpowid",*rows->getString("txpowid"));row.put("timemilli",rows->getLong("timemilli"));
        if(archive||onchain)row.put("block",rows->getLong("block"));
        if(onchain)row.put("blockid",*rows->getString("blockid"));
        else {
            auto bytes=*rows->getBytes(archive?"syncdata":"txpowdata");MiniData data(bytes);row.put("wire",data.to0xString());std::shared_ptr<TxPoW> txpow;
            if(archive){auto block=TxBlock::convertMiniDataVersion(data);if(!block)throw std::runtime_error("invalid persisted TxBlock");txpow=block->getSharedTxPoW();row.put("wireagain",MiniData(block->serialise()).to0xString());}
            else {auto decoded=TxPoW::convertMiniDataVersion(data);if(!decoded)throw std::runtime_error("invalid persisted TxPoW");txpow=std::make_shared<TxPoW>(std::move(*decoded));row.put("wireagain",MiniData(txpow->serialise()).to0xString());for(auto col:{"isblock","istransaction","isrelevant"})row.put(col,rows->getInt(col));row.put("parentid",*rows->getString("parentid"));}
            row.put("txpow",txpow->toJSON());
            if(archive){txpow->setSuperParent(0,MiniData());txpow->calculateTXPOWID();row.put("before_parent_id",txpow->getTxPoWID());}
        }
        output.add(row);
    }
    statement->close();connection->close();return output;
}
}
int main(int argc,char** argv) {
    if(argc!=5)return 2;
    try {
        NativeH2Runtime engine;std::string action=argv[1],name=argv[2],mode=argv[3];std::filesystem::path home=argv[4];
        if(action=="read"){auto rows=read(name,home,engine);if(engine.liveHandles()!=0)throw std::runtime_error("read leaked H2 handles");std::cout<<rows.toString()<<'\n';return 0;}
        std::vector<std::string> args;if(mode=="args")args={"unused","value"};auto input=mode=="null"?nullptr:&args;
        std::ostringstream output;auto previous=std::cout.rdbuf(output.rdbuf());std::string error="none";
        MinimaLogger::Runtime logger;logger.println=[](const auto& line){auto text=unicode::encodeUTF8(line);std::cout<<text.substr(text.find(" : ")+3)<<std::endl;};MinimaLogger::setRuntime(logger);
        SqlDBServices services{engine.connector(),[]{return false;},[](const auto& text,bool notify){MinimaLogger::log(text,notify);}};
        try {
            if(name=="ArchiveManager")ArchiveManager::main(input,services,home);
            else if(name=="TxPoWOnChainDB")TxPoWOnChainDB::main(input,services,home);
            else if(name=="TxPoWSqlDB")TxPoWSqlDB::main(input,services,home);
            else throw std::runtime_error("unknown main");
            if(engine.liveHandles()!=0)throw std::runtime_error("main leaked H2 handles");
        }catch(const std::exception& e){error=nativeExceptionText(e);}
        MinimaLogger::setRuntime({});std::cout.rdbuf(previous);json::JSONObject result;result.put("stdout",output.str());result.put("error",error);std::cout<<result.toString()<<'\n';
    }catch(const std::exception& error){std::cerr<<nativeExceptionText(error)<<'\n';return 1;}
}
