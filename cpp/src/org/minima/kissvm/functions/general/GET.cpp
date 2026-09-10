#include "GET.hpp"
#include "../../Contract.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/objects/mmr/MMRProof.hpp"
#include "../../statements/StatementParser.hpp"
#include "../../tokens/ScriptTokenizer.hpp"
#include "../string/REPLACE.hpp"
#include "support/JavaUnicode.hpp"
#include <bit>
#include <algorithm>

namespace minima::kissvm {
ValuePtr GET::runFunction(Contract& contract) {
    checkMinParamNumber(requiredParams());
    std::string key;for(const auto& expression:getAllParameters()) {auto value=expression->getValue(contract);checkIsOfType(*value,Value::VALUE_NUMBER);key+=value->toString()+",";}auto value=contract.getVariable(key);
if(!value) throw ExecutionException("GET Variable not found : "+key);return value;
}

}
