#include "PeerHarness.hpp"
#include "org/minima/system/network/minima/NIOClientInfo.hpp"
#include "support/JavaDateString.hpp"
#include "org/minima/utils/Crypto.hpp"
#include <regex>
using namespace minima;
static JSONValue data(const std::vector<uint8_t>* value){if(!value)return {};JSONObject out;out.put("size",int64_t(value->size()));out.put("hash",MiniData(Crypto::getInstance().hashSHA2(*value)).to0xString());return out;}
struct Channel final : NIOSocket {
    std::deque<uint8_t> input;std::vector<uint8_t> output;int32_t rcap=8192,wcap=8192,reads=0,writes=0,closes=0;bool open=true,eof=false,readError=false,writeError=false;
    void feed(const std::vector<uint8_t>& bytes){input.insert(input.end(),bytes.begin(),bytes.end());}
    int32_t read(std::span<uint8_t> target) override{++reads;if(!open)throw wire::IOException("Closed");if(readError)throw wire::IOException("Read failure");if(target.empty())return 0;if(input.empty()&&eof)return -1;auto count=std::min({input.size(),target.size(),size_t(rcap)});for(size_t i=0;i<count;++i){target[i]=input.front();input.pop_front();}return int32_t(count);}
    int32_t write(std::span<const uint8_t> source) override{++writes;if(!open)throw wire::IOException("Closed");if(writeError)throw wire::IOException("Write failure");auto count=std::min(source.size(),size_t(wcap));output.insert(output.end(),source.begin(),source.begin()+count);return int32_t(count);}
    void close() override{if(open){++closes;open=false;}}
};
struct Key final : NIOSelectionKey {
    bool valid=true,cancelError=false,interestError=false,wakeError=false;int32_t interest=OP_READ,cancels=0,wakes=0;
    bool isValid() const override{return valid;}
    void interestOps(int32_t value) override{if(!valid||interestError)throw std::runtime_error("Interest failure");interest=value;}
    void cancel() override{++cancels;if(cancelError)throw std::runtime_error("Cancel failure");valid=false;}
    void wakeup() override{if(wakeError)throw std::runtime_error("Wake failure");++wakes;}
};
namespace minima {
struct NIOClientFixture {
    static void uid(NIOClient& client){client.mUID="fixture";}
    static void connectedTime(NIOClient& client,int64_t value){client.mTimeConnected=value;}
    static void initialTimes(NIOClient& client,int64_t value){client.mTimeConnected=value;client.mLastMessageRead=value;}
    static JSONValue buffer(const std::unique_ptr<NIOClient::Buffer>& buffer){if(!buffer)return {};JSONObject out;out.put("position",buffer->position);out.put("limit",buffer->limit);out.put("hash",MiniData(buffer->bytes).sha2().to0xString());return out;}
    static JSONObject state(const NIOClient& client){JSONObject out;out.put("in",buffer(client.mBufferIn));out.put("out",buffer(client.mBufferOut));out.put("mReadCurrentPosition",client.mReadCurrentPosition);out.put("mReadCurrentLimit",client.mReadCurrentLimit);out.put("mWritePosition",client.mWritePosition);out.put("mWriteLimit",client.mWriteLimit);out.put("mWriteStart",client.mWriteStart);
        if(client.mReadByteArray){auto bytes=client.mReadByteArray->toByteArray();out.put("stream",data(&bytes));}else out.put("stream",nullptr);out.put("temp",client.mBufferIn?data(&client.mReadByteArrayTemp):JSONValue{});out.put("writeData",client.mWriteData?data(&client.mWriteData->bytes()):JSONValue{});
        if(client.mMessages){JSONArray queue;for(const auto& value:*client.mMessages)queue.add(value?data(&value->bytes()):JSONValue{});out.put("queue",queue);}else out.put("queue",nullptr);return out;
    }
};
}
static MessageObject extra(const JSONValue& value){if(value.isNull())return {};auto shared=std::make_shared<JSONValue>(value);return MessageObject(shared,std::function<std::string(const JSONValue&)>([](const JSONValue& v){return v.toJSONString();}));}
static JSONValue extraValue(const MessageObject& value){return value.isNull()?JSONValue{}:*value.as<std::shared_ptr<JSONValue>>();}
static JSONObject snapshot(const NIOClient& client,const Channel& channel,const Key& key,NIOTraffic& traffic,const std::vector<std::shared_ptr<Message>>& posted,const std::shared_ptr<NIOClientInfo>& info){
    JSONObject out;out.put("json",client.toJSON());out.put("full",client.getFullAddress());out.put("fullMinima",client.getFullMinimaAddress());out.put("outgoing",client.isOutgoing());out.put("last",client.getLastReadTime());out.put("attempts",client.getConnectAttempts());out.put("p2p",client.hasReceivedP2PGreeting());out.put("disconnected",client.hasMaximaDiscxonnected());try{out.put("maximaClient",client.isMaximaClient());}catch(const std::exception&){out.put("maximaClient","ERROR");}try{out.put("maximaMLS",client.isMaximaMLS());}catch(const std::exception&){out.put("maximaMLS","ERROR");}out.put("extra",extraValue(client.getExtraData()));
    auto internal=NIOClientFixture::state(client);for(const auto& [name,value]:internal.entries())out.put(name,value);
    JSONArray messages;for(const auto& message:posted){JSONObject msg;msg.put("type",nullable(message->getMessageType()));msg.put("uid",nullable(message->getString("uid")));msg.put("host",nullable(message->getString("fullhost")));msg.put("data",data(&message->getObject("data").as<std::shared_ptr<MiniData>>()->bytes()));messages.add(msg);}out.put("posted",messages);out.put("totalRead",traffic.getTotalRead());out.put("totalWrite",traffic.getTotalWrite());out.put("validKey",key.valid);out.put("interest",key.interest);out.put("wakes",key.wakes);out.put("cancels",key.cancels);out.put("channelOpen",channel.open);out.put("reads",channel.reads);out.put("writes",channel.writes);out.put("closes",channel.closes);out.put("input",int64_t(channel.input.size()));out.put("output",data(&channel.output));out.put("info",info?JSONValue(info->toJSON()):JSONValue{});
    if(info){try{out.put("infoTime",info->getTimeConnected());}catch(const std::exception&){out.put("infoTime","ERROR");}try{out.put("infoExtra",extraValue(info->getExtraData()));}catch(const std::exception&){out.put("infoExtra","ERROR");}out.put("infoValid",info->ismValidGreeting());}return out;
}
static int32_t num(const JSONValue& value){return std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));}
static JSONValue run(JSONObject& request){
    if(request.containsKey("date"))return javaDateString(std::get<int64_t>(request.get("date").value));
    auto channel=std::make_shared<Channel>();auto key=std::make_shared<Key>();auto runtime=std::make_shared<NIORuntime>();auto traffic=std::make_shared<NIOTraffic>();runtime->traffic=traffic;std::vector<std::shared_ptr<Message>> posted;bool postError=false;int64_t clock=1700000000000LL;runtime->currentTimeMillis=[&]{return clock;};runtime->postMessage=[&](std::shared_ptr<Message> message){if(postError)throw std::runtime_error("Post error");posted.push_back(std::move(message));};NIOClient::runtimeLookup=[&]{if(request.containsKey("runtime")&&!request.get("runtime").asBool())throw std::runtime_error("Missing Main");return request.containsKey("nullManager")&&request.get("nullManager").asBool()?nullptr:runtime;};struct Reset{~Reset(){NIOClient::runtimeLookup={};}}reset;
    auto client=request.get("ctor").asString()=="connecting"?std::make_shared<NIOClient>(string(request.get("host")),num(request.get("port"))):std::make_shared<NIOClient>(request.get("incoming").asBool(),string(request.get("host")),num(request.get("port")),channel,key);
    bool uid=std::regex_match(client->getUID().value(),std::regex("[0-9A-V]{1,13}"));NIOClientFixture::uid(*client);if(request.get("ctor").asString()!="connecting")NIOClientFixture::initialTimes(*client,clock);std::shared_ptr<NIOClientInfo> info;JSONArray states;states.add(snapshot(*client,*channel,*key,*traffic,posted,info));
    for(const auto& raw:array(request.get("ops"))->elements()){auto op=object(raw);auto name=op->get("op").asString();auto field=op->containsKey("field")?op->get("field").asString():"";const auto& value=op->get("value");JSONValue result="OK";try{
        if(name=="largeSend")client->sendData(std::make_shared<MiniData>(std::vector<uint8_t>(size_t(num(value)))));else if(name=="feed")channel->feed(MiniData::fromHex(value.asString()).bytes());else if(name=="read")client->handleRead();else if(name=="write")client->handleWrite();else if(name=="send")client->sendData(value.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(value.asString())));else if(name=="disconnect")client->disconnect();
        else if(name=="set"){
            if(field=="host")client->overrideHost(string(value));else if(field=="port")client->setPort(num(value));else if(field=="minimaport")client->setMinimaPort(num(value));else if(field=="welcome")client->setWelcomeMessage(string(value));else if(field=="maxima")client->setMaximaIdent(string(value));else if(field=="mls")client->setMaximaMLS(string(value));else if(field=="valid")client->setValidGreeting(value.asBool());else if(field=="sent")client->setSentGreeting(value.asBool());else if(field=="attempts")client->setConnectAttempts(num(value));else if(field=="extra")client->setExtraData(extra(value));else throw std::invalid_argument(field);
        }else if(name=="increment")client->incrementConnectAttempts();else if(name=="p2p")client->setReceivedP2PGreeting();else if(name=="maximaDisconnected")client->setMaximaDisconnected();else if(name=="clock")clock=std::get<int64_t>(value.value);else if(name=="connectedTime")NIOClientFixture::connectedTime(*client,std::get<int64_t>(value.value));else if(name=="info")info=std::make_shared<NIOClientInfo>(client,value.asBool());else if(name=="explicitInfo")info=std::make_shared<NIOClientInfo>("manual",std::nullopt,-1,true);else if(name=="nullInfo")info=std::make_shared<NIOClientInfo>(nullptr,true);else if(name=="infoExtra"){if(!info)throw std::runtime_error("Null info");info->setExtrasData(extra(value));}
        else if(name=="control"){
            if(field=="rcap")channel->rcap=num(value);else if(field=="wcap")channel->wcap=num(value);else if(field=="eof")channel->eof=value.asBool();else if(field=="readError")channel->readError=value.asBool();else if(field=="writeError")channel->writeError=value.asBool();else if(field=="keyValid")key->valid=value.asBool();else if(field=="cancelError")key->cancelError=value.asBool();else if(field=="interestError")key->interestError=value.asBool();else if(field=="wakeError")key->wakeError=value.asBool();else if(field=="postError")postError=value.asBool();else if(field=="nullTraffic")runtime->traffic=value.asBool()?nullptr:traffic;else throw std::invalid_argument(field);
        }else throw std::invalid_argument(name);
    }catch(const std::exception&){result="ERROR";}JSONObject item;item.put("result",result);item.put("state",snapshot(*client,*channel,*key,*traffic,posted,info));states.add(JSONValue::parseWithException(item.toJSONString()));}
    JSONObject out;out.put("uid",uid);out.put("states",states);return out;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto request=object(JSONValue::parseWithException(line));std::cout<<run(*request).toJSONString()<<'\n';}catch(const std::exception&){std::cout<<"\"ERROR\"\n";}}
