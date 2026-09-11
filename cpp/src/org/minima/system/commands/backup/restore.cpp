#include "restore.hpp"
#include "vault.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/CipherInput.hpp"
#include "support/GzipInput.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaExceptions.hpp"
namespace minima::commands {
restore::restore():Command("restore","[file:] (password:) - Restore the entire system."){}
std::optional<std::string> restore::getFullHelp() const{return std::string(
"\nrestore\n"
"\n"
"Restore your node from a backup. You MUST wait until all your original keys are created before this is allowed.\n"
"\n"
"file:\n"
"    Specify the filename or local path of the backup to restore\n"
"\n"
"password: (optional)\n"
"    Enter the password of the backup \n"
"\n"
"Examples:\n"
"\n"
"restore file:my-full-backup-01-Jan-22 password:Longsecurepassword456\n");}
json::JSONValue restore::runCommand(){
    auto result=getJSONReply();vault::checkAllKeysCreated();
    auto file=getParam("file","");if(file.empty())throw JavaCheckedException("MUST specify a file to restore from");
    auto password=getParam("password","minima");if(password.empty())throw CommandException("Cannot have a blank password");
    auto shutdown=getBooleanParam("shutdown",true);auto input=MiniFile::createBaseFile(file);std::error_code error;
    if(input.string().find('\0')!=std::string::npos||!std::filesystem::exists(input,error))throw JavaCheckedException("Restore file doesn't exist : "+javaUnixAbsoluteFile(input));
    if(auto collect=MinimaLogger::getRuntime().collectMemory)collect();
    auto folder=javaUnixChildFile(GeneralParams::DATA_FOLDER,"restore");javaUnixMkdirs(folder);
    auto bytes=MiniFile::readCompleteFile(input);size_t offset=0;
    auto salt=MiniData::deserialise(bytes,offset),iv=MiniData::deserialise(bytes,offset);
    auto key=GenerateKey::secretKey(password,salt.bytes());
    CipherInput cipher(GenerateKey::getCipherSYM(Cipher::DECRYPT_MODE,iv.bytes(),key.bytes),std::span(bytes).subspan(offset));
    std::unique_ptr<GzipInput> gzip;
    try{gzip=std::make_unique<GzipInput>([&](auto data){return cipher.read(data);},[&]{cipher.close();});}
    catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){throw CommandException("Incorrect Password!");}
    sqlRequired(Main::getInstance()).restoreReady(shutdown);
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
    sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).updateIncrementAllKeyUses(256);
    sqlRequired(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getSQLDB()).saveDB(false);
    sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).saveDB(false);auto archive=sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).getSQLFile();
    if(!archive)throw JavaNullPointerException("null archive SQL file");if(archive->string().find('\0')==std::string::npos)std::filesystem::remove(*archive,error);
    MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder);
    json::JSONObject response;response.put("file",javaUnixAbsoluteFile(input));result.put("restore",response);result.put("message","Restart Minima for restore to take effect!");
    sqlRequired(MinimaDB::getDB()).saveAllDB();
    if(shutdown){sqlRequired(Main::getInstance()).setHasShutDown();sqlRequired(Main::getInstance()).stopMessageProcessor();sqlRequired(Main::getInstance()).NotifyMainListenerOfShutDown();}
    return result;
}
int64_t restore::readNextBackup(const std::filesystem::path& output,GzipInput& input){auto data=MiniData::readFromStream([&](auto bytes){return input.read(bytes);});MiniFile::writeDataToFile(output,data.bytes());return javaUnixFileLength(output);}
std::shared_ptr<TxPoWList> restore::readNextTxPoWList(GzipInput& input){return TxPoWList::convertMiniDataVersion(MiniData::readFromStream([&](auto bytes){return input.read(bytes);}));}
}
