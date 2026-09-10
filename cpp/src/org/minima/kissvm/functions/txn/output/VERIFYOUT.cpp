#include "VERIFYOUT.hpp"
#include "../../../Contract.hpp"
namespace minima::kissvm {
ValuePtr VERIFYOUT::runFunction(Contract& contract) {
    if(getParameterNum()<4||getParameterNum()>5) throw ExecutionException("VERIFYOUT requires 4 or 5 parameters");
    int index=contract.getNumberParam(0,*this).getNumber().getAsInt();auto address=contract.getHexParam(1,*this).getMiniData();auto amount=contract.getNumberParam(2,*this).getNumber();auto token=contract.getHexParam(3,*this).getMiniData();
    bool checkState=getParameterNum()==5,keepState=false;if(checkState) keepState=contract.getBoolParam(4,*this).isTrue();
    const auto& coins=contract.getTransaction().getAllOutputs();if(index<0||size_t(index)>=coins.size()) throw ExecutionException("Output out of range "+std::to_string(index)+"/"+std::to_string(coins.size()));const auto& coin=coins[index];
    auto actual=coin.getAmount();if(!coin.getTokenID().isEqual(Token::TOKENID_MINIMA)) {if(!coin.getToken()) throw ExecutionException("No token specified @ Output coin "+std::to_string(index)+" "+coin.getTokenID().to0xString());actual=coin.getToken()->getScaledTokenAmount(coin.getAmount());}
    bool result=address.isEqual(coin.getAddress())&&token.isEqual(coin.getTokenID())&&amount.isEqual(actual);result=result&&(!checkState||coin.storeState()==keepState);
    if(!result) {auto flag=[](bool value){return value?"true":"false";};contract.traceLog("VERIFYOUT failed @ ouptut "+std::to_string(index)+" found (address:"+coin.getAddress().to0xString()+" amount:"+actual.toString()+" tokenid:"+coin.getTokenID().to0xString()+" keepstate:"+flag(coin.storeState())+" ) expected (address:"+address.to0xString()+" amount:"+amount.toString()+" tokenid:"+token.to0xString()+" keepstate:"+flag(keepState)+") CheckKeepState:"+flag(checkState));}return std::make_shared<BooleanValue>(result);
}
}
