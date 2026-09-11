#include "mysql.hpp"
#include "vault.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/database/archive/RawArchiveInput.hpp"
#include "org/minima/utils/BIP39.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/MySQLConfiguration.hpp"
#include "support/CommandResult.hpp"
#include "support/BufferedFileOutput.hpp"
#include "support/GzipOutput.hpp"
#include "support/JavaLowerCase.hpp"
#include "support/JavaUnixFile.hpp"
#include <bit>
#include <iostream>
#include "support/NativeThread.hpp"
namespace minima::commands {
namespace {
void log(const std::string& text){MinimaLogger::log(text);}
void collect(){if(auto callback=MinimaLogger::getRuntime().collectMemory)callback();}
int64_t now(){return MinimaLogger::getRuntime().currentTimeMillis();}
int64_t add(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)+uint64_t(b));}
int64_t subtract(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)-uint64_t(b));}
int32_t increment(int32_t n){return std::bit_cast<int32_t>(uint32_t(n)+1);}
void pause(int64_t delay){if(delay<0)throw JavaIllegalArgumentException("timeout value is negative");nativeThreadSleep(delay);}
std::shared_ptr<MinimaDB> database(){auto owner=MinimaDB::getDB();sqlRequired(owner);return owner;}
std::shared_ptr<Main> runtime(){auto owner=Main::getInstance();sqlRequired(owner);return owner;}
const MiniNumber& height(const std::shared_ptr<TxBlock>& block){return sqlRequired(block).getTxPoW().getBlockNumber();}
void removeExisting(const std::filesystem::path& file){std::error_code error;if(file.string().find('\0')==std::string::npos&&std::filesystem::exists(file,error))std::filesystem::remove(file,error);}
void checkParent(const std::shared_ptr<TxBlock>& block,const std::shared_ptr<TxBlock>& previous){
    if(!previous)return;
    if(!sqlRequired(block).getTxPoW().getParentID().isEqual(previous->getTxPoW().getTxPoWIDData()))throw CommandException("ERROR : block parents are incorrect @ "+height(block).toString());
    if(!height(block).isEqual(height(previous).increment()))throw CommandException("ERROR : block numbers are incorrect @ "+height(block).toString());
}
json::JSONObject mysqlInfo(int64_t first,int64_t last){json::JSONObject result;result.put("mysqlstart",last);result.put("mysqlend",first);result.put("mysqltotal",subtract(first,last));return result;}
json::JSONObject archiveInfo(const std::shared_ptr<TxBlock>& first,const std::shared_ptr<TxBlock>& last){
    auto total=height(first).sub(height(last)).getAsLong();json::JSONObject result;result.put("archivestart",height(last).getAsLong());result.put("archiveend",height(first).getAsLong());result.put("archivetotal",total);return result;
}
json::JSONObject exportInfo(int64_t first,int64_t end,const std::filesystem::path& output){json::JSONObject result;result.put("start",first);result.put("end",end);result.put("file",output.filename().string());result.put("path",javaUnixAbsoluteFile(output));result.put("size",MiniFormat::formatSize(javaUnixFileLength(output)));return result;}
std::string exceptionText(const std::exception& error){if(auto command=dynamic_cast<const CommandException*>(&error))return std::string("org.minima.system.commands.CommandException")+(command->getMessage()?": "+*command->getMessage():"");return nativeExceptionText(error);}
}
mysql::mysql():Command("mysql","Store and resync the archive data in a MySQL database"){}
std::optional<std::string> mysql::getFullHelp() const{return
"\nmysql\n"
"\n"
"Export archive data to a MySQL server.\n"
"\n"
"The MySQL db can be used to perform a chain re-sync to put users on the correct chain,\n"
"\n"
"or a seed re-sync to restore access to lost funds, using the seed phrase.\n"
"\n"
"Can query an address for its history of spent and unspent coins.\n"
"\n"
"Additionally export the MySQL db to a gzip file for resyncing with 'reset' or 'archive' command.\n"
"\n"
"You can use setlogin to auto set the login details"
"\n"
"host:\n"
"    The ip:port (or name of Docker container) running the MySQL db.\n"
"\n"
"database:\n"
"    name of the MySQL db being used to store the archive db data.\n"
"\n"
"user:\n"
"    MySQL user to login to as.\n"
"\n"
"password:\n"
"    MySQL password for the user provided.\n"
"\n"
"readonly:\n"
"    true or false, Connect in readonly mode.\n"
"logs:\n"
"    Show detailed logs - default true.\n"
"\n"
"action:\n"
"    info : Show the blocks stored in the archive db and compare to the MySQL db.\n"
"    integrity : Check the block order and block parents are correct in the MySQL db.\n"
"    update : Update the MySQL db with the latest syncblocks from the node's archive db.\n"
"    addresscheck : Check the history of all the spent and unspent coins from an address.\n"
"    setlogin : Set the MySQL login details so you don't need to type them in every time.\n"
"    clearlogin : Clear MySQL login details.\n"
"    autobackup : Automatically save archive data to MySQL DB. Use with enable. Also stores all TxPoW the node sees.\n"
"    findtxpow : Search for an individual TxPoW (only works if autobackup is enabled).\n"
"    resync : Perform a chain or seed re-sync from the specified MySQL db.\n"
"             Will shutdown the node so you must restart it once complete.\n"
"    wipe :  Be careful. Wipe the MySQL db.\n"
"    h2export : export the MySQL db to an archive gzip file which can be used to resync a node.\n"
"    h2import : import an archive gzip file to the MySQL db.\n"
"    rawexport : export the MySQL db to raw.dat file which can be used to resync a node (faster than H2).\n"
"    rawimport : import a raw.dat to the MySQL db.\n"
"\n"
"phrase: (optional)\n"
"     Use with action:resync. The 24 word seed phrase of the node to re-sync.\n"
"     If provided, the node will be wiped and re-synced.\n"
"     If not provided, the node will be re-synced to the chain and will not be wiped.\n"
"\n"
"keys: (optional)\n"
"    If the seed phrase is provided, optionally set the number of keys to create.\n"
"    Default is 80.\n"
"\n"
"keyuses: (optional)\n"
"    If the seed phrase is provided, optionally set the number of previous uses for each key created.\n"
"    Default is 1000.\n"
"\n"
"address: (optional)\n"
"    Use with action:addresscheck. The address to check the history of spent and unspent coins for.\n"
"\n"
"enable: (optional)\n"
"    Use with action:autobackup. Automatically save data to MySQL archive DB.\n"
"\n"
"file: (optional)\n"
"    Name or path of the archive gzip file to export to or import from.\n"
"\n"
"Examples:\n"
"\n"
"mysql ..LOGIN_DETAILS.. action:setlogin\n"
"\n"
"mysql host:mysqlhost:port database:archivedb user:archiveuser password:archivepassword action:info\n"
"\n"
"mysql host:dockermysql database:archivedb user:archiveuser password:archivepassword action:info\n"
"\n"
"mysql (If you have not setlogin)..LOGIN_DETAILS.. action:integrity\n"
"\n"
"mysql action:update\n"
"\n"
"mysql action:addresscheck address:MxG08.. \n"
"\n"
"mysql action:resync\n"
"\n"
"mysql action:findtxpow txpowid:0x00FFEEDD..\n"
"\n"
"mysql action:resync phrase:\"24 WORDS HERE\" keys:90 keyuses:2000\n"
"\n"
"mysql action:rawexport file:archivexport-DDMMYY.gzip\n";}
std::vector<std::string> mysql::getValidParams() const{return {"action","host","database","user","password","keys","keyuses","phrase","address","txpowid","enable","file","statecheck","logs","maxexport","readonly","startfix","endfix","block"};}
json::JSONValue mysql::runCommand(){
    auto ret=getJSONReply();
    if(GeneralParams::IS_MOBILE)throw CommandException("Sorry - MySQL does not work on Android..");
    auto action=getParam("action","info");auto prefs=database()->getUserDB();
    std::string host,dbname,user,password;
    bool fresh=existsParam("host")&&existsParam("database");
    if(sqlRequired(database()->getUserDB()).getAutoLoginDetailsMySQL()&&!fresh){host=sqlRequired(prefs).getAutoMySQLHost();dbname=prefs->getAutoMySQLDB();user=prefs->getAutoMySQLUser();password=prefs->getAutoMySQLPassword();}
    else{host=getParam("host");dbname=getParam("database");user=getParam("user");password=getParam("password");}
    bool logs=getBooleanParam("logs",true);bool readonly=getBooleanParam("readonly",false);
    MySQLConnect connection(host,dbname,user,password,readonly);connection.init();auto arch=database()->getArchive();
    auto credentials=[&](bool enable){sqlRequired(prefs).setAutoMySQLHost(enable?host:"");prefs->setAutoMySQLDB(enable?dbname:"");prefs->setAutoMySQLUser(enable?user:"");prefs->setAutoMySQLPassword(enable?password:"");};
    auto describe=[&](json::JSONObject& response){response.put("user",user);response.put("password","***");response.put("host",host);response.put("database",dbname);};
    if(action=="info"){
        auto first=connection.loadFirstBlock();auto last=connection.loadLastBlock();auto archlast=sqlRequired(arch).loadLastBlock();auto archfirst=arch->loadFirstBlock();
        json::JSONObject archive;archive.put("archivestart",-1);archive.put("archiveend",-1);archive.put("archivetotal",0);
        // Pinned source deliberately uses OR here; a single null edge still throws.
        if(archlast||archfirst)archive=archiveInfo(archfirst,archlast);
        json::JSONObject response;response.put("archive",archive);response.put("mysql",mysqlInfo(first,last));response.put("autobackup",sqlRequired(database()->getUserDB()).getAutoBackupMySQL());response.put("logindetails",sqlRequired(database()->getUserDB()).getAutoLoginDetailsMySQL());describe(response);response.put("storealltxpow",GeneralParams::MYSQL_STORE_ALLTXPOW);ret.put("response",response);
    }else if(action=="wipe"){
        connection.wipeAll();ret.put("response","MySQL DB Wiped..");
    }else if(action=="setlogin"){
        credentials(true);prefs->setAutoLoginDetailsMySQL(true);json::JSONObject response;response.put("logindetails",prefs->getAutoLoginDetailsMySQL());describe(response);ret.put("response",response);
    }else if(action=="clearlogin"){
        credentials(false);prefs->setAutoLoginDetailsMySQL(false);json::JSONObject response;response.put("logindetails",prefs->getAutoLoginDetailsMySQL());ret.put("response",response);
    }else if(action=="autobackup"){
        auto enable=getBooleanParam("enable");sqlRequired(prefs).setAutoBackupMySQL(enable);credentials(enable);json::JSONObject response;response.put("autobackup",sqlRequired(database()->getUserDB()).getAutoBackupMySQL());ret.put("response",response);
    }else if(action=="integrity"){
        auto start=connection.loadLastBlock();(void)connection.loadFirstBlock();int64_t first=-1,end=-1;std::shared_ptr<TxBlock> previous;
        while(true){if(logs)MinimaLogger::log("MySQL Verifying from : "+std::to_string(start),false);auto blocks=connection.loadBlockRange(MiniNumber(start));if(blocks.empty())break;
            for(const auto& block:blocks){if(!previous)first=height(block).getAsLong();else checkParent(block,previous);previous=block;end=height(block).getAsLong();}start=add(end,1);
        }json::JSONObject response;response.put("start",first);response.put("end",end);ret.put("response",response);
    }else if(action=="update"){
        if(sqlRequired(arch).getSize()==0)throw CommandException("No blocks in ArchiveDB");
        auto last=height(arch->loadLastBlock()).getAsLong();auto first=height(arch->loadFirstBlock()).getAsLong();auto mysqlfirst=connection.loadFirstBlock();
        if(mysqlfirst>add(first,-1))throw CommandException("Archive nodes to low.. cannot sync arch:"+std::to_string(first)+" mysql:"+std::to_string(mysqlfirst));
        auto start=mysqlfirst;if(mysqlfirst==-1){start=0;
            // Java's (last != 0 || last != 1) is always true, including 0 and 1.
            log("Archive NOT starting from 0!.. Load from start of archive @ "+std::to_string(last));start=last;
        }
        std::shared_ptr<TxBlock> previous;
        while(true){auto end=add(start,100);if(logs)log("MySQL block transfer from "+std::to_string(start)+" to "+std::to_string(add(end,-1)));auto blocks=arch->loadBlockRange(MiniNumber(start),MiniNumber(end),false);if(blocks.empty()){log("All blocks added");break;}
            for(const auto& block:blocks){checkParent(block,previous);previous=block;auto number=height(block).getAsLong();connection.saveBlock(block.get());if(number>start)start=number;}
        }
        auto mysqlend=connection.loadFirstBlock();auto mysqlstart=connection.loadLastBlock();auto archlast=arch->loadLastBlock();auto archfirst=arch->loadFirstBlock();auto archive=archiveInfo(archfirst,archlast);json::JSONObject response;response.put("archive",archive);response.put("mysql",mysqlInfo(mysqlend,mysqlstart));ret.put("response",response);
    }else if(action=="resync"){
        vault::checkAllKeysCreated();runtime()->PostNotifyEvent("MDS_RESYNC_START",json::JSONObject());
        auto keys=getNumberParam("keys",MiniNumber(Wallet::NUMBER_GETADDRESS_KEYS.load())).getAsInt();auto keyuses=getNumberParam("keyuses",MiniNumber(1000)).getAsInt();auto phrase=getParam("phrase","");auto clean=BIP39::cleanSeedPhrase(phrase);
        if(!phrase.empty()){
            runtime()->archiveResetReady(true);log("Resetting all wallet private keys..");auto seed=BIP39::convertStringToSeed(clean);auto wallet=database()->getWallet();sqlRequired(wallet).updateSeedRow(clean,seed.to0xString());
            log("Creating a total of "+std::to_string(keys)+" keys / addresses..");for(int32_t i=0;i<keys;++i){log("Creating key "+std::to_string(i));wallet->createNewSimpleAddress(true);}log("All keys created..");wallet->updateAllKeyUses(keyuses);
        }else runtime()->archiveResetReady(false);
        if(GeneralParams::IS_MEGAMMR)sqlRequired(database()->getMegaMMR()).clear();
        auto start=MiniNumber::ZERO,end=MiniNumber::ZERO,first=MiniNumber::ZERO;bool found=false,firstrun=true;int32_t counter=0;log("System clean..");collect();
        while(true){
            sqlRequired(database()->getTxPoWDB()).wipeDBRAM();if(counter%20==0)runtime()->resetMemFull();bool showlogs=counter%10==0;counter=increment(counter);
            auto blocks=std::make_shared<std::vector<std::shared_ptr<TxBlock>>>(connection.loadBlockRange(start));auto ibd=std::make_shared<IBD>();ibd->setTxBlocks(blocks);
            // The source's cascade-handling block is commented out.
            if(!blocks->empty()){found=true;if(firstrun){firstrun=false;first=height(blocks->front());}end=height(blocks->back());start=end.increment();if(showlogs)log("Archive IBD received start : "+height(blocks->front()).toString()+" end : "+end.toString());}else log("No Archive TxBlocks left..");
            sqlRequired(runtime()->getTxPoWProcessor()).postProcessArchiveIBD(ibd,"0x00");bool error=false;auto tip=sqlRequired(database()->getTxPoWTree()).getTip();int attempts=0;
            while(found&&!tip){pause(250);tip=sqlRequired(database()->getTxPoWTree()).getTip();if(++attempts>128){error=true;break;}}
            if(error){log("ERROR : There was an error processing that IBD");break;}
            if(showlogs)log("Waiting for chain to catch up.. please wait");attempts=0;
            while(found){if(!sqlRequired(tip).getBlockNumber().isEqual(end))pause(50);else break;tip=sqlRequired(database()->getTxPoWTree()).getTip();if(++attempts>10000){error=true;break;}}
            if(error){log("ERROR : There was an error processing that IBD");break;}if(blocks->empty())break;
        }
        log("All Archive data received and processed.. shutting down..");json::JSONObject response;response.put("message","Archive sync completed.. shutting down now.. please restart after");response.put("start",first.toString());response.put("end",end.toString());if(!phrase.empty()){response.put("phrase",phrase);response.put("clean",clean);}ret.put("response",response);
        runtime()->shutdownFinalProcs();database()->saveAllDB();runtime()->setHasShutDown();runtime()->stopMessageProcessor();runtime()->NotifyMainListenerOfShutDown();
    }else if(action=="addresscheck"){
        auto address=getAddressParam("address");auto state=getParam("statecheck","");if(javaLowerCase(state).starts_with("mx")&&state.find('@')==std::string::npos)state=Address::convertMinimaAddress(state).to0xString();
        json::JSONArray created,spent;auto start=MiniNumber::ZERO;
        while(true){auto blocks=connection.loadBlockRange(start);if(blocks.empty())break;
            for(const auto& block:blocks){const auto& pow=sqlRequired(block).getTxPoW();auto id=pow.getTxPoWID();auto number=pow.getBlockNumber().getAsLong();auto date=MinimaLogger::DATEFORMAT.format(pow.getTimeMilli().getAsLong());
                auto scan=[&](const Coin& coin,json::JSONArray& list,const char* kind){if(coin.getAddress().to0xString()!=address||(!state.empty()&&!coin.checkForStateVariable(state)))return;log("BLOCK "+std::to_string(number)+" "+kind+" COIN : "+coin.toString());json::JSONObject item;item.put("block",number);item.put("blockid",id);item.put("date",date);item.put("coin",coin.toJSON());list.add(item);};
                for(const auto& coin:block->getOutputCoins())scan(coin,created,"CREATED");for(const auto& proof:block->getInputCoinProofs())scan(proof.getCoin(),spent,"SPENT");start=pow.getBlockNumber().increment();
            }
        }json::JSONObject response;response.put("created",created);response.put("spent",spent);ret.put("coins",response);
    }else if(action=="h2import"){
        auto began=now();auto input=MiniFile::createBaseFile(getParam("file"));auto temp=database()->newArchive();auto folder=javaUnixChildFile(GeneralParams::DATA_FOLDER,"archiverestore");javaUnixMkdirs(folder);auto path=folder/"archivetemp";removeExisting(path);temp->loadDB(path);
        log("Restoring H2 Archive DB..");temp->restoreFromFile(input,true);connection.wipeAll();auto cascade=temp->loadCascade();if(cascade){log("Cascade found.. ");connection.saveCascade(cascade.get());}
        auto least=height(temp->loadLastBlock()).getAsLong();auto greatest=height(temp->loadFirstBlock()).getAsLong();log("First block:"+std::to_string(least));log("Last block:"+std::to_string(greatest));int64_t end=-1,start=add(least,-1);int32_t counter=0;
        while(true){if(logs)log("Loading from H2 @ "+std::to_string(start));auto endload=add(start,250);auto blocks=temp->loadBlockRange(MiniNumber(start),MiniNumber(endload),false);if(blocks.empty())break;
            for(const auto& block:blocks){connection.saveBlock(block.get());start=height(block).getAsLong();}end=start;counter=increment(counter);if(counter%10==0)collect();
        }temp->saveDB(false);MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);json::JSONObject response;response.put("start",add(least,-1));response.put("end",end);response.put("time",MiniFormat::ConvertMilliToTime(subtract(now(),began)));ret.put("response",response);
    }else if(action=="h2export"){
        auto output=MiniFile::createBaseFile(getParam("file","archivebackup-"+std::to_string(now())+".h2.gzip"));removeExisting(output);auto temp=database()->newArchive();auto folder=javaUnixChildFile(GeneralParams::DATA_FOLDER,"archiverestore");javaUnixMkdirs(folder);auto path=folder/"archivetemp";removeExisting(path);temp->loadDB(path);
        auto start=connection.loadLastBlock();(void)connection.loadFirstBlock();int64_t first=-1,end=-1;std::shared_ptr<TxBlock> previous;int32_t counter=0;
        while(true){if(logs)log("Loading from MySQL @ "+std::to_string(start));auto blocks=connection.loadBlockRange(MiniNumber(start));if(blocks.empty())break;
            for(const auto& block:blocks){temp->saveBlock(block.get());if(!previous)first=height(block).getAsLong();previous=block;end=height(block).getAsLong();}start=add(end,1);counter=increment(counter);if(counter%20==0)collect();
        }auto cascade=connection.loadCascade();if(cascade)temp->checkCascadeRequired(cascade.get());else log("No cascade found in MySQL..");log("Exporting to H2 SQL file..");temp->backupToFile(output,true);temp->saveDB(false);MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);ret.put("response",exportInfo(first,end,output));
    }else if(action=="size"){
        json::JSONObject response;response.put("size",connection.getCount());ret.put("response",response);
    }else if(action=="rawexport"){
        auto output=MiniFile::createBaseFile(getParam("file","archivebackup-"+std::to_string(now())+".raw.dat"));removeExisting(output);BufferedFileOutput fileout(output,65536);GzipOutput gzip([&](auto bytes){fileout.write(bytes);},65536);
        auto cascade=connection.loadCascade();if(cascade){log("Cascade found in MySQL..");gzip.write(MiniByte::TRUE.serialise());gzip.write(cascade->serialise());}else{gzip.write(MiniByte::FALSE.serialise());log("No cascade found in MySQL..");}
        int32_t total=connection.getCount();log("Total records found : "+std::to_string(total));if(existsParam("maxexport")){auto maximum=getNumberParam("maxexport").getAsInt();log("Max export specified.. : "+std::to_string(maximum));if(total>maximum)total=maximum;}gzip.write(MiniNumber(total).serialise());int32_t outcount=0;
        auto start=connection.loadLastBlock();(void)connection.loadFirstBlock();int64_t first=-1,end=-1;std::shared_ptr<TxBlock> previous;int32_t counter=0;
        while(true){if(counter%20==0&&logs)log("Loading from MySQL @ "+std::to_string(start));auto blocks=connection.loadBlockRange(MiniNumber(start));if(blocks.empty())break;
            for(const auto& block:blocks){gzip.write(sqlRequired(block).serialise());if(!previous)first=height(block).getAsLong();previous=block;end=height(block).getAsLong();outcount=increment(outcount);if(outcount>=total)break;}
            if(outcount>=total){log("Finished loading blocks..");break;}start=add(end,1);counter=increment(counter);if(counter%20==0)collect();
        }
        fileout.flush();try{
            std::exception_ptr failure;try{gzip.finish();}catch(...){failure=std::current_exception();}try{fileout.close();}catch(...){if(!failure)failure=std::current_exception();}if(failure)std::rethrow_exception(failure);gzip.finish();fileout.close();
        }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
        ret.put("response",exportInfo(first,end,output));
    }else if(action=="reset"){
        json::JSONObject response;auto input=getParam("file");auto imported=CommandRunner::getRunner().runSingleCommand("mysql action:rawimport file:"+input);if(!command_result::status(imported))throw CommandException("Error importing data.. "+(imported.get("error").isString()?imported.get("error").asString():imported.get("error").toJSONString()));response.put("import",imported);
        auto synced=CommandRunner::getRunner().runSingleCommand("mysql action:resync");if(!command_result::status(synced))throw CommandException("Error resyncing.. "+(synced.get("error").isString()?synced.get("error").asString():synced.get("error").toJSONString()));response.put("resync",synced);ret.put("response",response);
    }else if(action=="rawimport"){
        runtime()->MYSQL_IMPORTING_NO_ACTION=true;
        try{
            auto began=now();auto input=MiniFile::createBaseFile(getParam("file"));RawArchiveInput raw(input);raw.connect();connection.wipeAll();auto cascade=raw.getCascade();if(cascade){log("Cascade found.. ");connection.saveCascade(cascade.get());}int32_t counter=0;
            while(true){auto ibd=raw.getNextIBD();auto blocks=ibd.getTxBlocks();if(logs&&counter%10==0&&!sqlRequired(blocks).empty())log("Loading from RAW Block : "+height(blocks->front()).toString());if(blocks->empty())break;for(const auto& block:*blocks)connection.saveBlock(block.get());counter=increment(counter);if(counter%10==0)collect();}
            raw.stop();json::JSONObject response;response.put("time",MiniFormat::ConvertMilliToTime(subtract(now(),began)));ret.put("response",response);
        }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){
            // CommandException is a Java checked exception; keep its class in the wrapper.
            if(dynamic_cast<const CommandException*>(&error))MinimaLogger::log(std::make_shared<NativeThrowable>(JavaTypedException("org.minima.system.commands.CommandException",dynamic_cast<const CommandException&>(error).getMessage().value_or(""),bool(dynamic_cast<const CommandException&>(error).getMessage()))));
            else MinimaLogger::log(std::make_shared<NativeThrowable>(error));runtime()->MYSQL_IMPORTING_NO_ACTION=false;throw CommandException(exceptionText(error));
        }runtime()->MYSQL_IMPORTING_NO_ACTION=false;
    }else if(action=="fixmissing"){
        auto began=now();auto input=MiniFile::createBaseFile(getParam("file"));RawArchiveInput raw(input);raw.connect();int64_t startchecking=0;if(existsParam("startfix"))startchecking=getNumberParam("startfix").getAsLong();int32_t counter=0,total=0;bool saving=false;
        while(true){auto ibd=raw.getNextIBD();auto blocks=ibd.getTxBlocks();if(logs&&counter%10==0&&!sqlRequired(blocks).empty())MinimaLogger::log("Loading from RAW Block : "+height(blocks->front()).toString()+" SAVING:"+(saving?"true":"false"),false);if(blocks->empty())break;
            for(const auto& block:*blocks){auto number=height(block).getAsLong();if(number>startchecking){saving=true;if(!connection.loadBlockFromNum(number)){connection.saveBlock(block.get());total=increment(total);log("Save MISSING block : "+std::to_string(number));}}}counter=increment(counter);if(counter%10==0)collect();
        }raw.stop();json::JSONObject response;response.put("missing",total);response.put("time",MiniFormat::ConvertMilliToTime(subtract(now(),began)));ret.put("response",response);
    }else if(action=="findtxpow"){
        json::JSONObject response;
        if(existsParam("txpowid")){auto id=getParam("txpowid");auto pow=connection.getTxPoW(id);if(pow){response.put("found",true);response.put("txpow",pow->toJSON());}else{auto block=connection.loadBlockFromID(id);response.put("found",bool(block));if(block)response.put("txpow",block->getTxPoW().toJSON());}}
        else if(existsParam("block")){auto number=getNumberParam("block");auto block=connection.loadBlockFromNum(number.getAsLong());response.put("found",bool(block));if(block)response.put("txpow",block->getTxPoW().toJSON());}
        else throw CommandException("MUST provide either txpowid or block for findtxpow function");ret.put("response",response);
    }else throw CommandException("Invalid action : "+action);
    connection.shutdown();return ret;
}
void mysql::convertMySQLParams(const std::optional<std::string>& details){mysql_detail::configure(details,pause);}
void mysql::main(const std::vector<std::string>&){
    MySQLConnect connection("127.0.0.1:3306","minimadb","minimauser","minimapassword");
    try{std::cout<<"Attemp Connect!\n";connection.init();std::cout<<"Connected!\n";auto count=connection.getCount();std::cout<<"Rows : "<<count<<'\n';connection.shutdown();std::cout<<"Shutdown!\n";}catch(const SqlException& error){NativeThrowable(error).printStackTrace(std::cerr);}
}
}
