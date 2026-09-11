#include "org/minima/objects/mmr/MMR.hpp"
#include "org/minima/objects/mmr/MegaMMR.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "support/JavaExceptions.hpp"
#include <sstream>
#include <iostream>
using namespace minima;
int main(int argc,char** argv) {
    if(argc<3)return 2;std::string operation=argv[1],mode=argv[2];std::vector<std::string> args;if(mode=="args")args={"ignored"};
    std::ostringstream output;auto previous=std::cout.rdbuf(output.rdbuf());std::string error="none",message;MMR tree;std::string before,after;
    try {
        if(operation=="MMR")MMR::main(mode=="null"?nullptr:&args);
        else if(operation=="MegaMMR"){GeneralParams::MEGAMMR_MEGAPRUNE=std::string(argv[3])=="true";MegaMMR::main(mode=="null"?nullptr:&args);}
        else {
            if(mode=="entry") {auto data=std::make_shared<MMRData>(MiniData::fromHex("0x01"),MiniNumber(argv[6]));data->setUnspendable(std::string(argv[7])=="true");tree.setEntry(std::stoi(argv[3]),MMREntryNumber(BigInt(argv[4]),std::stoi(argv[5])),data);}
            else if(mode=="many")for(int i=0;i<std::stoi(argv[3]);++i)tree.addEntry(MMRData(MiniData::fromHex("0x01"),MiniNumber(i)));
            before=BaseConverter::encode16(tree.serialise());
            if(operation=="printinfo")MMR::printinfo(mode=="null"?nullptr:&tree);else MMR::printmmrtree(mode=="null"?nullptr:&tree);
            after=BaseConverter::encode16(tree.serialise());
        }
    }catch(const std::exception& e){auto text=javaExceptionText(e);auto split=text.find(": ");error=text.substr(0,split);message=split==std::string::npos?"":text.substr(split+2);}
    std::cout.rdbuf(previous);json::JSONObject result;result.put("stdout",output.str());result.put("error",error);result.put("message",message);result.put("before",before);result.put("after",after);std::cout<<result.toString()<<'\n';
}
