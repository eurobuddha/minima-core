#include "HEX.hpp"
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
ValuePtr HEX::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto value=getParameter(0)->getValue(contract);MiniData result;
    switch(value->getValueType()) {
        case Value::VALUE_BOOLEAN:result=MiniData::fromHex(static_cast<BooleanValue&>(*value).isTrue()?"0x01":"0x00");break;
        case Value::VALUE_HEX:result=static_cast<HexValue&>(*value).getMiniData();break;
        case Value::VALUE_NUMBER: {const auto& number=static_cast<NumberValue&>(*value).getNumber();if(!number.floor().isEqual(number)||number.isLess(MiniNumber::ZERO)) throw ExecutionException("Can ONLY convert positive whole NUMBERs to HEX : "+number.toString());result=MiniData(number.getAsBigInteger());break;}
        case Value::VALUE_SCRIPT:result=MiniData(static_cast<StringValue&>(*value).getBytes());break;
        default:throw ExecutionException("Invalid Type in HEX cast "+std::to_string(value->getValueType()));
    }return std::make_shared<HexValue>(std::move(result));
}

}
