#include "DUMMYFUNCTION.hpp"
#include "org/minima/kissvm/values/BooleanValue.hpp"
namespace minima::kissvm::tests {
DUMMYFUNCTION::DUMMYFUNCTION():MinimaFunction("DUMMYFUNCTION"){}
ValuePtr DUMMYFUNCTION::runFunction(Contract&){return std::make_shared<BooleanValue>(true);}
std::shared_ptr<MinimaFunction> DUMMYFUNCTION::getNewFunction() const{return std::make_shared<DUMMYFUNCTION>();}
bool DUMMYFUNCTION::isRequiredMinimumParameterNumber() const{return true;}
int DUMMYFUNCTION::requiredParams() const{return 1;}
}
