#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class BITGET : public MinimaFunction {
public:
    BITGET():MinimaFunction("BITGET"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 2;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<BITGET>();}
    
};
}
