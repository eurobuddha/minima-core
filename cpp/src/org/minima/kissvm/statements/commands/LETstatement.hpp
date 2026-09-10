#pragma once
#include "../Statement.hpp"
#include <vector>
namespace minima::kissvm {
class LETstatement : public Statement {
    int mLETType;
    std::vector<ExpressionPtr> mArrayPos;
    std::string mName;
    ExpressionPtr mValue;
public:
    static constexpr int LET_VARIABLE=0,LET_ARRAY=1;
    LETstatement(std::string name,ExpressionPtr value):mLETType(LET_VARIABLE),mName(std::move(name)),mValue(std::move(value)){}
    LETstatement(std::vector<ExpressionPtr> positions,ExpressionPtr value):mLETType(LET_ARRAY),mArrayPos(std::move(positions)),mValue(std::move(value)){}
    void execute(Contract& contract) override;
    std::string toString() const override;
};
}
