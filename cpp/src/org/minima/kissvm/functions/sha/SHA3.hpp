#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class SHA3 : public MinimaFunction {
public:
    SHA3():MinimaFunction("SHA3"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 1;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<SHA3>();}
    
};
}
