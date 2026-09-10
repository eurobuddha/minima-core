#include "org/minima/utils/MiniFile.hpp"
#include "TxPowTree.hpp"
#include <bit>
namespace minima {
namespace {
class Action : public TxPoWTreeNodeAction {
    std::function<void(std::shared_ptr<TxPoWTreeNode>)> function;
public:
    explicit Action(std::function<void(std::shared_ptr<TxPoWTreeNode>)> f):function(std::move(f)){}
    void runAction(std::shared_ptr<TxPoWTreeNode> node) override {function(std::move(node));}
};
}
std::shared_ptr<TxPoWTreeNode> TxPowTree::findNode(const std::string& id) const {auto node=mFastLink.get(id);return node?*node:nullptr;}
void TxPowTree::setRoot(std::shared_ptr<TxPoWTreeNode> root) {mRoot=std::move(root);if(!mRoot) throw std::invalid_argument("Null tree root");mRoot->clearParent();recalculateTree();}
void TxPowTree::addFastLink(std::shared_ptr<TxPoWTreeNode> node) {auto id=node->getTxPoW().getTxPoWID();mFastLink.put(std::move(id),std::move(node));}
std::shared_ptr<TxPoWTreeNode> TxPowTree::traverseTree(TxPoWTreeNodeAction& action,std::shared_ptr<TxPoWTreeNode> root) const {
    if(!root) return nullptr;std::vector<std::shared_ptr<TxPoWTreeNode>> stack{root};
    while(!stack.empty()) {auto node=stack.back();stack.pop_back();action.runAction(node);if(action.isFinished()) return action.getReturnNode();for(const auto& child:node->getChildren()) stack.push_back(child);}
    return nullptr;
}
void TxPowTree::recalculateTree() {
    mFastLink={};std::vector<std::shared_ptr<TxPoWTreeNode>> allnodes;
    Action reset([&](auto node){node->setTotalWeight(node->getTxPoW().getWeight());allnodes.push_back(node);addFastLink(node);});traverseTree(reset,mRoot);mTotalNodes=getSize();
    std::stable_sort(allnodes.begin(),allnodes.end(),[](const auto& a,const auto& b){return a->getBlockNumber().isMore(b->getBlockNumber());});
    for(const auto& node:allnodes) for(const auto& child:node->getChildren()) node->addToTotalWeight(child->getTotalWeight());
    mTip=mRoot;mLength=0;
    while(mTip) {mLength=std::bit_cast<int32_t>(uint32_t(mLength)+1u);auto children=mTip->getChildren();if(children.empty()) break;mTip=children.front();for(size_t i=1;i<children.size();++i) if(children[i]->getTotalWeight().isMore(mTip->getTotalWeight())) mTip=children[i];}
    calculatePulseList();
}
void TxPowTree::calculatePulseList() {auto blocks=std::make_shared<std::vector<MiniData>>();auto current=mTip;for(int i=0;current && i<512;++i,current=current->getParent()) blocks->push_back(current->getTxPoW().getTxPoWIDData());mPulseList=std::move(blocks);}
std::vector<std::shared_ptr<TxPoWTreeNode>> TxPowTree::getHeaviestBranch() const {std::vector<std::shared_ptr<TxPoWTreeNode>> branch;for(auto current=mTip;current;current=current->getParent()) branch.push_back(current);return branch;}
std::vector<std::shared_ptr<TxPoWTreeNode>> TxPowTree::setLength(int32_t maxLength) {
    std::shared_ptr<TxPoWTreeNode> newroot;int32_t count=0;
    for(auto current=mTip;current;current=current->getParent()) {newroot=current;count=std::bit_cast<int32_t>(uint32_t(count)+1u);if(count>=maxLength) break;}
    if(!newroot) throw std::invalid_argument("Null tree root");
    // Keep removed ancestors alive while detaching and rebuilding the native tree.
    std::vector<std::shared_ptr<TxPoWTreeNode>> removed;for(auto current=newroot->getParent();current;current=current->getParent()) removed.push_back(current);
    setRoot(newroot);std::reverse(removed.begin(),removed.end());return removed;
}
std::string TxPowTree::printTree(int32_t depth) const {
    auto tip=mTip;if(!tip) return "";
    // Java evaluates depth-1 in signed 32-bit arithmetic, including wraparound.
    auto back=std::bit_cast<int32_t>(uint32_t(depth)-1u);
    for(int64_t i=0;i<back;++i) if(tip->getParent()) tip=tip->getParent();auto rootblock=tip->getBlockNumber();std::string result;
    Action printer([&](auto node){const auto& txpow=node->getTxPoW();auto block=txpow.getBlockNumber();int indent=block.sub(rootblock).getAsInt();for(int i=0;i<indent;++i) result+=(i==indent-1)?"-->":"   ";
        auto count=txpow.getTransactions().size()+(txpow.isTransaction()?1:0);
        result+=" "+block.toString()+" [0/"+std::to_string(txpow.getSuperLevel())+"] "+txpow.getTxPoWID()+" txns:"+std::to_string(count)+"  weight:"+txpow.getWeight().toBigDecimalString()+"/"+node->getTotalWeight().toBigDecimalString()+" @ "+txpow.getTxHeader().toJSON().getString("date")+"\n";});
    traverseTree(printer,tip);return result;
}
std::vector<uint8_t> TxPowTree::serialise() const {auto data=MiniNumber(mTotalNodes).serialise();Action writer([&](auto node){wire::append(data,node->serialise());});traverseTree(writer,mRoot);return data;}
void TxPowTree::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mRoot.reset();mTip.reset();mFastLink={};mLength=0;
    int32_t length=MiniNumber::deserialise(data,offset).getAsInt();
    for(int32_t i=0;i<length;++i) {auto node=TxPoWTreeNode::deserialise(data,offset);addFastLink(node);TxPoWTreeNode::services.addSQLTxPoW(node->getTxPoW());TxPoWTreeNode::services.addTxBlock(node->getTxBlock());
        if(!mRoot) {mRoot=node;mRoot->clearParent();mTip=node;}else {auto parent=findNode(node->getTxPoW().getParentID().to0xString());if(!parent) throw std::invalid_argument("Missing tree parent");parent->addChildNode(node);}}
    recalculateTree();
}
}

namespace minima {
void TxPowTree::loadDB(const std::filesystem::path& file) {MiniFile::loadObjectSlow(file,*this);}
void TxPowTree::saveDB(const std::filesystem::path& file) {MiniFile::saveObjectDirect(file,*this);}
}
