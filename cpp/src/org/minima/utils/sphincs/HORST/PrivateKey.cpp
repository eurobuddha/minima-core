#include "PrivateKey.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::sphincs::horst {
PrivateKey::PrivateKey(const MiniData* seed):mPrivateKeys(65536) {
    for(int i=0;i<getSize();++i) {
        if(seed)mPrivateKeys[i]=std::make_shared<MiniData>(Crypto::getInstance().hashAllObjects(*seed,MiniNumber(i)));
        else {
            // Java Crypto.hashAllObjects catches a null Streamable, logs it,
            // and returns null separately for each of the 65,536 derivations.
            JavaNullPointerException error("");
            MinimaLogger::log(std::make_shared<NativeThrowable>(error));
        }
    }
}
std::shared_ptr<MiniData> PrivateKey::getKey(int position) const {
    if(position<0||position>=getSize())throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(position)+" out of bounds for length "+std::to_string(getSize()));
    return mPrivateKeys[size_t(position)];
}
}
