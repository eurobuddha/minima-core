// Native adaptation of the matching pinned Java test class.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "org/minima/kissvm/statements/commands/EXECstatement.hpp"
#include <stdexcept>
using namespace minima;
using namespace minima::kissvm;
static void require(bool ok) {if(!ok) throw std::runtime_error("EXECstatementTests assertion");}
int main() {
    const std::vector<std::string> scripts{"","RETURN TRUE","RETURN FALSE","Hello World"};
    for(size_t index=0;index<scripts.size();++index) {
        EXECstatement statement(std::make_shared<ConstantExpression>(std::make_shared<StringValue>(scripts[index])));
        require(statement.toString()=="EXEC "+scripts[index]);
        Contract contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());
        bool rejected=false;
        try {statement.execute(contract);} catch(const ExecutionException&) {rejected=true;}
        require(rejected==(index==3));
        require(contract.isSuccessSet()==(index==1||index==2));
        require(contract.isSuccess()==(index==1));
        require(contract.getNumberOfInstructions()==(index==1||index==2?3:1));
    }
}
