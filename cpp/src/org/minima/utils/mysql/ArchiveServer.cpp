#include "ArchiveServer.hpp"
#include "org/minima/objects/IBD.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
#include "support/NIOConnections.hpp"
#include "support/JavaInteger.hpp"
#include "support/JavaStringTrim.hpp"
#include "support/JavaIOExceptionText.hpp"
#include "support/JavaSocketExceptions.hpp"
#include "support/NativeThrowable.hpp"
#include <bit>
#include <cstdlib>
#include <iostream>
namespace minima {
ArchiveServer::Runtime ArchiveServer::configured(Runtime runtime){
    if(!runtime.sleep)runtime.sleep=[](int64_t delay){std::this_thread::sleep_for(std::chrono::milliseconds(delay));};
    if(!runtime.loadDriver)runtime.loadDriver=[]{throw SqlRuntimeException("java.lang.ClassNotFoundException","com.mysql.cj.jdbc.Driver");};
    if(!runtime.environment)runtime.environment=[](const std::string& name)->Text{auto value=std::getenv(name.c_str());return value?Text(value):std::nullopt;};
    if(!runtime.openInput)runtime.openInput=openStandardConsoleInput;
    if(!runtime.println)runtime.println=[](const std::string& text){std::cout<<text<<std::endl;};
    if(!runtime.printStackTrace)runtime.printStackTrace=[](const std::string& text){std::cerr<<text<<std::endl;};
    return runtime;
}
ArchiveServer::ArchiveServer(int32_t port,Runtime runtime):HTTPServer(port),mRuntime(configured(std::move(runtime))){}
std::shared_ptr<ArchiveServer> ArchiveServer::create(int32_t port,Text host,Text database,Text user,Text password,Runtime runtime){
    auto server=std::shared_ptr<ArchiveServer>(new ArchiveServer(port,std::move(runtime)));
    server->mMySQL=std::make_shared<MySQLConnect>(std::move(host),std::move(database),std::move(user),std::move(password),false,server->mRuntime.mysql);
    server->mMySQL->init();server->mCascade=server->mMySQL->loadCascade();
    if(!server->mCascade)MinimaLogger::log("No Cascade found..");
    else MinimaLogger::log("Cascade found.. tip:"+sqlRequired(server->mCascade->getTip()).getTxPoW().getBlockNumber().toString());
    server->start();return server;
}
void ArchiveServer::SystemClean(){
    std::lock_guard lock(mCleanMonitor);auto runtime=MinimaLogger::getRuntime();int64_t now=runtime.currentTimeMillis();
    if(std::bit_cast<int64_t>(uint64_t(now)-uint64_t(mLastClean))>10000){
        MinimaLogger::log("System Clean..");if(runtime.collectMemory)runtime.collectMemory();mLastClean=now;
    }
}
std::shared_ptr<ArchiveServer::Blocks> ArchiveServer::ArchiveHandler::reconnectLoadTxBlocks(const MiniNumber* firstBlock){
    std::lock_guard lock(mMonitor);
    try{return std::make_shared<Blocks>(sqlRequired(mServer->mMySQL).loadBlockRange(firstBlock));}
    catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
    catch(const std::exception& error){
        MinimaLogger::log("Connection failed.. reconnecting.. : "+nativeExceptionText(error));
        try{mServer->mRuntime.sleep(1000);}catch(const JavaTypedException& interrupted){if(interrupted.className()!="java.lang.InterruptedException")throw;}
        return std::make_shared<Blocks>(sqlRequired(mServer->mMySQL).loadBlockRange(firstBlock));
    }
}
void ArchiveServer::ArchiveHandler::run(){
    try{
        if(!mSocket)throw JavaNullPointerException("null archive socket");
        // Java obtains both socket streams before entering its quiet framing catch.
        if(mSocket->descriptor()<0)throw wire::SocketException("Socket is closed");
        MiniData data;
        try{data=MiniData::readFromStream([&](std::span<uint8_t> bytes){auto count=nioReadSome(mSocket,bytes);return count<0?size_t(0):size_t(count);});}
        catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){return;}
        size_t offset=0;(void)MiniByte::deserialise(data.bytes(),offset);
        MiniNumber firstBlock;
        try{firstBlock=MiniNumber::deserialise(data.bytes(),offset);}
        catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){return;}
        MinimaLogger::log("Received request first block : "+firstBlock.toString(),false);IBD ibd;
        if(firstBlock.isEqual(MiniNumber::MINUSONE)){(void)reconnectLoadTxBlocks(&MiniNumber::ZERO);}
        else{
            if(firstBlock.isEqual(MiniNumber::ZERO)&&mServer->mCascade){MinimaLogger::log("Adding cascade..");ibd.setCascade(mServer->mCascade);}
            auto rows=reconnectLoadTxBlocks(&firstBlock);
            if(rows){auto target=ibd.getTxBlocks();target->insert(target->end(),rows->begin(),rows->end());}
            else{mSocket->close();return;}
        }
        auto response=NIOManager::createNIOMessage(NIOMessage::MSG_ARCHIVE_DATA,ibd);
        nioWriteAll(mSocket,response.serialise()); // SocketOutputStream is unbuffered.
        mServer->mRuntime.sleep(10000);
        mSocket->close(); // ByteArrayInputStream.close is a no-op; either socket stream closes its socket.
    }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
    catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    SystemClean();
}
std::function<void()> ArchiveServer::getSocketHandler(const std::shared_ptr<PosixNIOSocket>& socket){
    auto handler=std::make_shared<ArchiveHandler>(std::static_pointer_cast<ArchiveServer>(shared_from_this()),socket);
    return [handler]{handler->run();};
}
int ArchiveServer::main(const Args* args,Runtime runtime){
    runtime=configured(std::move(runtime));MinimaLogger::log("Starting Archive Server v1.3");
    try{runtime.loadDriver();}catch(const SqlRuntimeException& error){if(error.className()!="java.lang.ClassNotFoundException")throw;runtime.printStackTrace(nativeExceptionText(error));return 0;}
    Text host=runtime.environment("MYSQL_HOST"),database=runtime.environment("MYSQL_DB"),user=runtime.environment("MYSQL_USER"),password=runtime.environment("MYSQL_PASSWORD");
    int32_t port=8888;auto envPort=runtime.environment("ARCHIVE_PORT");if(envPort)port=javaParseInt(*envPort);
    if(!args)throw JavaNullPointerException("null archive arguments");
    size_t index=0;
    auto next=[&]()->Text{if(index>=args->size())throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(index)+" out of bounds for length "+std::to_string(args->size()));auto value=(*args)[index++];return value?Text(unicode::encodeUTF8(*value)):std::nullopt;};
    while(index<args->size()){
        auto arg=next();if(!arg)throw JavaNullPointerException("null archive argument");
        if(*arg=="-mysqlhost")host=next();else if(*arg=="-mysqldb")database=next();else if(*arg=="-mysqluser")user=next();else if(*arg=="-mysqlpassword")password=next();
        else if(*arg=="-port"){auto value=next();if(!value)throw JavaNumberFormatException("Cannot parse null string");port=javaParseInt(*value);}
        else if(*arg=="-help"){
            for(const auto* line:{"Minima Archive Server v0.8 Help"," -mysqlhost      : The MySQL Host server"," -mysqldb        : The MySQL Database"," -mysqluser      : The MySQL User"," -mysqlpassword  : The MySQL password"," -help           : Print this help"})runtime.println(line);
            return 1;
        }else{runtime.println("Unknown parameter : "+*arg);return 1;}
    }
    auto server=create(port,std::move(host),std::move(database),std::move(user),std::move(password),runtime);auto input=runtime.openInput();
    for(;;){
        try{auto line=sqlRequired(input).readLine();if(line&&!line->empty()){auto text=javaStringTrim(*line);if(text==u"quit")break;MinimaLogger::log("Unknown command : "+unicode::encodeUTF8(text),false);}}
        catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    }
    try{input->closeBufferedReader();input->closeInputStreamReader();}catch(const wire::IOException& error){MinimaLogger::log(javaIOExceptionText(error),false);}
    server->shutdown();MinimaLogger::log("Archive Server stopped",false);server->join();return 0;
}
}
