#include "restoresync.hpp"
#include "archive.hpp"
#include "org/minima/system/commands/network/connect.hpp"
#include "support/JavaDateString.hpp"
#include <bit>
#include <thread>
#include "vault.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/CipherInput.hpp"
#include "support/GzipInput.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaExceptions.hpp"
namespace minima::commands {
namespace {
int64_t now(){return MinimaLogger::getRuntime().currentTimeMillis();}
int64_t subtract(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)-uint64_t(b));}
int32_t increment(int32_t a){return std::bit_cast<int32_t>(uint32_t(a)+1);}
void pause(int64_t delay){std::this_thread::sleep_for(std::chrono::milliseconds(delay));}
std::shared_ptr<MinimaDB> db(){auto owner=MinimaDB::getDB();sqlRequired(owner);return owner;}
std::shared_ptr<Main> main(){auto owner=Main::getInstance();sqlRequired(owner);return owner;}
const MiniNumber& height(const std::shared_ptr<TxBlock>& block){return sqlRequired(block).getTxPoW().getBlockNumber();}
void log(const std::string& text){MinimaLogger::log(text);}
void collect(){if(auto callback=MinimaLogger::getRuntime().collectMemory)callback();}
void finish(){sqlRequired(Main::getInstance()).setHasShutDown();sqlRequired(Main::getInstance()).shutdownFinalProcs();sqlRequired(MinimaDB::getDB()).saveAllDB();sqlRequired(Main::getInstance()).stopMessageProcessor();sqlRequired(Main::getInstance()).NotifyMainListenerOfShutDown();}
}
restoresync::restoresync():Command("restoresync","[file:] (password:) (host:) (keyuses:) - Restore the entire system AND perform an archive sync. Use when the backup is old."){}
std::optional<std::string> restoresync::getFullHelp() const{return std::string(
"\nrestoresync\n"
"\n"
"Restore your node from a backup and then sync to the top block using an archive node.\n"
"\n"
"You MUST wait until all the keys for the node are created before this is allowed.\n"
"\n"
"Reverts to a standard restore if backup is not older than 2 days.\n"
"\n"
"file:\n"
"    Specify the filename or local path of the backup to restore\n"
"\n"
"password: (optional)\n"
"    Enter the password of the backup \n"
"\n"
"host: (optional)\n"
"    ip:port of the archive node to sync from.\n"
"\n"
"keyuses: (optional) \n"
"    Increment (not set) the number of key uses per key.\n"
"\n"
"Examples:\n"
"\n"
"restoresync file:my-full-backup-01-Jan-22 password:Longsecurepassword456 host:89.98.89.98:9001\n");}
json::JSONValue restoresync::runCommand(){
    auto result=getJSONReply();vault::checkAllKeysCreated();
    auto file=getParam("file","");if(file.empty())throw JavaTypedException("java.lang.Exception","MUST specify a file to restore from",true);
    auto password=getParam("password","minima");if(password.empty())throw CommandException("Cannot have a blank password");
    auto input=MiniFile::createBaseFile(file);std::error_code error;
    if(input.string().find('\0')!=std::string::npos||!std::filesystem::exists(input,error))throw JavaTypedException("java.lang.Exception","Restore file doesn't exist : "+javaUnixAbsoluteFile(input),true);
    if(auto collect=MinimaLogger::getRuntime().collectMemory)collect();
    auto folder=javaUnixChildFile(GeneralParams::DATA_FOLDER,"restore");javaUnixMkdirs(folder);
    auto bytes=MiniFile::readCompleteFile(input);size_t offset=0;
    auto salt=MiniData::deserialise(bytes,offset),iv=MiniData::deserialise(bytes,offset);
    auto key=GenerateKey::secretKey(password,salt.bytes());
    CipherInput cipher(GenerateKey::getCipherSYM(Cipher::DECRYPT_MODE,iv.bytes(),key.bytes),std::span(bytes).subspan(offset));
    std::unique_ptr<GzipInput> gzip;
    try{gzip=std::make_unique<GzipInput>([&](auto data){return cipher.read(data);},[&]{cipher.close();});}
    catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){throw CommandException("Incorrect Password!");}
    sqlRequired(Main::getInstance()).restoreReady(false);
    int64_t total=1;total+=readNextBackup(folder/"wallet.sql",*gzip);
    sqlRequired(MinimaDB::getDB()).setAllowSaveState(false);MinimaLogger::log("Restoring backup files..");
    auto base=sqlRequired(MinimaDB::getDB()).getBaseDBFolder();auto cascade=base/"cascade.db",tree=base/"chaintree.db",user=base/"userprefs.db",peers=base/"p2p.db";
    total+=readNextBackup(cascade,*gzip);total+=readNextBackup(tree,*gzip);total+=readNextBackup(user,*gzip);total+=readNextBackup(peers,*gzip);(void)total;
    sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).loadDB(user);sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).clearUninstalledMiniDAPP();
    sqlRequired(sqlRequired(MinimaDB::getDB()).getP2PDB()).loadDB(peers);sqlRequired(sqlRequired(MinimaDB::getDB()).getCascade()).loadDB(cascade);sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).loadDB(tree);
    auto history=readNextTxPoWList(*gzip);auto sql=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getSQLDB();sqlRequired(sql).wipeDB();
    auto rows=sqlRequired(history).mTxPoWs;if(!rows)throw JavaNullPointerException("null TxPoWList rows");for(const auto& txpow:*rows)sqlRequired(sql).addTxPoW(txpow.get(),true);
    gzip->close();cipher.close();gzip->close();
    sqlRequired(MinimaDB::getDB()).setAllowSaveState(true);
    sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).restoreFromFile(folder/"wallet.sql");
    sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).saveDB(false);
    sqlRequired(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getSQLDB()).saveDB(false);
    sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).saveDB(false);auto archive=sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).getSQLFile();
    if(!archive)throw JavaNullPointerException("null archive SQL file");if(archive->string().find('\0')==std::string::npos)std::filesystem::remove(*archive,error);
    MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);
    sqlRequired(Main::getInstance()).restoreReadyForSync();
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto time=sqlRequired(tip).getTxPoW().getTimeMilli().getAsLong();auto difference=subtract(now(),time);
    auto uses=getNumberParam("keyuses",MiniNumber(256)).getAsInt();
    if(difference<172800000||!existsParam("host")){
        if(existsParam("host"))MinimaLogger::log("No Sync required as new backup");
        sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).updateIncrementAllKeyUses(uses);finish();
        json::JSONObject response;response.put("file",javaUnixAbsoluteFile(input));result.put("restore",response);result.put("message","Restart Minima for restore to take effect!");return result;
    }
    if(existsParam("host")){
        auto parent=tip->getParent(128);auto start=sqlRequired(parent).getBlockNumber();MinimaLogger::log("Start sync from "+start.toString());
        auto host=getParam("host");performResync(host,uses,start,true);
        tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();MinimaLogger::log("End sync on "+sqlRequired(tip).getBlockNumber().toString());
    }
    finish();json::JSONObject response;response.put("file",javaUnixAbsoluteFile(input));result.put("restore",response);result.put("message","Restart Minima for restore to take effect!");
    return result;
}
int64_t restoresync::readNextBackup(const std::filesystem::path& output,GzipInput& input){auto data=MiniData::readFromStream([&](auto bytes){return input.read(bytes);});MiniFile::writeDataToFile(output,data.bytes());return javaUnixFileLength(output);}
std::shared_ptr<TxPoWList> restoresync::readNextTxPoWList(GzipInput& input){return TxPoWList::convertMiniDataVersion(MiniData::readFromStream([&](auto bytes){return input.read(bytes);}));}
json::JSONObject restoresync::performResync(const std::optional<std::string>& fullhost,int32_t keyUses,const MiniNumber* startBlock,bool incrementKeys){
    auto listener=Main::getMinimaListener();archive::NotifyListener(listener,"Loading sync blocks from "+(startBlock?startBlock->toString():std::string("null")));
    auto connection=connect::createConnectMessage(fullhost);auto host=sqlRequired(connection).getString("host");auto port=connection->getInteger("port");
    if(!archive::sendArchiveReq(host,port,MiniNumber::MINUSONE)){log("Could not connect to Archive host! @ "+host.value_or("null")+":"+std::to_string(port));return json::JSONObject();}
    main()->archiveResetReady(false,false);auto wallet=db()->getWallet();
    if(incrementKeys)sqlRequired(wallet).updateIncrementAllKeyUses(keyUses);else sqlRequired(wallet).updateAllKeyUses(keyUses);
        auto startblock=startBlock?std::make_shared<MiniNumber>(*startBlock):nullptr;auto endblock=MiniNumber::ZERO,firstStart=MiniNumber::ZERO;int64_t starttime=0,lastlog=0;bool found=false,first=true;int32_t counter=0;
        log("System clean..");collect();
        while(true){
            sqlRequired(db()->getTxPoWDB()).wipeDBRAM();counter=increment(counter);
            if(counter%5==0){auto runtime=MinimaLogger::getRuntime();if(subtract(runtime.totalMemory(),runtime.freeMemory())>262144000)main()->resetMemFull();}
            auto ibd=archive::sendArchiveReq(host,port,startblock.get());
            if(!ibd){log("No blocks returned..");ibd=std::make_shared<IBD>();}
            if(sqlRequired(startblock).isEqual(MiniNumber::ZERO)&&ibd->hasCascade()){
                log("Cascade Received.. "+sqlRequired(sqlRequired(ibd->getCascade()).getTip()).getTxPoW().getBlockNumber().toString());db()->setIBDCascade(ibd->getCascade());sqlRequired(db()->getArchive()).checkCascadeRequired(ibd->getCascade().get());
            }
            auto blocks=ibd->getTxBlocks();auto size=sqlRequired(blocks).size();
            if(size>0){found=true;auto start=blocks->front();if(first){first=false;firstStart=height(start);}auto last=blocks->back();endblock=height(last);startblock=std::make_shared<MiniNumber>(endblock.increment());starttime=last->getTxPoW().getTimeMilli().getAsLong();archive::NotifyListener(listener,"Loading "+height(start).toString()+" @ "+javaDateString(start->getTxPoW().getTimeMilli().getAsLong()));}
            else log("No Archive TxBlocks left..");
            sqlRequired(main()->getTxPoWProcessor()).postProcessArchiveIBD(ibd,"0x00");bool error=false;auto tip=sqlRequired(db()->getTxPoWTree()).getTip();int attempts=0;
            while(found&&!tip){pause(250);tip=sqlRequired(db()->getTxPoWTree()).getTip();if(++attempts>5000){error=true;break;}}
            if(error){log("ERROR : There was an error processing that FIRST IBD");break;}
            (void)now();attempts=0;
            while(found){if(!sqlRequired(tip).getBlockNumber().isEqual(endblock))pause(50);else break;tip=sqlRequired(db()->getTxPoWTree()).getTip();if(++attempts>20000){error=true;break;}}
            if(subtract(now(),lastlog)>5000){log("IBD Processed.. block:"+sqlRequired(startblock).toString()+" @ "+MinimaLogger::DATEFORMAT.format(starttime));lastlog=now();}
            if(error){log("ERROR : There was an error processing that IBD - took too long");break;}if(size==0)break;
        }
        archive::NotifyListener(listener,"All blocks loaded.. pls wait");log("All Archive data received and processed.. shutting down..");json::JSONObject response;
        response.put("message","Archive sync completed.. shutting down now.. please restart after");response.put("start",firstStart.toString());response.put("end",endblock.toString());return response;
}
}
