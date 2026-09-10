#include "PREVSTATE.hpp"
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
ValuePtr PREVSTATE::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int port=contract.getNumberParam(0,*this).getNumber().getAsInt();return contract.getPrevState(port);
}

}
