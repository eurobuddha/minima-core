#pragma once
#include "../expressions/Expression.hpp"
#include <vector>
namespace minima::kissvm {
class MinimaFunction {
    std::string mName;
    std::vector<ExpressionPtr> mParameters;
protected:
    void checkIsOfType(const Value& value,int type) const;
    void checkExactParamNumber(int number) const;
    void checkMinParamNumber(int number) const;
public:
    explicit MinimaFunction(std::string name);
    virtual ~MinimaFunction();
    static std::vector<std::shared_ptr<MinimaFunction>>& allFunctions();
    void addParameter(ExpressionPtr parameter) {mParameters.push_back(std::move(parameter));}
    ExpressionPtr getParameter(int number) const;
    int getParameterNum() const {return int(mParameters.size());}
    std::vector<ExpressionPtr>& getAllParameters() {return mParameters;}
    const std::vector<ExpressionPtr>& getAllParameters() const {return mParameters;}
    const std::string& getName() const {return mName;}
    virtual ValuePtr runFunction(Contract& contract)=0;
    virtual std::shared_ptr<MinimaFunction> getNewFunction() const=0;
    virtual int requiredParams() const=0;
    virtual bool isRequiredMinimumParameterNumber() const {return false;}
    void checkParamNumberCorrect() const;
    static std::shared_ptr<MinimaFunction> getFunction(std::string name);
};
}
