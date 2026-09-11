#pragma once
#include "EncryptDecrypt.hpp"
#include "support/ByteIO.hpp"
namespace minima {
class CryptoPackage {
    std::optional<MiniData> mIvParam,mSecret,mData;
public:
    static void main(const std::vector<std::string>* args);
    void encrypt(std::span<const uint8_t> data,std::span<const uint8_t> publicKey);
    std::vector<uint8_t> decrypt(std::span<const uint8_t> privateKey) const;
    MiniData getCompleteEncryptedData() const {return MiniData(serialise());}
    void ConvertMiniDataVersion(const MiniData& data);
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static CryptoPackage deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
