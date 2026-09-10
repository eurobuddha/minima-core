#include "org/minima/utils/Maths.hpp"
#include "org/minima/utils/Stack.hpp"
#include "org/minima/utils/TimedRequest.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/JavaExceptions.hpp"
#include <iostream>
#include <bit>
using namespace minima;
using namespace minima::json;
static JSONValue run(JSONObject& request) {
    auto op=request.get("op").asString();
    if(op=="timed") {
        TimedRequest timed;JSONArray output;bool clock=true;
        for(const auto& item:std::get<std::shared_ptr<JSONArray>>(request.get("ops").value)->elements()) {
            auto& action=*std::get<std::shared_ptr<JSONObject>>(item.value);JSONValue result;
            try {
                auto name=action.get("op").asString();auto id=action.get("id").isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(action.get("id").asString()));
                if(name=="add") {
                    auto before=TimerMessage::currentTimeMillis();auto unit=std::make_shared<TimedRequest::RequestUnit>(id);auto after=TimerMessage::currentTimeMillis();clock &= before<=unit->mTimeMilli&&unit->mTimeMilli<=after;
                    if(action.containsKey("time"))unit->mTimeMilli=std::stoll(action.get("time").asString());
                    if(!timed.mPreviousRequests) throw std::runtime_error("Null list");timed.mPreviousRequests->push_back(unit);
                } else if(name=="send")result=timed.sendNow(id);
                else if(name=="nulllist")timed.mPreviousRequests.reset();
                else if(name=="reset")timed.mPreviousRequests=std::make_shared<std::vector<std::shared_ptr<TimedRequest::RequestUnit>>>();
                else {
                    if(!timed.mPreviousRequests) throw std::runtime_error("Null list");
                    if(name=="nullunit")timed.mPreviousRequests->push_back(nullptr);
                    else if(name=="clear")timed.mPreviousRequests->clear();
                    else if(name=="change") {auto unit=timed.mPreviousRequests->at(size_t(std::get<int64_t>(action.get("index").value)));if(!unit)throw std::runtime_error("Null unit");unit->mDataID=id;}
                    else throw std::invalid_argument("Unknown operation");
                }
            }catch(const std::exception&) {result="ERROR";}
            JSONObject state;state.put("result",result);state.put("size",timed.mPreviousRequests?int(timed.mPreviousRequests->size()):-1);output.add(state);
        }
        JSONObject result;result.put("clock",clock);result.put("results",output);return result;
    }
    if(op=="versions") return Maths::compareVersions(request.get("a").asString(),request.get("b").asString());
    if(op=="super") {GlobalParams::MINIMA_CASCADE_LEVELS=int(std::get<int64_t>(request.get("levels").value));return Maths::getSuperLevel(MiniData::fromHex(request.get("a").asString()),MiniData::fromHex(request.get("b").asString()));}
    if(op=="log"||op=="logbig") {
        double result=op=="log"?Maths::log2(std::bit_cast<double>(uint64_t(std::stoull(request.get("a").asString())))):Maths::log2BI(BigInt(request.get("a").asString()));
        return std::to_string(std::isnan(result)?uint64_t(0x7ff8000000000000):std::bit_cast<uint64_t>(result));
    }
    Stack stack;JSONArray output;std::vector<std::shared_ptr<JSONValue>> values;
    for(const auto& value:std::get<std::shared_ptr<JSONArray>>(request.get("values").value)->elements()) values.push_back(std::make_shared<JSONValue>(value));
    for(const auto& item:std::get<std::shared_ptr<JSONArray>>(request.get("ops").value)->elements()) {
        auto& action=*std::get<std::shared_ptr<JSONObject>>(item.value);JSONValue result;
        try {
            auto name=action.get("op").asString();
            if(name=="push") {auto value=values.at(size_t(std::get<int64_t>(action.get("index").value)));stack.push(value->isNull()?MessageObject{}:MessageObject(value,std::function<std::string(const JSONValue&)>([](const auto& v){return v.toJSONString();})));}
            else if(name=="pop"||name=="peek") {auto value=name=="pop"?stack.pop():stack.peek();if(!value.isNull()) result=*value.as<std::shared_ptr<JSONValue>>();}
            else if(name=="mutate") std::get<std::shared_ptr<JSONObject>>(values.at(size_t(std::get<int64_t>(action.get("index").value)))->value)->put("text",action.get("text"));
            else throw std::invalid_argument("Unknown stack action");
        }catch(const std::exception&) {result="ERROR";}
        JSONObject state;state.put("result",result);state.put("empty",stack.isEmpty());output.add(state);
    }
    return output;
}
int main() {
    std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line))try {auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);std::cout<<run(*request).toJSONString()<<'\n';}
    catch(const JavaNumberFormatException& error) {std::cout<<JSONValue(std::string("NUMBER_ERROR:")+error.what()).toJSONString()<<'\n';}
    catch(const std::exception&) {std::cout<<"\"ERROR\"\n";}
}
