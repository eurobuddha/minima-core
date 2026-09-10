#include "SUBSTR.hpp"
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
ValuePtr SUBSTR::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int start=contract.getNumberParam(0,*this).getNumber().getAsInt();int end=contract.getNumberParam(1,*this).getNumber().getAsInt();int length=std::bit_cast<int32_t>(uint32_t(end)-uint32_t(start));
    if(length<0) throw ExecutionException("Negative SUBSTR length "+std::to_string(length));auto main=contract.getStringParam(2,*this).getUTF16();
    if(start<0||int64_t(end)>int64_t(main.size())) throw ExecutionException("SUBSTR range outside size of String "+std::to_string(start)+"-"+std::to_string(end)+" length:"+std::to_string(main.size()));
    if(end<start||size_t(start)>main.size()) throw std::out_of_range("SUBSTR substring range");return std::make_shared<StringValue>(main.substr(start,size_t(end)-start));
}

}
