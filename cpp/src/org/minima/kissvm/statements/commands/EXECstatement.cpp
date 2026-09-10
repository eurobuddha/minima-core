#include "EXECstatement.hpp"
#include "../../Contract.hpp"
#include "../StatementParser.hpp"
#include "../../tokens/ScriptTokenizer.hpp"
namespace minima::kissvm {
void EXECstatement::execute(Contract& contract) {
    auto value=mValue->getValue(contract);const auto& script=dynamic_cast<StringValue&>(*value).toString();
    try {auto tokens=ScriptTokenizer(script).tokenize();auto block=StatementParser::parseTokens(tokens,contract.getStackDepth());block->run(contract);}
    catch(const ExecutionException&) {throw;}catch(const std::exception& error) {throw ExecutionException(exceptionString(error));}
}
}
