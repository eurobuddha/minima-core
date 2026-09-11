#include "SPHINCSUtils.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "support/SPHINCSHelpers.hpp"
namespace minima::sphincs {
namespace {
template<class T> T& required(T* value) {if(!value)throw JavaNullPointerException("");return *value;}

}
const std::string SPHINCSUtils::KISSVM_SPHINCS_SCRIPT = "LET sphincspublickey=#USER_PUBLIC_KEY LET incoins=STRING(GETINID(0)) LET counter=1 WHILE counter LT @TOTIN DO LET incoins=incoins+STRING(GETINID(counter)) LET counter=INC(counter) ENDWHILE LET calcoutput=[LET returnvalue=STRING(GETOUTADDR($1))+[SPHINCS]+STRING(GETOUTAMT($1))+[SPHINCS]+STRING(GETOUTTOK($1))+STRING(GETOUTKEEPSTATE($1))] LET outcoins=FUNCTION(calcoutput 0) LET counter=1 WHILE counter LT @TOTOUT DO LET outcoins=outcoins+FUNCTION(calcoutput counter) LET counter=INC(counter) ENDWHILE LET hashedmessage=SHA3(STRING(@TOTIN)+[SPHINCS]+STRING(@TOTOUT)+[SPHINCS]+incoins+[COINJOIN]+outcoins) LET forsrootdata=STATE(101) ASSERT CHECKSIG(sphincspublickey forsrootdata STATE(100)) LET counter=0 WHILE counter LT 16 DO LET statepos=counter*5 LET horstroot=STATE(statepos) ASSERT PROOF(horstroot counter forsrootdata 120 STATE(statepos+1)) LET keypos=counter*2 LET ref=NUMBER(SUBSET(keypos keypos+2 hashedmessage)) ASSERT PROOF(SHA3(STATE(statepos+2)) ref horstroot 2147450880 STATE(statepos+3)) LET counter=INC(counter) ENDWHILE RETURN TRUE";
std::string SPHINCSUtils::getKISSVMScript(const MiniData* publicKey) {
    auto replacement=required(publicKey).to0xString();auto script=KISSVM_SPHINCS_SCRIPT;
    script.replace(script.find("#USER_PUBLIC_KEY"),std::string("#USER_PUBLIC_KEY").size(),replacement);return script;
}
std::string SPHINCSUtils::getOutCoinString(const Coin& coin) {
    return detail::formatOutputCoin(coin,"SPHINCS","SPHINCS");
}
MiniData SPHINCSUtils::calculateTransactionID(const Transaction* transaction) {
    const auto inputsOwner=required(transaction).getAllInputs();const auto& inputs=*inputsOwner;const auto outputsOwner=transaction->getAllOutputs();const auto& outputs=*outputsOwner;
    if(inputs.empty())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index 0 out of bounds for length 0");
    auto input=inputs[0].getCoinID().to0xString();for(size_t i=1;i<inputs.size();++i)input+=inputs[i].getCoinID().to0xString();
    std::string output;for(const auto& coin:outputs)output+=getOutCoinString(coin);
    auto text=std::to_string(inputs.size())+"SPHINCS"+std::to_string(outputs.size())+"SPHINCS"+input+"COINJOIN"+output;
    return MiniData(MiniString(text).getData()); // Deliberately unhashed source message bytes.
}
void SPHINCSUtils::setupTransaction(Transaction* transaction,const SPHINCSSignature* signature) {
    detail::installSignatureState(transaction,signature);
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);transaction->calculateTransactionID();
}
}
