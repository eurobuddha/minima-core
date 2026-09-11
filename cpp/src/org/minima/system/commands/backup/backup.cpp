#include "backup.hpp"
#include "vault.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/database/txpowdb/sql/TxPoWList.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "org/minima/utils/encrypt/GenerateKey.hpp"
#include "support/BufferedFileOutput.hpp"
#include "support/GzipOutput.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::commands {
backup::backup():Command("backup","(password:) (file:) (auto:) (maxhistory:) - Backup the system. Uses a timestamped name by default"){}
std::optional<std::string> backup::getFullHelp() const{return std::string(
"\nbackup\n"
"\n"
"Backup your node. Uses a timestamped name by default.\n"
"\n"
"password: (optional)\n"
"    Set a password using letters and numbers only.\n"
"\n"
"file: (optional)\n"
"    Specify a filename ending in .bak, optionally include a local path for the backup.\n"
"    Default location for a backup is the Minima data folder.\n"
"\n"
"auto: (optional)\n"
"    true or false, true will schedule a non-password protected backup every 24 hours.\n"
"\n"
"maxhistory: (optional)\n"
"    Max relevant TxPoW to add - your history.\n"
"\n"
"Examples:\n"
"\n"
"backup password:Longsecurepassword456\n"
"\n"
"backup password:Longsecurepassword456 confirm:Longsecurepassword456\n"
"\n"
"backup password:Longsecurepassword456 file:my-backup-01-Jan-22.bak\n"
"\n"
"backup auto:true\n");}
json::JSONValue backup::runCommand(){
    auto result=getJSONReply();vault::checkAllKeysCreated();
    if(existsParam("auto")){
        auto enabled=getBooleanParam("auto");sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).setAutoBackup(enabled);
        if(!enabled){json::JSONObject response;response.put("autobackup",false);result.put("backup",response);return result;}
    }
    auto file=getParam("file","");if(file.empty())file="minima-backup-"+std::to_string(TimerMessage::currentTimeMillis())+".bak";
    auto password=getParam("password","minima");if(password.empty())throw CommandException("Cannot have a blank password");
    if(existsParam("confirm")&&password!=getParam("confirm"))throw CommandException("Passwords do NOT match!");
    (void)getBooleanParam("complete",false);auto debug=getBooleanParam("debug",false);
    auto output=MiniFile::createBaseFile(file);if(debug)MinimaLogger::log("Backup file : "+javaUnixAbsoluteFile(output));
    std::error_code error;if(output.string().find('\0')==std::string::npos&&std::filesystem::exists(output,error))std::filesystem::remove(output,error);
    auto folder=javaUnixChildFile(GeneralParams::DATA_FOLDER,"backup");javaUnixMkdirs(folder);
    if(debug)MinimaLogger::log("Backup folder : "+javaUnixAbsoluteFile(folder));
    sqlRequired(MinimaDB::getDB()).readLock(true);
    try{
        sqlRequired(MinimaDB::getDB()).saveState();
        auto wallet=folder/"wallet.sql";sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).backupToFile(wallet);MiniData walletData(MiniFile::readCompleteFile(wallet));
        auto cascade=folder/"cascade.bak";sqlRequired(sqlRequired(MinimaDB::getDB()).getCascade()).saveDB(cascade);MiniData cascadeData(MiniFile::readCompleteFile(cascade));
        auto chain=folder/"chaintree.bak";sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).saveDB(chain);MiniData chainData(MiniFile::readCompleteFile(chain));
        auto user=folder/"userdb.bak";sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).saveDB(user);MiniData userData(MiniFile::readCompleteFile(user));
        auto peers=folder/"p2p.bak";sqlRequired(sqlRequired(MinimaDB::getDB()).getP2PDB()).saveDB(peers);MiniData peersData(MiniFile::readCompleteFile(peers));
        auto max=getNumberParam("maxhistory",TxPoWSqlDB::MAX_RELEVANT_TXPOW).getAsInt();
        auto txps=sqlRequired(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getSQLDB()).getAllRelevant(max);
        TxPoWList history(std::make_shared<TxPoWList::Rows>(std::move(txps)));MiniData historyData(history.serialise());
        BufferedFileOutput destination(output);
        MiniData iv(GenerateKey::IvParam()),salt=MiniData::getRandomData(8);
        for(const auto* value:{&salt,&iv}){
            auto frame=value->serialise();destination.writeUnbuffered(std::span(frame).first(4));destination.writeUnbuffered(std::span(frame).subspan(4));
        }
        auto secret=GenerateKey::secretKey(password,salt.bytes());auto cipher=GenerateKey::getCipherSYM(Cipher::ENCRYPT_MODE,iv.bytes(),secret.bytes);
        GzipOutput gzip([&](auto bytes){destination.writeUnbuffered(cipher.update(bytes));});
        for(const auto* section:{&walletData,&cascadeData,&chainData,&userData,&peersData,&historyData}){
            wire::Bytes prefix;for(int shift=24;shift>=0;shift-=8)prefix.push_back(uint8_t(uint32_t(section->getLength())>>shift));gzip.write(prefix);gzip.write(section->bytes());
        }
        auto closeCipher=[&]{destination.writeUnbuffered(cipher.doFinal({}));destination.close();};
        // GZIPOutputStream.close also closes its cipher after a finish IOException.
        try{gzip.finish();}catch(const wire::IOException&){closeCipher();throw;}
        closeCipher();
        auto total=javaUnixFileLength(wallet)+javaUnixFileLength(cascade)+javaUnixFileLength(chain)+javaUnixFileLength(user)+javaUnixFileLength(peers)+historyData.getLength();
        json::JSONObject files;files.put("wallet",MiniFormat::formatSize(javaUnixFileLength(wallet)));files.put("cascade",MiniFormat::formatSize(javaUnixFileLength(cascade)));
        files.put("chain",MiniFormat::formatSize(javaUnixFileLength(chain)));files.put("user",MiniFormat::formatSize(javaUnixFileLength(user)));files.put("p2p",MiniFormat::formatSize(javaUnixFileLength(peers)));files.put("txpow",MiniFormat::formatSize(historyData.getLength()));
        json::JSONObject response;
        if(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip())response.put("block",json::JSONValue::raw(sqlRequired(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip()).getTxPoW().getBlockNumber().toString()));
        else response.put("block","-1");
        response.put("files",files);response.put("uncompressed",MiniFormat::formatSize(total));response.put("file",javaUnixAbsoluteFile(output));response.put("size",MiniFormat::formatSize(javaUnixFileLength(output)));response.put("auto",sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).isAutoBackup());result.put("backup",response);
        MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);
    }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){
        sqlRequired(MinimaDB::getDB()).readLock(false);MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);
        if(auto command=dynamic_cast<const CommandException*>(&error))throw CommandException(std::string("org.minima.system.commands.CommandException")+(command->getMessage()?": "+*command->getMessage():""));
        throw CommandException(nativeExceptionText(error));
    }
    sqlRequired(MinimaDB::getDB()).readLock(false);MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);return result;
}
}
