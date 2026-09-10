#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class MULTISIG : public MinimaFunction {
public:
    MULTISIG():MinimaFunction("MULTISIG"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 2;}
    bool isRequiredMinimumParameterNumber() const override {return true;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<MULTISIG>();}
    
};
}
