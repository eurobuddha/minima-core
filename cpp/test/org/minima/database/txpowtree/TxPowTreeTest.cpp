// Adapted from upstream test/org/minima/database/txpowtree/TxPowTreeTest.java.
#include "org/minima/database/txpowtree/TxPowTree.hpp"
#include <stdexcept>
using namespace minima;
static void require(bool b) {if(!b) throw std::runtime_error("TxPowTreeTest assertion");}
static auto node(const std::string& id,int height,int weight) {return std::make_shared<TxPoWTreeNode>(TxPoW(id,height,weight));}
int main() {
    for(auto [a,b,expected,total]:std::vector<std::tuple<int,int,std::string,int>>{{1,1,"0x04",5},{4,1,"0x01",8},{1,3,"0x02",7},{2,2,"0x01",7}}) {
        TxPowTree tree;auto root=node("0x00",0,1);root->addChildNode(node("0x01",1,a));root->addChildNode(node("0x02",1,b));auto child=node("0x03",1,1);root->addChildNode(child);child->addChildNode(node("0x04",2,1));tree.setRoot(root);
        require(tree.getTip()->getTxPoW().getTxPoWID()==expected);require(root->getTotalWeight().isEqual(MMREntryNumber(total)));require(tree.getSize()==5);
        for(const auto& id:{"0x00","0x01","0x02","0x03","0x04"}) require(tree.findNode(id)&&tree.findNode(id)->getTxPoW().getTxPoWID()==id);
        require(!tree.findNode("0x05")&&!tree.findNode("0x11"));
        if(a==1 && b==1) {auto branch=tree.getHeaviestBranch();require(branch.size()==3&&branch[0]->getTxPoW().getTxPoWID()=="0x04"&&branch[1]==child&&branch[2]==root);}
    }
    TxPowTree tree;auto root=node("0x00",0,1);root->addChildNode(node("0x01",1,2));root->addChildNode(node("0x02",1,2));auto child=node("0x03",1,1);root->addChildNode(child);child->addChildNode(node("0x04",2,1));child->addChildNode(node("0x05",2,1));auto child2=node("0x06",2,1);child->addChildNode(child2);child2->addChildNode(node("0x07",3,1));tree.setRoot(root);
    require(tree.getTip()->getTxPoW().getTxPoWID()=="0x07"&&root->getTotalWeight().isEqual(MMREntryNumber(10))&&tree.getSize()==8);
    auto removed=tree.setLength(2);require(removed.size()==2&&removed[0]==root&&removed[1]==child);require(tree.getRoot()==child2&&tree.getHeaviestBranchLength()==2&&tree.getSize()==2&&!child2->getParent()&&!child2->getMMR()->getParent());
    tree.recalculateTree();require(tree.getTip()->getTxPoW().getTxPoWID()=="0x07");
}
