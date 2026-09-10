#include "VERIFYIN.hpp"
#include "../../../Contract.hpp"
namespace minima::kissvm {
ValuePtr VERIFYIN::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    int index=contract.getNumberParam(0,*this).getNumber().getAsInt();auto address=contract.getHexParam(1,*this).getMiniData();auto amount=contract.getNumberParam(2,*this).getNumber();auto token=contract.getHexParam(3,*this).getMiniData();
    const auto& coins=contract.getTransaction().getAllInputs();if(index<0||size_t(index)>=coins.size()) throw ExecutionException("Input number out of range "+std::to_string(index)+"/"+std::to_string(coins.size()));const auto& coin=coins[index];
    auto actual=coin.getAmount();if(!coin.getTokenID().isEqual(Token::TOKENID_MINIMA)) {if(!coin.getToken()) throw ExecutionException("No token specified @ Input coin "+std::to_string(index)+" "+coin.getTokenID().to0xString());actual=coin.getToken()->getScaledTokenAmount(coin.getAmount());}
    bool result=address.isEqual(coin.getAddress())&&token.isEqual(coin.getTokenID())&&amount.isEqual(actual);return std::make_shared<BooleanValue>(result);
}
}
