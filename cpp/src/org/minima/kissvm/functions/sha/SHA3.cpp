#include "SHA3.hpp"
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
ValuePtr SHA3::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto value=getParameter(0)->getValue(contract);checkIsOfType(*value,Value::VALUE_HEX|Value::VALUE_SCRIPT);
    auto bytes=value->getValueType()==Value::VALUE_HEX?static_cast<HexValue&>(*value).getRawData():static_cast<StringValue&>(*value).getBytes();return std::make_shared<HexValue>(MiniData(std::move(bytes)).sha3());
}

}
