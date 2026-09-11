#include "org/minima/system/commands/help.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "support/JavaExceptions.hpp"
#include <iostream>
#include <set>
using namespace minima;using namespace minima::json;
static const JSONObject& object(const JSONValue& value){return *std::get<std::shared_ptr<JSONObject>>(value.value);}
static std::optional<std::string> nullable(const JSONValue& value){return value.isNull()?std::nullopt:std::optional(value.asString());}
class Probe:public Command {
    std::optional<std::string> full;
public:
    Probe(std::optional<std::string> name,std::optional<std::string> shortHelp,std::optional<std::string> fullHelp):Command(name,shortHelp),full(fullHelp){}
    std::optional<std::string> getFullHelp() const override{return full;}
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<Probe>(getName(),getHelp(),full);}
    JSONValue runCommand() override{throw JavaError("java.lang.AssertionError","help must not execute target command");}
};
static JSONObject run(const JSONObject& q){
    JSONObject out;commands::help cmd;auto original=CommandRunner::ALL_COMMANDS[0];
    try{
        if(q.containsKey("probe")){const auto& p=object(q.get("probe"));CommandRunner::ALL_COMMANDS[0]=std::make_shared<Probe>(nullable(p.get("name")),nullable(p.get("help")),nullable(p.get("fullhelp")));}
        *cmd.getParams()=object(q.get("params"));auto op=q.getString("op");
        if(op=="registry"){std::set<std::string> names;for(auto c:CommandRunner::ALL_COMMANDS)names.insert(*c->getName());JSONArray values;for(auto name:names)values.add(name);out.put("names",values);}
        else if(op=="meta"){out.put("name",*cmd.getName());out.put("help",*cmd.getHelp());out.put("fullhelp",*cmd.getFullHelp());JSONArray params;for(auto p:cmd.getValidParams())params.add(p);out.put("params",params);auto fresh=cmd.getFunction();out.put("fresh",fresh->getJSONReply());out.put("distinct",fresh.get()!=&cmd&&fresh->getParams()!=cmd.getParams());}
        else if(op=="pad"){std::optional<std::u16string> text;if(!q.get("units").isNull()){text.emplace();for(const auto& unit:std::get<std::shared_ptr<JSONArray>>(q.get("units").value)->elements())text->push_back(char16_t(std::get<int64_t>(unit.value)));}auto value=cmd.getStrOfLength(int32_t(std::get<int64_t>(q.get("length").value)),text);JSONArray units;for(auto c:value)units.add(int(c));out.put("units",units);}
        else if(op=="runner")out.put("value",CommandRunner::getRunner().runMultiCommand(q.getString("caller"),q.getString("text")));
        else{auto reply=cmd.runCommand();if(op=="alias")cmd.getParams()->put("later","changed");out.put("value",reply);out.put("serialized",reply.toJSONString());}
    }catch(const CommandException& e){out.put("exception","org.minima.system.commands.CommandException");out.put("message",e.getMessage()?JSONValue(*e.getMessage()):JSONValue());}
    catch(const JavaNullPointerException&){out.put("exception","java.lang.NullPointerException");}
    catch(const JavaClassCastException&){out.put("exception","java.lang.ClassCastException");}
    catch(const JavaTypedException& e){out.put("exception",e.className());out.put("message",e.what());}
    catch(const std::exception& e){out.put("exception",javaExceptionText(e));}
    CommandRunner::ALL_COMMANDS[0]=original;return out;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}
