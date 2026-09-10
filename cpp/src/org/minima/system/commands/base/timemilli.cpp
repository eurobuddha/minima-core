#include "timemilli.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include "support/JavaDateString.hpp"
namespace minima::commands {
timemilli::timemilli():Command("timemilli","Returns the current time in milliseconds"){}
std::optional<std::string> timemilli::getFullHelp() const {return std::string("\timemilli\n\nReturn the current timemilli. Can go back minutes or hours.\n\nminutesback: (optional)\n    Go back this many minutes\n\nhoursback: (optional)\n    Go back this many hours\n\nExamples:\n\ntimemilli\n\ntimemilli minutesback:120\n\ntimemilli hoursback:24\n\n");}
json::JSONValue timemilli::runCommand(){
    auto result=getJSONReply();auto now=TimerMessage::currentTimeMillis();
    auto subtract=[&](const std::string& name,uint32_t factor){
        auto offset=std::bit_cast<int32_t>(factor*uint32_t(getNumberParam(name).getAsInt()));
        now=std::bit_cast<int64_t>(uint64_t(now)-uint64_t(int64_t(offset)));
    };
    if(existsParam("minutesback"))subtract("minutesback",60000);else if(existsParam("hoursback"))subtract("hoursback",3600000);
    json::JSONObject response;response.put("timemilli",now);response.put("date",javaDateString(now));result.put("response",response);return result;
}
}
