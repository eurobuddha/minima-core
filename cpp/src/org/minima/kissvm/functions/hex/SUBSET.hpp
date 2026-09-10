#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class SUBSET : public MinimaFunction {
public:
    SUBSET():MinimaFunction("SUBSET"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 3;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<SUBSET>();}
    
};
}
