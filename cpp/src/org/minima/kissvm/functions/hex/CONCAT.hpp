#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class CONCAT : public MinimaFunction {
public:
    CONCAT():MinimaFunction("CONCAT"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 2;}
    bool isRequiredMinimumParameterNumber() const override {return true;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<CONCAT>();}
    
};
}
