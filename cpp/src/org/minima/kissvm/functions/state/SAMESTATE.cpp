#include "SAMESTATE.hpp"
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
ValuePtr SAMESTATE::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int start=contract.getNumberParam(0,*this).getNumber().getAsInt();int end=contract.getNumberParam(1,*this).getNumber().getAsInt();
    if(start<0||end<start) throw ExecutionException("Invalid range check for SAMESTATE "+std::to_string(start)+" "+std::to_string(end));
    for(int64_t i=start;i<=end;++i) {auto old=contract.getPrevState(int(i))->toString();auto current=contract.getState(int(i))->toString();if(old!=current) {contract.traceLog("SAMESTATE FAIL ["+std::to_string(i)+"] PREV:"+old+" / CURRENT:"+current);return Value::getValue("FALSE");}}return Value::getValue("TRUE");
}

}
