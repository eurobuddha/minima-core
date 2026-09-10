#include "BITGET.hpp"
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
ValuePtr BITGET::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto data=contract.getHexParam(0,*this).getRawData();int total=int(data.size()*8)-1;int bit=contract.getNumberParam(1,*this).getNumber().getAsInt();
    if(bit<0||bit>total) throw ExecutionException("BitGet too large "+std::to_string(bit)+" / "+std::to_string(total));
return std::make_shared<BooleanValue>((data[bit/8]&(1u<<(bit%8)))!=0);
}

}
