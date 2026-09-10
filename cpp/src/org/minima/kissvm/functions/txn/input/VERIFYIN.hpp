#pragma once
#include "../../MinimaFunction.hpp"
namespace minima::kissvm {
class VERIFYIN : public MinimaFunction {
public:
    VERIFYIN():MinimaFunction("VERIFYIN"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 4;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<VERIFYIN>();}
};
}
