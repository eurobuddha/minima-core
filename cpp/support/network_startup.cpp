#include "PeerHarness.hpp"
#include "support/JavaExceptions.hpp"
#include "org/minima/objects/Greeting.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/commands/network/connect.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
static JSONValue message(const std::shared_ptr<Message>& msg){if(!msg)return {};JSONObject out;out.put("type",nullable(msg->getMessageType()));out.put("host",nullable(msg->getString("host")));out.put("port",msg->getInteger("port"));return out;}
static JSONValue runStartup(const JSONObject& request){
    auto kind=request.get("kind").asString();
    if(kind=="connect"){
        JSONArray posted;bool fail=request.get("fail").asBool();commands::connect::postMessage=[&](auto msg){posted.add(message(msg));if(fail)throw std::runtime_error("Post failure");};struct Reset{~Reset(){commands::connect::postMessage={};}}reset;
        JSONObject out;try{out.put("parsed",message(commands::connect::createConnectMessage(string(request.get("host")))));}catch(const std::exception&){out.put("parsed","ERROR");}
        if(request.containsKey("command"))out.put("response",CommandRunner::getRunner().runMultiCommand(request.get("command").asString()));
        else{commands::connect command;command.getParams()->put("host",request.get("host"));try{out.put("response",command.runCommand());}catch(const JavaCheckedException& error){out.put("response","EXCEPTION:"+std::string(error.what()));}catch(const std::exception&){out.put("response","ERROR");}}
        commands::connect prototype;out.put("help",nullable(prototype.getFullHelp()));out.put("name",nullable(prototype.getName()));out.put("shortHelp",nullable(prototype.getHelp()));JSONArray params;for(const auto& param:prototype.getValidParams())params.add(param);out.put("params",params);out.put("fresh",prototype.getFunction()->getParams()->size()==0);out.put("posted",posted);return out;
    }
    Greeting greeting;greeting.setTopBlock(MiniNumber(123));greeting.getChain().push_back(MiniData::fromHex("0xFF"));greeting.getExtraData().put("retained",true);
    JSONArray states,calls;int depth=0;std::shared_ptr<JSONObject> config;std::vector<std::shared_ptr<TxPoWTreeNode>> nodes;SocketAddressList peers;
    auto check=[&](const std::string& stage){calls.add(stage);if(config->get("fail").asString()==stage)throw std::runtime_error("Injected "+stage);};
    Greeting::services.readLock=[&](bool lock){check(lock?"lock":"unlock");depth+=lock?1:-1;};
    Greeting::services.getWelcome=[&]{check("welcome");return string(config->get("welcome"));};
    Greeting::services.getTip=[&]{check("tip");return nodes.empty()?nullptr:nodes.back();};
    Greeting::services.getPeersCopy=[&]{check("peers");return peers;};struct Reset{~Reset(){Greeting::services={};}}reset;
    for(const auto& value:array(request.get("ops"))->elements()){
        config=object(value);calls=JSONArray();nodes.clear();int count=int(std::get<int64_t>(config->get("count").value));
        for(int i=0;i<count;++i){TxPoW tx;tx.clearBody();tx.setTimeMilli(MiniNumber(0));tx.setBlockNumber(MiniNumber(i+1));tx.calculateTXPOWID();auto node=std::make_shared<TxPoWTreeNode>(std::move(tx));if(!nodes.empty())nodes.back()->addChildNode(node);nodes.push_back(node);}
        peers=list(config->get("peers"));GeneralParams::IS_HOST_SET=config->get("hostSet").asBool();GeneralParams::MINIMA_HOST=config->get("host").asString();GeneralParams::MINIMA_PORT=int32_t(std::get<int64_t>(config->get("port").value));GeneralParams::P2P_ENABLED=config->get("p2p").asBool();
        JSONValue result;try{result=&greeting.createGreeting()==&greeting;}catch(const std::exception&){result="ERROR";}
        JSONObject state;state.put("result",result);state.put("calls",calls);state.put("depth",depth);state.put("wire",MiniData(greeting.serialise()).to0xString());state.put("extra",greeting.getExtraData());state.put("top",greeting.getTopBlock().toString());JSONArray chain;for(const auto& id:greeting.getChain())chain.add(id.to0xString());state.put("chain",chain);states.add(JSONValue::parseWithException(state.toJSONString()));
    }
    return states;
}
int main(){GeneralParams::USE_SQL_COINDB=false;std::string line;while(std::getline(std::cin,line))try{std::cout<<runStartup(*object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\"\n";}}
