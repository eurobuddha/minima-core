#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class MIN : public MinimaFunction {
public:
    MIN():MinimaFunction("MIN"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 2;}
    bool isRequiredMinimumParameterNumber() const override {return true;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<MIN>();}
};
}
