#pragma once
#include "../MinimaFunction.hpp"
namespace minima::kissvm {
class REPLACE : public MinimaFunction {
public:
    static void main(const std::vector<std::string>* args=nullptr);
    REPLACE():MinimaFunction("REPLACE"){}
    ValuePtr runFunction(Contract& contract) override;
    int requiredParams() const override {return 3;}
    bool isRequiredMinimumParameterNumber() const override {return false;}
    std::shared_ptr<MinimaFunction> getNewFunction() const override {return std::make_shared<REPLACE>();}
    static std::u16string safeReplaceAll(const std::u16string& original,const std::u16string& search,const std::u16string& replacement);
    static std::string safeReplaceAll(const std::string& original,const std::string& search,const std::string& replacement);
    static std::u16string replaceFirst(const std::u16string& original,const std::u16string& search,const std::u16string& replacement);
};
}
