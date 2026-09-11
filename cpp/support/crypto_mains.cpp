#include "org/minima/utils/encrypt/EncryptDecrypt.hpp"
#include "org/minima/utils/encrypt/PasswordCrypto.hpp"
#include "org/minima/utils/encrypt/CryptoPackage.hpp"
#include "org/minima/utils/encrypt/SignVerify.hpp"
#include <iostream>
using namespace minima;
int main(int argc,char** argv){
    if(argc<3)return 2;std::string name=argv[1],mode=argv[2];std::vector<std::string> args;
    if(mode=="args")args={"unused","value"};auto input=mode=="null"?nullptr:&args;
    try {
        if(name=="EncryptDecrypt")EncryptDecrypt::main(input);
        else if(name=="PasswordCrypto")PasswordCrypto::main(input);
        else if(name=="CryptoPackage")CryptoPackage::main(input);
        else if(name=="SignVerify")SignVerify::main(input);
        else return 2;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
