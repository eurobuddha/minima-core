#include "GETINADDR.hpp"
#include "../../../Contract.hpp"
namespace minima::kissvm {
ValuePtr GETINADDR::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int index=contract.getNumberParam(0,*this).getNumber().getAsInt();
    const auto coinsOwner=contract.getTransaction().getAllInputs();const auto& coins=*coinsOwner;if(index<0||size_t(index)>=coins.size()) throw ExecutionException("Input number out of range "+std::to_string(index)+"/"+std::to_string(coins.size()));const auto& coin=coins[index];
    return std::make_shared<HexValue>(coin.getAddress());
}
}
