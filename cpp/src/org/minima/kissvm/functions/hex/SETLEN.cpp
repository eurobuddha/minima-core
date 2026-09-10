#include "SETLEN.hpp"
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
ValuePtr SETLEN::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int length=contract.getNumberParam(0,*this).getNumber().getAsInt();if(length<1||length>Contract::MAX_DATA_SIZE) throw ExecutionException("SETLEN size MUST be > 0 and < 65536 : "+std::to_string(length));
    auto original=contract.getHexParam(1,*this).getRawData();std::vector<uint8_t> result(length);size_t count=std::min(result.size(),original.size());std::copy(original.end()-count,original.end(),result.end()-count);return std::make_shared<HexValue>(std::move(result));
}

}
