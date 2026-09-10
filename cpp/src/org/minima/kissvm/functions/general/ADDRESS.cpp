#include "ADDRESS.hpp"
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
ValuePtr ADDRESS::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    return std::make_shared<HexValue>(Address(contract.getStringParam(0,*this).toString()).getAddressData());
}

}
