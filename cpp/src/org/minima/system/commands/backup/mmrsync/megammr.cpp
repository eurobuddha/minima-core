#include "megammr.hpp"
#include "../../CommandRunner.hpp"
#include "megammrsync.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/BufferedFileOutput.hpp"
#include "support/JavaDateString.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/NativeThrowable.hpp"
#include <thread>
namespace minima::commands {
namespace {
void log(const std::string& text){MinimaLogger::log(text);}
std::shared_ptr<MinimaDB> db(){auto value=MinimaDB::getDB();sqlRequired(value);return value;}
std::shared_ptr<Main> mainOwner(){auto value=Main::getInstance();sqlRequired(value);return value;}
bool fileExists(const std::filesystem::path& path){std::error_code error;return path.string().find('\0')==std::string::npos&&std::filesystem::exists(path,error);}
// File.createNewFile's exclusive open is reused from MiniFile::prepareFile.
void createNew(const std::filesystem::path& path){auto name=path.string();if(name.find('\0')!=std::string::npos)throw wire::IOException("Invalid file path");int descriptor;do{descriptor=::open(name.c_str(),O_WRONLY|O_CREAT|O_EXCL,0666);}while(descriptor<0&&errno==EINTR);if(descriptor<0){if(errno!=EEXIST)throw wire::IOException(std::strerror(errno));}else ::close(descriptor);}
}
megammr::megammr():Command("megammr","(action:) (file:) - Get Info on or Import / Export the MegaMMR data"){}
std::optional<std::string> megammr::getFullHelp() const{return std::string(
"\nmegammr\n"
"\n"
"View information about your MegaMMR. Export and Import complete MegaMMR data.\n"
"\n"
"You must be running -megammr.\n"
"\n"
"action: (optional)\n"
"    info   : Shows info about your MegaMMR.\n"
"    export : Export a MegaMMR data file.\n"
"    import : Import a MegaMMR data file.\n"
"\n"
"file: (optional)\n"
"    Use with export and import.\n"
"\n"
"Examples:\n"
"\n"
"megammr\n"
"\n"
"megammr action:export\n"
"\n"
"megammr action:export file:thefile\n"
"\n"
"megammr action:import file:thefile\n");}
json::JSONValue megammr::runCommand(){
    auto result=getJSONReply();auto action=getParam("action","info");auto mega=db()->getMegaMMR();
    if(action=="info"){
        json::JSONObject response;response.put("enabled",GeneralParams::IS_MEGAMMR);response.put("mmr",sqlRequired(mega).getMMR().toJSON(false));response.put("coins",int64_t(mega->getAllCoins().size()));result.put("response",response);
    }else if(action=="export"){
        if(!GeneralParams::IS_MEGAMMR)throw CommandException("MegaMMR not enabled");auto file=getParam("file","");if(file.empty())file="megammr_"+MiniUtil::DATEFORMAT.format(MinimaLogger::getRuntime().currentTimeMillis())+".megammr";
        auto output=MiniFile::createBaseFile(file);if(fileExists(output)){std::error_code error;std::filesystem::remove(output,error);}createNew(output);auto ibd=std::make_shared<IBD>();db()->readLock(true);
        try{ibd->createCompleteIBD();MegaMMRBackup backup(mega,ibd);BufferedFileOutput stream(output,65536);stream.write(backup.serialise());stream.flush();stream.flush();stream.close();stream.close();}
        catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){db()->readLock(false);throw CommandException(nativeExceptionText(error));}
        db()->readLock(false);json::JSONObject response;response.put("megammrtip",json::JSONValue::raw(sqlRequired(mega).getMMR().getBlockTime().toString()));response.put("ibdtip",json::JSONValue::raw(ibd->getTreeTip().toString()));response.put("backup",javaUnixAbsoluteFile(output));response.put("size",MiniFormat::formatSize(javaUnixFileLength(output)));result.put("response",response);
    }else if(action=="import"){
        if(!GeneralParams::IS_MEGAMMR)throw CommandException("MegaMMR not enabled");auto file=getParam("file","");if(file.empty())throw CommandException("MUST specify a file to restore from");auto input=MiniFile::createBaseFile(file);if(!fileExists(input))throw CommandException("Restore file doesn't exist : "+javaUnixAbsoluteFile(input));MegaMMRBackup backup;
        try{log("Loading MegaMMR.. size:"+MiniFormat::formatSize(javaUnixFileLength(input)));MiniFile::loadObjectSlow(input,backup);}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){throw CommandException(nativeExceptionText(error));}
        mainOwner()->archiveResetReady(false);sqlRequired(db()->getMegaMMR()).clear();db()->hardSetMegaMMR(backup.getMegaMMR());log("Process new IBD");sqlRequired(mainOwner()->getTxPoWProcessor()).postProcessIBD(backup.getIBD(),"0x00",true);
        while(true){std::this_thread::sleep_for(std::chrono::milliseconds(250));if(sqlRequired(mainOwner()->getTxPoWProcessor()).isIBDProcessFinished())break;}
        log("System memory clean..");if(auto collect=MinimaLogger::getRuntime().collectMemory)collect();auto tree=db()->getTxPoWTree();auto tip=sqlRequired(tree).getTip();log("Current Top Block : "+sqlRequired(tip).getTxPoW().getBlockNumber().toString());auto root=tree->getRoot();log("Current Tree Root : "+sqlRequired(root).getTxPoW().getBlockNumber().toString());log("Current MegaMMR Tip : "+sqlRequired(db()->getMegaMMR()).getMMR().getBlockTime().toString());log("Get all your CoinProofs");
        auto data=megammrsync::getMyDetails();auto proofs=megammrsync::getAllCoinProofs(data);log("Transfer your CoinProofs.. "+std::to_string(proofs->size()));for(const auto& proof:*proofs)CommandRunner::getRunner().runSingleCommand("coinimport track:true data:"+MiniData(sqlRequired(proof).serialise()).to0xString());
        json::JSONObject response;response.put("message","MegaMMR import finished.. please restart");result.put("response",response);mainOwner()->setHasShutDown();mainOwner()->shutdownFinalProcs();db()->saveAllDB();mainOwner()->stopMessageProcessor();mainOwner()->NotifyMainListenerOfShutDown();
    }else if(action=="integrity"){
        auto input=MiniFile::createBaseFile(getParam("file"));if(!fileExists(input))throw CommandException("MegaMMR file doesn't exist : "+javaUnixAbsoluteFile(input));MegaMMRBackup backup;log("Load MegaMMR.. "+MiniFormat::formatSize(javaUnixFileLength(input)));MiniFile::loadObjectSlow(input,backup);auto weight=checkMegaMMR(backup);auto ibd=backup.getIBD();auto cascade=sqlRequired(sqlRequired(ibd).getCascade()).getTip();auto& pow=sqlRequired(cascade).getTxPoW();auto number=pow.getBlockNumber();auto length=sqlRequired(ibd->getTxBlocks()).size();
        json::JSONObject response;response.put("cascadetip",json::JSONValue::raw(number.toString()));response.put("cascadedate",javaDateString(pow.getTimeMilli().getAsLong()));response.put("chaintip",json::JSONValue::raw(number.add(MiniNumber(int64_t(length))).toString()));response.put("weight",weight.toDecimalString());result.put("response",response);
    }return result;
}
BigInt megammr::checkMegaMMR(const std::filesystem::path& file){MegaMMRBackup backup;log("Load MegaMMR.. "+MiniFormat::formatSize(javaUnixFileLength(file)));MiniFile::loadObjectSlow(file,backup);return checkMegaMMR(backup);}
BigInt megammr::checkMegaMMR(MegaMMRBackup& backup){
    auto mega=backup.getMegaMMR();auto mmr=sqlRequired(backup.getMegaMMR()).getSharedMMR();auto ibd=backup.getIBD();log("Check IBD..");if(!sqlRequired(ibd).checkValidData())throw CommandException("Invalid IBD");auto last=mmr->getBlockTime();
    for(const auto& block:sqlRequired(ibd->getTxBlocks())){auto number=sqlRequired(block).getTxPoW().getBlockNumber();if(!number.isEqual(last.increment()))throw CommandException("Invalid block number.. not incremental.. last_in_mega:"+last.toString()+" new_block:"+number.toString());last=number;mega->addBlock(*block);}
    mmr->finalizeSet();log("Now check all coin proofs..");auto& coins=sqlRequired(backup.getMegaMMR()).getAllCoins();auto size=coins.size();int64_t checked=0;
    for(const auto& coin:coins.elements()){auto& current=sqlRequired(coin);auto leaf=MMRData::CreateMMRDataLeafNode(current,current.getAmount());MMRProof proof;
        try{proof=mmr->getProof(coin->getMMREntryNumber());}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){throw CommandException("Error chcking coin @ "+coin->toJSON().toString()+" "+nativeExceptionText(error));}
        if(!mmr->checkProofTimeValid(coin->getMMREntryNumber(),leaf,proof))throw CommandException("INVALID Coin proof! @ "+coin->toJSON().toString());++checked;if(checked%5000==0)log("Checking coins @ "+std::to_string(checked)+" / "+std::to_string(size));
    }log("All coins checked "+std::to_string(checked)+" / "+std::to_string(size));return ibd->getTotalWeight();
}
void megammr::main(const std::vector<std::string>&){checkMegaMMR(std::filesystem::path("./bin/minima_megammr.mmr"));}
}
