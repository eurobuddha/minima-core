#include "org/minima/Minima.hpp"
#include "support/NativeThread.hpp"
#include <iostream>
int main(int argc,char** argv){
    minima::Minima::Args args;for(int index=1;index<argc;++index)args.emplace_back(minima::unicode::decodeUTF8(argv[index]));
    int status=1;minima::runNativeThread(minima::NativeThread{u"main"},[&]{status=minima::Minima::main(args);minima::Minima::joinStarterThreads();});return status;
}
