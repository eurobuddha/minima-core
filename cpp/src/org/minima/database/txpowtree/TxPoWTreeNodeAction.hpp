#pragma once
#include <memory>
#include <optional>
#include <string>
namespace minima {
class TxPoWTreeNode;
class TxPoWTreeNodeAction {
    std::shared_ptr<TxPoWTreeNode> mReturnNode;
    std::optional<std::string> mExtraData;
public:
    TxPoWTreeNodeAction()=default;
    explicit TxPoWTreeNodeAction(std::string extra):mExtraData(std::move(extra)){}
    virtual ~TxPoWTreeNodeAction()=default;
    bool isFinished() const;
    std::shared_ptr<TxPoWTreeNode> getReturnNode() const {return mReturnNode;}
    void setReturnObject(std::shared_ptr<TxPoWTreeNode> node) {mReturnNode=std::move(node);}
    const std::optional<std::string>& getExtraData() const {return mExtraData;}
    virtual void runAction(std::shared_ptr<TxPoWTreeNode> node)=0;
};
}
