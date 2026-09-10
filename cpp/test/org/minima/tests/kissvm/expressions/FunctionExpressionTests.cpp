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

    auto ctr=context();auto concat=std::make_shared<CONCAT>();concat->addParameter(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x01234567")));concat->addParameter(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x89")));
    auto length=std::make_shared<LEN>();length->addParameter(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x01234567")));
    auto absolute=std::make_shared<ABS>();absolute->addParameter(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(-10)));
    auto increment=std::make_shared<INC>();increment->addParameter(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(10)));
    std::vector<std::shared_ptr<MinimaFunction>> functions{concat,length,absolute,increment};std::vector<std::string> expected{"0x0123456789","4","10","11"};
    for(size_t i=0;i<functions.size();++i) {FunctionExpression expression(functions[i]);require(expression.getValue(ctr)->toString()==expected[i]);}
    require(FunctionExpression(concat).toString()=="function:CONCAT, params:[0x01234567, 0x89]");require(ctr.getStackDepth()==0);
}
