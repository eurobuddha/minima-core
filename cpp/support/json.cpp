#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
using namespace minima::json;
using namespace minima::json::parser;
struct Handler : ContentHandler {
    JSONArray events;int count=0,step;bool ended=false;
    explicit Handler(int step):step(step){}
    bool event(std::string type,JSONValue value=nullptr) {JSONArray row;row.add(type);row.add(value);events.add(row);return step==0||++count%step!=0;}
    void startJSON() override {event("startJSON");}
    void endJSON() override {event("endJSON");ended=true;}
    bool startObject() override {return event("startObject");}
    bool endObject() override {return event("endObject");}
    bool startObjectEntry(const std::u16string& key) override {return event("key",key);}
    bool endObjectEntry() override {return event("endEntry");}
    bool startArray() override {return event("startArray");}
    bool endArray() override {return event("endArray");}
    bool primitive(const JSONValue& value) override {return event("value",value);}
};
int main() {
    std::string line;
    while(std::getline(std::cin,line)) {
        std::string out;JSONArray events;std::optional<Handler> handler;
        try {
            std::istringstream input(line);std::string command,hex;int mode;input>>command>>mode>>hex;auto bytes=BaseConverter::decode16(hex);auto text=MiniString(std::span<const uint8_t>(bytes)).toString();
            if(command=="lex") {Yylex lexer(unicode::decodeUTF8(text));while(auto token=lexer.yylex()) {JSONArray row;row.add(lexer.getPosition());row.add(token->type);row.add(token->value);events.add(row);}out=events.toJSONString()+"|"+std::to_string(lexer.getPosition());}
            else if(command=="stream") {handler.emplace(mode);JSONParser parser;parser.parse(text,*handler);for(int i=0;i<10000&&!handler->ended;++i) parser.parse("ignored on resume",*handler,true);out=handler->events.toJSONString()+"|"+std::to_string(parser.getPosition());}
            else {JSONParser parser;auto value=parser.parse(text);out=value.toJSONString()+"|"+std::to_string(parser.getPosition());}
        } catch(const ParseException& error) {out="PARSE|"+std::to_string(error.getErrorType())+"|"+std::to_string(error.getPosition())+"|"+error.getMessage()+"|"+(handler?handler->events:events).toJSONString();}
        catch(const std::exception&) {out="ERROR|"+(handler?handler->events:events).toJSONString();}
        std::cout<<BaseConverter::encode16(std::vector<uint8_t>(out.begin(),out.end()))<<'\n';
    }
}
