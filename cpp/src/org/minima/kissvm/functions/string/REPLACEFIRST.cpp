#include "REPLACEFIRST.hpp"
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
ValuePtr REPLACEFIRST::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto main=contract.getStringParam(0,*this);auto search=contract.getStringParam(1,*this);auto replacement=contract.getStringParam(2,*this);
    return std::make_shared<StringValue>(REPLACE::replaceFirst(main.getUTF16(),search.getUTF16(),replacement.getUTF16()));
}

}
