#pragma once
#include "../values/Value.hpp"
namespace minima::kissvm {
class Contract;
using ValuePtr=std::shared_ptr<Value>;
class Expression {
public:
    virtual ~Expression();
    virtual ValuePtr getValue(Contract& contract)=0;
    virtual std::string toString() const=0;
};
using ExpressionPtr=std::shared_ptr<Expression>;
}
