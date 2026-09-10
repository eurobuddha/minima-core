#include "ArchiveFixture.hpp"
#include "org/minima/objects/IBD.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static int integer(const JSONObject& o,const std::string& k,int fallback=0){return o.get(k).isNull()?fallback:int(std::get<int64_t>(o.get(k).value));}
static const JSONObject& object(const JSONObject& o,const std::string& k){return *std::get<std::shared_ptr<JSONObject>>(o.get(k).value);}
static auto tx(int height,int variant){auto t=support::archiveFixtureTxPoW(height,0);if(variant!=0){auto d=Crypto::MAX_VAL*BigInt(variant==1?2:1)/BigInt(variant==1?3:2);t->setBlockDifficulty(MiniData(d));int64_t nonce=height+100LL*variant;do{t->setNonce(MiniNumber(nonce++));t->calculateTXPOWID();}while(!t->isBlock());}return t;}
static IBD make(const JSONObject& config){IBD ibd;auto cascade=std::make_shared<Cascade>();int variant=integer(config,"variant");if(integer(config,"cascade")>=0){for(int i=0;i<integer(config,"cascade");++i)cascade->addToTip(*tx(integer(config,"start",1)+i,variant));cascade->cascadeChain();if(integer(config,"bad")>0&&cascade->getTip())cascade->getTip()->setLevel(1);ibd.setCascade(cascade);}int start=integer(config,"start",1)+std::max(integer(config,"cascade"),0);for(int i=0;i<integer(config,"count");++i)ibd.getTxBlocks()->push_back(std::make_shared<TxBlock>(*tx(start+i,variant)));return ibd;}
static JSONObject snapshot(const IBD& ibd){JSONObject out;out.put("hascascade",ibd.hasCascade());out.put("cascadeblocks",ibd.hasCascadeWithBlocks());JSONArray blocks;for(const auto& b:*ibd.getTxBlocks())blocks.add(b->getTxPoW().getTxPoWID());out.put("blocks",blocks);JSONArray cascade;if(ibd.getCascade())for(auto tip=ibd.getCascade()->getTip();tip;tip=tip->getParent())cascade.add(tip->getTxPoW().getTxPoWID());out.put("cascade",cascade);out.put("weight",ibd.getTotalWeight().toDecimalString());out.put("valid",ibd.checkValidData());try{out.put("root",ibd.getTreeRoot().toString());}catch(const std::exception&){out.put("root","ERROR");}try{out.put("tip",ibd.getTreeTip().toString());}catch(const std::exception&){out.put("tip","ERROR");}out.put("wire",MiniData(ibd.serialise()).to0xString());return out;}
struct Context {JSONArray calls;int depth=0;IBD archive;bool fail=false;void check() const {if(fail)throw std::logic_error("Injected archive failure");}};
static JSONValue run(const JSONObject& req){auto kind=req.getString("kind");
    if(kind=="wire"){auto data=MiniData::fromHex(req.getString("data"));size_t offset=0;IBD ibd;JSONObject out;try{ibd.readDataStream(data.bytes(),offset);out.put("result",snapshot(ibd));}catch(const wire::IOException&){out.put("error","IO_ERROR");}catch(const std::exception&){out.put("error","ERROR");}out.put("consumed",int64_t(offset));out.put("count",int64_t(ibd.getTxBlocks()->size()));out.put("cascade",ibd.hasCascade());return out;}
    auto current=make(object(req,"current"));auto ctx=std::make_shared<Context>();ctx->archive=make(object(req,"archive"));ctx->fail=integer(req,"fail")>0;auto archive=std::make_shared<IBDArchive>();
    archive->exists=[ctx](const auto& id){ctx->calls.add("exists:"+id);ctx->check();for(const auto& b:*ctx->archive.getTxBlocks())if(b->getTxPoW().getTxPoWID()==id)return b->getTxPoW().getBlockNumber();return MiniNumber(-1);};
    archive->loadBlockRange=[ctx](const auto& start,const auto& end,bool desc){ctx->calls.add("range:"+start.toString()+":"+end.toString()+":"+(desc?"true":"false"));ctx->check();std::vector<std::shared_ptr<TxBlock>> result;for(const auto& b:*ctx->archive.getTxBlocks()){auto n=b->getTxPoW().getBlockNumber();if(n.isMore(start)&&n.isLess(end))result.push_back(b);}if(desc)std::reverse(result.begin(),result.end());return result;};
    archive->loadSyncBlockRange=[ctx](const auto& start){ctx->calls.add("sync:"+start.toString());ctx->check();std::vector<std::shared_ptr<TxBlock>> result;for(const auto& b:*ctx->archive.getTxBlocks())if(b->getTxPoW().getBlockNumber().isLess(start))result.push_back(b);std::reverse(result.begin(),result.end());return result;};
    archive->loadLastBlock=[ctx]{ctx->calls.add("last");ctx->check();return ctx->archive.getTxBlocks()->empty()?std::shared_ptr<TxBlock>():ctx->archive.getTxBlocks()->front();};
    archive->loadCascade=[ctx]{ctx->calls.add("cascade");ctx->check();return ctx->archive.getCascade();};
    auto tree=std::make_shared<TxPowTree>();std::shared_ptr<TxPoWTreeNode> root,last;for(const auto& b:*current.getTxBlocks()){auto node=std::make_shared<TxPoWTreeNode>(b->getTxPoW());if(!root)root=node;else last->addChildNode(node);last=node;}if(root)tree->setRoot(root);
    IBD::services.getTree=[tree]{return tree;};IBD::services.getCascade=[cascade=current.getCascade()]{return cascade;};IBD::services.getArchive=[archive]{return archive;};IBD::services.readLock=[ctx](bool lock){ctx->depth+=lock?1:-1;ctx->calls.add(std::string("lock:")+(lock?"true":"false"));};IBD::services.isShuttingDown=[shutdown=integer(req,"shutdown")>0]{return shutdown;};
    if(kind=="make")return snapshot(current);
    if(kind=="shorten"){std::string id="0x00";int at=integer(req,"index",-1);std::vector<std::string> list;for(const auto& b:*current.getTxBlocks())list.push_back(b->getTxPoW().getTxPoWID());if(current.getCascade())for(auto tip=current.getCascade()->getTip();tip;tip=tip->getParent())list.push_back(tip->getTxPoW().getTxPoWID());if(at>=0&&size_t(at)<list.size())id=list[size_t(at)];return snapshot(IBD::createShortenedIBD(current,id));}
    if(kind=="compare")return IBD::checkOurChainHeavier(make(object(req,"other")));
    IBD ibd;JSONValue result="OK";if(integer(req,"preload")>0){ibd.getTxBlocks()->push_back(std::make_shared<TxBlock>(*tx(999,0)));ibd.setCascade(std::make_shared<Cascade>());}
    if(kind=="complete")ibd.createCompleteIBD();
    else if(kind=="sync")ibd.createSyncIBD(*tx(integer(req,"first",1),0));
    else if(kind=="archive")ibd.createArchiveIBD(MiniNumber(integer(req,"first")),archive,integer(req,"local")>0);
    else if(kind=="greet"){Greeting greeting;greeting.setTopBlock(MiniNumber(integer(req,"top",-1)));for(const auto& x:std::get<std::shared_ptr<JSONArray>>(req.get("chain").value)->elements()){auto a=std::get<std::shared_ptr<JSONArray>>(x.value)->elements();greeting.getChain().push_back(tx(int(std::get<int64_t>(a.at(0).value)),int(std::get<int64_t>(a.at(1).value)))->getTxPoWIDData());}result=ibd.createIBD(greeting);}
    else throw std::invalid_argument(kind);
    JSONObject out;out.put("result",result);out.put("ibd",snapshot(ibd));out.put("calls",ctx->calls);out.put("depth",ctx->depth);return out;
}
int main(){GeneralParams::USE_SQL_COINDB=false;std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto req=std::get<std::shared_ptr<JSONObject>>(parser::JSONParser().parse(line).value);std::cout<<run(*req).toJSONString()<<'\n';}catch(const std::exception&){std::cout<<"ERROR\n";}}
