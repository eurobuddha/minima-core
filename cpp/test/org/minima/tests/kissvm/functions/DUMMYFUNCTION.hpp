#pragma once
#include "org/minima/kissvm/functions/MinimaFunction.hpp"
namespace minima::kissvm::tests {
// Test-only function from the pinned source. It is not registered as a VM builtin.
class DUMMYFUNCTION : public MinimaFunction {
public:
    DUMMYFUNCTION();
    ValuePtr runFunction(Contract& contract) override;
    std::shared_ptr<MinimaFunction> getNewFunction() const override;
    bool isRequiredMinimumParameterNumber() const override;
    int requiredParams() const override;
};
}
