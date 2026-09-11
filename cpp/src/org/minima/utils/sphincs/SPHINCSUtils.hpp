#pragma once
#include "SPHINCSSignature.hpp"
#include "org/minima/objects/Transaction.hpp"
namespace minima::sphincs {
class SPHINCSUtils {
    static std::string getOutCoinString(const Coin& coin);
public:
    static const std::string KISSVM_SPHINCS_SCRIPT;
    static std::string getKISSVMScript(const MiniData* publicKey);
    static std::string getKISSVMScript(const MiniData& publicKey) {return getKISSVMScript(&publicKey);}
    static MiniData calculateTransactionID(const Transaction* transaction);
    static MiniData calculateTransactionID(const Transaction& transaction) {return calculateTransactionID(&transaction);}
    static void setupTransaction(Transaction* transaction,const SPHINCSSignature* signature);
    static void setupTransaction(Transaction& transaction,const SPHINCSSignature& signature) {setupTransaction(&transaction,&signature);}
};
}
