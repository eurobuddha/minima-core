#include "POW.hpp"
#include "../../Contract.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include <iostream>
#include <bit>
namespace minima::kissvm {
ValuePtr POW::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());auto exponent=contract.getNumberParam(0,*this);auto number=contract.getNumberParam(1,*this);const auto& value=exponent.getNumber();
    if(!value.floor().isEqual(value)) throw ExecutionException("POW must be to a whole Number");
    if(value.abs().isMoreEqual(MiniNumber::THOUSAND24)) throw ExecutionException("ABS POW exponent must be less than 1024");
    return std::make_shared<NumberValue>(number.getNumber().pow(value.getAsInt()));
}
void POW::main(const std::vector<std::string>*){
    MiniNumber value("0.01");auto start=TimerMessage::currentTimeMillis();
    for(int i=0;i<1000;++i){auto power=value.pow(1000);std::cout<<power.toString()<<std::endl;}
    auto elapsed=std::bit_cast<int64_t>(uint64_t(TimerMessage::currentTimeMillis())-uint64_t(start));
    std::cout<<"Time : "<<elapsed<<std::endl;
}

}
