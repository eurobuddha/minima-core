#pragma once
#include "org/minima/utils/sphincs/SPHINCSSignature.hpp"
#include "org/minima/objects/Transaction.hpp"
#include "support/MiniDataVersion.hpp"
namespace minima::sphincs::detail {
template<class T> T& required(T* value) {if(!value)throw JavaNullPointerException("");return *value;}
template<class T> const T& at(const std::vector<std::shared_ptr<T>>& list,size_t index) {
    if(index>=list.size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(index)+" out of bounds for length "+std::to_string(list.size()));
    return required(list[index].get());
}
template<class T> std::string versionHex(const T& object) {
    auto version=miniDataVersion(object);if(!version)throw JavaNullPointerException("");return version->to0xString();
}
// The two pinned callers use different literal separators.
inline std::string formatOutputCoin(const Coin& coin,const std::string& amountSeparator,const std::string& tokenSeparator) {
    auto keep=coin.storeState()?"TRUE":"FALSE";auto address=coin.getAddress().to0xString();
    auto amount=coin.getTokenAmount().toString();auto token=coin.getTokenID().to0xString();
    return address+amountSeparator+amount+tokenSeparator+token+keep;
}
// Extracted unchanged from SPHINCSUtils. This installs state only; the caller
// decides whether to recompute coin IDs and the ordinary transaction ID.
inline void installSignatureState(Transaction* transaction,const SPHINCSSignature* signature) {
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
 }
}
