#include "BlockFixture.hpp"
#include "SharedCheckerFixture.hpp"
#include "ArchiveFixture.hpp"
#include "SqlRecordingFixture.hpp"
#include "support/JavaIOExceptionText.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;using namespace minima::sql_fixture;
namespace {
JSONObject run(const JSONObject& input){
    if(input.containsKey("checker"))return support::sharedCheckerFixture(input);
    config=&input;auto source=support::blockFixture(int(integer(input,"size",2)),int(integer(input,"flags",1)),int(integer(input,"time",7)));auto sourceBytes=source.serialise();
    auto target=std::make_shared<TxBlock>(source);auto held=target->getSharedTxPoW();auto main=support::archiveFixtureTxPoW(11,0);auto other=support::archiveFixtureTxPoW(12,0);
    auto list=std::make_shared<TxBlock::TxPoWs>();list->push_back(other);list->push_back(main);std::shared_ptr<TxPoWTreeNode> node;
    JSONArray removed;for(const auto& coin:target->getOutputCoins())removed.add(coin.getCoinID().to0xString());
    std::vector<std::shared_ptr<TxPoW>> ids{held,main,other};auto id=[&](auto t){if(!t)return -1;auto it=std::find(ids.begin(),ids.end(),t);if(it==ids.end()){ids.push_back(t);return int(ids.size()-1);}return int(it-ids.begin());};JSONArray rows;
    for(const auto& raw:array(input.get("ops")).elements()){
        const auto& op=object(raw);JSONObject row;
        try {auto cmd=op.get("cmd").asString();
            if(cmd=="read"){size_t offset=0;auto bytes=sourceBytes;bytes.resize(std::min(bytes.size(),size_t(integer(op,"length",int64_t(bytes.size())))));try{target->readDataStream(bytes,offset);}catch(...){row.put("offset",int64_t(offset));throw;}row.put("offset",int64_t(offset));}
            else if(cmd=="held")held->setNonce(MiniNumber(integer(op,"nonce")));
            else if(cmd=="main")main->setNonce(MiniNumber(integer(op,"nonce")));
            else if(cmd=="target")target->getTxPoW().setNonce(MiniNumber(integer(op,"nonce")));
            else if(cmd=="simple")target=std::make_shared<TxBlock>(op.get("null").isNull()||!op.get("null").asBool()?main:nullptr);
            else if(cmd=="link")main->addBlockTxPOW(other->getTxPoWIDData());
            else if(cmd=="listNull")list.reset();
            else if(cmd=="nullEntry")list->insert(list->begin()+integer(op,"index"),nullptr);
            else if(cmd=="empty")list->clear();
            else if(cmd=="construct")target=std::make_shared<TxBlock>(op.get("nullParent").isNull()||!op.get("nullParent").asBool()?std::make_shared<MMR>():nullptr,op.get("nullMain").isNull()||!op.get("nullMain").asBool()?main:nullptr,list);
            else if(cmd=="node")node=std::make_shared<TxPoWTreeNode>(target,false);
            else if(cmd=="info"){}else throw std::invalid_argument(cmd);
        }catch(const std::exception& error){auto io=dynamic_cast<const wire::IOException*>(&error);row.put("error",io?javaIOExceptionText(*io):javaExceptionText(error));}
        row.put("tx",id(target->getSharedTxPoW()));row.put("held",held->getNonce().toString());row.put("main",main->getNonce().toString());
        try{row.put("wire",MiniData(target->serialise()).to0xString());}catch(const std::exception& e){row.put("wireError",javaExceptionText(e));}
        row.put("peaks",int64_t(target->getPreviousPeaks().size()));row.put("spent",int64_t(target->getInputCoinProofs().size()));row.put("outputs",int64_t(target->getOutputCoins().size()));
        if(node){row.put("nodeTx",id(node->getSharedTxPoW()));row.put("nodeNonce",node->getTxPoW().getNonce().toString());}
        JSONArray states;for(const auto& rawID:removed.elements()){auto value=target->removedState(rawID.asString());states.add(value?JSONValue(Coin::convertStateListToJSON(*value)):JSONValue());}row.put("removed",states);rows.add(row);
    }
    JSONObject out;out.put("bytes",int64_t(sourceBytes.size()));out.put("ops",rows);return out;
}
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
