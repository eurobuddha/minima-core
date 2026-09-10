#include "automine.hpp"
namespace minima::commands {
automine::automine():Command("automine","[enable:true|false|single] - Simulate traffic"){}
json::JSONValue automine::runCommand(){
    auto reply=getJSONReply();
    // The pinned source reads this parameter, but all mining branches are
    // commented out. Preserve getter failures and leave valid params inherited.
    (void)getParam("enable",std::string{});
    return reply;
}
std::shared_ptr<Command> automine::getFunction() const{return std::make_shared<automine>();}
}
