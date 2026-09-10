#include "LEN.hpp"
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
ValuePtr LEN::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto value=getParameter(0)->getValue(contract);
    if(value->getValueType()==Value::VALUE_HEX) return std::make_shared<NumberValue>(int64_t(static_cast<HexValue&>(*value).getRawData().size()));
    if(value->getValueType()==Value::VALUE_SCRIPT) return std::make_shared<NumberValue>(int64_t(static_cast<StringValue&>(*value).getUTF16().size()));
    throw ExecutionException("LEN requires HEX or STRING param @ "+value->toString());
}

}
