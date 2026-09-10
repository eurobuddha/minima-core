#include "REV.hpp"
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
ValuePtr REV::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto data=contract.getHexParam(0,*this).getRawData();std::reverse(data.begin(),data.end());return std::make_shared<HexValue>(std::move(data));
}

}
