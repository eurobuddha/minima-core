#pragma once
#include "FORSSignature.hpp"
#include "org/minima/utils/sphincs/HORST/HORST.hpp"
#include <array>
namespace minima::sphincs::fors {
class FORS {
    std::shared_ptr<MMR> mForestHORS;
    std::array<std::shared_ptr<horst::HORST>,horst::HORST::NUMBER_OF_CHUNK_BYTES> mHORSTLeafNodes;
public:
    explicit FORS(const MiniData* seed);
    explicit FORS(const MiniData& seed):FORS(&seed){}
    std::shared_ptr<MMRData> getForsRoot() const {return mForestHORS->getRoot();}
    FORSSignature signMessage(const MiniData* message) const;
    FORSSignature signMessage(const MiniData& message) const {return signMessage(&message);}
    static bool verifySignature(const MiniData* message,const FORSSignature* signature,const MMRData* root);
    static bool verifySignature(const MiniData& message,const FORSSignature& signature,const MMRData& root) {return verifySignature(&message,&signature,&root);}
    static void log(const std::optional<std::string>& message);
    static void main();
};
}
