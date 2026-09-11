#include "Winternitz.hpp"
#include "support/ByteIO.hpp"
#include "support/JavaExceptions.hpp"
namespace minima {
// Bouncy Castle GMSS WinternitzOTSignature, w=8 and SHA3-256.
// w counts bits: 32 message chains + 2 checksum chains, each up to 255 hashes.
MiniData Winternitz::chain(MiniData value,int rounds) {for(int i=0;i<rounds;++i) value=value.sha3();return value;}
std::array<int,34> Winternitz::digits(const MiniData& data) {
    auto hash=data.sha3();std::array<int,34> result{};int checksum=32<<8;
    for(size_t i=0;i<32;++i) {result[i]=hash.bytes()[i];checksum-=result[i];}
    result[32]=checksum&255;result[33]=(checksum>>8)&255;return result;
}
Winternitz::Winternitz(const MiniData& seed) {
    if(seed.size()<32) throw std::invalid_argument("Winternitz seed requires 32 bytes");
    auto current=seed.subset(0,32).bytes();mPrivateKey.emplace();std::vector<uint8_t> publicParts;
    for(auto& element:*mPrivateKey) {
        element=MiniData(current).sha3();
        // GMSSRandom.nextSeed: little-endian seed <- seed + H(seed) + 1 mod 2^256.
        unsigned carry=1;
        for(size_t j=0;j<32;++j) {unsigned sum=current[j]+element.bytes()[j]+carry;current[j]=uint8_t(sum);carry=sum>>8;}
        wire::append(publicParts,chain(element,255).bytes());
    }
    mPublicKey=MiniData(std::move(publicParts)).sha3();
}
MiniData Winternitz::sign(const MiniData& data) const {
    auto values=digits(data);std::vector<uint8_t> out;
    const auto& key=mPrivateKey.value();
    for(size_t i=0;i<key.size();++i) wire::append(out,chain(key[i],values[i]).bytes());
    return MiniData(std::move(out));
}
bool Winternitz::verify(const MiniData& publicKey,const MiniData& data,const MiniData& signature) {
    // Java's wrapper constructs MiniData(null) if BC rejects the signature size.
    if(signature.size()!=34*32) throw JavaNullPointerException("");
    auto values=digits(data);std::vector<uint8_t> publicParts;
    for(size_t i=0;i<values.size();++i) wire::append(publicParts,chain(signature.subset(int(i*32),32),255-values[i]).bytes());
    return MiniData(std::move(publicParts)).sha3().isEqual(publicKey);
}
}
