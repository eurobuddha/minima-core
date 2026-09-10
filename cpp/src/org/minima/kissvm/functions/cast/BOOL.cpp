#include "BOOL.hpp"
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
ValuePtr BOOL::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto value=getParameter(0)->getValue(contract);bool result=false;
    switch(value->getValueType()) {
        case Value::VALUE_BOOLEAN:result=static_cast<BooleanValue&>(*value).isTrue();break;
        case Value::VALUE_HEX:result=!MiniNumber(static_cast<HexValue&>(*value).getMiniData().getDataValue(),0).isZero();break;
        case Value::VALUE_NUMBER:result=!static_cast<NumberValue&>(*value).getNumber().isZero();break;
        case Value::VALUE_SCRIPT:result=static_cast<StringValue&>(*value).getUTF16()!=u"FALSE";break;
        default:throw ExecutionException("Invalid Type in BOOL cast "+std::to_string(value->getValueType()));
    }return std::make_shared<BooleanValue>(result);
}

}
