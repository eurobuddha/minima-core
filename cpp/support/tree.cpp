#include "org/minima/database/txpowtree/TxPowTree.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
static std::string id(std::shared_ptr<TxPoWTreeNode> n){return n?n->getTxPoW().getTxPoWID():"null";}
static std::string hex(const std::string& s){return BaseConverter::encode16(std::vector<uint8_t>(s.begin(),s.end()));}
static std::string result(TxPowTree& t,const std::vector<std::shared_ptr<TxPoWTreeNode>>& nodes,int depth){
    auto s=id(t.getRoot())+"|"+id(t.getTip())+"|"+std::to_string(t.getSize())+"|"+std::to_string(t.getHeaviestBranchLength());
    for(const auto& n:nodes)s+="|"+id(n)+":"+n->getTotalWeight().getUnscaled().toDecimalString()+":"+std::to_string(n->getTotalWeight().getScale())+":"+id(t.findNode(id(n)))+":"+id(n->getParent())+":"+id(n->getParent(-1))+":"+id(n->getParent(2))+":"+id(n->getPastNode(MiniNumber(0)));
    s+="|";for(const auto& n:t.getHeaviestBranch())s+=id(n)+",";return s+"|"+std::to_string(t.getPulseList().size())+"|"+hex(t.printTree(depth));
}
int main(){GeneralParams::USE_SQL_COINDB=false;std::string line;while(std::getline(std::cin,line)){try{
    std::istringstream in(line);std::string op;in>>op;
    if(op=="readnode"){std::string raw;in>>raw;auto bytes=BaseConverter::decode16(raw);size_t offset=0;auto n=TxPoWTreeNode::deserialise(bytes,offset);auto output=BaseConverter::encode16(n->serialise())+"|"+n->getMMR()->toJSON().toJSONString()+"|"+std::to_string(n->getAllCoins().size())+"|"+std::to_string(n->getRelevantCoins().size())+"|"+std::to_string(bytes.size()-offset);std::cout<<output<<'\n';continue;}
    if(op=="blocknode"){int count,mode;in>>count>>mode;TxPoW t;t.setTimeMilli(MiniNumber(0));t.setNonce(MiniNumber(count));t.setBlockNumber(MiniNumber(7));t.getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x1234"));t.setHeaderBodyHash();t.calculateTXPOWID();auto b=std::make_shared<TxBlock>(t);for(int i=0;i<count;++i)b->getOutputCoins().emplace_back(MiniData::fromHex(BaseConverter::numberToHex(i+1)),MiniData::fromHex("0x1122"),MiniNumber(i+1),MiniData::fromHex("0x00"),i%2==0);auto n=std::make_shared<TxPoWTreeNode>(b,false);for(int i=0;i<count;++i){auto& coin=b->getOutputCoins()[i];coin.setMMREntryNumber(MMREntryNumber(i));n->addCoinToNode(coin);if((i+mode)%2==0)n->getRelevantCoinsEntries().push_back(coin.getMMREntryNumber());}n->calculateRelevantCoins();if(mode&2)n->removeRelevantCoin(MMREntryNumber(0));if(mode&4)n->calculateRelevantCoins();auto output=BaseConverter::encode16(n->serialise())+"|"+n->getMMR()->toJSON().toJSONString()+"|"+std::to_string(n->getAllCoins().size())+"|"+std::to_string(n->getRelevantCoins().size());std::cout<<output<<'\n';continue;}
    int count,trim,depth;in>>count>>trim>>depth;std::vector<std::shared_ptr<TxPoWTreeNode>> nodes;TxPowTree tree;
    for(int i=0;i<count;++i){int parent,height,weight;std::string txid;in>>parent>>height>>weight>>txid;TxPoW t(txid,height,weight);t.setTimeMilli(MiniNumber(i));auto n=std::make_shared<TxPoWTreeNode>(t);if(parent>=0)nodes.at(size_t(parent))->addChildNode(n);nodes.push_back(n);}
    if(count>0)tree.setRoot(nodes.front());else tree.recalculateTree();std::string removed;std::vector<std::shared_ptr<TxPoWTreeNode>> retained;if(trim!=-999){retained=tree.setLength(trim);for(const auto& n:retained)removed+=id(n)+",";}
    auto output=result(tree,nodes,depth)+"|"+removed;std::cout<<output<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}}
