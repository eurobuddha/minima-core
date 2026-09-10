#include "ASCII.hpp"
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
ValuePtr ASCII::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto data=contract.getHexParam(0,*this).getRawData();std::u16string result;for(auto byte:data) result+=byte<128?char16_t(byte):u'�';return std::make_shared<StringValue>(result);
}

}
