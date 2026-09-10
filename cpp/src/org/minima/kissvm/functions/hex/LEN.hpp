#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class LEN : public MinimaFunction {
public:
    LEN():MinimaFunction("LEN"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 1;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<LEN>();}
    
};
}
