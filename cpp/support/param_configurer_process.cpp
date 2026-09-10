#include "org/minima/system/params/ParamConfigurer.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/system/params/TestParams.hpp"
#include <iostream>
int main(int argc,char** argv) {
    try {if(argc>1&&std::string(argv[1])=="__test_version"){minima::GlobalParams::MINIMA_VERSION="before";std::cout<<minima::TestParams::values().MINIMA_VERSION<<std::endl;minima::GlobalParams::MINIMA_VERSION="after";minima::TestParams::setTestParams();std::cout<<minima::GlobalParams::MINIMA_VERSION<<std::endl;return 0;}minima::ParamConfigurer::Args args;for(int i=1;i<argc;++i)args.emplace_back(minima::unicode::decodeUTF8(argv[i]));minima::ParamConfigurer().usingConfFile(args).usingEnvVariables(minima::ParamConfigurer::Environment{}).usingProgramArgs(args).configure();return 0;}
    catch(const std::exception& error){minima::NativeThrowable(error).printStackTrace(std::cerr);return 1;}
}
