#include "UTF8.hpp"
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
ValuePtr UTF8::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto data=contract.getHexParam(0,*this).getRawData();return std::make_shared<StringValue>(MiniString(std::span<const uint8_t>(data)).toString());
}

}
