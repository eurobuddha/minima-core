#include "BITCOUNT.hpp"
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
ValuePtr BITCOUNT::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    return std::make_shared<NumberValue>(int64_t(totalBits(contract.getHexParam(0,*this).getRawData())));
}
int BITCOUNT::totalBits(const std::vector<uint8_t>& data) {uint32_t count=0;for(auto byte:data) count+=static_cast<uint32_t>(BITSPERBYTE[byte]);return std::bit_cast<int32_t>(count);}
}
