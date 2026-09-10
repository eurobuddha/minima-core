#include "STRING.hpp"
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
ValuePtr STRING::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto value=getParameter(0)->getValue(contract);if(value->getValueType()==Value::VALUE_SCRIPT) return std::make_shared<StringValue>(static_cast<StringValue&>(*value));return std::make_shared<StringValue>(value->toString());
}

}
