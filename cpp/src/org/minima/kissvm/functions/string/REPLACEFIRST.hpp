#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class REPLACEFIRST : public MinimaFunction {
public:
    REPLACEFIRST():MinimaFunction("REPLACEFIRST"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 3;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<REPLACEFIRST>();}
    
};
}
