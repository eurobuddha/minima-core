#pragma once
#include "Statement.hpp"
#include <vector>
namespace minima::kissvm {
class StatementBlock {
    std::vector<StatementPtr> mStatements;
public:
    explicit StatementBlock(std::vector<StatementPtr> statements):mStatements(std::move(statements)){}
    void run(Contract& contract);
};
}
