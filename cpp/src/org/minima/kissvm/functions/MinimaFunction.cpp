#include "MinimaFunction.hpp"
#include "../exceptions/ExecutionException.hpp"
#include "../exceptions/MinimaParseException.hpp"
#include "number/ABS.hpp"
#include "number/CEIL.hpp"
#include "number/DEC.hpp"
#include "number/FLOOR.hpp"
#include "number/INC.hpp"
#include "number/SQRT.hpp"
#include "number/MAX.hpp"
#include "number/MIN.hpp"
#include "number/POW.hpp"
#include "number/SIGDIG.hpp"
#include "hex/BITCOUNT.hpp"
#include "hex/BITGET.hpp"
#include "hex/BITSET.hpp"
#include "hex/CONCAT.hpp"
#include "hex/LEN.hpp"
#include "hex/REV.hpp"
#include "hex/SETLEN.hpp"
#include "hex/SUBSET.hpp"
#include "hex/OVERWRITE.hpp"
#include "cast/ASCII.hpp"
#include "cast/UTF8.hpp"
#include "cast/STRING.hpp"
#include "cast/BOOL.hpp"
#include "cast/HEX.hpp"
#include "cast/NUMBER.hpp"
#include "general/ADDRESS.hpp"
#include "general/EXISTS.hpp"
#include "general/GET.hpp"
#include "state/STATE.hpp"
#include "state/PREVSTATE.hpp"
#include "state/SAMESTATE.hpp"
#include "sigs/SIGNEDBY.hpp"
#include "sigs/MULTISIG.hpp"
#include "sigs/CHECKSIG.hpp"
#include "sha/SHA2.hpp"
#include "sha/SHA3.hpp"
#include "sha/PROOF.hpp"
#include "string/SUBSTR.hpp"
#include "string/REPLACE.hpp"
#include "string/REPLACEFIRST.hpp"
#include "general/FUNCTION.hpp"
#include "txn/input/GETINADDR.hpp"
#include "txn/input/GETINAMT.hpp"
#include "txn/input/GETINID.hpp"
#include "txn/input/GETINTOK.hpp"
#include "txn/input/SUMINPUTS.hpp"
#include "txn/input/VERIFYIN.hpp"
#include "txn/output/GETOUTADDR.hpp"
#include "txn/output/GETOUTAMT.hpp"
#include "txn/output/GETOUTKEEPSTATE.hpp"
#include "txn/output/GETOUTTOK.hpp"
#include "txn/output/SUMOUTPUTS.hpp"
#include "txn/output/VERIFYOUT.hpp"

namespace minima::kissvm {
namespace {std::string upper(std::string name) {for(auto& c:name) if(c>='a'&&c<='z') c-=32;return name;}}
MinimaFunction::MinimaFunction(std::string name):mName(upper(std::move(name))){}
MinimaFunction::~MinimaFunction()=default;
ExpressionPtr MinimaFunction::getParameter(int number) const {if(number>=getParameterNum()) throw ExecutionException("Parameter missing for "+getName()+" num:"+std::to_string(number));return mParameters.at(number);}
void MinimaFunction::checkIsOfType(const Value& value,int type) const {if((value.getValueType()&type)==0) throw ExecutionException("Parameter is incorrect type in "+getName()+" Found:"+Value::getValueTypeString(value.getValueType())+" @ "+value.toString());}
void MinimaFunction::checkExactParamNumber(int number) const {if(getParameterNum()!=number) throw ExecutionException("Function requires "+std::to_string(number)+" parameters");}
void MinimaFunction::checkMinParamNumber(int number) const {if(getParameterNum()<number) throw ExecutionException("Function requires minimum of "+std::to_string(number)+" parameters");}
void MinimaFunction::checkParamNumberCorrect() const {
    int size=getParameterNum(),required=requiredParams();
    if(isRequiredMinimumParameterNumber()) {if(size<required) throw MinimaParseException(getName()+" function requires a  minimum of "+std::to_string(required)+" parameters not "+std::to_string(size));}
    else if(size!=required) throw MinimaParseException(getName()+" function requires exactly "+std::to_string(required)+" parameters not "+std::to_string(size));
}
std::shared_ptr<MinimaFunction> MinimaFunction::getFunction(std::string name) {auto canonical=upper(name);for(const auto& function:allFunctions()) if(function->getName()==canonical) return function->getNewFunction();throw MinimaParseException("Invalid Function : "+name);}
std::vector<std::shared_ptr<MinimaFunction>>& MinimaFunction::allFunctions() {
    static std::vector<std::shared_ptr<MinimaFunction>> functions={
        std::make_shared<CONCAT>(),
        std::make_shared<LEN>(),
        std::make_shared<REV>(),
        std::make_shared<SUBSET>(),
        std::make_shared<GET>(),
        std::make_shared<EXISTS>(),
        std::make_shared<ADDRESS>(),
        std::make_shared<BOOL>(),
        std::make_shared<HEX>(),
        std::make_shared<NUMBER>(),
        std::make_shared<STRING>(),
        std::make_shared<ASCII>(),
        std::make_shared<UTF8>(),
        std::make_shared<ABS>(),
        std::make_shared<CEIL>(),
        std::make_shared<FLOOR>(),
        std::make_shared<MAX>(),
        std::make_shared<MIN>(),
        std::make_shared<DEC>(),
        std::make_shared<INC>(),
        std::make_shared<SIGDIG>(),
        std::make_shared<POW>(),
        std::make_shared<SQRT>(),
        std::make_shared<FUNCTION>(),
        std::make_shared<SUMINPUTS>(),
        std::make_shared<SUMOUTPUTS>(),
        std::make_shared<SETLEN>(),
        std::make_shared<REPLACE>(),
        std::make_shared<REPLACEFIRST>(),
        std::make_shared<SUBSTR>(),
        std::make_shared<OVERWRITE>(),
        std::make_shared<SHA2>(),
        std::make_shared<SHA3>(),
        std::make_shared<PROOF>(),
        std::make_shared<BITSET>(),
        std::make_shared<BITGET>(),
        std::make_shared<BITCOUNT>(),
        std::make_shared<SIGNEDBY>(),
        std::make_shared<MULTISIG>(),
        std::make_shared<CHECKSIG>(),
        std::make_shared<GETINADDR>(),
        std::make_shared<GETINAMT>(),
        std::make_shared<GETINID>(),
        std::make_shared<GETINTOK>(),
        std::make_shared<VERIFYIN>(),
        std::make_shared<GETOUTADDR>(),
        std::make_shared<GETOUTAMT>(),
        std::make_shared<GETOUTTOK>(),
        std::make_shared<GETOUTKEEPSTATE>(),
        std::make_shared<VERIFYOUT>(),
        std::make_shared<STATE>(),
        std::make_shared<PREVSTATE>(),
        std::make_shared<SAMESTATE>()
    };return functions;
}
}
