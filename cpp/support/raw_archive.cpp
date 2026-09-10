#include "org/minima/database/archive/RawArchiveInput.hpp"
#include "org/minima/utils/Crypto.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::istringstream in(line);std::string op,path,action,result;in>>op>>path;RawArchiveInput raw(path);while(in>>action){try{if(action=="c"){raw.connect();result+="OK";}else if(action=="s"){raw.stop();result+="STOP";}else{auto ibd=raw.getNextIBD();result+=std::to_string(ibd.getTxBlocks()->size())+","+(ibd.hasCascade()?"true":"false")+","+Crypto::getInstance().hashObject(ibd).to0xString();}}catch(const wire::IOException&){result+="IO_ERROR";}catch(const std::exception&){result+="ERROR";}result+="|";}
result+="cascade:"+(raw.getCascade()?Crypto::getInstance().hashObject(*raw.getCascade()).to0xString():"null");try{raw.stop();}catch(const std::exception&){}std::cout<<result<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
