#include "SPHINCSUtils.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "support/MiniDataVersion.hpp"
namespace minima::sphincs {
namespace {
template<class T> T& required(T* value) {if(!value)throw JavaNullPointerException("");return *value;}
template<class T> const T& at(const std::vector<std::shared_ptr<T>>& list,size_t index) {
    if(index>=list.size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(index)+" out of bounds for length "+std::to_string(list.size()));
    return required(list[index].get());
}
template<class T> std::string versionHex(const T& object) {
    auto version=miniDataVersion(object);if(!version)throw JavaNullPointerException("");return version->to0xString();
}
}
const std::string SPHINCSUtils::KISSVM_SPHINCS_SCRIPT = "LET sphincspublickey=#USER_PUBLIC_KEY LET incoins=STRING(GETINID(0)) LET counter=1 WHILE counter LT @TOTIN DO LET incoins=incoins+STRING(GETINID(counter)) LET counter=INC(counter) ENDWHILE LET calcoutput=[LET returnvalue=STRING(GETOUTADDR($1))+[SPHINCS]+STRING(GETOUTAMT($1))+[SPHINCS]+STRING(GETOUTTOK($1))+STRING(GETOUTKEEPSTATE($1))] LET outcoins=FUNCTION(calcoutput 0) LET counter=1 WHILE counter LT @TOTOUT DO LET outcoins=outcoins+FUNCTION(calcoutput counter) LET counter=INC(counter) ENDWHILE LET hashedmessage=SHA3(STRING(@TOTIN)+[SPHINCS]+STRING(@TOTOUT)+[SPHINCS]+incoins+[COINJOIN]+outcoins) LET forsrootdata=STATE(101) ASSERT CHECKSIG(sphincspublickey forsrootdata STATE(100)) LET counter=0 WHILE counter LT 16 DO LET statepos=counter*5 LET horstroot=STATE(statepos) ASSERT PROOF(horstroot counter forsrootdata 120 STATE(statepos+1)) LET keypos=counter*2 LET ref=NUMBER(SUBSET(keypos keypos+2 hashedmessage)) ASSERT PROOF(SHA3(STATE(statepos+2)) ref horstroot 2147450880 STATE(statepos+3)) LET counter=INC(counter) ENDWHILE RETURN TRUE";
std::string SPHINCSUtils::getKISSVMScript(const MiniData* publicKey) {
    auto replacement=required(publicKey).to0xString();auto script=KISSVM_SPHINCS_SCRIPT;
    script.replace(script.find("#USER_PUBLIC_KEY"),std::string("#USER_PUBLIC_KEY").size(),replacement);return script;
}
std::string SPHINCSUtils::getOutCoinString(const Coin& coin) {
    auto keep=coin.storeState()?"TRUE":"FALSE";auto address=coin.getAddress().to0xString();
    auto amount=coin.getTokenAmount().toString();auto token=coin.getTokenID().to0xString();
    return address+"SPHINCS"+amount+"SPHINCS"+token+keep;
}
MiniData SPHINCSUtils::calculateTransactionID(const Transaction* transaction) {
    const auto& inputs=required(transaction).getAllInputs();const auto& outputs=transaction->getAllOutputs();
    if(inputs.empty())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index 0 out of bounds for length 0");
    auto input=inputs[0].getCoinID().to0xString();for(size_t i=1;i<inputs.size();++i)input+=inputs[i].getCoinID().to0xString();
    std::string output;for(const auto& coin:outputs)output+=getOutCoinString(coin);
    auto text=std::to_string(inputs.size())+"SPHINCS"+std::to_string(outputs.size())+"SPHINCS"+input+"COINJOIN"+output;
    return MiniData(MiniString(text).getData()); // Deliberately unhashed source message bytes.
}
void SPHINCSUtils::setupTransaction(Transaction* transaction,const SPHINCSSignature* signature) {
    auto minisig=versionHex(required(required(signature).getWOTSSignature().get()));
    StateVariable wots(100,minisig);required(transaction).addStateVariable(std::move(wots));
    StateVariable root(101,required(signature->getFORSRoot().get()).getData().to0xString());transaction->addStateVariable(std::move(root));
    auto fors=signature->getFORSSignature();
    for(size_t i=0;i<16;++i) {
        int port=int(i*5);
        StateVariable innerRoot(port,at(*required(fors.get()).getHORSTRoots(),i).getData().to0xString());transaction->addStateVariable(std::move(innerRoot));
        auto outerProof=versionHex(at(*fors->getHORSTTreeProofs(),i));
        StateVariable outer(port+1,outerProof);transaction->addStateVariable(std::move(outer));
        StateVariable secret(port+2,at(*fors->getHORSTSignature()->getSignatureValues(),i).to0xString());transaction->addStateVariable(std::move(secret));
        auto innerProof=versionHex(at(*fors->getHORSTSignature()->getPublicKeyTreeProofs(),i));
        StateVariable inner(port+3,innerProof);transaction->addStateVariable(std::move(inner));
    }
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);transaction->calculateTransactionID();
}
}
