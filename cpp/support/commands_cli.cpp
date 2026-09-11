#include "org/minima/system/commands/CommandRunner.hpp"
#include <iostream>
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
int main(int argc,char** argv){
    minima::TxPoWSearcher::services.getTip=[](){return std::shared_ptr<minima::TxPoWTreeNode>{};};
    auto customTransactions=std::make_shared<minima::TxnDB>(std::make_shared<minima::UserDB>());
    minima::TxnDB::runtimeLookup=[customTransactions]{return customTransactions;};
    if(argc==2&&std::string(argv[1])=="--help"){std::cout<<"Native Minima command subset (full node port incomplete).\nPass a quoted command or supply one command per input line.\nCustom transactions live in memory for this session; use txnexport to save them.\nAvailable:";for(const auto& cmd:minima::CommandRunner::ALL_COMMANDS())std::cout<<' '<<cmd->getName().value();std::cout<<"\nExample: minima_commands 'hash data:hello type:sha3'\n";return 0;}
    auto runner=minima::CommandRunner::getRunner();auto run=[&](const std::string& line){try{std::cout<<runner.runMultiCommand(line).toJSONString()<<'\n';return 0;}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}};
    if(argc>1){std::string command;for(int i=1;i<argc;++i){if(i>1)command+=' ';command+=argv[i];}return run(command);}
    std::string line;int result=0;while(std::getline(std::cin,line))result|=run(line);return result;
}
