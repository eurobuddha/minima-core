#include "maths.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/objects/mmr/MMR.hpp"
namespace minima::commands {
maths::maths():Command("maths","[calculate:] (logs:)- Run maths with Minima precision MiniNumber"){}
std::optional<std::string> maths::getFullHelp() const {return std::string(
"\nmaths\n"
"\n"
"Run some maths with Minima MiniNUmber precision\n"
"\n"
"Returns the result - na d full logs if required.\n"
"\n"
"calculate:\n"
"    The maths you want calculated\n"
"\n"
"logs: (boolean)\n"
"    true or false if you want the logs.\n"
"\n"
"Examples:\n"
"\n"
"maths calculate:\"1+2 * (3/4)\"\n"
"\n"
"maths calculate:\"1+2 * SIGDIG(1 3/4)\" logs:true\n");}
std::vector<std::string> maths::getValidParams() const {return {"calculate","logs"};}
json::JSONValue maths::runCommand(){
    auto result=getJSONReply();auto calculate=getParam("calculate");bool logs=getBooleanParam("logs",false);
    kissvm::Contract contract("LET returnvalue = "+calculate,std::vector<MiniData>{},std::make_shared<Witness>(),std::make_shared<Transaction>(),{},false);
    contract.run();auto value=contract.getVariable("returnvalue");if(!value)throw std::runtime_error("Null maths returnvalue");
    json::JSONObject response;if(logs)response.put("logs",contract.getCompleteTraceLog());response.put("result",value->toString());result.put("response",response);return result;
}
std::shared_ptr<Command> maths::getFunction() const {return std::make_shared<maths>();}
}
