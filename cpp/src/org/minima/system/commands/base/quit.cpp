#include "quit.hpp"
#include "org/minima/system/Main.hpp"
namespace minima::commands {
quit::quit():Command("quit","(compact:) - Shutdown Minima. Compact the Databases if you want"){}
std::optional<std::string> quit::getFullHelp() const {return std::string(
"\nquit\n"
"\n"
"Shutdown Minima safely.\n"
"\n"
"Ensure you have a backup before shutting down.\n"
"\n"
"Examples:\n"
"\n"
"quit\n"
"\n"
"quit compact:true\n");}
json::JSONValue quit::runCommand(){
    auto result=getJSONReply();
    bool compact=getBooleanParam("compact",false);
    if(Main::getInstance())sqlRequired(Main::getInstance()).shutdown(compact);
    result.put("message","Shutdown complete");return result;
}
}
