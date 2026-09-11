#include "SqlRecordingFixture.hpp"
#include "support/NativeThread.hpp"
#include "org/minima/utils/mysql/ArchiveServer.hpp"
#include "ArchiveFixture.hpp"
#include "support/NIOConnections.hpp"
#include "support/NativeThrowable.hpp"
#include <sys/socket.h>
#include <chrono>
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima {
class ArchiveServerFixture {
public:
    static auto create(){return std::shared_ptr<ArchiveServer>(new ArchiveServer(0,ArchiveServer::Runtime{}));}
    static void cleanTime(int64_t value){ArchiveServer::mLastClean=value;}
    static int64_t cleanTime(){return ArchiveServer::mLastClean;}
};
}
namespace {
using namespace minima::sql_fixture;
class NullRange:public ArchiveServer::ArchiveHandler {public:using ArchiveHandler::ArchiveHandler;std::shared_ptr<ArchiveServer::Blocks> reconnectLoadTxBlocks(const MiniNumber*)override{return nullptr;}};
class CleanMain:public MinimaLogger::MainReceiver {bool nested=false;public:void PostNotifyEvent(const std::string&,std::shared_ptr<JSONObject>)override{if(!nested){nested=true;ArchiveServer::SystemClean();}}};
JSONObject run(const JSONObject& request){
    connectionID=preparedID=calls=0;events=JSONArray();JSONObject empty;empty.put("rows",JSONArray());empty.put("columns",JSONArray());config=&empty;
    MySQLConnectServices mysql;mysql.connect=[](const auto& url,const auto& user,const auto& password){event("connect",{url,user?JSONValue(*user):JSONValue(),password?JSONValue(*password):JSONValue()});return flag("nullConnection")?nullptr:std::make_shared<Connection>();};MySQLConnect::runtimeServices=mysql;
    JSONArray captured;auto logger=MinimaLogger::getRuntime();MinimaLogger::Runtime runtime=logger;runtime.println=[&](const std::u16string& full){auto text=unicode::encodeUTF8(full);auto at=text.find("] : ");if(at!=text.npos){auto body=text.substr(at+4);if(!body.starts_with("     "))captured.add(body);}};MinimaLogger::setRuntime(runtime);
    struct Reset{MinimaLogger::Runtime logger;~Reset(){MinimaLogger::setRuntime(std::move(logger));MySQLConnect::runtimeServices={};}}reset{logger};
    auto start=std::chrono::steady_clock::now();JSONObject out;std::shared_ptr<ArchiveServer> server;std::shared_ptr<PosixNIOSocket> client,socket;
    try{
        auto op=request.getString("op");ArchiveServerFixture::cleanTime(0);
        if(op=="clean"){auto last=integer(request,"last");ArchiveServerFixture::cleanTime(last);if(!request.get("reentrant").isNull()&&request.get("reentrant").asBool()){auto receiver=std::make_shared<CleanMain>();runtime.getMain=[receiver]{return receiver;};MinimaLogger::setRuntime(runtime);}ArchiveServer::SystemClean();out.put("changed",ArchiveServerFixture::cleanTime()!=last);}
        else if(op=="constructor"){
            config=&request;server=ArchiveServer::create(0,"fixture:3306","fixture","student","synthetic");
            auto end=std::chrono::steady_clock::now()+std::chrono::seconds(5);while(!server->getBoundPort()&&std::chrono::steady_clock::now()<end)std::this_thread::sleep_for(std::chrono::milliseconds(1));
            out.put("port",server->getPort());out.put("listening",server->getBoundPort()!=0);out.put("cascade",server->mCascade?JSONValue(MiniData(server->mCascade->serialise()).to0xString()):JSONValue());server->shutdown();server->join();
        }else{
            server=ArchiveServerFixture::create();server->mMySQL=std::make_shared<MySQLConnect>("fixture:3306","fixture","student","synthetic");server->mMySQL->init();
            if(!request.get("cascade").isNull()&&request.get("cascade").asBool()){server->mCascade=std::make_shared<Cascade>();for(int i=0;i<3;++i)server->mCascade->addToTip(*support::archiveFixtureTxPoW(i,0));}
            config=&request;events=JSONArray();calls=0;
            if(op=="retry"){
                if(flag("interrupt"))interruptCurrentNativeThread();
                ArchiveServer::ArchiveHandler handler(server,nullptr);auto first=string(request.get("first"));std::optional<MiniNumber> number=first?std::optional(MiniNumber(*first)):std::nullopt;auto rows=handler.reconnectLoadTxBlocks(number?&*number:nullptr);JSONArray blocks;for(const auto& block:*rows)blocks.add(block?JSONValue(MiniData(block->serialise()).to0xString()):JSONValue());out.put("blocks",blocks);
            }else{
                if(!flag("nullSocket")){int descriptors[2];if(::socketpair(AF_UNIX,SOCK_STREAM,0,descriptors))throw std::runtime_error("socketpair failed");client=std::make_shared<PosixNIOSocket>(descriptors[0]);socket=std::make_shared<PosixNIOSocket>(descriptors[1]);nioWriteAll(client,MiniData::fromHex(request.getString("frame")).bytes());::shutdown(client->descriptor(),SHUT_WR);if(flag("closed"))socket->close();}
                std::shared_ptr<ArchiveServer::ArchiveHandler> handler=flag("nullRange")?std::make_shared<NullRange>(server,socket):std::make_shared<ArchiveServer::ArchiveHandler>(server,socket);handler->run();out.put("closed",socket?JSONValue(socket->descriptor()<0):JSONValue());wire::Bytes bytes;
                if(client){std::array<uint8_t,4096> buffer;for(;;){auto n=client->read(buffer);if(n<=0)break;bytes.insert(bytes.end(),buffer.begin(),buffer.begin()+n);}}out.put("reply",MiniData(bytes).to0xString());
            }
        }
    }catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
    if(request.containsKey("interrupt"))out.put("threadInterrupted",nativeThreadInterrupted());
    if(socket)socket->close();if(client)client->close();out.put("logs",captured);out.put("events",events);out.put("milliseconds",int64_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count()));return out;
}
}
int main(){std::string line;while(std::getline(std::cin,line))try{auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);std::cout<<run(*request).toJSONString()<<std::endl;}catch(const std::exception& error){std::cerr<<nativeExceptionText(error)<<std::endl;return 1;}}
