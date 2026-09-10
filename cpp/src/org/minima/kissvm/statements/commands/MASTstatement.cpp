#include "MASTstatement.hpp"
#include "../../Contract.hpp"
#include "../StatementParser.hpp"
#include "../../tokens/ScriptTokenizer.hpp"
namespace minima::kissvm {
void MASTstatement::execute(Contract& contract) {
    auto value=mValue->getValue(contract);const auto& mast=dynamic_cast<HexValue&>(*value);auto proof=contract.getWitness().getScript(mast.getMiniData());
    if(!proof) throw ExecutionException("No script found for MAST "+mast.getMiniData().to0xString());auto script=proof->getScript().toString();
    try {auto tokens=ScriptTokenizer(script).tokenize();auto block=StatementParser::parseTokens(tokens,contract.getStackDepth());block->run(contract);}
    catch(const ExecutionException&) {throw;}catch(const std::exception& error) {throw ExecutionException(exceptionString(error));}
}
}
