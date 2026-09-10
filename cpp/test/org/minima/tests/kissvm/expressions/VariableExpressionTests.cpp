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

    auto ctr=context();std::vector<ValuePtr> values{std::make_shared<BooleanValue>(true),std::make_shared<HexValue>(MiniData()),std::make_shared<NumberValue>(0),std::make_shared<StringValue>("[]")};
    std::vector<std::string> names{"BooleanValue","HEXValue","NumberValue","ScriptValue"};
    for(size_t i=0;i<names.size();++i) {VariableExpression expression(names[i]);rejected([&]{expression.getValue(ctr);});ctr.setVariable(names[i],values[i]);require(expression.getValue(ctr)==values[i]);require(expression.toString()=="variable:"+names[i]);}
}
