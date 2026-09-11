#include "Crypto.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
const MiniData Crypto::MAX_HASH(std::vector<uint8_t>(32,255));
const BigInt Crypto::MAX_VAL=BigInt::fromMagnitudeBytes(std::vector<uint8_t>(32,255));
std::shared_ptr<MiniData> Crypto::hashObjectOrNull(const Streamable& object) const {
    try {return std::make_shared<MiniData>(hashObject(object));}
    catch(const JavaError&) {throw;}
    catch(const std::bad_alloc&) {throw;}
    catch(const std::exception& error) {MinimaLogger::log(std::make_shared<NativeThrowable>(error));return nullptr;}
}
Crypto& Crypto::getInstance() { static Crypto instance; return instance; }
std::vector<uint8_t> Crypto::hashSHA2(std::span<const uint8_t> data) const { return MiniData::fromBytes(data.data(),data.size()).sha2().bytes(); }
std::vector<uint8_t> Crypto::hashData(std::span<const uint8_t> data) const { return MiniData::fromBytes(data.data(),data.size()).sha3().bytes(); }
}
