#include "PROOF.hpp"
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
ValuePtr PROOF::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto value=getParameter(0)->getValue(contract);checkIsOfType(*value,Value::VALUE_HEX|Value::VALUE_SCRIPT);auto sum=contract.getNumberParam(1,*this);
    auto leaf=value->getValueType()==Value::VALUE_HEX?MMRData::CreateMMRDataLeafNode(static_cast<HexValue&>(*value).getMiniData(),sum.getNumber()):MMRData::CreateMMRDataLeafNode(static_cast<StringValue&>(*value).getMiniString(),sum.getNumber());
    auto rootHash=contract.getHexParam(2,*this);auto rootSum=contract.getNumberParam(3,*this);MMRData expected(rootHash.getMiniData(),rootSum.getNumber());auto chain=contract.getHexParam(4,*this);MMRProof proof;
    try {proof=MMRProof::convertMiniDataVersion(chain.getMiniData());}catch(const std::exception&) {throw ExecutionException("Invalid MMRProof at PROOF "+chain.toString());}
    return std::make_shared<BooleanValue>(proof.calculateProof(leaf).isEqual(expected));
}

}
