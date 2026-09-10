#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class PREVSTATE : public MinimaFunction {
public:
    PREVSTATE():MinimaFunction("PREVSTATE"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 1;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<PREVSTATE>();}
    
};
}
