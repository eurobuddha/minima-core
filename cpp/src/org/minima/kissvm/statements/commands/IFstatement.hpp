#pragma once
#include "../StatementBlock.hpp"
namespace minima::kissvm {
class IFstatement : public Statement {
    std::vector<ExpressionPtr> mConditions;
    std::vector<std::shared_ptr<StatementBlock>> mActions;
public:
    void addCondition(ExpressionPtr condition,std::shared_ptr<StatementBlock> action) {mConditions.push_back(std::move(condition));mActions.push_back(std::move(action));}
    void execute(Contract& contract) override;
    std::string toString() const override;
};
}
