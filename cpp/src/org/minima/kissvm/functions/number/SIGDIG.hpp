#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class SIGDIG : public MinimaFunction {
public:
    SIGDIG():MinimaFunction("SIGDIG"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 2;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<SIGDIG>();}
};
}
