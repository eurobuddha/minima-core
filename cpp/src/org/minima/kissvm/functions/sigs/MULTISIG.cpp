#include "MULTISIG.hpp"
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
ValuePtr MULTISIG::runFunction(Contract& contract) {
    checkMinParamNumber(requiredParams());
    int required=contract.getNumberParam(0,*this).getNumber().getAsInt();if(required<0) throw ExecutionException("CANNOT check negative sigs in MULTISIG "+std::to_string(required));
    int found=0;for(int i=1;i<getParameterNum();++i) {auto signature=contract.getHexParam(i,*this);if(contract.checkSignature(signature)) ++found;if(found>=required) break;}return Value::getValue(found>=required?"TRUE":"FALSE");
}

}
