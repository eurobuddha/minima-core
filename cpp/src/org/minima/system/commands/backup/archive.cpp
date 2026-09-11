#include "archive.hpp"
#include "vault.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/commands/network/connect.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/BIP39.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/CommandResult.hpp"
#include "support/GzipOutput.hpp"
#include "support/BufferedFileOutput.hpp"
#include "support/JavaDateString.hpp"
#include "support/JavaLowerCase.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/NIOConnections.hpp"
#include "support/NativeThrowable.hpp"
#include <bit>
#include "support/NativeThread.hpp"
namespace minima::commands {
namespace {
void log(const std::string& text){MinimaLogger::log(text);}
void collect(){if(auto callback=MinimaLogger::getRuntime().collectMemory)callback();}
int64_t now(){return MinimaLogger::getRuntime().currentTimeMillis();}
int64_t subtract(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)-uint64_t(b));}
int32_t intadd(int32_t a,int32_t b){return std::bit_cast<int32_t>(uint32_t(a)+uint32_t(b));}
void pause(int64_t delay){nativeThreadSleep(delay);}
void removeFile(const std::filesystem::path& file){std::error_code error;if(file.string().find('\0')==std::string::npos)std::filesystem::remove(file,error);}
bool fileExists(const std::filesystem::path& file){std::error_code error;return file.string().find('\0')==std::string::npos&&std::filesystem::exists(file,error);}
std::shared_ptr<MinimaDB> db(){auto owner=MinimaDB::getDB();sqlRequired(owner);return owner;}
std::shared_ptr<Main> main(){auto owner=Main::getInstance();sqlRequired(owner);return owner;}
const MiniNumber& height(const std::shared_ptr<TxBlock>& block){return sqlRequired(block).getTxPoW().getBlockNumber();}
void logArchive(const std::shared_ptr<ArchiveManager>& archive){
    auto cascade=sqlRequired(archive).loadCascade();if(cascade)log("Archive DB cascade start : "+sqlRequired(cascade->getTip()).getTxPoW().getBlockNumber().toString()+" length:"+std::to_string(cascade->getLength()));
    auto first=archive->loadFirstBlock();if(first)log("Archive DB first block : "+height(first).toString());
    auto last=archive->loadLastBlock();if(last)log("Archive DB last block : "+height(last).toString());
}
void finishSync(){main()->setHasShutDown();main()->shutdownFinalProcs();db()->saveAllDB();main()->stopMessageProcessor();main()->NotifyMainListenerOfShutDown();}
}
archive::archive():Command("archive","[action:] (host:) (phrase:) (keys:) (keyuses:) - Resync your chain with seed phrase if necessary (otherwise wallet remains the same)"){}
std::optional<std::string> archive::getFullHelp() const{return std::string(
"\narchive\n"
"\n"
"Perform a chain or seed re-sync from an archive node or archive export file.\n"
"\n"
"A chain re-sync will put your node on the correct chain.\n"
"\n"
"Use a chain re-sync if your node has been offline for too long and cannot catchup. Seed Phrase is not required.\n"
"\n"
"A seed re-sync will wipe the wallet, re-generate your private keys and restore your coins.\n"
"\n"
"Only do a seed re-sync if you have lost your node and do not have a backup.\n"
"\n"
"You can also perform checks on your archive db or archive file and check an address.\n"
"\n"
"action:\n"
"    integrity : Check the integrity of your archive db. No host required.\n"
"    inspect : inspect an archive export .gzip file. If 'last:1', the file can re-sync any node from genesis.\n"
"    export : Export your archive db to a .gzip file. \n"
"    exportraw : Export your archive db to a raw .dat file (recommended).\n"
"    resync : Use with 'host' to do a chain or seed re-sync from an archive node. \n"
"    import : Use with 'file' to do a chain or seed re-sync using a .dat or .gzip archive file. \n"
"    addresscheck : check your archive db for spent and unspent coins at a specific address.\n"
"\n"
"host: (optional) \n"
"    ip:port of the archive node to sync from. Use with action:resync.\n"
"\n"
"file: (optional) \n"
"    name or path of the archive export gzip file to export/import/inspect.\n"
"\n"
"phrase: (optional)\n"
"    To seed re-sync, enter your seed phrase in double quotes. Use with action:import or resync.\n"
"    This will replace the current seed phrase of this node. You do NOT have to do this if you still have access to your wallet.\n"
"    In this case, use action:import or resync without 'phrase' to get on the correct chain.\n"
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
"address: (optional) \n"
"    The wallet or script address to search for in the archive. Use with action:addresscheck.\n"
"    If using a script address, also provide an address or public key in the 'statecheck' parameter.\n"
"    Use with action:addresscheck.\n"
"\n"
"statecheck: (optional) \n"
"    Data to search for in a coin's state variables.\n"
"    Combine with a script address in the 'address' parameter to search for coins locked in a contract.\n"
"\n"
"logs: (optional) \n"
"    true or false. Show detailed logs, default false.\n"
"\n"
"maxexport: (optional) \n"
"    How many blocks to export to a raw .dat file. Useful for testing purposes.\n"
"\n"
"Examples:\n"
"\n"
"archive action:integrity\n"
"\n"
"archive action:inspect file:archiveexport-ddmmyy.gzip\n"
"\n"
"archive action:exportraw file:archiveexport-ddmmyy.raw.dat\n"
"\n"
"archive action:resync host:89.98.89.98:9001\n"
"\n"
"archive action:import file:archiveexport-ddmmyy.raw.dat\n"
"\n"
"archive action:import file:archiveexport-ddmmyy.raw.dat phrase:\"YOUR 24 WORD SEED PHRASE\" keyuses:2000\n"
"\n"
"archive action:addresscheck address:0xFED.. statecheck:0xABC..\n");}
json::JSONValue archive::runCommand(){
    auto ret=getJSONReply();auto action=getParam("action");auto arch=db()->getArchive();
    if(action=="integrity"){
        log("Checking Archive DB.. this may take some time..");auto starter=sqlRequired(arch).loadLastBlock();
        bool startcheck=true;auto lastlog=MiniNumber::ZERO,start=MiniNumber::ZERO;
        if(!starter){log("You have no Archive blocks..");startcheck=false;}
        else{lastlog=height(starter);start=lastlog;if(height(starter).isEqual(MiniNumber::ONE))log("ArchiveDB starts at root");}
        auto cascstart=MiniNumber::MINUSONE;json::JSONObject cascjson;auto cascade=arch->loadCascade();
        if(cascade){cascjson.put("exists",true);cascstart=sqlRequired(cascade->getTip()).getTxPoW().getBlockNumber();cascjson.put("tip",cascstart.toString());cascjson.put("length",cascade->getLength());}
        else cascjson.put("exists",false);
        std::optional<MiniData> parenthash;std::optional<MiniNumber> parentnum;int32_t errors=0,total=0;
        auto archstart=start,archend=archstart;json::JSONObject archjson;archjson.put("start",archstart.toString());
        while(startcheck){
            if(lastlog.isLess(start.sub(MiniNumber(2000)))){log("Now checking from  "+start.toString());lastlog=start;}
            auto end=start.add(MiniNumber::TWOFIVESIX);auto blocks=arch->loadBlockRange(start.decrement(),end,false);
            for(const auto& block:blocks){archend=height(block);total=intadd(total,1);const auto& pow=sqlRequired(block).getTxPoW();
                if(!parenthash){parenthash=pow.getTxPoWIDData();parentnum=pow.getBlockNumber();lastlog=*parentnum;archstart=*parentnum;log("ArchiveDB blocks resync start at block "+parentnum->toString()+" @ "+javaDateString(pow.getTimeMilli().getAsLong()));}
                else{if(!pow.getBlockNumber().isEqual(parentnum->increment())){log("Incorrect child block @ "+pow.getBlockNumber().toString()+" parent:"+parentnum->toString());errors=intadd(errors,1);}
                    else if(!pow.getParentID().isEqual(*parenthash)){log("Parent hash incorrect @ "+pow.getBlockNumber().toString());errors=intadd(errors,1);}
                    parenthash=pow.getTxPoWIDData();parentnum=pow.getBlockNumber();}
            }
            if(blocks.empty())break;start=parentnum->increment();
        }
        archjson.put("end",archend.toString());archjson.put("blocks",total);auto startresync=archstart;bool valid=false;
        if(!archstart.isEqual(MiniNumber::ONE)){if(cascstart.isMoreEqual(archstart.sub(MiniNumber::ONE))&&cascstart.isLessEqual(archend))valid=true;startresync=cascstart;}else valid=true;
        json::JSONObject response;response.put("message","Archive integrity check completed");response.put("cascade",cascjson);response.put("archive",archjson);response.put("valid",valid);
        if(!valid)response.put("notvalid","Your cascade and blocks do not line up.. new cascade required.. pls restart Minima");
        response.put("from",json::JSONValue::raw(startresync.toString()));response.put("errors",errors);
        response.put("recommend",errors>0?"There are errors in your Archive DB blocks - you should wipe then resync with a valid host":"Your ArchiveDB is correct and has no errors.");ret.put("response",response);
    }else if(action=="resync"){
        vault::checkAllKeysCreated();auto listener=Main::getMinimaListener();auto fullhost=getParam("host");bool local=false;
        if(fullhost==LOCAL_ARCHIVE){if(H2_TEMPARCHIVE.load()&&!source(STATIC_TEMPARCHIVE))throw CommandException("No Local STATIC Archive DB Found..");local=true;}
        auto connection=connect::createConnectMessage(fullhost);std::optional<std::string> host;int32_t port=0;
        if(!local){if(!connection)throw CommandException("Invalid HOST format for resync : "+fullhost);host=connection->getString("host");port=connection->getInteger("port");}
        auto keys=getNumberParam("keys",MiniNumber(Wallet::NUMBER_GETADDRESS_KEYS.load())).getAsInt();auto uses=getNumberParam("keyuses",MiniNumber(1000)).getAsInt();
        if(!local&&!sendArchiveReq(host,port,MiniNumber::MINUSONE))throw CommandException("Could not connect to Archive host! @ "+host.value_or("null")+":"+std::to_string(port));
        main()->PostNotifyEvent("MDS_RESYNC_START",json::JSONObject());if(GeneralParams::IS_MEGAMMR)sqlRequired(db()->getMegaMMR()).clear();
        auto phrase=getParam("phrase","");
        if(!phrase.empty()){
            auto anyphrase=getBooleanParam("anyphrase",false);auto cleanphrase=anyphrase?phrase:BIP39::cleanSeedPhrase(phrase);
            main()->archiveResetReady(true);log("Resetting all wallet private keys..");auto seed=BIP39::convertStringToSeed(cleanphrase);auto wallet=db()->getWallet();sqlRequired(wallet).updateSeedRow(cleanphrase,seed.to0xString());
            log("Creating a total of "+std::to_string(keys)+" keys / addresses..");for(int32_t i=0;i<keys;++i){NotifyListener(listener,"Creating key "+std::to_string(i));log("Creating key "+std::to_string(i));wallet->createNewSimpleAddress(true);}log("All keys created..");wallet->updateAllKeyUses(uses);
        }else main()->archiveResetReady(false);
        auto startblock=MiniNumber::ZERO,endblock=MiniNumber::ZERO,firstStart=MiniNumber::ZERO;int64_t starttime=0,lastlog=0;bool found=false,first=true;int32_t counter=0;
        log("System clean..");collect();
        while(true){
            sqlRequired(db()->getTxPoWDB()).wipeDBRAM();counter=intadd(counter,1);if(counter%10==0)main()->resetMemFull();std::shared_ptr<IBD> ibd;
            if(!local)ibd=sendArchiveReq(host,port,startblock);
            else if(H2_TEMPARCHIVE.load()){ibd=std::make_shared<IBD>();ibd->createArchiveIBD(startblock,MinimaDB::archiveServices(source(STATIC_TEMPARCHIVE)),true);}
            else ibd=std::make_shared<IBD>(sqlRequired(source(STATIC_RAW)).getNextIBD());
            if(!ibd)ibd=std::make_shared<IBD>();
            if(startblock.isEqual(MiniNumber::ZERO)&&ibd->hasCascade()){
                log("Cascade Received.. "+sqlRequired(sqlRequired(ibd->getCascade()).getTip()).getTxPoW().getBlockNumber().toString());db()->setIBDCascade(ibd->getCascade());sqlRequired(db()->getArchive()).checkCascadeRequired(ibd->getCascade().get());
            }
            auto blocks=ibd->getTxBlocks();auto size=sqlRequired(blocks).size();
            if(size>0){found=true;auto start=blocks->front();if(first){first=false;firstStart=height(start);}auto last=blocks->back();endblock=height(last);startblock=endblock.increment();starttime=last->getTxPoW().getTimeMilli().getAsLong();NotifyListener(listener,"Loading "+height(start).toString()+" @ "+javaDateString(start->getTxPoW().getTimeMilli().getAsLong()));}
            else log("No Archive TxBlocks left..");
            sqlRequired(main()->getTxPoWProcessor()).postProcessArchiveIBD(ibd,"0x00");bool error=false;auto tip=sqlRequired(db()->getTxPoWTree()).getTip();int attempts=0;
            while(found&&!tip){pause(250);tip=sqlRequired(db()->getTxPoWTree()).getTip();if(++attempts>5000){error=true;break;}}
            if(error){log("ERROR : There was an error processing that FIRST IBD");break;}
            (void)now();attempts=0;
            while(found){if(!sqlRequired(tip).getBlockNumber().isEqual(endblock))pause(50);else break;tip=sqlRequired(db()->getTxPoWTree()).getTip();if(++attempts>20000){error=true;break;}}
            if(subtract(now(),lastlog)>5000){log("IBD Processed.. block:"+startblock.toString()+" @ "+MinimaLogger::DATEFORMAT.format(starttime));lastlog=now();}
            if(error){log("ERROR : There was an error processing that IBD - took too long");break;}if(size==0)break;
        }
        NotifyListener(listener,"All blocks loaded.. pls wait");log("All Archive data received and processed.. shutting down..");json::JSONObject response;
        response.put("message","Archive sync completed.. shutting down now.. please restart after");response.put("start",firstStart.toString());response.put("end",endblock.toString());ret.put("response",response);finishSync();
    }else if(action=="export"){
        auto file=getParam("file","archivebackup-"+std::to_string(now())+".gzip");auto output=MiniFile::createBaseFile(file);if(fileExists(output))removeFile(output);
        log("Exporting ArchiveDB to H2 GZIPPED SQL..");sqlRequired(db()->getArchive()).backupToFile(output,true);auto length=javaUnixFileLength(output);
        json::JSONObject response;response.put("message","Archive DB GZIPPED");response.put("rows",sqlRequired(db()->getArchive()).getSize());response.put("file",javaUnixAbsoluteFile(output));response.put("size",MiniFormat::formatSize(length));ret.put("response",response);
    }else if(action=="exportraw"){
        if(sqlRequired(arch).getSize()==0)throw CommandException("No blocks in ArchiveDB");auto logs=getBooleanParam("logs",false);auto file=getParam("file","archivebackup-"+std::to_string(now())+".raw.dat");auto output=MiniFile::createBaseFile(file);if(fileExists(output))removeFile(output);
        auto parent=std::filesystem::path(javaUnixAbsoluteFile(output)).parent_path();if(!fileExists(parent))javaUnixMkdirs(parent);
        BufferedFileOutput fileout(output,65536);GzipOutput gzip([&](auto bytes){fileout.write(bytes);},65536);
        auto last=height(arch->loadLastBlock()).getAsInt(),first=height(arch->loadFirstBlock()).getAsInt();log("Exporting ArchiveDB to RAW block format..");bool allow=true;
        if(last==1){allow=false;log("Archive starts from 1 no need to use a cascade..");}auto cascade=arch->loadCascade();
        if(cascade&&allow){log("Cascade found in Archive DB..");gzip.write(MiniByte::TRUE.serialise());gzip.write(cascade->serialise());last=intadd(sqlRequired(cascade->getTip()).getTxPoW().getBlockNumber().getAsInt(),1);log("Save blocks from cascade tip onwards.. "+std::to_string(intadd(last,-1)));}
        else{gzip.write(MiniByte::FALSE.serialise());log("No cascade added");}
        auto total=intadd(intadd(first,std::bit_cast<int32_t>(uint32_t(0)-uint32_t(last))),1);log("Add records from  : "+std::to_string(last));log("Total records to add : "+std::to_string(total));
        if(existsParam("maxexport")){auto maximum=getNumberParam("maxexport").getAsInt();log("Max export specified.. : "+std::to_string(maximum));if(total>maximum)total=maximum;}
        gzip.write(MiniNumber(total).serialise());int32_t outcount=0,counter=0;int64_t firstblock=-1,endblock=-1,startload=intadd(last,-1);std::shared_ptr<TxBlock> lastblock;
        while(true){if(counter%20==0&&logs)log("Loading from Archive @ "+std::to_string(startload));auto blocks=arch->loadBlockRange(MiniNumber(startload),MiniNumber(startload).add(MiniNumber::HUNDRED),false);if(blocks.empty())break;
            for(const auto& block:blocks){gzip.write(sqlRequired(block).serialise());if(!lastblock)firstblock=height(block).getAsLong();lastblock=block;endblock=height(block).getAsLong();outcount=intadd(outcount,1);if(outcount>=total)break;}
            if(outcount>=total){log("Finished loading blocks..");break;}startload=endblock;counter=intadd(counter,1);if(counter%20==0)collect();
        }
        fileout.flush();try{
            std::exception_ptr failure;try{gzip.finish();}catch(...){failure=std::current_exception();}
            try{fileout.close();}catch(...){if(!failure)failure=std::current_exception();}
            if(failure)std::rethrow_exception(failure);gzip.finish();fileout.close();
        }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
        json::JSONObject response;response.put("start",firstblock);response.put("end",endblock);response.put("total",outcount);response.put("file",output.filename().string());response.put("path",javaUnixAbsoluteFile(output));response.put("size",MiniFormat::formatSize(javaUnixFileLength(output)));ret.put("response",response);
    }else if(action=="importold"||action=="import"){
        auto file=getParam("file");auto input=MiniFile::createBaseFile(file);if(!fileExists(input))throw JavaTypedException("java.lang.Exception","Restore file doesn't exist : "+javaUnixAbsoluteFile(input),true);
        bool h2=action=="importold"||!file.ends_with(".dat");std::shared_ptr<ArchiveManager> temp;std::filesystem::path folder;
        if(h2){if(action=="import")log("H2 archive imprt started..");temp=db()->newArchive();if(action=="import")H2_TEMPARCHIVE=true;folder=javaUnixChildFile(GeneralParams::DATA_FOLDER,"archiverestore");javaUnixMkdirs(folder);auto path=folder/"archivetemp";if(fileExists(path))removeFile(path);temp->loadDB(path);log("Restoring ArchiveDB from file..");temp->restoreFromFile(input,true);logArchive(temp);replaceSource(STATIC_TEMPARCHIVE,temp);}
        else{log("RAW archive import started..");H2_TEMPARCHIVE=false;replaceSource(STATIC_RAW,std::make_shared<RawArchiveInput>(input));sqlRequired(source(STATIC_RAW)).connect();}
        auto command="archive action:resync host:"+LOCAL_ARCHIVE;
        if(existsParam("phrase"))command+=" phrase:\""+getParam("phrase")+"\"";
        for(const auto* key:{"keys","keyuses","anyphrase"})if(existsParam(key))command+=" "+std::string(key)+":"+getParam(key);
        auto result=command_result::first(command);
        if(h2){temp->saveDB(false);MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);replaceSource(STATIC_TEMPARCHIVE,std::shared_ptr<ArchiveManager>());}
        else{sqlRequired(source(STATIC_RAW)).stop();replaceSource(STATIC_RAW,std::shared_ptr<RawArchiveInput>());}
        json::JSONObject response;response.put("archiveresync",result);ret.put("response",response);
    }else if(action=="importraw"){
        auto file=getParam("file");auto input=MiniFile::createBaseFile(file);if(!fileExists(input))throw JavaTypedException("java.lang.Exception","Restore file doesn't exist : "+javaUnixAbsoluteFile(input),true);main()->archiveResetReady(false);
        RawArchiveInput raw(input);raw.connect();int32_t count=0;
        while(true){auto ibd=std::make_shared<IBD>(raw.getNextIBD());auto blocks=ibd->getTxBlocks();auto size=sqlRequired(blocks).size();if(size==0)break;
            auto processor=main()->getTxPoWProcessor();sqlRequired(processor).postProcessArchiveIBD(ibd,"0x00");count=intadd(count,1);
            if(count%20==0){auto block=blocks->back();auto tip=sqlRequired(db()->getTxPoWTree()).getTip();while(!tip){pause(50);tip=sqlRequired(db()->getTxPoWTree()).getTip();}
                auto starttime=sqlRequired(block).getTxPoW().getTimeMilli().getAsLong();log("Processing block:"+height(block).toString()+" @ "+MinimaLogger::DATEFORMAT.format(starttime));
                while(!sqlRequired(tip).getBlockNumber().isEqual(height(block))){pause(50);tip=sqlRequired(db()->getTxPoWTree()).getTip();}main()->resetMemFull();}
        }
        raw.stop();log("All blocks processed..");json::JSONObject response;response.put("message","Archive sync completed.. shutting down now.. please restart after");ret.put("response",response);finishSync();
    }else if(action=="inspectraw"){
        auto timestart=now();auto file=getParam("file");RawArchiveInput raw(MiniFile::createBaseFile(file));raw.connect();if(raw.getCascade())log("Cascade found.. ");int32_t counter=0;auto last=MiniNumber::ZERO;
        while(true){auto ibd=raw.getNextIBD();auto blocks=ibd.getTxBlocks();if(counter%10==0&&!sqlRequired(blocks).empty())MinimaLogger::log("Loading from RAW Block : "+height(blocks->front()).toString(),false);if(blocks->empty())break;bool exit=false;
            for(const auto& block:*blocks){auto number=height(block);if(!number.isEqual(last.increment())){log("INVALID non sequential blocks.. lastblock:"+last.toString()+" new:"+number.toString());exit=true;break;}last=number;}
            counter=intadd(counter,1);if(counter%10==0)collect();if(exit)break;
        }
        raw.stop();json::JSONObject response;response.put("time",MiniFormat::ConvertMilliToTime(subtract(now(),timestart)));ret.put("response",response);
    }else if(action=="inspect"){
        auto file=getParam("file","");if(file.ends_with(".dat"))throw CommandException("inspect only works for H2 archive files");auto folder=javaUnixChildFile(GeneralParams::DATA_FOLDER,"archiverestore");MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);std::shared_ptr<ArchiveManager> temp;
        if(!file.empty()){auto input=MiniFile::createBaseFile(file);if(!fileExists(input))throw JavaTypedException("java.lang.Exception","Restore file doesn't exist : "+javaUnixAbsoluteFile(input),true);temp=db()->newArchive();javaUnixMkdirs(folder);auto path=folder/"archivetemp";if(fileExists(path))removeFile(path);temp->loadDB(path);log("Restoring ArchiveDB from file..");temp->restoreFromFile(input,true);}else temp=db()->getArchive();
        json::JSONObject response,cascjson,archjson;cascjson.put("exists",false);cascjson.put("start","-1");cascjson.put("length","-1");archjson.put("first","-1");archjson.put("last","-1");archjson.put("size","-1");
        auto cascade=sqlRequired(temp).loadCascade();if(cascade){cascjson.put("exists",true);cascjson.put("start",sqlRequired(cascade->getTip()).getTxPoW().getBlockNumber().toString());cascjson.put("length",cascade->getLength());}
        auto first=temp->loadFirstBlock();if(first)archjson.put("first",height(first).toString());auto last=temp->loadLastBlock();if(last)archjson.put("last",height(last).toString());archjson.put("size",temp->getSize());
        if(!file.empty())temp->saveDB(false);MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);response.put("cascade",cascjson);response.put("archive",archjson);ret.put("response",response);
    }else if(action=="addresscheck"){
        auto address=getAddressParam("address");auto state=getParam("statecheck","");if(javaLowerCase(state).starts_with("mx")&&state.find('@')==std::string::npos)state=Address::convertMinimaAddress(state).to0xString();
        json::JSONObject response;json::JSONArray inputs,outputs;auto archive=db()->getArchive();auto start=sqlRequired(archive).loadLastBlock();auto first=MiniNumber::ZERO;bool canstart=true;
        if(!start)canstart=false;else{first=height(start).decrement();log("Start archive @ "+first.toString());}
        auto scan=[&](const TxBlock& block){const auto& pow=block.getTxPoW();auto number=pow.getBlockNumber().getAsLong();auto date=MinimaLogger::DATEFORMAT.format(pow.getTimeMilli().getAsLong());
            for(const auto& coin:block.getOutputCoins())if(coin.getAddress().to0xString()==address&&(state.empty()||coin.checkForStateVariable(state))){log("BLOCK "+std::to_string(number)+" CREATED COIN : "+coin.toString());json::JSONObject item;item.put("block",number);item.put("date",date);item.put("datemilli",pow.getTimeMilli().toString());item.put("coin",coin.toJSON());outputs.add(item);}
            for(const auto& proof:block.getInputCoinProofs()){const auto& coin=proof.getCoin();if(coin.getAddress().to0xString()==address&&(state.empty()||coin.checkForStateVariable(state))){log("BLOCK "+std::to_string(number)+" SPENT COIN : "+coin.toString());json::JSONObject item;item.put("block",number);item.put("date",date);item.put("datemilli",pow.getTimeMilli().toString());item.put("coin",coin.toJSON());inputs.add(item);}}
        };
        while(canstart){auto blocks=archive->loadBlockRange(first,first.add(MiniNumber::HUNDRED),false);if(blocks.empty())break;for(const auto& block:blocks){first=height(block);scan(sqlRequired(block));}}
        if(start)log("End archive @ "+first.toString());log("Checking BlockChain.. descending");if(db()->getTxPoWTree()){auto top=db()->getTxPoWTree()->getTip();while(top){scan(top->getTxBlock());top=top->getParent();}}
        log("All checks complete..");response.put("created",outputs);response.put("spent",inputs);ret.put("coins",response);
    }else throw CommandException("Invalid action : "+action);
    return ret;
}
void archive::NotifyListener(const std::shared_ptr<MessageListener>& listener,const std::optional<std::string>& text){
    if(!listener)return;json::JSONObject data,notify;data.put("message",text?json::JSONValue(*text):json::JSONValue());notify.put("event","ARCHIVEUPDATE");notify.put("data",data);auto message=std::make_shared<Message>(NotifyManager::NOTIFY_POST);message->addObject("notify",MessageObject(std::make_shared<json::JSONObject>(notify)));listener->processMessage(message);
}
std::shared_ptr<IBD> archive::sendArchiveReq(const std::optional<std::string>& host,int32_t port,const MiniNumber* start,int32_t maximum){
    std::shared_ptr<IBD> ibd;int32_t attempts=0;
    while(attempts<maximum){try{
        auto message=NIOManager::createNIOMessage(NIOMessage::MSG_ARCHIVE_REQ,std::function<wire::Bytes()>([&]{if(!start)throw JavaNullPointerException("null archive start block");return start->serialise();}));
        auto socket=nioConnect(InetSocketAddress(host,port),10000);nioWriteAll(socket,message.serialise());sqlRequired(sqlRequired(main()->getNIOManager()).getTrafficListener()).addWriteBytes("sendArchiveReq",int64_t(message.size()));
        auto response=MiniData::readFromStream([&](auto bytes)->size_t{auto count=nioReadSome(socket,bytes,10000);return count<0?0:size_t(count);});sqlRequired(sqlRequired(main()->getNIOManager()).getTrafficListener()).addReadBytes("sendArchiveReq",int64_t(response.size()));socket->close();size_t offset=0;(void)MiniByte::deserialise(response.bytes(),offset);ibd=std::make_shared<IBD>(IBD::deserialise(response.bytes(),offset));break;
    }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){
        log("Archive connection : "+nativeExceptionText(error)+" @ "+host.value_or("null")+":"+std::to_string(port));ibd.reset();attempts=intadd(attempts,1);
        if(attempts<maximum){log(std::to_string(attempts)+" Attempts > Wait 10 seconds and re-attempt..");try{pause(10000);}catch(const JavaTypedException& e){if(e.className()!="java.lang.InterruptedException")throw;}log("Re-attempt started..");}
    }}return ibd;
}
}
