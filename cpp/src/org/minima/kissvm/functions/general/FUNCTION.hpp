#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class FUNCTION : public MinimaFunction {
public:
    FUNCTION():MinimaFunction("FUNCTION"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 1;}
    bool isRequiredMinimumParameterNumber() const override {return true;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<FUNCTION>();}
    static constexpr const char* FUNCTION_RETURN="returnvalue";
};
}
