#include "org/minima/utils/MinimaRPCClient.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>

int main(int argc,char** argv) {
    try {
        std::vector<minima::MinimaRPCClient::String> args;
        for(int i=1;i<argc;++i)args.emplace_back(minima::unicode::decodeUTF8(argv[i]));
        return minima::MinimaRPCClient::main(args);
    }catch(const std::exception& error) {
        std::cerr<<"Exception in thread \"main\" ";
        minima::NativeThrowable(error).printStackTrace(std::cerr);
        return 1;
    }
}
