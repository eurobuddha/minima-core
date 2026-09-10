// Native adaptation of the matching pinned Java test class.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "org/minima/kissvm/statements/commands/ASSERTstatement.hpp"
#include <stdexcept>
using namespace minima;
using namespace minima::kissvm;
static void require(bool ok) {if(!ok) throw std::runtime_error("ASSERTstatementTests assertion");}
int main() {
    std::vector<ValuePtr> values{std::make_shared<BooleanValue>(true),std::make_shared<BooleanValue>(false)};
    for(const auto* hex:{"","0x00","0x12345678","0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"}) values.push_back(std::make_shared<HexValue>(hex));
    for(int value:{-1,0,1}) values.push_back(std::make_shared<NumberValue>(value));
    for(const auto* text:{"","Hello World"}) values.push_back(std::make_shared<StringValue>(text));
    const std::vector<std::string> expected{"TRUE","FALSE","","0x00","0x12345678","0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF","-1","0","1","","Hello World"};
    for(size_t index=0;index<values.size();++index) {
        ASSERTstatement statement(std::make_shared<ConstantExpression>(values[index]));
        require(statement.toString()=="ASSERT "+expected[index]);
        Contract contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());
        bool rejected=false;
        try {statement.execute(contract);} catch(const ExecutionException&) {rejected=true;}
        require(rejected==(index>=2));
        require(contract.isSuccessSet()==(index==1));
        require(!contract.isSuccess());
    }
}
