#include "org/minima/Minima.hpp"
#include "support/NativeThread.hpp"
#ifdef MINIMA_NATIVE_H2
#include "support/NativeH2Connection.hpp"
#endif
#include <iostream>
int main(int argc,char** argv){
    minima::Minima::Args args;for(int index=1;index<argc;++index)args.emplace_back(minima::unicode::decodeUTF8(argv[index]));
    int status=1;minima::runNativeThread(minima::NativeThread{u"main"},[&]{
        minima::Minima::Runtime runtime;
#ifdef MINIMA_NATIVE_H2
        runtime.services.sql.connect=minima::NativeH2Runtime().connector();
#endif
        status=minima::Minima::main(args,std::move(runtime));minima::Minima::joinStarterThreads();
    });return status;
}
