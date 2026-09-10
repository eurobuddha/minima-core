#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class PROOF : public MinimaFunction {
public:
    PROOF():MinimaFunction("PROOF"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 5;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<PROOF>();}
    
};
}
