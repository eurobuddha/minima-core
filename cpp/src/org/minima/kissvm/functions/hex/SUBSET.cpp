#include "SUBSET.hpp"
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
ValuePtr SUBSET::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int start=contract.getNumberParam(0,*this).getNumber().getAsInt();int end=contract.getNumberParam(1,*this).getNumber().getAsInt();int length=std::bit_cast<int32_t>(uint32_t(end)-uint32_t(start));
    if(length<0) throw ExecutionException("Negative SUBSET length "+std::to_string(length));if(length>Contract::MAX_DATA_SIZE) throw ExecutionException("SUBSET size too large "+std::to_string(length));
    auto data=contract.getHexParam(2,*this).getRawData();std::string range=std::to_string(start)+"-"+std::to_string(end)+" length:"+std::to_string(data.size());
    if(start<0||size_t(start)>data.size()) throw ExecutionException("SUBSET start outside size of data array "+range);
    if(end<0||size_t(end)>data.size()) throw ExecutionException("SUBSET end outside size of data array "+range);
    return std::make_shared<HexValue>(std::vector<uint8_t>(data.begin()+start,data.begin()+end));
}

}
