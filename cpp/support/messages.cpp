#include "org/minima/utils/messages/TimerMessage.hpp"
#include "org/minima/utils/messages/MessageStack.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
struct Stack : MessageStack {auto pop(){return getNextMessage();}};
static Message::Key key(const JSONValue& value){return value.isNull()?std::nullopt:Message::Key(value.asString());}
static int64_t num(const JSONObject& o,const std::string& k){return std::get<int64_t>(o.get(k).value);}
static MessageObject value(const JSONObject& o){auto type=o.getString("type");auto v=o.get("value");if(type=="null"||v.isNull())return nullptr;if(type=="int")return std::bit_cast<int32_t>(uint32_t(num(o,"value")));if(type=="float")return std::bit_cast<float>(uint32_t(num(o,"value")));if(type=="bool")return v.asBool();if(type=="string")return v.asString();throw std::invalid_argument(type);}
static auto run(const JSONObject& req){std::vector<std::shared_ptr<Message>> messages;Stack stack;JSONArray output;
    for(const auto& op:std::get<std::shared_ptr<JSONArray>>(req.get("ops").value)->elements()){
        const auto& o=*std::get<std::shared_ptr<JSONObject>>(op.value);JSONValue result="OK";
        try{auto cmd=o.getString("cmd");auto m=o.containsKey("id")?messages.at(size_t(num(o,"id"))):nullptr;auto k=key(o.get("key"));
            if(cmd=="new")messages.push_back(o.containsKey("value")?std::make_shared<Message>(key(o.get("value"))):std::make_shared<Message>());
            else if(cmd=="timer"){auto t=std::make_shared<TimerMessage>(num(o,"delay"),m?m->getMessageType():key(o.get("value")),1000);if(m){// Preserve the shared payload with a deterministic clock.
                struct Timed : TimerMessage {Timed(int64_t delay,const std::shared_ptr<Message>& message):TimerMessage(delay,message->getMessageType(),1000){mContents=message->getAllContents();}};
                t=std::make_shared<Timed>(num(o,"delay"),m);
            }messages.push_back(t);}
            else if(!m&&(cmd=="put"||cmd=="putraw"||cmd=="remove"||cmd=="clearcontents"||cmd=="settype"||cmd=="is"||cmd=="exists"||cmd=="get"))throw std::invalid_argument("Null message");
            else if(cmd=="put"||cmd=="putraw"){auto v=value(o);if(cmd=="put"&&o.getString("type")=="string")m->addString(k,key(o.get("value")));else m->addObject(k,v);}
            else if(cmd=="remove"){auto map=m->getAllContents();auto v=k?map->remove(*k):map->remove(nullptr);result=v?v->toString():"null";}
            else if(cmd=="clearcontents")m->getAllContents()->clear();
            else if(cmd=="settype")m->setMessageType(key(o.get("value")));
            else if(cmd=="is")result=m->isMessageType(key(o.get("value")));
            else if(cmd=="exists")result=m->exists(k);
            else if(cmd=="get"){auto type=o.getString("type");if(type=="int")result=std::to_string(m->getInteger(k));else if(type=="float")result=javaFloating(m->getFloat(k));else if(type=="bool")result=m->getBoolean(k)?"true":"false";else if(type=="default")result=m->getBoolean(k,o.get("value").asBool())?"true":"false";else if(type=="string"){auto s=m->getString(k);result=s?JSONValue(*s):JSONValue();}else result=m->getObject(k).toString();}
            else if(cmd=="post")stack.PostMessage(m);
            else if(cmd=="pop"){auto popped=stack.pop();auto it=std::find(messages.begin(),messages.end(),popped);result=popped?int64_t(it-messages.begin()):-1;}
            else if(cmd=="clear")stack.clear();
            else if(cmd=="except"){std::vector<Message::Key> keys;for(const auto& v:std::get<std::shared_ptr<JSONArray>>(o.get("value").value)->elements())keys.push_back(key(v));stack.clearExcept(keys);}
            else if(cmd=="contains")stack.clearExceptString(o.get("value").asString());
            else throw std::invalid_argument(cmd);
        }catch(const std::exception&){result="ERROR";}
        JSONObject state;state.put("result",result);state.put("size",stack.getSize());state.put("next",stack.isNextMessage());JSONArray list;for(const auto& m:messages){auto timer=std::dynamic_pointer_cast<TimerMessage>(m);list.add(timer?timer->toStringAt(100):m->toString());}state.put("messages",list);output.add(state);
    }return output;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto o=std::get<std::shared_ptr<JSONObject>>(parser::JSONParser().parse(line).value);std::cout<<run(*o).toJSONString()<<'\n';}catch(const std::exception&){std::cout<<"ERROR\n";}}
