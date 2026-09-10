#include "NUMBER.hpp"
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
ValuePtr NUMBER::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto value=getParameter(0)->getValue(contract);
    switch(value->getValueType()) {
        case Value::VALUE_BOOLEAN:return std::make_shared<NumberValue>(int64_t(static_cast<BooleanValue&>(*value).isTrue()?1:0));
        case Value::VALUE_HEX:return std::make_shared<NumberValue>(MiniNumber(static_cast<HexValue&>(*value).getMiniData().getDataValue(),0));
        case Value::VALUE_SCRIPT:return std::make_shared<NumberValue>(value->toString());
        case Value::VALUE_NUMBER:return std::make_shared<NumberValue>(static_cast<NumberValue&>(*value));
        default:throw ExecutionException("Invalid Type in NUMBER cast "+std::to_string(value->getValueType()));
    }
}

}
