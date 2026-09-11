#include "org/minima/system/commands/tutorial.hpp"
#include "org/minima/system/commands/base/tutorial.hpp"
#include "org/minima/system/commands/whitepaper.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static const JSONObject& object(const JSONValue& v){return *std::get<std::shared_ptr<JSONObject>>(v.value);}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line)){
    auto q=object(JSONValue::parseWithException(line));JSONObject out;std::shared_ptr<Command> cmd;auto kind=q.getString("kind");
    if(kind=="tutorial")cmd=std::make_shared<commands::tutorial>();else if(kind=="base/tutorial")cmd=std::make_shared<commands::base::tutorial>();else cmd=std::make_shared<commands::whitepaper>();
    *cmd->getParams()=object(q.get("params"));auto op=q.getString("op");
    if(op=="meta"){out.put("name",cmd->getName().value());out.put("help",cmd->getHelp().value());out.put("fullhelp",cmd->getFullHelp().value());JSONArray params;for(auto p:cmd->getValidParams())params.add(p);out.put("params",params);out.put("fresh",cmd->getFunction()->getJSONReply());}
    else if(op=="runner")out.put("value",CommandRunner::getRunner().runMultiCommand(q.getString("caller"),q.getString("text")));
    else if(op=="constant")out.put("value",commands::whitepaper::WP);
    else{auto reply=cmd->runCommand();if(op=="alias")cmd->getParams()->put("later","changed");out.put("value",reply);}
    std::cout<<out.toJSONString()<<'\n';
}}
