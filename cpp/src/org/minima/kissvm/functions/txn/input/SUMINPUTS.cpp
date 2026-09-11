#include "SUMINPUTS.hpp"
#include "../../../Contract.hpp"
namespace minima::kissvm {
ValuePtr SUMINPUTS::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());
    auto token=contract.getHexParam(0,*this).getMiniData();MiniNumber total=MiniNumber::ZERO;
    for(const auto txnItems=contract.getTransaction().getAllInputs();const auto& coin:*txnItems) if(coin.getTokenID().isEqual(token)) {auto amount=token.isEqual(Token::TOKENID_MINIMA)?coin.getAmount():coin.getToken().value().getScaledTokenAmount(coin.getAmount());total=total.add(amount);}return std::make_shared<NumberValue>(total);
}
}
