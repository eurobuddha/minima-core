#include "benchmark.hpp"
#include "org/minima/objects/TxPoW.hpp"
#include "support/JNICommandRuntime.hpp"
#include <bit>
namespace minima::commands {
const MiniNumber benchmark::START_NONCE_BYTES("100000000000000000.00000000000000000000000000000000000000001");
benchmark::benchmark():Command("benchmark","(hashes:) - Check the speed of header hashing with JNI and JAVA calls. Defaults to 1000 hashes"){}
std::optional<std::string> benchmark::getFullHelp() const {return "\nnoncetest\n\nCheck the speed of hashing of this device. Defaults to 1 million hashes.\n\nReturns the time taken in milliseconds and speed in megahashes/second.\n\nE.g. A speed of 0.5 MH/s indicates 500000 hashes per second.\n\namount: (optional)\n    Number of hashes to execute.\n\nExamples:\n\nhashtest\n\nbenchmark hashes:2000000\n";}
std::vector<std::string> benchmark::getValidParams() const {return {"hashes","testnonce", "maxattempts", "targetdifficulty"};}
std::string benchmark::outputByteArray(const std::vector<uint8_t>& bytes) {
    std::string out;for(auto byte:bytes)out+=std::to_string(int(std::bit_cast<int8_t>(byte)))+",";return out;
}
json::JSONValue benchmark::runCommand() {
    auto ret=getJSONReply();
    const int amount=getNumberParam("hashes",MiniNumber(1000)).getAsInt();
    auto targetOwner=existsParam("targetdifficulty")?std::make_shared<MiniData>(getDataParam("targetdifficulty")):Magic::MIN_TXPOW_WORK;
    auto& targetdiff=TxHeader::required(targetOwner);
    auto testnonce=getNumberParam("testnonce",MiniNumber("12345.54321"));
    auto testnoncedata=testnonce.serialise();
    TxPoW txp;txp.setHeaderBodyHash();txp.setNonce(START_NONCE_BYTES);
    auto data=txp.getTxHeader().serialise();
    auto datajava=txp.getTxHeader().serialise();
    const auto timenowjava=TimerMessage::currentTimeMillis();
    BigInt newnoncejava=BigInt::zero();
    for(int i=0;i<amount;++i) {
        auto noncebytesjava=newnoncejava.toJavaBytes();newnoncejava=newnoncejava+BigInt::one();
        JNICommandRuntime::copyNonce(noncebytesjava,datajava);
        auto hashedbytes=Crypto::getInstance().hashData(datajava);
    }
    const auto timediffjava=std::bit_cast<int64_t>(uint64_t(TimerMessage::currentTimeMillis())-uint64_t(timenowjava));
    auto newnonce=BigInt::zero();auto noncebytes=newnonce.toJavaBytes();
    JNICommandRuntime::copyNonce(noncebytes,data);
    JNICommandRuntime::sayHello();
    const auto timenow=TimerMessage::currentTimeMillis();
    auto diffbytes=targetdiff.getBytes();
    auto result=JNICommandRuntime::hashHeaderWithDiff(testnoncedata,amount,diffbytes,data);
    const auto timediff=std::bit_cast<int64_t>(uint64_t(TimerMessage::currentTimeMillis())-uint64_t(timenow));
    if(!result)throw JavaNullPointerException("null JNI result");
    size_t offset=0;auto finalnonce=MiniNumber::deserialise(*result,offset);
    txp.setNonce(finalnonce);txp.calculateTXPOWID();
    bool foundvalidtxpow=txp.getTxPoWIDData().isLess(targetdiff);
    json::JSONObject resp;
    resp.put("hashes",amount);
    json::JSONObject javaresp;javaresp.put("millitime",timediffjava);
    resp.put("java",javaresp);
    json::JSONObject jniresp;jniresp.put("millitime",timediff);
    (void)foundvalidtxpow; // Source computes this but does not return it.
    resp.put("jni",jniresp);
    ret.put("response",resp);return ret;
}
}
