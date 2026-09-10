#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class EXISTS : public MinimaFunction {
public:
    EXISTS():MinimaFunction("EXISTS"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 1;}
    bool isRequiredMinimumParameterNumber() const override {return true;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<EXISTS>();}
    
};
}
