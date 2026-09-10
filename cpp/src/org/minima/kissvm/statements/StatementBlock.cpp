#include "StatementBlock.hpp"
#include "../Contract.hpp"
namespace minima::kissvm {
void StatementBlock::run(Contract& contract) {
    contract.incrementStackDepth();
    for(const auto& statement:mStatements) {
        if(contract.isSuccessSet()) return;
        contract.incrementInstructions();contract.traceLog(statement->toString());statement->execute(contract);
    }
    contract.decrementStackDepth();
}
}
