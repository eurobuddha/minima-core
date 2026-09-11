#include "TransactionTest.hpp"
#include "SPHINCS.hpp"
#include "support/SPHINCSHelpers.hpp"
#include "org/minima/kissvm/Contract.hpp"
namespace minima::sphincs {
void TransactionTest::log(const std::optional<std::string>& message) {SPHINCS::log(message);}
std::string TransactionTest::getOutCoinString(const Coin* coin) {
    return detail::formatOutputCoin(detail::required(coin),"AMOUNT","TOKENID");
}
void TransactionTest::main() {
    log("Start SPHINCS..");auto seed=MiniData::fromHex("0x0011223344");SPHINCS signer;signer.initSeed(seed);
    log("SPHINCS public key : "+signer.getPublicKey()->to0xString());
    log("SPHINCS total WOTS keys : "+signer.getTotalWotsKeys()->toDecimalString());
    const std::string script="LET sphincspublickey=0x30DE42D7DE7B2F7BC5700B8C7256DF551FF24567C5C69B8528C027155FF6822E LET incoins=STRING(GETINID(0)) LET counter=1 WHILE counter LT @TOTIN DO LET incoins=incoins+STRING(GETINID(counter)) LET counter=INC(counter) ENDWHILE LET calcoutput=[LET returnvalue=STRING(GETOUTADDR($1))+[AMOUNT]+STRING(GETOUTAMT($1))+[TOKENID]+STRING(GETOUTTOK($1))+STRING(GETOUTKEEPSTATE($1))] LET outcoins=FUNCTION(calcoutput 0) LET counter=1 WHILE counter LT @TOTOUT DO LET outcoins=outcoins+FUNCTION(calcoutput counter) LET counter=INC(counter) ENDWHILE LET message=STRING(@TOTIN)+[SPHINCS]+STRING(@TOTOUT)+[SPHINCS]+incoins+[COINJOIN]+outcoins LET hashedmessage=SHA3(STRING(@TOTIN)+[SPHINCS]+STRING(@TOTOUT)+[SPHINCS]+incoins+[COINJOIN]+outcoins) LET forsrootdata=STATE(101) ASSERT CHECKSIG(sphincspublickey forsrootdata STATE(100)) LET counter=0 WHILE counter LT 1 DO LET statepos=counter*5 LET horstroot=STATE(statepos) ASSERT PROOF(horstroot counter forsrootdata 120 STATE(statepos+1)) LET keypos=counter*2 LET ref=NUMBER(SUBSET(keypos keypos+2 hashedmessage)) ASSERT PROOF(SHA3(STATE(statepos+2)) ref horstroot 2147450880 STATE(statepos+3)) LET counter=INC(counter) ENDWHILE RETURN TRUE";
    auto transaction=std::make_shared<Transaction>();auto witness=std::make_shared<Witness>();
    Token token(MiniData::fromHex("0x00"),MiniNumber::ONE,MiniNumber::ONE,MiniString("paddy"),MiniString("RETURN TRUE"));
    std::vector<Coin> inputs,outputs;
    for(int i=0;i<7;++i) {
        Coin coin(MiniData::fromHex("0x0"+std::to_string(i)),MiniData::fromHex("0x00"),MiniNumber::ONE,token.getTokenID().value(),false);
        inputs.push_back(coin);transaction->addInput(coin);witness->addCoinProof(CoinProof(coin,MMRProof()));
    }
    for(int i=0;i<2;++i) {
        Coin coin(MiniData::fromHex("0xF"+std::to_string(i)),MiniData::fromHex("0xAA"),MiniNumber::ONE,token.getTokenID().value(),false);
        coin.setToken(token);transaction->addOutput(coin);outputs.push_back(coin);
    }
    auto input=inputs[0].getCoinID().to0xString();for(size_t i=1;i<inputs.size();++i)input+=inputs[i].getCoinID().to0xString();
    std::string output;for(const auto& coin:outputs)output+=getOutCoinString(coin);
    auto text=std::to_string(inputs.size())+"SPHINCS"+std::to_string(outputs.size())+"SPHINCS"+input+"COINJOIN"+output;
    log("Message Str : "+text);MiniData message(MiniString(text).getData());log("Message : "+message.to0xString());
    auto signature=signer.signMessage(message);detail::installSignatureState(transaction.get(),&signature);
    // Preserve this demo's fixed script key/one iteration and absence of the
    // SPHINCSUtils precomputeTransactionCoinID/calculateTransactionID calls.
    kissvm::Contract contract(script,std::vector<MiniData>{},witness,transaction,{},false);
    contract.setGlobalVariable("@TOTIN",std::make_shared<kissvm::NumberValue>(int64_t(transaction->getAllInputs().size())));
    contract.setGlobalVariable("@TOTOUT",std::make_shared<kissvm::NumberValue>(int64_t(transaction->getAllOutputs().size())));
    contract.run();log(std::string("Valid Contract : ")+(contract.isSuccess()?"true":"false"));
    log("Operations     : "+std::to_string(contract.getNumberOfInstructions()));
    log("Variables      : "+contract.getAllVariables().toJSONString());
}
}
