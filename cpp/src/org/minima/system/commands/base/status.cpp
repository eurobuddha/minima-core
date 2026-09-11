#include "status.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaDateString.hpp"
#include "support/JavaUnixFile.hpp"
#include <bit>
#include <iostream>
namespace minima::commands {
namespace {
int64_t fileLength(const SqlDB::File& file){if(!file)throw JavaNullPointerException("null database file");return javaUnixFileLength(*file);}
}
status::status():Command("status","(clean:true) - Show general status for Minima and clean RAM"){}
std::optional<std::string> status::getFullHelp() const {return "\nstatus\n\nShow the general status for Minima and your node. Optionally clean the RAM.\n\nPrints details for general status, memory used, chain info, stored txpow units, network connections, p2p connections and traffic.\n\nclean: (optional)\n    true only, clear the RAM.\n\nExamples:\n\nstatus\n\nstatus clean:true\n";}
json::JSONValue status::runCommand(){
    auto reply=getJSONReply();
    if(existsParam("clean"))MinimaLogger::getRuntime().collectMemory();
    bool debug=getBooleanParam("debug",false);bool complete=getBooleanParam("complete",false);
    auto txpdb=sqlRequired(MinimaDB::getDB()).getTxPoWDB();
    auto txptree=sqlRequired(MinimaDB::getDB()).getTxPoWTree();
    auto cascade=sqlRequired(MinimaDB::getDB()).getCascade();
    auto archive=sqlRequired(MinimaDB::getDB()).getArchive();
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    json::JSONObject details;details.put("version",GlobalParams::getFullMicroVersion());
    details.put("uptime",MiniFormat::ConvertMilliToTime(sqlRequired(Main::getInstance()).getUptimeMilli()));
    details.put("locked",!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable());
    details.put("megammr",GeneralParams::IS_MEGAMMR);
    if(complete){
        bool normal=sqlRequired(Main::getInstance()).isNormalMineMode();details.put("powmode",normal?"normal":"low");
        auto difficulty=sqlRequired(sqlRequired(txptree).getTip()).getTxPoW().getBlockDifficulty().getDataValue();
        if(difficulty.isZero())throw JavaArithmeticException("Division by zero");
        auto weight=MMREntryNumber::dividePositive(Crypto::MAX_VAL,difficulty,7);
        auto rate=sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).getHashRate();
        if(rate.isLess((*Magic::MIN_HASHES)))rate=(*Magic::MIN_HASHES);
        auto ratio=MiniNumber(weight.getUnscaled(),weight.getScale()).div(rate);
        if(!sqlRequired(Main::getInstance()).isNormalMineMode())ratio=ratio.mult(MiniNumber::TEN);
        details.put("devices",ratio.ceil().toString());
    }
    BigInt chainweight(0),cascweight(0);
    if(sqlRequired(txptree).getTip()){
        int32_t branchLength=txptree->getHeaviestBranchLength();int32_t cascadeLength=sqlRequired(cascade).getLength();
        details.put("length",int64_t(std::bit_cast<int32_t>(uint32_t(branchLength)+uint32_t(cascadeLength))));
        chainweight=sqlRequired(txptree->getRoot()).getTotalWeight().toBigInteger();
        cascweight=sqlRequired(sqlRequired(MinimaDB::getDB()).getCascade()).getTotalWeight().toBigInteger();
        details.put("weight",(chainweight+cascweight).toDecimalString());
        details.put("minima",sqlRequired(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip()).getTxPoW().getMMRTotal().toString());
        details.put("coins",sqlRequired(sqlRequired(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip()).getMMR()).getEntryNumber().toBigDecimalString());
    }else{details.put("length",0);details.put("weight","0");details.put("minima","0");details.put("coins","0");}
    details.put("data",GeneralParams::DATA_FOLDER);
    if(debug)MinimaLogger::log("Main Settings Done..");
    json::JSONObject files;auto runtime=MinimaLogger::getRuntime();auto total=runtime.totalMemory();auto free=runtime.freeMemory();
    files.put("ram",MiniFormat::formatSize(std::bit_cast<int64_t>(uint64_t(total)-uint64_t(free))));
    files.put("disk",MiniFormat::formatSize(MiniFile::getTotalFileSize(GeneralParams::DATA_FOLDER)));
    json::JSONObject database;
    database.put("txpowdb",MiniFormat::formatSize(fileLength(sqlRequired(txpdb).getSqlFile())));
    database.put("archivedb",MiniFormat::formatSize(fileLength(sqlRequired(archive).getSQLFile())));
    database.put("cascade",MiniFormat::formatSize(sqlRequired(MinimaDB::getDB()).getCascadeFileSize()));
    database.put("chaintree",MiniFormat::formatSize(sqlRequired(MinimaDB::getDB()).getTxPowTreeFileSize()));
    database.put("wallet",MiniFormat::formatSize(fileLength(sqlRequired(wallet).getSQLFile())));
    database.put("userdb",MiniFormat::formatSize(sqlRequired(MinimaDB::getDB()).getUserDBFileSize()));
    database.put("p2pdb",MiniFormat::formatSize(sqlRequired(MinimaDB::getDB()).getP2PFileSize()));
    if(complete){json::JSONObject all;MiniFile::getTotalFileSizeWithNames(GeneralParams::DATA_FOLDER,all,3,0);database.put("allfiles",all);}
    files.put("files",database);details.put("memory",files);
    if(debug)MinimaLogger::log("Memory Done..");
    json::JSONObject tree;
    if(sqlRequired(txptree).getRoot()){
        tree.put("block",sqlRequired(txptree->getTip()).getTxPoW().getBlockNumber().getAsLong());
        tree.put("time",javaDateString(sqlRequired(txptree->getTip()).getTxPoW().getTimeMilli().getAsLong()));
        tree.put("hash",sqlRequired(txptree->getTip()).getTxPoW().getTxPoWID());
        if(sqlRequired(txptree->getTip()).getTxPoW().getBlockNumber().isLessEqual(MiniNumber::TWO))tree.put("speed",1);
        else{
            auto start=txptree->getTip();auto end=sqlRequired(start).getParent(GlobalParams::MINIMA_BLOCKS_SPEED_CALC.getAsInt());
            auto medianStart=TxPoWGenerator::getMedianTimeBlock(start);auto medianEnd=TxPoWGenerator::getMedianTimeBlock(end);
            auto diff=sqlRequired(medianStart).getBlockNumber().sub(sqlRequired(medianEnd).getBlockNumber());
            if(diff.isEqual(MiniNumber::ZERO))tree.put("speed",json::JSONValue::raw(MiniNumber::MINUSONE.toString()));
            else tree.put("speed",json::JSONValue::raw(TxPoWGenerator::getChainSpeed(medianStart,diff).setSignificantDigits(5).toString()));
        }
        tree.put("difficulty",MiniData::withMinLength(sqlRequired(txptree->getTip()).getTxPoW().getBlockDifficulty().bytes(),32).to0xString());
        tree.put("size",txptree->getSize());tree.put("length",txptree->getHeaviestBranchLength());
        int32_t size=txptree->getSize();int32_t length=txptree->getHeaviestBranchLength();
        tree.put("branches",std::bit_cast<int32_t>(uint32_t(size)-uint32_t(length)));
        (void)sqlRequired(txptree->getRoot()).getTotalWeight();tree.put("weight",chainweight.toDecimalString());
    }else{tree.put("block",0);tree.put("time","NO BLOCKS YET");tree.put("hash","0x00");tree.put("length",0);tree.put("branches",0);}
    if(debug)MinimaLogger::log("Chain Tree done..");
    json::JSONObject casc;
    if(sqlRequired(cascade).getTip())casc.put("start",sqlRequired(cascade->getTip()).getTxPoW().getBlockNumber().getAsLong());else casc.put("start",-1);
    casc.put("length",cascade->getLength());casc.put("weight",cascweight.toDecimalString());tree.put("cascade",casc);details.put("chain",tree);
    database=json::JSONObject();database.put("mempool",int64_t(sqlRequired(txpdb).getAllUnusedTxns().size()));
    if(debug)MinimaLogger::log("Mempool done..");
    database.put("ramdb",txpdb->getRamSize());if(debug)MinimaLogger::log("RamDB done..");
    database.put("txpowdb",txpdb->getSqlSize());if(debug)MinimaLogger::log("txpowdb done..");
    database.put("archivedb",sqlRequired(archive).getSize());if(debug)MinimaLogger::log("archivedb done..");
    details.put("txpow",database);if(debug)MinimaLogger::log("Database done..");
    auto network=sqlRequired(Main::getInstance()).getNetworkManager();if(network)details.put("network",network->getStatus());
    if(debug)MinimaLogger::log("Network done..");
    reply.put("response",details);return reply;
}
void status::main(const std::vector<std::string>&){
    json::JSONObject files;MiniFile::getTotalFileSizeWithNames("C:\\Users\\spartacusrex\\.minima\\1.0",files,3,0);
    std::cout<<MiniFormat::JSONPretty(files)<<std::endl;
}
}
