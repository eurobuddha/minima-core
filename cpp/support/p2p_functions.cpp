#include "P2PFunctionsFixture.hpp"
static JSONValue run(const JSONObject& request){
    if(request.containsKey("url")){try{openURLAndClose(request.get("url").asString());return true;}catch(const std::exception&){return false;}}
    JSONArray events,posted,states;auto manager=NIOManagerFixture::create();NIOManager::runtimeLookup=[manager]{return manager;};NIOClient::runtimeLookup=[manager]{return manager->clientRuntime();};
    struct Reset{~Reset(){NIOManager::runtimeLookup={};NIOClient::runtimeLookup={};P2PFunctions::logger={};P2PFunctions::openURL={};P2PFunctions::enumerateInterfaces={};P2PFunctionsFixture::cache(nullptr);SwapLinksFunctions::mainPresent={};}}reset;
    std::vector<std::shared_ptr<NIOClient>> clients;
    if(request.containsKey("clients"))for(const auto& raw:array(request.get("clients"))->elements()){auto row=object(raw);bool connected=flag(*row,"connected");auto client=std::make_shared<NIOClient>(flag(*row,"incoming"),string(row->get("host")),int32_t(num(*row,"port",9001)),nullptr,std::make_shared<NativeNIOSelectionKey>([]{}));NIOClientFixture::uid(*client,row->get("uid").asString());client->setValidGreeting(flag(*row,"valid"));clients.push_back(client);manager->add(client,connected);}
    GeneralParams::ALLOW_ALL_IP=flag(request,"allow");GeneralParams::TXBLOCK_NODE=false;GeneralParams::NETWORKING_LOGS=false;P2PParams::LOG_LEVEL=request.containsKey("level")&&request.get("level").isNull()?std::nullopt:std::optional<P2PLogLevel>(P2PLogLevel(num(request,"level",0)));P2PFunctions::clearInvalidPeers();P2PFunctionsFixture::cache(nullptr);P2PFunctionsFixture::seed(num(request,"seed",0));
    P2PFunctions::logger=[&events,&request](const auto& text){if(!flag(request,"noMain"))events.add("log:"+text);};
    P2PFunctions::openURL=[&events,&request](const auto& url){for(const auto& stage:std::vector<std::string>{"open:"+url,"connect","input","close"}){events.add(stage);auto failure=request.get("failure");if(!failure.isNull()&&stage.starts_with(failure.asString()))throw wire::IOException("Injected availability error");}};
    if(flag(request,"missingManager")||flag(request,"noMain"))NIOManager::runtimeLookup=[]{return std::shared_ptr<NIOManager>{};};
    if(request.containsKey("locals")){auto local=std::make_shared<JavaStringSet>();for(const auto& raw:array(request.get("locals"))->elements())local->add(string(raw));P2PFunctionsFixture::cache(local);}
    if(request.containsKey("invalid"))for(const auto& raw:array(request.get("invalid"))->elements())P2PFunctions::mInvalidPeers.add(string(raw));
    auto state=std::make_shared<P2PState>();if(request.containsKey("maps")){auto maps=object(request.get("maps"));for(const char* name:{"in","out","none","not"})if(maps->containsKey(name)){auto links=map(maps->get(name));if(std::string(name)=="in")state->setInLinks(links);else if(std::string(name)=="out")state->setOutLinks(links);else if(std::string(name)=="none")state->setNoneP2PLinks(links);else state->setNotAcceptingConnP2PLinks(links);}}
    SwapLinksFunctions::mainPresent=[&request]{return !flag(request,"noMain");};
    if(flag(request,"swap")){GeneralParams::MINIMA_PORT=int32_t(num(request,"minimaPort",9001));state->setAcceptingInLinks(!request.containsKey("accept")||flag(request,"accept"));state->setHostSet(flag(request,"hostSet"));state->setDoingDiscoveryConnection(flag(request,"discovery"));state->setMaxNumP2PConnections(int32_t(num(request,"max",5)));state->setMaxNumNoneP2PConnections(int32_t(num(request,"maxNone",50)));if(request.containsKey("my"))state->setMyMinimaAddress(address(request.get("my")));if(request.containsKey("secret"))state->setIpReqSecret(request.get("secret").isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(request.get("secret").asString())));if(request.containsKey("known")){auto known=list(request.get("known"));for(const auto& value:*known)state->getKnownPeers()->add(value);}if(flag(request,"nullState"))state=nullptr;}
    std::shared_ptr<P2PWalkLinks> walk;if(request.containsKey("walk")&&!request.get("walk").isNull()){auto row=object(request.get("walk"));walk=std::make_shared<P2PWalkLinks>(flag(*row,"in"),flag(*row,"returning"),list(row->get("path")));walk->setClientWalk(flag(*row,"client"));walk->setJoiningWalk(flag(*row,"joining"));walk->setAvailableNoneP2PConnectionSlots(int32_t(num(*row,"slots",0)));walk->setSecret(row->get("secret").isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(row->get("secret").asString())));}
    std::shared_ptr<JavaStringSet> saved;
    for(const auto& raw:array(request.get("ops"))->elements()){auto op=object(raw);auto kind=op->get("op").asString();JSONValue result="OK";try{
        if(kind=="w_filter")result=snapshots(WalkLinksFuncs::removeIPsInBFromA(map(op->get("map")),list(op->get("list"))));
        else if(kind=="w_address")result=snapshot(WalkLinksFuncs::getAddressFromUID(state,string(op->get("uid"))));
        else if(kind=="w_next")result=walkMessage(WalkLinksFuncs::createNextHopMsg(address(op->get("address")),walk,state));
        else if(kind=="w_generate")result=walkMessage(WalkLinksFuncs::generateDoSwapMessageFromWalk(state,walk,int32_t(num(*op,"target",5))));
        else if(kind=="w_in")result=walkMessage(WalkLinksFuncs::onInLinkWalkMsg(state,walk,walkClient(op->get("info")),{}));
        else if(kind=="w_out")result=walkMessage(WalkLinksFuncs::onOutLinkWalkMsg(state,walk,walkClient(op->get("info")),int32_t(num(*op,"target",5)),{}));
        else if(kind=="w_response")result=walkMessage(WalkLinksFuncs::onWalkLinkResponseMsg(state,walk));
        else if(kind=="w_return")result=swapMessages(WalkLinksFuncs::onReturnedWalkMsg(state,walk,int32_t(num(*op,"target",5))));
        else if(kind=="w_load_return")result=swapMessages(WalkLinksFuncs::onReturnedLoadBalanceWalkMsg(state,walk));
        else if(kind=="w_load")result=swapMessages(WalkLinksFuncs::genLoadBalanceDoSwaps(state,address(op->get("address")),int32_t(num(*op,"slots",1))));
        else if(kind=="s_connected"){int index=int(num(*op,"client",0));auto client=index<0?std::shared_ptr<NIOClient>{}:clients.at(size_t(index));result=swapMessages(SwapLinksFunctions::onConnected(state,flag(*op,"incoming"),client));}
        else if(kind=="s_balance")result=swapMessages(SwapLinksFunctions::onConnectedLoadBalanceRequest(state,{}));
        else if(kind=="s_join")result=swapMessages(SwapLinksFunctions::joinScaleOutLinks(state,int32_t(num(*op,"target",5)),{}));
        else if(kind=="s_in")result=swapMessages(SwapLinksFunctions::requestInLinks(state,int32_t(num(*op,"target",5)),{}));
        else if(kind=="s_disconnect"){auto message=std::make_shared<Message>();if(!op->get("uid").isNull())message->addString("uid",string(op->get("uid")));SwapLinksFunctions::onDisconnected(state,flag(*op,"nullMessage")?nullptr:message);}
        else if(kind=="s_greeting"){ClientInfoPtr client;if(!op->get("info").isNull()){auto row=object(op->get("info"));client=std::make_shared<NIOClientInfo>(string(row->get("uid")),string(row->get("host")),int32_t(num(*row,"port",9001)),flag(*row,"incoming"));}auto greeting=op->get("greeting").isNull()?nullptr:std::make_shared<P2PGreeting>(P2PGreeting::fromJSON(*object(op->get("greeting"))));result=SwapLinksFunctions::processGreeting(state,greeting,client,flag(*op,"noconnect"));}
        else if(kind=="s_request")result=SwapLinksFunctions::processRequestIPMsg(op->get("message").isNull()?nullptr:object(op->get("message")),string(op->get("host")));
        else if(kind=="s_response")SwapLinksFunctions::processResponseIPMsg(state,op->get("message").isNull()?nullptr:object(op->get("message")));
        else if(kind=="s_echo"){if(!state||!state->getIpReqSecret())throw std::runtime_error("Null fixture secret");auto message=std::make_shared<JSONObject>();message->put("secret",state->getIpReqSecret()->to0xString());message->put("res_ip",op->get("host"));SwapLinksFunctions::processResponseIPMsg(state,message);}
        else if(kind=="s_wrap_alias"){auto data=std::make_shared<JSONObject>();auto wrapped=SwapLinksFunctions::wrapP2PMsg(data);data->put("after",true);result=wrapped;}
        else if(kind=="s_wrap")result=SwapLinksFunctions::wrapP2PMsg(op->get("message").isNull()?nullptr:object(op->get("message")));
        else if(kind=="ipv6")result=P2PFunctions::isIPv6(string(op->get("value")));
        else if(kind=="local")result=P2PFunctions::isIPLocal(string(op->get("value")));
        else if(kind=="invalid")result=P2PFunctions::isInvalidPeer(string(op->get("value")));
        else if(kind=="addInvalid")P2PFunctions::addInvalidPeer(string(op->get("value")));
        else if(kind=="clearInvalid")P2PFunctions::clearInvalidPeers();
        else if(kind=="connect")P2PFunctions::connect(string(op->get("host")),int32_t(num(*op,"port",9001)));
        else if(kind=="check")result=P2PFunctions::checkConnect(string(op->get("host")),int32_t(num(*op,"port",9001)));
        else if(kind=="disconnect")P2PFunctions::disconnect(string(op->get("uid")));
        else if(kind=="all")result=infos(P2PFunctions::getAllConnections());
        else if(kind=="connected")result=infos(P2PFunctions::getAllConnectedConnections());
        else if(kind=="info")result=info(P2PFunctions::getNIOCLientInfo(string(op->get("uid"))));
        else if(kind=="send"){auto message=op->get("message").isNull()?std::shared_ptr<JSONObject>{}:object(op->get("message"));P2PFunctions::sendP2PMessage(string(op->get("uid")),message);}
        else if(kind=="log"){P2PFunctions::log_node_runner(string(op->get("value")));P2PFunctions::log_info(string(op->get("value")));P2PFunctions::log_debug(string(op->get("value")));}
        else if(kind=="available")result=P2PFunctions::isNetAvailable();
        else if(kind=="locals")result=strings(P2PFunctions::getLocalAddresses());
        else if(kind=="saveLocal")saved=P2PFunctions::getLocalAddresses();
        else if(kind=="localAdd"){P2PFunctions::getLocalAddresses()->add(string(op->get("value")));result=saved==P2PFunctions::getLocalAddresses();}
        else if(kind=="localClear")P2PFunctions::getLocalAddresses()->clear();
        else if(kind=="find")result=info(UtilFuncs::getClientFromInetAddress(address(op->get("address")),state));
        else if(kind=="search")result=info(UtilFuncs::searchLinksMapForAddress(address(op->get("address")),map(op->get("map"))));
        else if(kind=="select")result=snapshot(UtilFuncs::selectRandomAddress(list(op->get("list"))));
        else if(kind=="draw")result=P2PFunctionsFixture::draw(int32_t(num(*op,"bound",1)));
        else throw std::runtime_error("Unknown operation");
    }catch(const std::exception&){result="ERROR";}
    auto messages=manager->drain();for(const auto& msg:messages.elements())posted.add(msg);JSONObject row;row.put("result",result);row.put("events",events);row.put("posted",posted);row.put("invalid",strings(std::shared_ptr<JavaStringSet>(&P2PFunctions::mInvalidPeers,[](auto*){})));JSONArray queues;for(const auto& client:clients)queues.add(NIOClientFixture::packets(*client));row.put("queues",queues);row.put("traffic",manager->getTrafficListener()->getBreakdown());if(request.containsKey("walk"))row.put("walk",walkSnapshot(walk));if(flag(request,"swap")){row.put("state",swapState(state));JSONArray greetings;for(const auto& client:clients)greetings.add(client->hasReceivedP2PGreeting());row.put("greetings",greetings);}states.add(JSONValue::parseWithException(row.toJSONString()));}
    return states;
}
int main(){std::string line;while(std::getline(std::cin,line))try{std::cout<<run(*object(JSONValue::parseWithException(line))).toJSONString()<<std::endl;}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\""<<std::endl;}}
