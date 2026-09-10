#pragma once
#include "../expressions/Expression.hpp"
namespace minima::kissvm {
class Statement {
public:
    virtual ~Statement();
    virtual void execute(Contract& contract)=0;
    virtual std::string toString() const=0;
};
using StatementPtr=std::shared_ptr<Statement>;
}
