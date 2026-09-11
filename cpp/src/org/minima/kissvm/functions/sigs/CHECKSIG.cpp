#include "CHECKSIG.hpp"
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
ValuePtr CHECKSIG::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    contract.incrementInstructions(31);auto pubkey=contract.getHexParam(0,*this);auto data=contract.getHexParam(1,*this);auto sig=contract.getHexParam(2,*this);
    if(pubkey.getRawData().empty()||sig.getRawData().empty()) throw ExecutionException("Invalid ZERO length params for CHECKSIG");
    TreeKey checker;checker.setPublicKey(pubkey.getMiniData());auto signature=Signature::convertMiniDataVersion(sig.getMiniData());
    return std::make_shared<BooleanValue>(checker.verify(data.getMiniData(),signature?&*signature:nullptr));
}

}
