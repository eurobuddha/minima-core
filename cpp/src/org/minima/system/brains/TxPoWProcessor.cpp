#include "TxPoWProcessor.hpp"
#include "TxPoWChecker.hpp"
#include "org/minima/utils/Stack.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/JavaIOExceptionText.hpp"
#include <bit>
#include <iostream>
namespace minima {
namespace {
template<class T> T& required(const std::shared_ptr<T>& value){if(!value)throw JavaNullPointerException("null processor receiver");return *value;}
template<class T> std::shared_ptr<T> payload(const Message& message,const std::string& key){auto value=message.getObject(key);return value.isNull()?nullptr:value.as<std::shared_ptr<T>>();}
// TxBlock and IBD inherit Java Object's diagnostic rendering, not serialization.
template<class T> MessageObject object(std::shared_ptr<T> value,const std::string& name){return MessageObject(std::move(value),std::function<std::string(const T&)>([name](const T&){return name;}));}
std::shared_ptr<Message> ibdMessage(const std::string& type,std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid){auto message=std::make_shared<Message>(type);message->addObject("ibd",object(std::move(ibd),"org.minima.objects.IBD"));if(!uid)throw JavaNullPointerException("null message String");message->addString("uid",uid);return message;}
std::string errorText(const std::exception& error){if(auto io=dynamic_cast<const wire::IOException*>(&error))return javaIOExceptionText(*io);return sqlExceptionText(error);}
int64_t difference(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)-uint64_t(b));}
bool validRange(const MiniNumber& block,const MiniNumber& tip,const MiniNumber& root){return GeneralParams::TEST_PARAMS?block.isMore(root):(tip.isLess(MiniNumber::THOUSAND)||block.isMore(root.add(GlobalParams::MINIMA_BLOCKS_SPEED_CALC)));}
// Unlike a throwing destructor, this also permits a Java finally exception to
// replace a pending exception. Resolve cleanup receivers at the source call.
template<class Body,class Finish> void withFinally(Body body,Finish finish){try{body();}catch(...){finish();throw;}finish();}
}
TxPoWProcessor::TxPoWProcessor(TxPoWProcessorServices services)
    :MessageProcessor("TXPOWPROCESSOR",services.releaseLocks),mServices(std::move(services)),mFirstIBD(0){
    if(!mServices.isShuttingDown||!mServices.isShuttingDownOrRestoring||!mServices.postMain||!mServices.postNotifyEvent||!mServices.setSyncIBD||!mServices.getDB||!mServices.now)
        throw std::invalid_argument("TxPoWProcessor requires Main, database and clock services");
    mFirstIBD=mServices.now();
    if(!mServices.postNIO)mServices.postNIO=[](auto message){required(NIOManager::runtimeLookup()).PostMessage(std::move(message));};
    if(!mServices.disconnect)mServices.disconnect=[](const auto& uid,bool remove){required(NIOManager::runtimeLookup()).disconnect(uid,remove);};
    if(!mServices.sendAll)mServices.sendAll=[](const auto& type,const auto& data){NIOManager::sendNetworkMessageAll(type,data);};
    if(!mServices.send)mServices.send=[](const auto& uid,const auto& type,const auto& data){NIOManager::sendNetworkMessage(uid,type,data);};
    if(!mServices.log)mServices.log=[](const auto& text){std::clog<<text<<'\n';};
}
std::shared_ptr<TxPoWProcessor> TxPoWProcessor::create(TxPoWProcessorServices services){auto result=std::shared_ptr<TxPoWProcessor>(new TxPoWProcessor(std::move(services)));result->startMessageProcessor();return result;}
void TxPoWProcessor::postProcessTxPoW(std::shared_ptr<TxPoW> txpow){
    if(mServices.isShuttingDownOrRestoring())return;
    bool relevant=required(required(mServices.getDB()).getTxPoWDB()).addTxPoW(txpow);
    if(relevant||GeneralParams::NOTIFY_ALL_TXPOW){json::JSONObject data;data.put("relevant",relevant);data.put("txpow",required(txpow).toJSON());mServices.postNotifyEvent("NEWTXPOW",data);}
    if(GeneralParams::TXBLOCK_NODE)return;
    auto message=std::make_shared<Message>("TXP_PROCESSTXPOW");message->addObject("txpow",MessageObject(txpow));PostMessage(message);
}
void TxPoWProcessor::postProcessTxBlock(std::shared_ptr<TxBlock> block){
    if(mServices.isShuttingDownOrRestoring()||!GeneralParams::TXBLOCK_NODE)return;
    required(required(mServices.getDB()).getTxBlockDB()).addTxBlock(block);
    required(required(mServices.getDB()).getTxPoWDB()).addTxPoW(required(block).getSharedTxPoW());
    auto message=std::make_shared<Message>("TXP_PROCESSTXBLOCK");message->addObject("txblock",object(block,"org.minima.objects.TxBlock"));PostMessage(message);
}
void TxPoWProcessor::postProcessIBD(std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid,bool overrideRestore){
    if(mServices.isShuttingDownOrRestoring()&&!overrideRestore)return;
    if(overrideRestore)mIBDSyncFinished=false;
    auto message=ibdMessage("TXP_PROCESS_IBD",std::move(ibd),uid);message->addBoolean("notifyfinish",overrideRestore);PostMessage(message);
}
void TxPoWProcessor::postProcessSyncIBD(std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid){PostMessage(ibdMessage("TXP_PROCESS_SYNCIBD",std::move(ibd),uid));}
void TxPoWProcessor::postProcessArchiveIBD(std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid){PostMessage(ibdMessage("TXP_PROCESS_ARCHIVEIBD",std::move(ibd),uid));}
void TxPoWProcessor::processTxPoW(std::shared_ptr<TxPoW> input){
    bool recalculate=false;
    auto txpdb=required(mServices.getDB()).getTxPoWDB();auto tree=required(mServices.getDB()).getTxPoWTree();auto cascade=required(mServices.getDB()).getCascade();
    Stack stack;stack.push(MessageObject(input));
    while(!stack.isEmpty()){
        auto value=stack.pop();auto txpow=value.isNull()?nullptr:value.as<std::shared_ptr<TxPoW>>();
        auto blocknum=required(txpow).getBlockNumber();auto tipnum=required(required(tree).getTip()).getBlockNumber();auto rootnum=required(tree->getRoot()).getBlockNumber();
        bool range=validRange(blocknum,tipnum,rootnum);
        if(txpow->isBlock()&&!range&&GeneralParams::BLOCK_LOGS)mServices.log("Invalid range for block check @ "+blocknum.toString()+" root:"+rootnum.toString()+" tip:"+tipnum.toString()+" txpowid:"+txpow->getTxPoWID());
        if(!txpow->isBlock()||!range||tree->findNode(txpow->getTxPoWID()))continue;
        auto parent=tree->findNode(txpow->getParentID().to0xString());
        if(!parent){auto candidate=required(txpdb).getTxPoW(txpow->getParentID().to0xString());if(candidate)stack.push(MessageObject(candidate));continue;}
        auto ids=txpow->getTransactions();TxPoWDB::IDs nullableIDs(ids.begin(),ids.end());auto transactions=std::make_shared<TxPoWChecker::TxPoWs>(required(txpdb).getAllTxPoW(nullableIDs));
        if(transactions->size()!=ids.size())continue;
        if(!TxPoWChecker::checkTxPoWBlockTimed(parent,txpow,transactions)){
            mServices.log("[!] Failed block check @ "+blocknum.toString()+" txpowid:"+txpow->getTxPoWID()+" root:"+rootnum.toString()+" tip:"+tipnum.toString());continue;
        }
        auto block=std::make_shared<TxBlock>(parent->getMMR(),txpow,transactions);required(required(mServices.getDB()).getTxBlockDB()).addTxBlock(block);
        if(GeneralParams::BLOCK_LOGS)mServices.log("Added block to tree : "+block->getTxPoW().getBlockNumber().toString()+" "+block->getTxPoW().getTxPoWID());
        try{mServices.sendAll(NIOMessage::MSG_TXBLOCKID,block->getTxPoW().getTxPoWIDData());}catch(const std::exception& e){mServices.log(errorText(e));}
        auto node=std::make_shared<TxPoWTreeNode>(block);parent->addChildNode(node);tree->addFastLink(node);recalculate=true;
        for(auto child:required(txpdb).getChildBlocks(txpow->getTxPoWID()))stack.push(MessageObject(child));
    }
    if(recalculate)recalculateTree();
}
void TxPoWProcessor::processTxBlock(std::shared_ptr<TxBlock> input){
    if(!GeneralParams::TXBLOCK_NODE)return;bool recalculate=false;
    auto tree=required(mServices.getDB()).getTxPoWTree();auto cascade=required(mServices.getDB()).getCascade();
    Stack stack;stack.push(object(input,"org.minima.objects.TxBlock"));
    while(!stack.isEmpty()){
        auto value=stack.pop();auto block=value.isNull()?nullptr:value.as<std::shared_ptr<TxBlock>>();auto txpow=required(block).getSharedTxPoW();
        auto blocknum=required(txpow).getBlockNumber();auto tipnum=required(required(tree).getTip()).getBlockNumber();auto rootnum=required(tree->getRoot()).getBlockNumber();
        bool range=validRange(blocknum,tipnum,rootnum);
        if(txpow->isBlock()&&!range&&GeneralParams::BLOCK_LOGS)mServices.log("[!] Invalid range for txblock check slavemode @ "+blocknum.toString()+" root:"+rootnum.toString()+" tip:"+tipnum.toString()+" txpowid:"+txpow->getTxPoWID());
        if(!txpow->isBlock()||!range||tree->findNode(txpow->getTxPoWID()))continue;
        auto parent=tree->findNode(txpow->getParentID().to0xString());
        if(!parent){auto candidate=required(required(mServices.getDB()).getTxBlockDB()).findTxBlock(txpow->getParentID().to0xString());if(candidate)stack.push(object(candidate,"org.minima.objects.TxBlock"));continue;}
        if(!TxPoWChecker::checkTxBlockOnly(parent,*block)){mServices.log("[!] Failed txblock check @ "+blocknum.toString()+" txpowid:"+txpow->getTxPoWID()+" root:"+rootnum.toString()+" tip:"+tipnum.toString());continue;}
        if(GeneralParams::BLOCK_LOGS)mServices.log("Added TxBlock to tree : "+block->getTxPoW().getBlockNumber().toString()+" "+block->getTxPoW().getTxPoWID());
        auto node=std::make_shared<TxPoWTreeNode>(block);parent->addChildNode(node);tree->addFastLink(node);recalculate=true;
        for(auto child:required(required(mServices.getDB()).getTxBlockDB()).getChildBlocks(txpow->getTxPoWID()))stack.push(object(child,"org.minima.objects.TxBlock"));
    }
    if(recalculate)recalculateTree();
}
bool TxPoWProcessor::processSyncBlock(std::shared_ptr<TxBlock> block){
    auto cascade=required(mServices.getDB()).getCascade();auto txpdb=required(mServices.getDB()).getTxPoWDB();auto tree=required(mServices.getDB()).getTxPoWTree();
    required(required(mServices.getDB()).getTxBlockDB()).addTxBlock(block);required(txpdb).addTxPoW(required(block).getSharedTxPoW());
    if(!required(tree).getTip()){
        if(auto tip=required(cascade).getTip()){
            auto blocknum=block->getTxPoW().getBlockNumber();auto cascnum=tip->getTxPoW().getBlockNumber();
            if(!cascnum.isEqual(blocknum.sub(MiniNumber::ONE)))throw JavaCheckedException("Invalid SyncBlock Cascade Tip not parent "+blocknum.toString()+" casctip:"+cascnum.toString());
        }
        tree->setRoot(std::make_shared<TxPoWTreeNode>(block));return true;
    }
    if(tree->findNode(block->getTxPoW().getTxPoWID()))return false;
    auto parent=tree->findNode(block->getTxPoW().getParentID().to0xString());
    if(!parent)throw JavaCheckedException("Invalid SyncBlock as NO PARENT! syncblock:"+block->getTxPoW().getBlockNumber().toString());
    auto node=std::make_shared<TxPoWTreeNode>(block);parent->addChildNode(node);tree->addFastLink(node);return true;
}
void TxPoWProcessor::recalculateTree(){
    auto txpdb=required(mServices.getDB()).getTxPoWDB();auto tree=required(mServices.getDB()).getTxPoWTree();auto cascade=required(mServices.getDB()).getCascade();auto archive=required(mServices.getDB()).getArchive();auto mega=required(mServices.getDB()).getMegaMMR();
    required(mServices.getDB()).writeLock(true);
    withFinally([&]{try{
        auto current=required(tree).getTip();tree->recalculateTree();
        auto maxlen=GlobalParams::MINIMA_CASCADE_START_DEPTH.add(GlobalParams::MINIMA_CASCADE_FREQUENCY).getAsInt();
        if(tree->getHeaviestBranchLength()>=maxlen){
            auto tip=tree->getTip();auto newroot=required(tip).getPastNode(tip->getTxPoW().getBlockNumber().sub(GlobalParams::MINIMA_CASCADE_START_DEPTH).increment());required(newroot).copyParentRelevantCoins();
            auto removed=tree->setLength(GlobalParams::MINIMA_CASCADE_START_DEPTH.getAsInt());
            for(auto node:removed){auto txpow=required(node).getSharedTxPoW();required(txpdb).setInCascade(required(txpow).getTxPoWID());for(const auto& id:txpow->getTransactions())txpdb->setInCascade(id);
                required(archive).saveBlock(node->getTxBlock());required(cascade).addToTip(node->getTxPoW());
                try{TxPoWTreeNode::CheckTxBlockForNotifyCoins(node->getTxBlock());}catch(const std::exception& e){mServices.log(errorText(e));}
                if(GeneralParams::IS_MEGAMMR)required(mega).addBlock(node->getTxBlock());
            }
            if(GeneralParams::IS_MEGAMMR)required(mega).getMMR().pruneTree();required(cascade).cascadeChain();
            required(required(mServices.getDB()).getTxBlockDB()).clearOld(newroot->getBlockNumber().sub(MiniNumber::HUNDRED));
        }
        required(txpdb).clearMainChainTxns();
        for(auto tip=tree->getTip();tip;tip=tip->getParent()){auto txpow=tip->getSharedTxPoW();txpdb->setOnMainChain(required(txpow).getTxPoWID());for(const auto& id:txpow->getTransactions())txpdb->setOnMainChain(id);}
        auto next=tree->getTip();if(current&&next&&!current->getTxPoW().getTxPoWIDData().isEqual(next->getTxPoW().getTxPoWIDData())){
            auto message=std::make_shared<Message>("MAIN_NEWBLOCK");message->addObject("txpow",MessageObject(next->getSharedTxPoW()));mServices.postMain(message);
        }
    }catch(const std::exception& e){mServices.log(errorText(e));}},[&]{required(mServices.getDB()).writeLock(false);});
}
void TxPoWProcessor::processIBD(std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid,bool notifyFinish){
    if(!required(ibd).checkValidData())return;
    withFinally([&]{try{
        mServices.setSyncIBD(true);
        if(GeneralParams::IBDSYNC_LOGS)mServices.log("Processing main IBD length : "+std::to_string(required(ibd->getTxBlocks()).size()));auto start=mServices.now();
        if(ibd->hasCascade()&&!required(required(mServices.getDB()).getCascade()).getTip()){
            bool ignore=false;auto root=required(required(mServices.getDB()).getTxPoWTree()).getRoot();
            if(root){auto number=root->getBlockNumber();auto casctip=required(required(ibd->getCascade()).getTip()).getTxPoW().getBlockNumber();
                if(!casctip.isEqual(number.decrement())){ignore=true;mServices.log("[!] IGNORE IBD - My Tree Root : "+number.toString()+" IBD TIP : "+casctip.toString());}
                else if(!root->getTxPoW().getParentID().isEqual(ibd->getCascade()->getTip()->getTxPoW().getTxPoWIDData())){ignore=true;mServices.log("[!] IGNORE IBD - Invalid Hash for parents");}
            }
            if(!ignore){required(mServices.getDB()).writeLock(true);withFinally([&]{try{
                required(mServices.getDB()).setIBDCascade(ibd->getCascade());required(required(mServices.getDB()).getArchive()).checkCascadeRequired(ibd->getCascade().get());
            }catch(const std::exception& e){mServices.log(errorText(e));}},[&]{required(mServices.getDB()).writeLock(false);});}
        }
        auto tree=required(mServices.getDB()).getTxPoWTree();MiniNumber now(mServices.now());auto diff=difference(mServices.now(),mFirstIBD.load());
        if(diff>MAX_FIRST_IBD_TIME&&!GeneralParams::TXBLOCK_NODE&&required(tree).getTip()&&!required(ibd->getTxBlocks()).empty()){
            MiniNumber threshold(GeneralParams::TEST_PARAMS?300000:10800000);
            if(tree->getTip()->getTxPoW().getTimeMilli().sub(now).abs().isLess(threshold)){
                mServices.log("Your chain tip is up to date - no TxBlocks accepted - only FULL TxPoW");mServices.setSyncIBD(false);askToSyncTxBlocks(uid);return;
            }
        }
        int additions=0;auto blocks=ibd->getTxBlocks();
        for(auto block:required(blocks))try{
            processSyncBlock(block);++additions;requestMissingTxns(uid,block);
            if(additions>256){recalculateTree();/* Native owned temporaries release without a JVM GC request. */additions=0;if(GeneralParams::IBDSYNC_LOGS)mServices.log("[!] Processed IBD block @ "+required(block).getTxPoW().getBlockNumber().toString());}
        }catch(const std::exception& e){mServices.log(errorText(e));mServices.disconnect(uid,true);break;}
        recalculateTree();auto time=difference(mServices.now(),start);if(GeneralParams::IBDSYNC_LOGS)mServices.log("Processing main IBD finished "+std::to_string(time)+"ms");
        mServices.setSyncIBD(false);required(required(mServices.getDB()).getArchive()).checkForCleanDB();askToSyncTxBlocks(uid);if(notifyFinish)mIBDSyncFinished=true;
    }catch(const std::exception& e){mServices.log("[!] TXPOWPROCESSOR_PROCESS_IBD ERROR "+errorText(e));mServices.log(errorText(e));}},[&]{mServices.setSyncIBD(false);});
}
void TxPoWProcessor::processMessage(std::shared_ptr<Message> message){
    if(mServices.isShuttingDown())return;
    if(required(message).isMessageType("TXP_PROCESSTXPOW")){if(!GeneralParams::TXBLOCK_NODE)processTxPoW(payload<TxPoW>(*message,"txpow"));}
    else if(message->isMessageType("TXP_PROCESSTXBLOCK")){if(GeneralParams::TXBLOCK_NODE)processTxBlock(payload<TxBlock>(*message,"txblock"));}
    else if(message->isMessageType("TXP_PROCESS_IBD")){auto uid=message->getString("uid");auto ibd=payload<IBD>(*message,"ibd");bool finish=message->getBoolean("notifyfinish");processIBD(ibd,uid,finish);}
    else if(message->isMessageType("TXP_PROCESS_SYNCIBD")){
        auto uid=message->getString("uid");auto ibd=payload<IBD>(*message,"ibd");auto blocks=required(ibd).getTxBlocks();if(required(blocks).empty())return;
        auto archive=required(mServices.getDB()).getArchive();auto last=required(archive).loadLastBlock();
        auto lastpow=last?last->getSharedTxPoW():required(required(required(mServices.getDB()).getTxPoWTree()).getRoot()).getSharedTxPoW();
        for(auto block:*blocks){auto lastnum=required(lastpow).getBlockNumber();
            if(required(block).getTxPoW().getBlockNumber().isEqual(lastnum.decrement())){
                if(block->getTxPoW().getTxPoWIDData().isEqual(lastpow->getParentID()))archive->saveBlock(*block);
                else {mServices.log("[-] Invalid block parent in TxBlock sync.. @ "+block->getTxPoW().getBlockNumber().toString()+" from "+uid.value_or("null"));return;}
            }
            lastpow=block->getSharedTxPoW();
        }
        required(required(mServices.getDB()).getArchive()).checkForCleanDB();askToSyncTxBlocks(uid);
    }else if(message->isMessageType("TXP_PROCESS_ARCHIVEIBD")){
        auto ibd=payload<IBD>(*message,"ibd");auto uid=message->getString("uid");int additions=0;auto blocks=required(ibd).getTxBlocks();
        if(!required(blocks).empty()&&GeneralParams::IBDSYNC_LOGS)mServices.log("Processing Archive IBD length:"+std::to_string(blocks->size())+" start:"+required(blocks->front()).getTxPoW().getBlockNumber().toString());
        for(auto block:*blocks)try{processSyncBlock(block);if(++additions>1000){recalculateTree();additions=0;}}
            catch(const std::exception& e){mServices.log(errorText(e));mServices.disconnect(uid,false);break;}
        recalculateTree();
    }else if(message->isMessageType("TXPOWPROCESSOR_CHECKER_CALL"))mServices.log("TXPOWPROCESSOR Checker Call Recieved..");
}
void TxPoWProcessor::askToSyncTxBlocks(const std::optional<std::string>& uid){
    if(!uid)throw JavaNullPointerException("null client UID");if(*uid=="0x00")return;
    if(!GeneralParams::NO_SYNC_IBD){auto message=std::make_shared<Message>(NIOManager::NIO_SYNCTXBLOCK);message->addString("client",uid);mServices.postNIO(message);}
}
void TxPoWProcessor::requestMissingTxns(const std::optional<std::string>& uid,const std::shared_ptr<TxBlock>& block){
    if(GeneralParams::TXBLOCK_NODE)return;auto txpow=required(block).getSharedTxPoW();MiniNumber now(mServices.now());auto minimum=now.sub(MiniNumber(10800000));
    if(required(txpow).getTimeMilli().isLess(minimum))return;
    try{for(const auto& id:txpow->getBlockTransactions())if(!required(required(mServices.getDB()).getTxPoWDB()).exists(id.to0xString()))mServices.send(uid,NIOMessage::MSG_TXPOWREQ,id);}
    catch(const wire::IOException& e){mServices.log(errorText(e));}
}
}
