#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class OVERWRITE : public MinimaFunction {
public:
    OVERWRITE():MinimaFunction("OVERWRITE"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 5;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<OVERWRITE>();}
    
};
}
