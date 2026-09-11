#include "NIOMessage.hpp"
#include "NIOManager.hpp"
#include "org/minima/system/brains/TxPoWChecker.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/network/p2p/messages/InetSocketAddressIO.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/objects/Pulse.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "support/JavaInteger.hpp"
#include "support/JavaDateString.hpp"
#include "RelayPolicy.hpp"
#include <set>
#include <cmath>
namespace minima {
NIOMessageServices NIOMessage::services;
namespace {
int64_t difference(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)-uint64_t(b));}
int64_t plus(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)+uint64_t(b));}
template<class T> std::shared_ptr<T> required(std::shared_ptr<T> value){if(!value)throw std::runtime_error("Null NIO receiver");return value;}
void log(const std::string& text){std::clog<<text<<'\n';}
const char* boolean(bool value){return value?"true":"false";}
json::JSONValue jsonFloat(float value){return std::isfinite(value)?json::JSONValue::raw(javaFloating(value)):json::JSONValue{};}
}
std::shared_ptr<NIOManager> NIOMessage::manager(){return required(NIOManager::runtimeLookup());}
std::shared_ptr<TxPowTree> NIOMessage::tree(){return required(services.getTree());}
int64_t NIOMessage::now(){return services.currentTimeMillis?services.currentTimeMillis():TimerMessage::currentTimeMillis();}
void NIOMessage::receiveGreeting(std::span<const uint8_t> data,size_t& offset){
    auto client=required(manager()->getNIOServer())->getClient(mClientUID);if(!client){log(mClientUID.value_or("null")+" Error null client on Greeting NIOMessage..");return;}
    auto greeting=Greeting::deserialise(data,offset);const auto& version=greeting.getVersion().toString();bool test=version.find("TEST")!=std::string::npos;
    if(GeneralParams::TEST_PARAMS!=test){
        log("Greeting with Incompatible Version! "+version+" .. we are "+GlobalParams::MINIMA_VERSION+" from "+client->getFullAddress()+" incoming:"+boolean(client->isIncoming()));
        services.addInvalidPeer(client->getFullAddress());auto message=std::make_shared<Message>("P2P_NOCONNECT");message->addObject("client",MessageObject(client)).addString("uid",client->getUID());services.postP2P(message);
        if(client->isIncoming()&&!client->haveSentGreeting()){
            client->setSentGreeting(true);Greeting response;response.createGreeting();NIOManager::sendNetworkMessage(client->getUID(),MSG_GREETING,response);
            auto timer=std::make_shared<TimerMessage>(2000,NIOManager::NIO_DISCONNECT);timer->addString("uid",mClientUID);manager()->PostTimerMessage(timer);
        }else manager()->disconnect(mClientUID);
        return;
    }
    if(client->getHost().value()=="127.0.0.1"&&greeting.getExtraData().containsKey("host"))log("Greeting from SSH Port with HOST set : "+greeting.getExtraDataValue("host").value_or("null"));
    if(greeting.getExtraData().containsKey("host"))client->overrideHost(greeting.getExtraDataValue("host"));
    if(greeting.getExtraData().containsKey("port"))client->setMinimaPort(javaParseInt(greeting.getExtraDataValue("port").value()));
    client->setWelcomeMessage("Minima v"+version);client->setValidGreeting(true);
    auto message=std::make_shared<Message>("P2P_CONNECTED");message->addString("uid",client->getUID()).addBoolean("incoming",client->isIncoming()).addObject("client",MessageObject(client));services.postP2P(message);
    if(client->isIncoming()&&!client->haveSentGreeting()){client->setSentGreeting(true);Greeting response;response.createGreeting();NIOManager::sendNetworkMessage(client->getUID(),MSG_GREETING,response);}
    auto address=client->getFullMinimaAddress();
    if(GeneralParams::LIMIT_IBD_30MINS&&mHaveSentIBDRecently.contains(address))log("Already sent an IBD to "+address+" in last 30 mins..");
    else{
        if(!mHaveSentIBDRecently.contains(address))mHaveSentIBDRecently.add(address);
        IBD ibd;bool valid=ibd.createIBD(greeting);if(!valid&&!mFullAdrress.value().empty())services.addInvalidPeer(*mFullAdrress);
        NIOManager::sendNetworkMessage(mClientUID,MSG_IBD,ibd);
    }
}
void NIOMessage::receiveTxPoW(std::span<const uint8_t> data,size_t& offset){
    auto txpow=std::make_shared<TxPoW>(TxPoW::deserialise(data,offset));if(services.txPoWExists(txpow->getTxPoWID()))return;
    auto tip=tree()->getTip(),cascade=tree()->getRoot();if(!tip)return;
    auto cascadeBlock=required(cascade)->getBlockNumber(),block=txpow->getBlockNumber();
    // DECIMAL32 has seven significant digits. Its nearest values on either side
    // of 0.01 are farther apart than double rounding, so this exact comparison
    // preserves the Java divide(...DECIMAL32).doubleValue() < 0.01 decision.
    auto ratio=MMREntryNumber::dividePositive(tip->getTxPoW().getBlockDifficulty().getDataValue(),txpow->getBlockDifficulty().getDataValue(),7);
    auto start=now();bool disconnect=false;
    if(!txpow->getChainID().isEqual(TxPoWChecker::CURRENT_NETWORK)){log("Wrong Block ChainID! from "+mClientUID.value_or("null")+" "+txpow->getChainID().to0xString()+" "+txpow->getTxPoWID());disconnect=true;}
    else if(!TxPoWChecker::checkTxPoWBasic(*txpow)){log("TxPoW FAILS Basic checks from "+mClientUID.value_or("null")+" "+txpow->getTxPoWID());disconnect=true;}
    else if(!TxPoWChecker::checkSignatures(*txpow)){log("Invalid signatures on txpow from "+mClientUID.value_or("null")+" "+txpow->getTxPoWID());disconnect=true;}
    else if(!RelayPolicy::checkMaxStateStoreSize(*txpow,tip->getTxPoW().getMagic().getMaxTxPoWSize().getAsLong())){log("TxPoW state store too large..");disconnect=true;}
    if(disconnect){manager()->disconnect(mClientUID);return;}
    (void)now();bool valid=true;
    if(txpow->isBlock()&&ratio.isLess(MMREntryNumber(BigInt(1),2)))valid=false;
    bool beforeCascade=block.isLess(cascadeBlock);if(beforeCascade)valid=false;
    if(!RelayPolicy::checkAllPolicies(*txpow,GeneralParams::MAX_RELAY_STORESTATESIZE))valid=false;
    (void)now();
    if(!TxPoWChecker::checkTxPoWScripts(tip->getMMR().get(),*txpow,tip->getTxPoW())){
        if(txpow->isMonotonic()){log("Error Monotonic TxPoW failed script check from Client:"+mClientUID.value_or("null")+" "+txpow->getTxPoWID());return;}
        log("NON-Monotonic TxPoW failed script check from Client:"+mClientUID.value_or("null")+" "+txpow->getTxPoWID());valid=false;
    }
    (void)now();
    if(txpow->isBlock()&&txpow->getTimeMilli().isMore(MiniNumber(plus(now(),7200000)))){log("TxPoW block received with millitime MORE than 2 hours in future "+javaDateString(txpow->getTimeMilli().getAsLong())+" "+txpow->getTxPoWID());valid=false;}
    auto size=txpow->getSizeinBytesWithoutBlockTxns();if(size>tip->getTxPoW().getMagic().getMaxTxPoWSize().getAsLong()){log("TxPoW received size too large.. "+std::to_string(size)+" "+txpow->getTxPoWID());valid=false;}
    (void)now();if(TxPoWChecker::checkMemPoolCoins(*txpow))valid=false;
    (void)now();if(!TxPoWChecker::checkMMR(tip->getMMR().get(),*txpow,false))valid=false;
    (void)now();if(TxPoWGenerator::isMempoolFull()){auto burn=txpow->getBurn();if(burn.isLessEqual(TxPoWGenerator::getMinMempoolBurn())){log("Received TxPoW with low burn when MEMPOOL full "+burn.toString());valid=false;}}
    auto elapsed=difference(now(),start);if(elapsed>20000){log("Message took a long time ("+std::to_string(elapsed)+"ms) to process @ txpowid:"+txpow->getTxPoWID());valid=false;}
    services.postProcessTxPoW(txpow);if(valid)NIOManager::sendNetworkMessageAll(MSG_TXPOWID,txpow->getTxPoWIDData());
    if(!GeneralParams::TXBLOCK_NODE&&txpow->isBlock()&&!beforeCascade){
        for(const auto blockIDs=txpow->getBlockTransactions();const auto id:TxBody::required(blockIDs))if(!services.txPoWExists(TxBody::required(id).to0xString()))NIOManager::sendNetworkMessage(mClientUID,MSG_TXPOWREQ,TxBody::required(id));
        auto current=txpow;int counter=0;
        while(counter<512){
            if(current->getBlockNumber().isLessEqual(cascadeBlock))break;
            auto parentID=current->getParentID();if(TxPoWSearcher::searchChainForTxPoWBlock(parentID))break;
            auto parent=services.getTxPoW(current->getParentID().to0xString());
            if(!parent){NIOManager::sendNetworkMessage(mClientUID,MSG_TXPOWREQ,current->getParentID());break;}
            for(const auto blockIDs=parent->getBlockTransactions();const auto id:TxBody::required(blockIDs))if(!services.txPoWExists(TxBody::required(id).to0xString()))NIOManager::sendNetworkMessage(mClientUID,MSG_TXPOWREQ,TxBody::required(id));
            current=parent;++counter;
        }
        if(mClientUID.value()=="0x00"||*mClientUID=="0x01")return;
        auto last=mLastChainSync.get(*mClientUID).value_or(0);auto time=now();if(difference(time,last)<600000)return;mLastChainSync.put(*mClientUID,time);
        counter=0;auto node=tree()->getTip();while(node&&counter<256){++counter;if(!node->checkFullTxns(services.txPoWExists))for(const auto blockIDs=node->getTxPoW().getBlockTransactions();const auto id:TxBody::required(blockIDs))if(!services.txPoWExists(TxBody::required(id).to0xString()))NIOManager::sendNetworkMessage(mClientUID,MSG_TXPOWREQ,TxBody::required(id));node=node->getParent();}
    }
}
void NIOMessage::receivePulse(std::span<const uint8_t> data,size_t& offset){
    auto pulse=Pulse::deserialise(data,offset);auto myList=tree()->getPulseList();std::vector<MiniData> requests;std::set<std::string> full;
    for(const auto& block:myList)full.insert(block.to0xString());auto start=now();bool found=false;const auto& blocks=pulse.getBlockList();
    if(blocks.size()>1000){log("Too many messages in PULSE "+std::to_string(blocks.size())+" max:1000");return;}
    int counter=0;for(const auto& block:blocks){
        if(TxPoWSearcher::searchChainForTxPoWBlock(block)){found=true;break;}
        ++counter;auto id=block.to0xString();auto check=services.getTxPoW(id);
        if(!check)requests.insert(requests.begin(),block);
        else if(!GeneralParams::TXBLOCK_NODE)for(const auto blockIDs=check->getBlockTransactions();const auto txn:TxBody::required(blockIDs))if(!services.txPoWExists(TxBody::required(txn).to0xString()))requests.insert(requests.begin(),TxBody::required(txn));
        if(full.contains(id))found=true;
    }
    auto elapsed=difference(now(),start);if(counter>0)log("PULSE("+std::to_string(counter)+"/"+std::to_string(blocks.size())+") from:"+mClientUID.value_or("null")+" TIME:"+std::to_string(elapsed)+"ms req:"+std::to_string(requests.size())+" crossover:"+boolean(found));
    if(found){for(const auto& id:requests)NIOManager::sendNetworkMessage(mClientUID,GeneralParams::TXBLOCK_NODE?MSG_TXBLOCKREQ:MSG_TXPOWREQ,id);}
    else{
        if(!mFullAdrress.value().empty())services.addInvalidPeer(*mFullAdrress);
        auto client=required(manager()->getNIOServer())->getClient(mClientUID);if(!client){manager()->disconnect(mClientUID,true);return;}
        int port=client->getPort();if(client->getMinimaPort()==-1)port=client->getMinimaPort();
        log("[!] No Crossover found whilst syncing with new node. They are on a different chain. Please check you are on the correct chain.. disconnecting from "+client->getHost().value_or("null")+":"+std::to_string(port));
        services.addInvalidPeer(client->getFullAddress());manager()->disconnect(mClientUID,true);
    }
}
void NIOMessage::receivePing(std::span<const uint8_t> data,size_t& offset){
    (void)MiniData::deserialise(data,offset);Greeting greeting;auto& extra=greeting.getExtraData();extra.put("welcome","hi there!");auto tip=tree()->getTip();
    if(tip){extra.put("topblock",tip->getBlockNumber().toString());extra.put("tophash",tip->getTxPoW().getTxPoWID());auto past=required(tip->getParent(100));extra.put("50block",past->getBlockNumber().toString());extra.put("50hash",past->getTxPoW().getTxPoWID());}
    else{extra.put("topblock","0");extra.put("tophash","0x00");extra.put("50block","0");extra.put("50hash","0x00");}
    extra.put("connections",manager()->getAllConnectedDetails());
    if(GeneralParams::P2P2_ENABLED){auto peers=services.getP2P2Peers();extra.put("peers-list",peers?json::JSONValue(*MiniUtil::convertArrayList(*peers)):json::JSONValue{});}
    else if(GeneralParams::P2P_ENABLED){extra.put("peers-list",p2p::InetSocketAddressIO::addressesListToJSONArray(services.getPeersCopy()));extra.put("clients",jsonFloat(services.getP2PClients()));}
    else{extra.put("peers-list",json::JSONArray());extra.put("clients",0);}
    NIOManager::sendNetworkMessage(mClientUID,MSG_SINGLE_PONG,greeting);
}
std::shared_ptr<TxBlock> NIOMessage::lookupTxBlock(const std::string& id,bool archive){auto block=services.findTxBlock(id);if(!block){auto node=tree()->findNode(id);if(node)block=node->getSharedTxBlock();}if(!block&&archive)block=services.loadArchiveBlock(id);return block;}
void NIOMessage::run(){
    if(!mData)throw std::runtime_error("Null NIO message data");auto retained=mData;const auto& data=retained->bytes();
    if(services.isShuttingDownOrRestoring()){mData.reset();return;}
    struct Clear{std::shared_ptr<MiniData>& data;~Clear(){data.reset();}} clear{mData};
    size_t offset=0;
    try{
        auto type=MiniByte::deserialise(data,offset);auto code=type.getValue();
        if(services.isSyncIBD()&&(code==2||code==18||code==6)){log("Ignoring NIOmessage during IBD Sync.. type:"+convertMessageType(type));return;}
        if(GeneralParams::TXBLOCK_NODE&&code==2)return;
        auto trace="[NIOMessage] uid:"+mClientUID.value_or("null")+" type:"+convertMessageType(type)+" size:"+MiniFormat::formatSize(data.size());
        if(mTrace&&unicode::decodeUTF8(trace).find(unicode::decodeUTF8(mFilter.value()))!=std::u16string::npos)log(trace);
        if(GeneralParams::NETWORKING_LOGS)log("[NETLOGS RECEIVED] from:"+mClientUID.value_or("null")+" type:"+convertMessageType(type)+" size:"+MiniFormat::formatSize(data.size()));
        try{required(manager()->getTrafficListener())->addReadBytes(convertMessageType(type),int64_t(data.size()));}catch(const std::bad_alloc&){throw;}catch(const std::exception&){}
        switch(code){
        case 0:receiveGreeting(data,offset);break;
        case 1:{
            if(GeneralParams::IBDSYNC_LOGS)log("Received IBD size:"+MiniFormat::formatSize(data.size()));auto ibd=std::make_shared<IBD>(IBD::deserialise(data,offset));
            if(GeneralParams::IBDSYNC_LOGS)log("Received IBD blocks:"+std::to_string(ibd->getTxBlocks()->size()));
            if(!ibd->checkValidData()){log("Received INVALID IBD from "+mClientUID.value_or("null"));if(!mFullAdrress.value().empty())services.addInvalidPeer(*mFullAdrress);manager()->disconnect(mClientUID,true);return;}
            if(required(services.getCascade())->getLength()>0&&ibd->hasCascadeWithBlocks()){
                if(!IBD::checkOurChainHeavier(*ibd)){
                    services.notifyEvent("MDS_HEAVIER_CHAIN",json::JSONObject());log("[!] CONNECTED TO HEAVIER CHAIN.. from "+mClientUID.value_or("null")+" ..disconnecting");manager()->disconnect(mClientUID,true);
                    if(!GeneralParams::RESCUE_MEGAMMR_NODE.empty()){HEAVIER_CHAIN_FOUND=std::bit_cast<int32_t>(uint32_t(HEAVIER_CHAIN_FOUND)+1);if(HEAVIER_CHAIN_FOUND>0){log("RESCUE NODE FOUND.. attempting rescue @ "+GeneralParams::RESCUE_MEGAMMR_NODE);services.postMainTimer(std::make_shared<TimerMessage>(1000,"MAIN_DO_RESCUE"));}}
                    return;
                }else log("[!] Received IBD with cascade even though we have one.. from "+mClientUID.value_or("null"));
            }
            log("[+] Connected to the blockchain Initial Block Download received. size:"+MiniFormat::formatSize(data.size())+" blocks:"+std::to_string(ibd->getTxBlocks()->size()));services.postProcessIBD(ibd,mClientUID);break;
        }
        case 2:{auto id=MiniData::deserialise(data,offset);if(!services.txPoWExists(id.to0xString()))NIOManager::sendNetworkMessage(mClientUID,MSG_TXPOWREQ,id);break;}
        case 3:{auto id=MiniData::deserialise(data,offset);auto txpow=services.getTxPoW(id.to0xString());if(txpow)NIOManager::sendNetworkMessage(mClientUID,MSG_TXPOW,txpow);break;}
        case 4:receiveTxPoW(data,offset);break;
        case 5:if(!GeneralParams::ALLOW_GEN_MESSAGE){log("Simple Message from  "+mClientUID.value_or("null")+" not allowed.. disconnecting");manager()->disconnect(mClientUID,true);return;}else{auto message=MiniString::deserialise(data,offset);log(mClientUID.value_or("null")+":"+message.toString());}break;
        case 6:receivePulse(data,offset);break;
        case 7:{
            auto text=MiniString::deserialise(data,offset);if(!GeneralParams::P2P_ENABLED)return;auto client=required(manager()->getNIOServer())->getClient(mClientUID);if(!client){log(mClientUID.value_or("null")+" Error null client on P2P NIOMessage..");return;}
            [[maybe_unused]] NIOClientInfo info(client,true);auto parsed=json::parser::JSONParser().parse(text.toString());auto json=parsed.isNull()?std::shared_ptr<json::JSONObject>{}:std::get<std::shared_ptr<json::JSONObject>>(parsed.value);
            if(!client->hasReceivedP2PGreeting()){auto message=std::make_shared<TimerMessage>(10000,"P2P_MESSAGE");message->addString("uid",mClientUID).addObject("message",MessageObject(json));services.postP2PTimer(message);}
            else{auto message=std::make_shared<Message>("P2P_MESSAGE");message->addString("uid",mClientUID).addObject("message",MessageObject(json));services.postP2P(message);}break;
        }
        case 8:(void)MiniData::deserialise(data,offset);break;
        case 11:receivePing(data,offset);break;
        case 13:{
            auto last=TxPoW::deserialise(data,offset);
            if(GeneralParams::ARCHIVESYNC_LIMIT_BANDWIDTH){auto total=required(manager()->getTrafficListener())->getTotalWrite();auto text=MiniFormat::formatSize(total);if(total>NIOManager::MAX_ARCHIVE_WRITE){log("MAX Bandwith used already ("+text+") - no more archive sync for 24hours..");return;}}
            auto previous=mlastSyncReq.get(mClientUID.value());if(previous&&previous->isEqual(last.getBlockNumber())){log("[+] Received SAME Sync IBD Request from "+*mClientUID+" @ "+last.getBlockNumber().toString()+" IGNORING");return;}
            mlastSyncReq.put(*mClientUID,last.getBlockNumber());IBD ibd;ibd.createSyncIBD(last);NIOManager::sendNetworkMessage(mClientUID,MSG_IBD_RESP,ibd);break;
        }
        case 14:{auto ibd=std::make_shared<IBD>(IBD::deserialise(data,offset));if(!ibd->getTxBlocks()->empty()){auto top=ibd->getTxBlocks()->at(0)->getTxPoW().getBlockNumber();if(GeneralParams::IBDSYNC_LOGS){auto time=ibd->getTxBlocks()->at(0)->getTxPoW().getTimeMilli().getAsLong();log("[+] Received Sync IBD. size:"+MiniFormat::formatSize(data.size())+" blocks:"+std::to_string(ibd->getTxBlocks()->size())+" top:"+top.toString()+" @ "+javaDateString(time));}services.postProcessSyncIBD(ibd,mClientUID);}break;}
        case 15:{auto first=MiniNumber::deserialise(data,offset);IBD ibd;if(first.isEqual(MiniNumber::MINUSONE))log("Archive IBD connection test..");else{log("Archive IBD request start @ "+first.toString());ibd.createArchiveIBD(first);}NIOManager::sendNetworkMessage(mClientUID,MSG_ARCHIVE_DATA,ibd);break;}
        case 16:log("Received MSG_ARCHIVE_DATA msg.. ignoring.. from "+mClientUID.value_or("null"));break;
        case 17:break; // Entire pinned single-archive request body is commented out.
        case 18:if(GeneralParams::TXBLOCK_NODE){auto id=MiniData::deserialise(data,offset);if(!lookupTxBlock(id.to0xString(),false))NIOManager::sendNetworkMessage(mClientUID,MSG_TXBLOCKREQ,id);}break;
        case 19:{auto id=MiniData::deserialise(data,offset);auto block=lookupTxBlock(id.to0xString(),true);if(block)NIOManager::sendNetworkMessage(mClientUID,MSG_TXBLOCK,block);else log("Request for TxBlock we don't have : "+id.to0xString());break;}
        case 20:if(GeneralParams::TXBLOCK_NODE){auto block=std::make_shared<TxBlock>(TxBlock::deserialise(data,offset));services.postProcessTxBlock(block);auto root=required(tree()->getRoot());if(block->getTxPoW().getBlockNumber().isMoreEqual(root->getBlockNumber())){auto parent=block->getTxPoW().getParentID();if(!lookupTxBlock(parent.to0xString(),false)){log("Request Parent TxBlock.. @ "+block->getTxPoW().getBlockNumber().toString());NIOManager::sendNetworkMessage(mClientUID,MSG_TXBLOCKREQ,parent);}}}break;
        case 21:if(GeneralParams::TXBLOCK_NODE){auto txpow=std::make_shared<TxPoW>(TxPoW::deserialise(data,offset));TxBody::required(txpow->getTxBody()).resetRandomPRNG();auto time=now();if(difference(time,LAST_TXBLOCKMINE_MSG.load())<services.getAutomineTimer())return;LAST_TXBLOCKMINE_MSG=time;services.mineTxPoWAsync(txpow);}break;
        case 22:if(!GeneralParams::IS_MEGAMMR){log("[!] Attempt to MegaMMR Sync when -megammr not enabled");manager()->disconnect(mClientUID);return;}else{auto sync=MegaMMRSyncData::deserialise(data,offset);log("Received MegaMMR SYNC request.. addresses:"+std::to_string(sync.getAllAddresses()->size())+" pubkeys:"+std::to_string(sync.getAllPublicKeys()->size()));auto ibd=services.getCurrentMegaMMRIBD(sync);NIOManager::sendNetworkMessage(mClientUID,MSG_MEGAMMRSYNC_RESP,ibd);}break;
        default:log("Unknown Message type received from "+mClientUID.value_or("null")+" type:"+type.toString()+" size:"+std::to_string(data.size()));break;
        }
    }catch(const std::bad_alloc&){throw;}catch(const std::exception& error){log(error.what());}
}
}
