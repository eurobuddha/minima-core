#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class CHECKSIG : public MinimaFunction {
public:
    CHECKSIG():MinimaFunction("CHECKSIG"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 3;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<CHECKSIG>();}
    
};
}
