#include "org/minima/utils/mysql/ArchiveServer.hpp"
#include "support/NativeThread.hpp"
#ifdef MINIMA_NATIVE_H2
#include "support/NativeH2Connection.hpp"
#endif
int main(int argc,char** argv){
    minima::ArchiveServer::Args args;for(int i=1;i<argc;++i)args.emplace_back(minima::unicode::decodeUTF8(argv[i]));
    int status=1;minima::runNativeThread(minima::NativeThread{u"main"},[&]{
        minima::ArchiveServer::Runtime runtime;
#ifdef MINIMA_NATIVE_H2
        auto database=std::make_shared<minima::NativeH2Runtime>();
        runtime.mysql.connect=database->nullableConnector();runtime.loadDriver=[database]{database->loadMySQLDriver();};
#endif
        status=minima::ArchiveServer::main(args,std::move(runtime));
    });return status;
}
