#include "org/minima/database/cascade/Cascade.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
static std::string weight(const MMREntryNumber& x) {return x.getUnscaled().toDecimalString()+":"+std::to_string(x.getScale())+":"+x.toBigDecimalString();}
static std::string hex(const std::string& s) {return BaseConverter::encode16(std::vector<uint8_t>(s.begin(),s.end()));}
static TxPoW pow(int n,int divisor) {
    TxPoW t;t.setTimeMilli(MiniNumber(1788883200000LL+n));t.setNonce(MiniNumber(n));t.setBlockNumber(MiniNumber(n));t.getTxBody()->mPRNG=MiniData::fromHex("0x000102");
    t.setBlockDifficulty(MiniData(Crypto::MAX_VAL/BigInt(divisor)));t.setHeaderBodyHash();t.calculateTXPOWID();return t;
}
static std::string result(const Cascade& c) {
    auto s=BaseConverter::encode16(c.serialise())+"|"+weight(c.getTotalWeight())+"|"+std::to_string(c.getLength())+"|"+(Cascade::checkCascadeCorrect(c)?"true":"false");
    for(auto n=c.getTip();n;n=n->getParent()) s+="|"+n->getTxPoW().getBlockNumber().toString()+":"+std::to_string(n->getLevel())+":"+std::to_string(n->getSuperLevel())+":"+weight(n->getCurrentWeight())+":"+(n->getTxPoW().hasBody()?"true":"false");
    return s+"|"+hex(c.printCascade());
}
int main() {
    std::string line;while(std::getline(std::cin,line)) {try {
        std::istringstream in(line);std::string op;in>>op;
        if(op=="weight") {std::string a,b,f;int sa,sb;in>>a>>sa>>f>>b>>sb;MMREntryNumber x(BigInt(a),sa);std::cout<<weight(x.multiplyDecimal32(BigInt(f)))<<"|"<<weight(x.add(MMREntryNumber(BigInt(b),sb)))<<'\n';continue;}
        if(op=="node") {int n,d,l;in>>n>>d>>l;CascadeNode node(pow(n,d));node.setLevel(l);auto output=BaseConverter::encode16(node.serialise())+"|"+weight(node.getCurrentWeight())+"|"+hex(node.toString());std::cout<<output<<'\n';continue;}
        if(op=="readnode") {std::string raw;in>>raw;auto bytes=BaseConverter::decode16(raw);size_t offset=0;auto n=CascadeNode::deserialise(bytes,offset);auto output=BaseConverter::encode16(n.serialise())+"|"+weight(n.getCurrentWeight())+"|"+std::to_string(n.getSuperLevel())+"|"+(n.getTxPoW().hasBody()?"true":"false")+"|"+std::to_string(bytes.size()-offset);std::cout<<output<<'\n';continue;}
        in>>GlobalParams::MINIMA_CASCADE_LEVEL_NODES;
        if(op=="read") {std::string raw;in>>raw;auto bytes=BaseConverter::decode16(raw);size_t offset=0;auto c=Cascade::deserialise(bytes,offset);auto output=result(c)+"|"+std::to_string(bytes.size()-offset);std::cout<<output<<'\n';continue;}
        Cascade c;int count,divisor,mode;in>>count>>divisor>>mode;std::vector<MiniData> parents(32,MiniData::fromHex("0x00"));
        for(int i=0;i<count;++i) {auto t=pow(i,divisor);for(int j=0;j<32;++j)t.setSuperParent(j,parents[j]);t.calculateTXPOWID();for(int j=0;j<=t.getSuperLevel();++j)parents[j]=t.getTxPoWIDData();c.addToTip(t);if(mode&1)c.cascadeChain();}
        if(c.getTip()) {auto n=c.getTip();if(mode&2)n->setLevel(3);if(mode&4)n->getTxPoW().setSuperParent(0,MiniData::fromHex("0x00"));if(mode&8)n->getTxPoW().setSuperParent(1,MiniData::fromHex("0x1234"));if(mode&16)n->getTxPoW().setSuperParent(0,n->getTxPoW().getTxPoWIDData());}
        if(mode&32)c.cascadeChain();if(mode&64)c=c.deepCopy();auto output=result(c);std::cout<<output<<'\n';
    }catch(const std::exception&) {std::cout<<"ERROR\n";}}
}
