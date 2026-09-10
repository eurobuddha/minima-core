#pragma once
#include "../../MinimaFunction.hpp"
namespace minima::kissvm {
class VERIFYOUT : public MinimaFunction {
public:
    VERIFYOUT():MinimaFunction("VERIFYOUT"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 4;}
    bool isRequiredMinimumParameterNumber() const override {return true;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<VERIFYOUT>();}
};
}
