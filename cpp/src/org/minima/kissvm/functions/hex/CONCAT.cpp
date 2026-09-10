#include "CONCAT.hpp"
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
ValuePtr CONCAT::runFunction(Contract& contract) {
    checkMinParamNumber(requiredParams());
    std::vector<uint8_t> bytes;
    for(const auto& expression:getAllParameters()) {auto value=expression->getValue(contract);checkIsOfType(*value,Value::VALUE_HEX);const auto& next=static_cast<HexValue&>(*value).getRawData();
        size_t total=bytes.size()+next.size();if(total>Contract::MAX_DATA_SIZE) throw ExecutionException("MAX HEX value size reached : "+std::to_string(total)+"/65536");bytes.insert(bytes.end(),next.begin(),next.end());}
    return std::make_shared<HexValue>(std::move(bytes));
}

}
