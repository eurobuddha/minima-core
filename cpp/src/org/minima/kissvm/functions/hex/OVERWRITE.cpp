#include "OVERWRITE.hpp"
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
ValuePtr OVERWRITE::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto source=contract.getHexParam(0,*this).getRawData();int sourcePos=contract.getNumberParam(1,*this).getNumber().getAsInt();auto dest=contract.getHexParam(2,*this).getRawData();int destPos=contract.getNumberParam(3,*this).getNumber().getAsInt();int length=contract.getNumberParam(4,*this).getNumber().getAsInt();
    auto sum=[](int a,int b){return std::bit_cast<int32_t>(uint32_t(a)+uint32_t(b));};
    if(sum(destPos,length)>int(dest.size())) throw ExecutionException("OVERWRITE destination array too short");
    if(sum(sourcePos,length)>int(source.size())) throw ExecutionException("OVERWRITE src array too short");
    if(length<0) throw ExecutionException("Cannot have negative length "+std::to_string(length));
    if(sourcePos<0||destPos<0||size_t(sourcePos)>source.size()||size_t(destPos)>dest.size()||size_t(length)>source.size()-sourcePos||size_t(length)>dest.size()-destPos) throw std::out_of_range("OVERWRITE arraycopy range");
    std::copy_n(source.begin()+sourcePos,length,dest.begin()+destPos);return std::make_shared<HexValue>(std::move(dest));
}

}
