#include "SIGNEDBY.hpp"
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
ValuePtr SIGNEDBY::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto pubkey=contract.getHexParam(0,*this);return std::make_shared<BooleanValue>(contract.checkSignature(pubkey));
}

}
