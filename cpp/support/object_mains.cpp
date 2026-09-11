#include "org/minima/objects/Address.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "support/JavaExceptions.hpp"
#include <iostream>
namespace minima {
class ObjectMainsFixture {
public:
    static void fixed(const std::string& name,const MiniData& data) {
        if(name=="Address")Address::mainWithData(data);
        else if(name=="MiniData")MiniData::mainWithData(data);
        else throw std::invalid_argument("unknown fixed main");
    }
};
}
using namespace minima;
int main(int argc,char** argv)try {
    if(argc<3)return 2;std::string name=argv[1],mode=argv[2];
    if(mode=="fixed") {if(argc!=4)return 2;ObjectMainsFixture::fixed(name,MiniData::fromHex(argv[3]));}
    else if(name=="randomLength") {auto data=MiniData::getRandomData(std::stoi(mode));std::cout<<data.getLength()<<":"<<data.to0xString()<<std::endl;}
    else {
        std::vector<std::string> args;if(mode=="args")args={"ignored","--not-a-real-option"};
        auto ptr=mode=="null"?nullptr:&args;
        if(name=="Address")Address::main(ptr);else if(name=="MiniData")MiniData::main(ptr);else if(name=="TxPoWGenerator")TxPoWGenerator::main(ptr);else return 2;
    }
}catch(const std::exception& e){std::cout<<"EXCEPTION:"<<javaExceptionText(e)<<std::endl;}
