#include "txncoinlock.hpp"
#include "txnaddamount.hpp"
namespace minima::commands {
txncoinlock::txncoinlock():Command("txncoinlock","(action:) - Lock coins used in un-broadcast transactions"){}
std::optional<std::string> txncoinlock::getFullHelp() const {return std::string(
"\txncoinlock\n"
"\n"
"Lock coins used in un-broadcast transactions.\n"
"\n"
"\n"
"action: (optional)\n"
"    lock - lock coins.\n"
"    unlock - unlock coins.\n"
"\n"
"Examples:\n"
"\n"
"txncoinlock\n"
"\n"
"txncoinlock action:lock\n"
"\n"
"txncoinlock action:unlock\n"
"\n");}
std::vector<std::string> txncoinlock::getValidParams() const {return {"action"};}
std::shared_ptr<Command> txncoinlock::getFunction() const {return std::make_shared<txncoinlock>();}
json::JSONValue txncoinlock::runCommand() {
    auto result=getJSONReply();auto action=getParam("action","");
    if(action=="lock")txnaddamount::enableCoinLock(true);
    else if(action=="unlock")txnaddamount::enableCoinLock(false);
    json::JSONObject response;response.put("coinslocked",txnaddamount::isCoinLockEnabled());
    result.put("response",response);return result;
}
}
