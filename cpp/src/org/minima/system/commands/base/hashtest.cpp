#include "hashtest.hpp"
#include <bit>
#include "org/minima/system/brains/TxPoWMiner.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
namespace minima::commands {
hashtest::hashtest():Command("hashtest","(amount:) - Check the speed of hashing of this device. Defaults to 1 million hashes"){}
std::optional<std::string> hashtest::getFullHelp() const {return std::string(
"\nhashtest\n"
"\n"
"Check the speed of hashing of this device. Defaults to 1 million hashes.\n"
"\n"
"Returns the time taken in milliseconds and speed in megahashes/second.\n"
"\n"
"E.g. A speed of 0.5 MH/s indicates 500000 hashes per second.\n"
"\n"
"amount: (optional)\n"
"    Number of hashes to execute.\n"
"\n"
"Examples:\n"
"\n"
"hashtest\n"
"\n"
"hashtest amount:2000000\n");}
json::JSONValue hashtest::runCommand(){
    auto result=getJSONReply();auto hashes=getNumberParam("amount",MiniNumber::MILLION);
    auto start=TimerMessage::currentTimeMillis();auto speed=TxPoWMiner::calculateHashSpeed(hashes);
    auto elapsed=std::bit_cast<int64_t>(uint64_t(TimerMessage::currentTimeMillis())-uint64_t(start));
    MinimaLogger::log("Speed : "+speed.toString());
    auto megaspeed=speed.div(MiniNumber::MILLION).setSignificantDigits(4);
    json::JSONObject response;response.put("hashes",json::JSONValue::raw(hashes.toString()));response.put("millitime",elapsed);response.put("speed",megaspeed.toString()+" MH/s");
    result.put("response",response);return result;
}
}
