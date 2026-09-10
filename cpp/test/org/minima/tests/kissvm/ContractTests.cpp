// Native adaptation of the matching pinned Java test class.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "org/minima/kissvm/expressions/VariableExpression.hpp"
#include "org/minima/kissvm/expressions/GlobalExpression.hpp"
#include "org/minima/kissvm/expressions/OperatorExpression.hpp"
#include "org/minima/kissvm/expressions/FunctionExpression.hpp"
#include "org/minima/kissvm/functions/hex/CONCAT.hpp"
#include "org/minima/kissvm/functions/hex/LEN.hpp"
#include "org/minima/kissvm/functions/number/ABS.hpp"
#include "org/minima/kissvm/functions/number/INC.hpp"
#include "org/minima/kissvm/statements/StatementBlock.hpp"
#include "org/minima/kissvm/statements/commands/LETstatement.hpp"
#include "org/minima/kissvm/statements/commands/RETURNstatement.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok) {if(!ok) throw std::runtime_error("KISS mapped test assertion");}
template<class F> static void rejected(F fn) {bool threw=false;try {fn();}catch(const ExecutionException&) {threw=true;}require(threw);}
static Contract context(const std::string& script="") {return Contract(script,std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
int main() {

    auto ctr=context();require(ctr.isParseOK()&&!ctr.isException()&&ctr.getException().empty());require(!ctr.isTrace()&&!ctr.isSuccess()&&!ctr.isSuccessSet()&&ctr.isMonotonic());require(ctr.getNumberOfInstructions()==0&&ctr.getGlobalVariables()->size()==0&&ctr.getAllVariables().size()==0);
    auto pass=context("RETURN TRUE");pass.run();require(pass.isSuccessSet()&&pass.isSuccess());pass.setRETURNValue(false);require(pass.isSuccess());
    auto invalid=context("lorem ipsum");invalid.run();require(!invalid.isParseOK()&&invalid.isException());require(invalid.getException()=="org.minima.kissvm.exceptions.MinimaParseException: Invalid Token where there should be a COMMMAND - lorem");
    auto globals=std::make_shared<JavaHashtable<ValuePtr>>();globals->put("@SCRIPT",std::make_shared<StringValue>("0"));globals->put("@INPUT",std::make_shared<NumberValue>(3));ctr.setAllGlobalVariables(globals);require(ctr.getGlobalVariables()==globals&&ctr.getGlobal("@INPUT")->toString()=="3");
    ctr.setVariable("BooleanValue",Value::getValue("TRUE"));ctr.setVariable("HEXValue",Value::getValue("0x12"));ctr.setVariable("NumberValue",Value::getValue("0"));ctr.setVariable("ScriptValue",Value::getValue("[]"));ctr.setVariable("Array,0,1,2,3",Value::getValue("0"));require(ctr.getAllVariables().size()==5);
    std::string script;for(int i=0;i<1025;++i) script+="LET a="+std::to_string(i)+" ";auto exhausted=context(script);exhausted.run();require(exhausted.isParseOK()&&exhausted.isException());require(exhausted.getException()=="org.minima.kissvm.exceptions.ExecutionException: MAX instruction number reached! 1025");
}
