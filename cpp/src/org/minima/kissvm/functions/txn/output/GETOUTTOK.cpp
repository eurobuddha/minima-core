#include "GETOUTTOK.hpp"
#include "../../../Contract.hpp"
namespace minima::kissvm {
ValuePtr GETOUTTOK::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int index=contract.getNumberParam(0,*this).getNumber().getAsInt();
    const auto& coins=contract.getTransaction().getAllOutputs();if(index<0||size_t(index)>=coins.size()) throw ExecutionException("Output out of range "+std::to_string(index)+"/"+std::to_string(coins.size()));const auto& coin=coins[index];
    return std::make_shared<HexValue>(coin.getTokenID());
}
}
