// Direct adaptation of upstream test/org/minima/objects/TransactionTests.java.
#include "org/minima/objects/Transaction.hpp"
#include <stdexcept>
using namespace minima;
static void require(bool condition) {if(!condition) throw std::runtime_error("TransactionTests assertion");}
int main() {
    Transaction t;
    require(t.isEmpty()&&t.getAllInputs().empty()&&t.getAllOutputs().empty());
    require(t.sumInputs().isZero()&&t.sumOutputs().isZero());
    auto json=t.toJSON();for(const auto& key:{"inputs","outputs","state","linkhash"}) require(json.containsKey(key));
    auto address=MiniData::fromHex("0x2B1B3FD8AD198A5C9B0A8B376C73766B73ACE408E0A2537B827EC8DCFF4133BC");
    t.addInput(Coin(Coin::COINID_OUTPUT,address,MiniNumber(5),Token::TOKENID_MINIMA));
    require(!t.isEmpty()&&t.sumInputs().isEqual(MiniNumber(5)));
    t.addOutput(Coin(Coin::COINID_OUTPUT,address,MiniNumber(2),Token::TOKENID_MINIMA));
    require(t.getAllOutputs().size()==1&&t.sumOutputs().isEqual(MiniNumber(2))&&t.checkValid());
    t.addOutput(Coin(Coin::COINID_OUTPUT,address,MiniNumber(3),Token::TOKENID_MINIMA));
    require(t.getAllOutputs().size()==2&&t.sumOutputs().isEqual(MiniNumber(5))&&t.checkValid());
    for(int i=0;i<256;++i) require(!t.stateExists(i));
    t.addStateVariable(StateVariable(1,"[HelloWorld]"));require(t.stateExists(1)&&t.getStateValue(1)->toString()=="[HelloWorld]");
    auto bytes=t.serialise();size_t offset=0;auto read=Transaction::deserialise(bytes,offset);
    require(offset==bytes.size()&&read.getAllOutputs().size()==2&&read.sumOutputs().isEqual(MiniNumber(5))&&read.checkValid());
}
