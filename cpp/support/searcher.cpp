#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <unordered_map>
#include <unordered_set>
using namespace minima;using namespace minima::json;
namespace minima {struct TxPoWSearcherFixture {static void reset(){std::lock_guard lock(TxPoWSearcher::mImportedMutex);TxPoWSearcher::mImportedTokens.clear();}};}
static int number(const JSONObject& value,const std::string& key){return int(std::get<int64_t>(value.get(key).value));}
static auto array(const JSONObject& value,const std::string& key){return std::get<std::shared_ptr<JSONArray>>(value.get(key).value);}
static auto hex(const JSONObject& value,const std::string& key){return MiniData::fromHex(value.get(key).asString());}
template<class T>static T decode(const JSONValue& value){auto bytes=MiniData::fromHex(value.asString());size_t offset=0;return T::deserialise(bytes.bytes(),offset);}
static JSONValue coin(const std::optional<Coin>& value){return value?JSONValue(MiniData(value->serialise()).to0xString()):JSONValue();}
static JSONValue token(const std::shared_ptr<Token>& value){return value?JSONValue(MiniData(value->serialise()).to0xString()):JSONValue();}
static JSONArray coins(const std::vector<Coin>& values){JSONArray result;for(const auto& value:values)result.add(coin(value));return result;}
static auto strings(const JSONObject& request,const std::string& key){std::unordered_set<std::string> result;for(const auto& value:array(request,key)->elements())result.insert(value.asString());return result;}
static JSONValue run(JSONObject& request){
    TxPoWSearcherFixture::reset();GeneralParams::USE_SQL_COINDB=false;GeneralParams::TEST_PARAMS=request.containsKey("testparams")&&request.get("testparams").asBool();GeneralParams::IS_MEGAMMR=request.get("mega").asBool();std::vector<std::shared_ptr<TxPoWTreeNode>> nodes;std::unordered_map<std::string,std::shared_ptr<TxPoW>> pool;auto megaOwner=std::make_shared<MegaMMR>();auto& mega=*megaOwner;int locks=0;
    for(const auto& value:array(request,"pool")->elements()){auto t=std::make_shared<TxPoW>(decode<TxPoW>(value));pool.emplace(t->getTxPoWID(),t);}
    for(const auto& value:array(request,"nodes")->elements()){auto& spec=*std::get<std::shared_ptr<JSONObject>>(value.value);auto t=decode<TxPoW>(spec.get("pow"));for(const auto& id:array(spec,"txns")->elements())t.addBlockTxPOW(MiniData::fromHex(id.asString()));t.setHeaderBodyHash();t.calculateTXPOWID();auto node=std::make_shared<TxPoWTreeNode>(t);if(spec.containsKey("mmr"))*node->getMMR()=decode<MMR>(spec.get("mmr"));for(const auto& value:array(spec,"coins")->elements())node->addCoinToNode(decode<Coin>(value));for(const auto& value:array(spec,"relevant")->elements())node->getRelevantCoins().push_back(decode<Coin>(value));if(!nodes.empty())nodes.back()->addChildNode(node);nodes.push_back(node);}
    for(const auto& value:array(request,"megacoins")->elements()){auto c=std::make_shared<Coin>(decode<Coin>(value));mega.getAllCoins().put(c->getCoinID().to0xString(),c);}
    for(const auto& value:array(request,"tokens")->elements())TxPoWSearcher::importToken(value.isNull()?nullptr:std::make_shared<Token>(decode<Token>(value)));
    MMR::megaMMRLookup=[megaOwner]{return megaOwner->getSharedMMR();};auto& services=TxPoWSearcher::services;services.getTip=[&](){return nodes.empty()?nullptr:nodes.back();};services.getMegaMMR=[megaOwner]{return megaOwner;};services.readLock=[&](bool locked){locks+=locked?1:-1;};services.getTxPoW=[&](const std::string& id){auto found=pool.find(id);return found==pool.end()?nullptr:found->second;};
    auto& wallet=TxPoWTreeNode::services;wallet.isAddressRelevant=[values=strings(request,"addresses")](const std::string& address){return values.contains(address);};wallet.isAddressSimple=[values=strings(request,"simple")](const std::string& address){return values.contains(address);};wallet.isKeyRelevant=[values=strings(request,"keys")](const std::string& key){return values.contains(key);};
    auto nodeIndex=[&](const std::shared_ptr<TxPoWTreeNode>& node)->int {if(!node)return -1;auto found=std::find(nodes.begin(),nodes.end(),node);return found==nodes.end()?-1:int(found-nodes.begin());};JSONArray result;
    for(const auto& value:array(request,"queries")->elements()){auto& q=*std::get<std::shared_ptr<JSONObject>>(value.value);JSONValue response;try{auto op=q.get("op").asString();auto start=number(q,"start")<0?nullptr:nodes.at(number(q,"start"));if(q.containsKey("nodeid"))q.put("id",nodes.at(number(q,"nodeid"))->getTxPoW().getTxPoWID());
        if(op=="command"){auto command=CommandRunner::getRunner().getCommandOnly(q.get("name").asString());if(!command)throw std::runtime_error("Missing command");for(const auto& [key,value]:std::get<std::shared_ptr<JSONObject>>(q.get("params").value)->entries())command->getParams()->put(key,value);if(q.containsKey("meta")&&q.get("meta").asBool()){JSONObject meta;meta.put("name",command->getName().value());meta.put("help",command->getHelp().value());meta.put("fullhelp",command->getFullHelp().value());JSONArray valid;for(const auto& value:command->getValidParams())valid.add(value);meta.put("valid",valid);meta.put("fresh",command->getFunction()->getJSONReply());response=meta;}else response=command->runCommand();}
        else if(op=="coins")response=coins(TxPoWSearcher::searchCoins(start,q.get("relevant").asBool(),q.get("cid").asBool(),hex(q,"id"),q.get("cam").asBool(),MiniNumber(q.get("amount").asString()),q.get("caddr").asBool(),hex(q,"address"),q.get("ctok").asBool(),hex(q,"token"),q.get("cstate").asBool(),q.get("state").asString(),q.get("wild").asBool(),q.get("simple").asBool(),number(q,"depth"),q.get("mega").asBool()));
        else if(op=="relevant")response=coins(TxPoWSearcher::getAllRelevantUnspentCoins(start));
        else if(op=="relevanttoken")response=coins(TxPoWSearcher::getRelevantUnspentCoins(start,q.get("token").asString(),q.get("simple").asBool()));
        else if(op=="floating")response=coin(TxPoWSearcher::getFloatingCoin(start,MiniNumber(q.get("amount").asString()),hex(q,"address"),hex(q,"token")));
        else if(op=="coin")response=coin(TxPoWSearcher::searchCoin(hex(q,"id"),q.get("mega").asBool()));
        else if(op=="coindefault")response=coin(TxPoWSearcher::searchCoin(hex(q,"id")));
        else if(op=="nodecoin")response=nodeIndex(TxPoWSearcher::getTreeNodeForCoin(hex(q,"id")));
        else if(op=="block"){auto t=TxPoWSearcher::getTxPoWBlock(MiniNumber(q.get("amount").asString()));if(t)response=t->getTxPoWID();}
        else if(op=="chain"){auto t=TxPoWSearcher::searchChainForTxPoW(hex(q,"id"));if(t)response=t->getTxPoWID();}
        else if(op=="nodeblock")response=nodeIndex(TxPoWSearcher::searchChainForTxPoWBlock(hex(q,"id")));
        else if(op=="address"){JSONArray ids;for(const auto& t:TxPoWSearcher::searchTxPoWviaAddress(hex(q,"address")))ids.add(t->getTxPoWID());response=ids;}
        else if(op=="checkaddress"){if(!start)throw std::runtime_error("Null node");response=TxPoWSearcher::checkTxPoWForAddress(start->getTxPoW(),hex(q,"address"));}
        else if(op=="checkrelevant"){if(!start)throw std::runtime_error("Null node");response=TxPoWSearcher::checkTxPoWRelevant(start->getTxPoW(),wallet);}
        else if(op=="tokens"){JSONArray list;for(const auto& value:TxPoWSearcher::getAllTokens())list.add(token(value));response=list;}
        else if(op=="token")response=token(TxPoWSearcher::getToken(hex(q,"token")));else throw std::invalid_argument(op);
    }catch(const CommandException& error){response=std::string("COMMAND_ERROR:")+error.what();}catch(const std::exception&){response="ERROR";}JSONObject output;output.put("result",response);output.put("locks",locks);locks=0;result.add(output);}
    MMR::megaMMRLookup={};services={};wallet={};return result;
}
int main(){
    // SearcherOracle directs source diagnostics to stderr. Keep one JSON reply
    // per stdout line while retaining the actual production logger behavior.
    auto logger=MinimaLogger::getRuntime();logger.println=[](const std::u16string& text){std::cerr<<unicode::encodeUTF8(text)<<'\n';};MinimaLogger::setRuntime(std::move(logger));
    std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);std::cout<<run(*request).toJSONString()<<'\n';}catch(const std::exception& e){MMR::megaMMRLookup={};TxPoWSearcher::services={};TxPoWTreeNode::services={};std::clog<<e.what()<<'\n';std::cout<<"\"ERROR\"\n";}
}
