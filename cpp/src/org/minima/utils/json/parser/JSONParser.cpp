#include "JSONParser.hpp"
#include <iterator>
namespace minima::json::parser {
void JSONParser::reset() {token=Yytoken(Yytoken::TYPE_EOF);status=S_INIT;handlerStatusStack.reset();}
void JSONParser::reset(const std::u16string& text) {lexer.yyreset(text);reset();}
void JSONParser::nextToken() {token=lexer.yylex().value_or(Yytoken(Yytoken::TYPE_EOF));}
void JSONParser::unexpected() {status=S_IN_ERROR;throw ParseException(getPosition(),ParseException::ERROR_UNEXPECTED_TOKEN,token.toString());}
JSONValue JSONParser::parse(std::istream& input,ContainerFactory* factory) {
    std::string text{std::istreambuf_iterator<char>(input),{}};if(input.bad()) throw std::ios_base::failure("JSON input failure");return parse(text,factory);
}
JSONValue JSONParser::parse(const std::u16string& text,ContainerFactory* factory) {
    reset(text);
    struct Frame {int state;JSONValue value;std::u16string key;};
    std::vector<Frame> stack;JSONValue result;
    auto container=[&](int type) {
        JSONValue value;
        if(type==Yytoken::TYPE_LEFT_BRACE) {auto object=factory?factory->createObjectContainer():nullptr;value.value=object?object:std::make_shared<JSONObject>();}
        else {auto array=factory?factory->creatArrayContainer():nullptr;value.value=array?array:std::make_shared<JSONArray>();}
        return value;
    };
    while(true) {
        nextToken();
        if(status==S_IN_FINISHED_VALUE) {if(token.type==Yytoken::TYPE_EOF) return result;unexpected();}
        bool nested=token.type==Yytoken::TYPE_LEFT_BRACE||token.type==Yytoken::TYPE_LEFT_SQUARE;
        if(status==S_INIT) {
            if(token.type!=Yytoken::TYPE_VALUE&&!nested) unexpected();result=nested?container(token.type):token.value;
            if(!nested) {status=S_IN_FINISHED_VALUE;continue;}
            status=token.type==Yytoken::TYPE_LEFT_BRACE?S_IN_OBJECT:S_IN_ARRAY;stack.push_back({status,result,{}});continue;
        }
        auto& frame=stack.back();
        if(frame.state==S_IN_OBJECT) {
            if(token.type==Yytoken::TYPE_COMMA) continue;
            if(token.type==Yytoken::TYPE_VALUE&&token.value.isString()) {frame.key=token.value.asUTF16();frame.state=S_PASSED_PAIR_KEY;continue;}
            if(token.type!=Yytoken::TYPE_RIGHT_BRACE) unexpected();
        } else if(frame.state==S_IN_ARRAY&&token.type==Yytoken::TYPE_RIGHT_SQUARE) {
            // Close the current array below.
        } else {
            if(frame.state==S_PASSED_PAIR_KEY&&token.type==Yytoken::TYPE_COLON) continue;
            if(frame.state==S_IN_ARRAY&&token.type==Yytoken::TYPE_COMMA) continue;
            if(token.type!=Yytoken::TYPE_VALUE&&!nested) unexpected();auto value=nested?container(token.type):token.value;
            if(frame.state==S_PASSED_PAIR_KEY) {std::get<std::shared_ptr<JSONObject>>(frame.value.value)->put(frame.key,value);frame.state=S_IN_OBJECT;}
            else std::get<std::shared_ptr<JSONArray>>(frame.value.value)->add(value);
            if(nested) stack.push_back({token.type==Yytoken::TYPE_LEFT_BRACE?S_IN_OBJECT:S_IN_ARRAY,value,{}});
            continue;
        }
        stack.pop_back();if(stack.empty()) status=S_IN_FINISHED_VALUE;
    }
}
void JSONParser::parse(std::istream& input,ContentHandler& handler,bool resume) {
    if(resume&&handlerStatusStack) {parse(std::u16string{},handler,true);return;}
    std::string text{std::istreambuf_iterator<char>(input),{}};if(input.bad()) throw std::ios_base::failure("JSON input failure");parse(text,handler,resume);
}
void JSONParser::parse(const std::u16string& text,ContentHandler& handler,bool resume) {
    if(!resume||!handlerStatusStack) {reset(text);handlerStatusStack.emplace();}
    auto& stack=*handlerStatusStack;
    auto pop=[&]() {stack.pop_back();status=stack.empty()?S_IN_ERROR:stack.back();};
    auto close=[&]() {if(stack.size()>1) pop();else status=S_IN_FINISHED_VALUE;};
    try {
        while(true) {
            switch(status) {
                case S_INIT:
                    handler.startJSON();nextToken();
                    if(token.type==Yytoken::TYPE_VALUE) {status=S_IN_FINISHED_VALUE;stack.push_back(status);if(!handler.primitive(token.value)) return;}
                    else if(token.type==Yytoken::TYPE_LEFT_BRACE) {status=S_IN_OBJECT;stack.push_back(status);if(!handler.startObject()) return;}
                    else if(token.type==Yytoken::TYPE_LEFT_SQUARE) {status=S_IN_ARRAY;stack.push_back(status);if(!handler.startArray()) return;}
                    else unexpected();break;
                case S_IN_FINISHED_VALUE:
                    nextToken();if(token.type!=Yytoken::TYPE_EOF) unexpected();handler.endJSON();status=S_END;return;
                case S_IN_OBJECT:
                    nextToken();
                    if(token.type==Yytoken::TYPE_COMMA) break;
                    if(token.type==Yytoken::TYPE_VALUE&&token.value.isString()) {status=S_PASSED_PAIR_KEY;stack.push_back(status);if(!handler.startObjectEntry(token.value.asUTF16())) return;}
                    else if(token.type==Yytoken::TYPE_RIGHT_BRACE) {close();if(!handler.endObject()) return;}
                    else unexpected();break;
                case S_PASSED_PAIR_KEY:
                    nextToken();
                    if(token.type==Yytoken::TYPE_COLON) break;
                    if(token.type==Yytoken::TYPE_VALUE) {pop();if(!handler.primitive(token.value)) return;if(!handler.endObjectEntry()) return;}
                    else if(token.type==Yytoken::TYPE_LEFT_SQUARE||token.type==Yytoken::TYPE_LEFT_BRACE) {
                        stack.pop_back();stack.push_back(S_IN_PAIR_VALUE);status=token.type==Yytoken::TYPE_LEFT_SQUARE?S_IN_ARRAY:S_IN_OBJECT;stack.push_back(status);
                        if(!(status==S_IN_ARRAY?handler.startArray():handler.startObject())) return;
                    } else unexpected();break;
                case S_IN_PAIR_VALUE:pop();if(!handler.endObjectEntry()) return;break;
                case S_IN_ARRAY:
                    nextToken();
                    if(token.type==Yytoken::TYPE_COMMA) break;
                    if(token.type==Yytoken::TYPE_VALUE) {if(!handler.primitive(token.value)) return;}
                    else if(token.type==Yytoken::TYPE_RIGHT_SQUARE) {close();if(!handler.endArray()) return;}
                    else if(token.type==Yytoken::TYPE_LEFT_SQUARE||token.type==Yytoken::TYPE_LEFT_BRACE) {
                        status=token.type==Yytoken::TYPE_LEFT_SQUARE?S_IN_ARRAY:S_IN_OBJECT;stack.push_back(status);
                        if(!(status==S_IN_ARRAY?handler.startArray():handler.startObject())) return;
                    } else unexpected();break;
                case S_END:return;
                default:unexpected();
            }
        }
    } catch(...) {status=S_IN_ERROR;throw;}
}
}
