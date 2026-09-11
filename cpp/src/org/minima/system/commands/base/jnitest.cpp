#include "jnitest.hpp"
#include "org/minima/objects/TxPoW.hpp"
#include "support/JNICommandRuntime.hpp"
#include <bit>
namespace minima::commands {
const MiniNumber jnitest::START_NONCE_BYTES("100000000000000000.00000000000000000000000000000000000000001");
jnitest::jnitest():Command("jnitest","(maxattempts:) - Check the speed of header hashing with JNI calls. Defaults to 1000 hashes"){}
std::optional<std::string> jnitest::getFullHelp() const {return "\nnoncetest\n\nCheck the speed of hashing of this device. Defaults to 1 million hashes.\n\nReturns the time taken in milliseconds and speed in megahashes/second.\n\nE.g. A speed of 0.5 MH/s indicates 500000 hashes per second.\n\namount: (optional)\n    Number of hashes to execute.\n\nExamples:\n\nhashtest\n\nhashtest maxattempts:2000000\n";}
std::vector<std::string> jnitest::getValidParams() const {return {"testnonce", "maxattempts", "targetdifficulty"};}
std::string jnitest::outputByteArray(const std::vector<uint8_t>& bytes) {
    std::string out;for(auto byte:bytes)out+=std::to_string(int(std::bit_cast<int8_t>(byte)))+",";return out;
}
json::JSONValue jnitest::runCommand() {
    auto ret=getJSONReply();
    const int amount=getNumberParam("maxattempts",MiniNumber(1000)).getAsInt();
    auto targetOwner=existsParam("targetdifficulty")?std::make_shared<MiniData>(getDataParam("targetdifficulty")):Magic::MIN_TXPOW_WORK;
    auto& targetdiff=TxHeader::required(targetOwner);
    auto testnonce=getNumberParam("testnonce",MiniNumber("12345.54321"));
    auto testnoncedata=testnonce.serialise();
    TxPoW txp;txp.setHeaderBodyHash();txp.setNonce(START_NONCE_BYTES);
    auto data=txp.getTxHeader().serialise();
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
    resp.put("result",outputByteArray(*result));resp.put("nonce",json::JSONValue::raw(finalnonce.toString()));
    resp.put("millitime",timediff);resp.put("targetdifficulty",targetdiff.to0xString());
    resp.put("txpowid",txp.getTxPoWID());resp.put("success",foundvalidtxpow);
    ret.put("response",resp);return ret;
}
}
