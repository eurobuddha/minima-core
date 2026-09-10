#include "org/minima/system/params/ParamConfigurer.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/params/TestParams.hpp"
#include <fstream>
#include <iostream>
using namespace minima;
static void check(bool value){if(!value)throw std::runtime_error("ParamConfigurer assertion failed");}
static ParamConfigurer::Args args(std::initializer_list<const char*> text){ParamConfigurer::Args result;for(auto value:text)result.emplace_back(unicode::decodeUTF8(value));return result;}
int main(int argc,char** argv) {
    try {
        // Five original test methods, with fresh GeneralParams state per method.
        GeneralParams::resetDefaults();
        {auto values=args({});ParamConfigurer config;config.usingConfFile(values).usingEnvVariables(ParamConfigurer::Environment{}).usingProgramArgs(values).configure();check(GeneralParams::MINIMA_PORT==9001);check(!config.isDaemon());}
        GeneralParams::resetDefaults();
        {auto values=args({"-port","8888","-rpcenable","-daemon","-host","124.0.1.10","-p2prootnode","124.0.1.9","-genesis","-nop2p","-noconnect","-mobile","-test","-connect","124.0.1.9:7777"});ParamConfigurer config;config.usingConfFile(values).usingEnvVariables(ParamConfigurer::Environment{}).usingProgramArgs(values).configure();check(GeneralParams::MINIMA_PORT==8888);check(GeneralParams::MINIMA_HOST=="124.0.1.10");check(GeneralParams::IS_HOST_SET);check(GeneralParams::NOCONNECT);check(GeneralParams::IS_MOBILE);check(!GeneralParams::P2P_ENABLED);check(GeneralParams::P2P_ROOTNODE=="124.0.1.9");check(GeneralParams::CONNECT_LIST=="124.0.1.9:7777");check(GeneralParams::RPC_ENABLED);check(config.isDaemon());check(GeneralParams::CLEAN);check(GeneralParams::GENESIS);check(GeneralParams::TEST_PARAMS);check(TestParams::values().MINIMA_BLOCK_SPEED==GlobalParams::MINIMA_BLOCK_SPEED);}
        GeneralParams::resetDefaults();
        {bool caught=false;auto values=args({"-port","8888","-rpcenable","-notvalid","-connect","124.0.1.9:7777"});try{ParamConfigurer().usingConfFile(values).usingEnvVariables(ParamConfigurer::Environment{}).usingProgramArgs(values).configure();}catch(const ParamConfigurer::UnknownArgumentException&){caught=true;}check(caught);}
        GeneralParams::resetDefaults();
        ParamConfigurer::Environment env{{u"MINIMA_CONNECT",u"124.0.1.9:7777"},{u"MINIMA_RPCENABLE",u"true"},{u"MINIMA_DAEMON",u"true"},{u"MINIMA_NOCONNECT",u"something"},{u"OTHER_PROG_NOCONNECT",u"something"}};
        {auto values=args({"-port","8888"});ParamConfigurer config;config.usingConfFile(values).usingEnvVariables(env).usingProgramArgs(values).configure();check(GeneralParams::MINIMA_PORT==8888);check(!GeneralParams::NOCONNECT);check(GeneralParams::CONNECT_LIST=="124.0.1.9:7777");check(GeneralParams::RPC_ENABLED);check(config.isDaemon());}
        GeneralParams::resetDefaults();
        {if(argc!=2)throw std::runtime_error("Expected fixture conf path");std::ofstream file(argv[1]);file<<"port=999\nhost=19.0.1.6";file.close();auto values=args({"-port","8888","-conf",argv[1]});ParamConfigurer config;config.usingConfFile(values).usingEnvVariables(ParamConfigurer::Environment{{u"MINIMA_CONNECT",u"124.0.1.9:7777"}}).usingProgramArgs(values).configure();check(GeneralParams::MINIMA_PORT==8888);check(GeneralParams::MINIMA_HOST=="19.0.1.6");}
        std::cout<<"Five mapped ParamConfigurerTest methods passed\n";return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
