#include "megammrsync.hpp"
#include "../archive.hpp"
#include "../vault.hpp"
#include "../../network/connect.hpp"
#include "../../network/peers.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/BIP39.hpp"
#include "support/CommandResult.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/NIOConnections.hpp"
#include "support/NativeThrowable.hpp"
#include <set>
#include <thread>
namespace minima::commands {
namespace {
void log(const std::string& text){MinimaLogger::log(text);}
std::shared_ptr<MinimaDB> db(){auto value=MinimaDB::getDB();sqlRequired(value);return value;}
std::shared_ptr<Main> main(){auto value=Main::getInstance();sqlRequired(value);return value;}
const std::string& text(const std::optional<std::string>& value){if(!value)throw JavaNullPointerException("null String");return *value;}
json::JSONObject object(const json::JSONValue& value){if(value.isNull())throw JavaNullPointerException("null JSONObject");auto result=std::get_if<std::shared_ptr<json::JSONObject>>(&value.value);if(!result)throw JavaClassCastException("Cannot cast response to JSONObject");return sqlRequired(*result);}
void awaitIBD(){while(true){std::this_thread::sleep_for(std::chrono::milliseconds(250));if(sqlRequired(main()->getTxPoWProcessor()).isIBDProcessFinished())break;}}
}
megammrsync::megammrsync():Command("megammrsync","[action:] [host:] (phrase:) (keys:) (keyuses:) (file:) (password:) - Restore from a MegaMMR node"){}
std::optional<std::string> megammrsync::getFullHelp() const{return std::string(
"\nmegammrsync\n"
"\n"
"Perform a chain or seed re-sync from a MegaMMR node. Fast.\n"
"\n"
"If you are on the wrong chain - all you need to provide is the 'host' to connect to.\n"
"\n"
"If you are on a fresh node, with different seed phrase, provide 'host' and 'phrase' to resync with that Wallet.\n"
"\n"
"You can load an old backup and resync to the chain tip aswell.\n"
"\n"
"The host you connect to MUST be running with -megammr.\n"
"\n"
"action:\n"
"    mydetails : Shows which addresses and public keys you are searching for.\n"
"    resync : Perform the actual MegaMMR resync.\n"
"\n"
"host:\n"
"    ip:port of the node to sync from. Use with action:resync.\n"
"\n"
"phrase: (optional)\n"
"    To seed re-sync, enter your seed phrase in double quotes. Use with action:resync.\n"
"    This will replace the current seed phrase of this node. You do NOT have to do this if you still have access to your wallet.\n"
"\n"
"anyphrase: (optional)\n"
"    true or false. If you set a custom seed phrase on startup, you can set this to true. Default is false.\n"
"\n"
"keys: (optional) \n"
"    Number of keys to create if you need to do a seed re-sync. Default is 64.\n"
"\n"
"keyuses: (optional) \n"
"    How many times at most you used your keys..\n"
"    Every time you re-sync with seed phrase this needs to be higher as Minima Signatures are stateful.\n"
"    Defaults to 1000 - the max is 262144 for normal keys.\n"
"\n"
"file: (optional)\n"
"    Specify the filename or local path of the backup to restore\n"
"\n"
"password: (optional)\n"
"    Enter the password of the backup \n"
"\n"
"\n"
"Examples:\n"
"\n"
"megammrsync action:mydetails\n"
"\n"
"megammrsync action:resync host:98.65.45.34:9001\n"
"\n"
"megammrsync action:resync host:98.65.45.34:9001 file:myoldbakup.bak password:backup_password\n"
"\n"
"megammrsync action:resync host:98.65.45.34:9001 phrase:\"YOUR 24 WORD SEED PHRASE\" keyuses:2000\n");}
json::JSONValue megammrsync::runCommand(){
    auto result=getJSONReply();auto action=getParam("action");
    if(action=="mydetails"){
        auto rows=sqlRequired(db()->getWallet()).getAllAddresses();json::JSONArray details;
        for(const auto& row:rows){auto key=sqlRequired(row).getPublicKey();if(text(key)!="0x00"){json::JSONObject item;item.put("publickey",*key);auto address=row->getAddress();item.put("address",address?json::JSONValue(*address):json::JSONValue());details.add(item);}}
        json::JSONObject response;response.put("details",details);response.put("size",int64_t(details.size()));result.put("response",response);
    }else if(action=="resync"){
        vault::stopAllKeysCreated();auto fullhost=getParam("host");auto connection=connect::createConnectMessage(fullhost);
        if(!connection)throw CommandException("Invalid HOST format for resync : "+fullhost);
        auto host=connection->getString("host");auto port=connection->getInteger("port");
        if(!archive::sendArchiveReq(host,port,&MiniNumber::MINUSONE,1))throw CommandException("Could not connect to Archive host! @ "+host.value_or("null")+":"+std::to_string(port));
        log("Check Connection passed!");auto keys=getNumberParam("keys",MiniNumber(Wallet::NUMBER_GETADDRESS_KEYS.load())).getAsInt();auto uses=getNumberParam("keyuses",MiniNumber(1000)).getAsInt();
        auto file=getParam("file","");auto phrase=getParam("phrase","");
        if(!phrase.empty()){
            auto any=getBooleanParam("anyphrase",false);auto clean=any?phrase:BIP39::cleanSeedPhrase(phrase);main()->archiveResetReady(true);
            log("Resetting all wallet private keys..");auto seed=BIP39::convertStringToSeed(clean);auto wallet=db()->getWallet();sqlRequired(wallet).updateSeedRow(clean,seed.to0xString());
            log("Creating a total of "+std::to_string(keys)+" keys / addresses..");for(int32_t i=0;i<keys;++i){log("Creating key "+std::to_string(i));wallet->createNewSimpleAddress(true);}log("All keys created..");wallet->updateAllKeyUses(uses);
        }else if(!file.empty()){
            auto command="restore shutdown:false file:"+file;if(existsParam("password"))command+=" password:"+getParam("password");
            auto restored=CommandRunner::getRunner().runSingleCommand(command);if(!command_result::status(restored))throw CommandException("Error restoring.. "+restored.toJSONString());
            main()->restoreReadyForSync();main()->archiveResetReady(false);
        }else main()->archiveResetReady(false);
        auto data=getMyDetails();auto responseData=sendMegaMMRSyncReq(host,port,data);if(!responseData)throw CommandException("Error getting MegaMMR data from host");
        main()->PostNotifyEvent("MDS_RESYNC_START",json::JSONObject());if(GeneralParams::IS_MEGAMMR)sqlRequired(db()->getMegaMMR()).clear();
        sqlRequired(main()->getTxPoWProcessor()).postProcessIBD(responseData->getIBD(),"0x00",true);awaitIBD();
        auto proofs=responseData->getAllCoinProofs();auto size=sqlRequired(proofs).size();log("Import CoinProofs.. "+std::to_string(size));
        for(const auto& proof:*proofs)CommandRunner::getRunner().runSingleCommand("coinimport track:true data:"+MiniData(sqlRequired(proof).serialise()).to0xString());
        log("Peers List : "+responseData->getPeersList().value_or("null"));json::JSONObject response;response.put("message","MegaMMR sync fininshed.. please restart");response.put("coins",int64_t(size));result.put("response",response);
        main()->setHasShutDown();main()->shutdownFinalProcs();db()->saveAllDB();updateP2PDB(responseData->getPeersList());main()->stopMessageProcessor();main()->NotifyMainListenerOfShutDown();
    }
    return result;
}
void megammrsync::updateP2PDB(const std::optional<std::string>& peersText){
    const auto& peers=text(peersText);auto addresses=std::make_shared<std::vector<std::shared_ptr<InetSocketAddress>>>();size_t start=0;
    while(start<peers.size()){auto end=peers.find(',',start);if(end==std::string::npos)end=peers.size();if(end>start){auto peer=peers.substr(start,end-start);
        try{auto connection=connect::createConnectMessage(peer);auto host=sqlRequired(connection).getString("host");addresses->push_back(std::make_shared<InetSocketAddress>(host,connection->getInteger("port")));}
        catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){log("Invalid peer in MegaMMR peersList : "+peer);}
    }start=end+1;}
    auto file=db()->getBaseDBFolder()/"p2p.db";p2p::P2PDB peerDB;peerDB.loadDB(file);peerDB.setPeersList(addresses);peerDB.saveDB(file);
}
MegaMMRSyncData megammrsync::getMyDetails(){
    auto addresses=std::make_shared<MegaMMRSyncData::DataList>(),keys=std::make_shared<MegaMMRSyncData::DataList>();auto wallet=db()->getWallet();
    for(const auto& row:sqlRequired(wallet).getAllAddresses())if(sqlRequired(row).isTrack())addresses->push_back(std::make_shared<MiniData>(MiniData::fromHex(text(row->getAddress()))));
    for(const auto& row:wallet->getAllKeys())keys->push_back(std::make_shared<MiniData>(MiniData::fromHex(text(sqlRequired(row).getPublicKey()))));
    return MegaMMRSyncData(addresses,keys);
}
std::shared_ptr<MegaMMRIBD> megammrsync::getCurrentMegaMMRIBD(const MegaMMRSyncData& data){
    auto proofs=getAllCoinProofs(data);auto ibd=std::make_shared<IBD>();ibd->createCompleteIBD();auto result=std::make_shared<MegaMMRIBD>(ibd,proofs);result->setPeersList(peers::getPeersList(20));return result;
}
std::shared_ptr<MegaMMRIBD::Proofs> megammrsync::getAllCoinProofs(const MegaMMRSyncData& data){
    auto proofs=std::make_shared<MegaMMRIBD::Proofs>();auto coins=searchMegaCoins(data.getAllAddresses(),data.getAllPublicKeys());
    for(const auto& coin:coins){auto result=object(CommandRunner::getRunner().runSingleCommand("coinexport coinid:"+coin.getCoinID().to0xString()));auto response=object(result.get("response"));auto encoded=MiniData::fromHex(response.getString("data"));auto proof=CoinProof::convertMiniDataVersion(encoded);proofs->push_back(proof?std::make_shared<CoinProof>(*proof):nullptr);}return proofs;
}
std::vector<Coin> megammrsync::searchMegaCoins(const std::shared_ptr<MegaMMRSyncData::DataList>& addresses,const std::shared_ptr<MegaMMRSyncData::DataList>& keys){
    std::vector<Coin> found;auto tip=sqlRequired(db()->getTxPoWTree()).getTip();std::set<std::string> spent;bool mega=false;
    while(tip||mega){std::vector<Coin> coins;
        if(!mega)coins=tip->getAllCoins();else{db()->readLock(true);for(const auto& coin:sqlRequired(db()->getMegaMMR()).getAllCoins().elements())coins.push_back(sqlRequired(coin));}
        for(const auto& coin:coins){bool matches=false;
            for(const auto& address:sqlRequired(addresses)){if(coin.getAddress().isEqual(sqlRequired(address))||coin.checkForStateVariable(address->to0xString())){matches=true;break;}}
            if(!matches)for(const auto& key:sqlRequired(keys))if(coin.checkForStateVariable(sqlRequired(key).to0xString())){matches=true;break;}
            if(!matches)continue;auto id=coin.getCoinID().to0xString();if(coin.getSpent())spent.insert(id);else if(!spent.contains(id)){found.push_back(coin.deepCopy());spent.insert(id);}
        }
        if(!mega){tip=tip->getParent();if(!tip&&GeneralParams::IS_MEGAMMR)mega=true;}else{db()->readLock(false);break;}
    }return found;
}
std::shared_ptr<MegaMMRIBD> megammrsync::sendMegaMMRSyncReq(const std::optional<std::string>& host,int32_t port,const MegaMMRSyncData* data){
    try{auto message=NIOManager::createNIOMessage(NIOMessage::MSG_MEGAMMRSYNC_REQ,std::function<wire::Bytes()>([&]{if(!data)throw JavaNullPointerException("null sync data");return data->serialise();}));
        auto socket=nioConnect(InetSocketAddress(host,port),180000);nioWriteAll(socket,message.serialise());sqlRequired(sqlRequired(main()->getNIOManager()).getTrafficListener()).addWriteBytes("sendMegaMMRSyncReq",int64_t(message.size()));
        auto response=MiniData::readFromStream([&](auto bytes)->size_t{auto count=nioReadSome(socket,bytes,180000);return count<0?0:size_t(count);});sqlRequired(sqlRequired(main()->getNIOManager()).getTrafficListener()).addReadBytes("sendMegaMMRSyncReq",int64_t(response.size()));socket->close();size_t offset=0;(void)MiniByte::deserialise(response.bytes(),offset);return std::make_shared<MegaMMRIBD>(MegaMMRIBD::deserialise(response.bytes(),offset));
    }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){log("MegaMMR Sync connection : "+nativeExceptionText(error)+" @ "+host.value_or("null")+":"+std::to_string(port));return nullptr;}
}
}
