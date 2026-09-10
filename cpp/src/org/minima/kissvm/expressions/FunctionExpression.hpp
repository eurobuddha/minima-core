#pragma once
#include "Expression.hpp"
#include "../functions/MinimaFunction.hpp"
namespace minima::kissvm {
class FunctionExpression : public Expression {
    std::shared_ptr<MinimaFunction> mFunction;
public:
    explicit FunctionExpression(std::shared_ptr<MinimaFunction> function):mFunction(std::move(function)){}
    ValuePtr getValue(Contract& contract) override;
    std::string toString() const override;
};
}
