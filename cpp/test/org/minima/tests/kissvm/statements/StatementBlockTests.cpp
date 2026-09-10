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

    auto empty=context();StatementBlock({}).run(empty);require(!empty.isSuccessSet()&&!empty.isSuccess());
    for(bool flag:{true,false}) {auto ctr=context();StatementBlock block({std::make_shared<RETURNstatement>(std::make_shared<ConstantExpression>(Value::getValue(flag?"TRUE":"FALSE")))});block.run(ctr);require(ctr.isSuccessSet()&&ctr.isSuccess()==flag);}
    for(int number:{0,5}) {auto ctr=context();auto constant=std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(number));auto expression=std::make_shared<OperatorExpression>(constant,constant,OperatorExpression::OPERATOR_ADD);StatementBlock block({std::make_shared<LETstatement>("a",expression),std::make_shared<RETURNstatement>(std::make_shared<VariableExpression>("a"))});rejected([&]{block.run(ctr);});require(!ctr.isSuccessSet()&&!ctr.isSuccess());}
}
