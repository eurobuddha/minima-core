#include "mysqlcoins.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/mysql/MySQLConnect.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <bit>
namespace minima::commands {
namespace {
int32_t increment(int32_t value){return std::bit_cast<int32_t>(uint32_t(value)+1);}
int64_t nextBlock(int64_t value){return std::bit_cast<int64_t>(uint64_t(value)+1);}
}
mysqlcoins::mysqlcoins():Command("mysqlcoins","Create and search a coins database from your MySQL Archive"){}
std::optional<std::string> mysqlcoins::getFullHelp() const{return
"\nmysqlcoins\n"
"\n"
"Create a coins db from your mysql data and search it.\n"
"\n"
"Use the same database you already use for your TxBlocks. Just creates a new table.\n"
"\n"
"You can use setlogin in mysql function to auto set the login details"
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
"    Show detailed logs - default false.\n"
"\n"
"where:\n"
"    The search criteria. String data MUST be in single quotes. You can use multiple parameters.\n"
"\n"
"maxcoins:\n"
"    The maximum numnber of coins to add. The update can take a VERY long time so this way you can limit it.\n"
"\n"
"action:\n"
"    info : Get information about the Coins DB.\n"
"    wipe : Wipe the Coins DB.\n"
"    update : Update the coins db from the latest coin added with MySQL data.\n"
"    search : Perform a search on the data. You can specify any valid query param or do a specific address check.\n"
"           Other params :\n"
"           query:the full sql query\n"
"\n"
"           address:the address to check for\n"
"           spent:true/false\n"
"           limit:limit the number of rows returned\n"
"\n"
"Examples:\n"
"\n"
"mysqlcoins host:127.0.0.1:3306 database:coinsdb user:myuser password:myuser action:update maxcoins:100\n"
"\n"
"mysqlcoins (If you have not setlogin in mysql function)..LOGIN_DETAILS.. action:search where:\"address='0x791E78C60652B0E19B8FE9EB035B122B261490C477FD76E38C0C928187076103'\"\n"
"\n"
"mysqlcoins action:search where:\"address='0x791E78C60652B0E19B8FE9EB035B122B261490C477FD76E38C0C928187076103' spent:false limit:1\"\n"
"\n"
"mysqlcoins action:search address:Mx87DE..\n"
"\n"
"mysqlcoins action:search query:\"address='0x791E78C60652B0E19B8FE9EB035B122B261490C477FD76E38C0C928187076103' AND state LIKE '%0xFFEEDD%' LIMIT 10\"\n";}
std::vector<std::string> mysqlcoins::getValidParams() const{return {"action","host","database","user","password","logs","readonly","query","where","maxblocks","maxcoins","hidetoken","address","spent","limit","enable"};}
json::JSONValue mysqlcoins::runCommand(){
    auto ret=getJSONReply();
    if(GeneralParams::IS_MOBILE)throw CommandException("Sorry - MySQL does not work on Android..");
    auto action=getParam("action","info");
    auto udb=sqlRequired(MinimaDB::getDB()).getUserDB();
    std::string host,db,user,password;
    bool autologin=sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).getAutoLoginDetailsMySQL();
    if(autologin){host=sqlRequired(udb).getAutoMySQLHost();db=udb->getAutoMySQLDB();user=udb->getAutoMySQLUser();password=udb->getAutoMySQLPassword();}
    else{host=getParam("host");db=getParam("database");user=getParam("user");password=getParam("password");}
    bool logs=getBooleanParam("logs",false);
    int32_t maxblocks=getNumberParam("maxblocks",MiniNumber::BILLION).getAsInt();
    int32_t maxcoins=getNumberParam("maxcoins",MiniNumber::BILLION).getAsInt();
    bool readonly=getBooleanParam("readonly",false);
    MySQLConnect mysql(host,db,user,password,readonly);mysql.init();
    auto arch=sqlRequired(MinimaDB::getDB()).getArchive();
    if(action=="info"){
        auto total=mysql.getTotalCoins();auto last=mysql.getMaxCoinBlock();
        json::JSONObject response;response.put("lastblock",last);response.put("total",total);
        response.put("autobackup",sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).getAutoBackupMySQLCoins());ret.put("response",response);
    }else if(action=="wipe"){
        MinimaLogger::log("Wiping CoinsDB..");mysql.wipeCoinsDB();ret.put("respnse","CoinsDB wiped");
    }else if(action=="autobackup"){
        bool enable=getBooleanParam("enable");sqlRequired(udb).setAutoBackupMySQLCoins(enable);
        if(enable){udb->setAutoMySQLHost(host);udb->setAutoMySQLDB(db);udb->setAutoMySQLUser(user);udb->setAutoMySQLPassword(password);}
        else{udb->setAutoMySQLHost("");udb->setAutoMySQLDB("");udb->setAutoMySQLUser("");udb->setAutoMySQLPassword("");}
        json::JSONObject response;response.put("autobackup",sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).getAutoBackupMySQLCoins());ret.put("response",response);
    }else if(action=="update"){
        MinimaLogger::log("Updating CoinsDB..");auto startblock=mysql.loadLastBlock();auto lastcoin=mysql.getMaxCoinBlock();
        if(lastcoin!=-1){MinimaLogger::log("Last coin found @ "+std::to_string(lastcoin));startblock=nextBlock(lastcoin);}
        MinimaLogger::log("Starting from : "+std::to_string(startblock));
        int64_t firstblock=-1,endblock=-1,startload=startblock;
        int32_t counter=0,coincounter=0,blockcounter=0;
        auto starttimer=TimerMessage::currentTimeMillis();auto timer=starttimer;bool reached=false;
        while(!reached){
            auto blocks=mysql.loadBlockRange(MiniNumber(startload));if(blocks.empty())break;
            if(logs){auto now=TimerMessage::currentTimeMillis();if(std::bit_cast<int64_t>(uint64_t(now)-uint64_t(timer))>5000){timer=now;MinimaLogger::log("Loading from MySQL @ "+std::to_string(startload));}}
            if(counter%10==0){auto collect=MinimaLogger::getRuntime().collectMemory;if(collect)collect();}
            for(auto& pointer:blocks){
                blockcounter=increment(blockcounter);
                if(blockcounter>maxblocks){blockcounter=std::bit_cast<int32_t>(uint32_t(blockcounter)-1);reached=true;break;}
                auto& block=sqlRequired(pointer);auto blocknum=block.getTxPoW().getBlockNumber();
                if(firstblock==-1)firstblock=blocknum.getAsLong();
                auto date=MinimaLogger::DATEFORMAT.format(block.getTxPoW().getTimeMilli().getAsLong());
                for(auto& proof:block.getInputCoinProofs()){
                    auto& coin=proof.getCoin();coin.setSpent(true);
                    if(logs)MinimaLogger::log("Found Input coin @ "+blocknum.toString()+" : "+coin.toJSON().toString());
                    mysql.insertCoin(coin,blocknum.getAsLong(),date);coincounter=increment(coincounter);
                }
                for(auto& coin:block.getOutputCoins()){
                    coin.setSpent(false);coin.setBlockCreated(blocknum);
                    if(logs)MinimaLogger::log("Found Output coin @ "+blocknum.toString()+" : "+coin.toJSON().toString());
                    mysql.insertCoin(coin,0,date);coincounter=increment(coincounter);
                }
                endblock=block.getTxPoW().getBlockNumber().getAsLong();
                if(coincounter>maxcoins){MinimaLogger::log("Max coins added : "+std::to_string(coincounter)+"/"+std::to_string(maxcoins));reached=true;break;}
            }
            counter=increment(counter);startload=nextBlock(endblock);
        }
        auto timediff=std::bit_cast<int64_t>(uint64_t(TimerMessage::currentTimeMillis())-uint64_t(starttimer));
        json::JSONObject response;response.put("duration",std::to_string(timediff/1000)+" seconds");response.put("blocks",blockcounter);response.put("firstblock",firstblock);response.put("lastblock",endblock);response.put("coinsadded",coincounter);response.put("message","MySQL CoinsDB Updated..");ret.put("response",response);
    }else if(action=="search"){
        std::string sql;
        if(existsParam("where"))sql="SELECT * FROM coins WHERE "+getParam("where");
        else if(existsParam("address")){
            auto address=getAddressParam("address");sql="SELECT * FROM coins WHERE ( address='"+address+"' OR state LIKE '%"+address+"%' )";
            if(existsParam("spent"))sql+=getBooleanParam("spent")?" AND spent=1":" AND spent=0";
            sql+=" ORDER BY blockcreated ASC";
            if(existsParam("limit"))sql+=" LIMIT "+std::to_string(getNumberParam("limit").getAsInt());
        }else if(existsParam("query"))sql=getParam("query");
        else throw CommandException("MUST provide where or query parameter..");
        bool hide=getBooleanParam("hidetoken",false);ret.put("response",mysql.searchCoins(sql,hide));
    }else throw CommandException("Invalid action : "+action);
    mysql.shutdown();return ret;
}
}
