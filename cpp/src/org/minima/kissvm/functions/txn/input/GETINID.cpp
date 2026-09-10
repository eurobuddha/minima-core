#include "GETINID.hpp"
#include "../../../Contract.hpp"
namespace minima::kissvm {
ValuePtr GETINID::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int index=contract.getNumberParam(0,*this).getNumber().getAsInt();
    const auto& coins=contract.getTransaction().getAllInputs();if(index<0||size_t(index)>=coins.size()) throw ExecutionException("Input number out of range "+std::to_string(index)+"/"+std::to_string(coins.size()));const auto& coin=coins[index];
    (void)coin;return std::make_shared<HexValue>(contract.getWitness().getAllCoinProofs().at(index).getCoin().getCoinID());
}
}
