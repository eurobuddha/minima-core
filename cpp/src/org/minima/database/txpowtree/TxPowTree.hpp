#pragma once
#include <filesystem>
#include "TxPoWTreeNode.hpp"
#include "TxPoWTreeNodeAction.hpp"
namespace minima {
class TxPowTree {
    std::shared_ptr<TxPoWTreeNode> mRoot,mTip;
    int32_t mLength=0,mTotalNodes=0;
    JavaHashtable<std::shared_ptr<TxPoWTreeNode>> mFastLink;
    std::shared_ptr<std::vector<MiniData>> mPulseList=std::make_shared<std::vector<MiniData>>();
    void calculatePulseList();
    std::shared_ptr<TxPoWTreeNode> traverseTree(TxPoWTreeNodeAction& action,std::shared_ptr<TxPoWTreeNode> root) const;
public:
    std::shared_ptr<TxPoWTreeNode> findNode(const std::string& id) const;
    void setRoot(std::shared_ptr<TxPoWTreeNode> root);
    std::shared_ptr<TxPoWTreeNode> getRoot() const {return mRoot;}
    std::shared_ptr<TxPoWTreeNode> getTip() const {return mTip;}
    int32_t getSize() const {return int32_t(mFastLink.size());}
    void addFastLink(std::shared_ptr<TxPoWTreeNode> node);
    void recalculateTree();
    std::shared_ptr<std::vector<MiniData>> getSharedPulseList() const {return mPulseList;}
    std::vector<MiniData>& getPulseList() {return *mPulseList;}
    const std::vector<MiniData>& getPulseList() const {return *mPulseList;}
    std::vector<std::shared_ptr<TxPoWTreeNode>> getHeaviestBranch() const;
    int32_t getHeaviestBranchLength() const {return mLength;}
    std::vector<std::shared_ptr<TxPoWTreeNode>> setLength(int32_t maxLength);
    std::string printTree(int32_t depth) const;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    void loadDB(const std::filesystem::path& file);
    void saveDB(const std::filesystem::path& file);
};
}
