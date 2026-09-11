#include "reset.hpp"
#include "vault.hpp"
#include "org/minima/system/Main.hpp"
#include "support/CommandResult.hpp"
namespace minima::commands {
reset::reset():Command("reset","[archivefile:] [action:] (file:) (password:) (keys:) (keyuses:) - Reset the entire system using an Archive Backup file..."){}
std::optional<std::string> reset::getFullHelp() const{return std::string(
"\nreset\n"
"\n"
"Reset your node in various ways. You MUST wait until all your original keys are created before this is allowed.\n"
"\n"
"archivefile:\n"
"    Specify the the archive gzip file. Should be recently exported from an archive node.\n"
"\n"
"action:\n"
"    chainsync: Re-sync all blocks from the archivefile to get back onto the right chain.\n"
"               Seed phrase is not required, the private keys will remain unchanged.\n"
"    seedsync: Wipe the wallet and re-generate your keys from your seed phrase. Your coins will be restored.\n"
"    restore: Restore a backup and re-sync the entire chain from the archivefile.\n"
"\n"
"file:\n"
"    Specify the filename or local path of the backup to restore\n"
"\n"
"password: (optional)\n"
"    Enter the password of the backup \n"
"\n"
"phrase: (optional)\n"
"    Your 24 word seed phrase in double quotes. Use with 'action:seedsync'.\n"
"\n"
"keys: (optional)\n"
"    Number of keys to create if you need to do a seed re-sync. Default is 64. Use with 'action:seedsync'.\n"
"\n"
"keyuses: (optional)\n"
"    How many times at most you used your keys. Use with 'action:seedsync'.\n"
"    Every time you re-sync with seed phrase this needs to be higher as Minima Signatures are stateful.\n"
"    Defaults to 1000 - the max is 262144 for normal keys.\n"
"\n"
"Examples:\n"
"\n"
"reset archivefile:archiveexport-jul23.gz action:chainsync\n"
"\n"
"reset archivefile:archiveexport-jul23.gz action:seedsync keyuses:1000 phrase:\"ENTER 24 WORDS HERE\"\n"
"\n"
"reset archivefile:archiveexport-jul23.gz action:restore file:backup-jul23.bak password:Longsecurepassword456\n");}
json::JSONValue reset::runCommand(){
    auto result=getJSONReply();vault::checkAllKeysCreated();
    auto archivefile=getParam("archivefile");auto action=getParam("action");
    if(action=="chainsync"){
        result.put("response",command_result::first("archive action:import file:"+archivefile));
    }else if(action=="seedsync"){
        auto phrase=getParam("phrase");auto command="archive action:import file:"+archivefile+" phrase:\""+phrase+"\"";
        if(existsParam("keys"))command+=" keys:"+getParam("keys");
        if(existsParam("keyuses"))command+=" keyuses:"+getParam("keyuses");
        result.put("response",command_result::first(command));
    }else if(action=="restore"){
        auto file=getParam("file");auto command="restore shutdown:false file:"+file;
        if(existsParam("password"))command+=" password:"+getParam("password");
        auto restored=command_result::first(command);
        if(!command_result::status(restored))throw CommandException("Error restoring.. "+restored.toJSONString());
        json::JSONObject response;response.put("restore",restored);
        sqlRequired(Main::getInstance()).restoreReadyForSync();
        response.put("chainsync",command_result::first("archive action:import file:"+archivefile));
        result.put("response",response);
    }else throw CommandException("Invalid action : "+action);
    return result;
}
}
