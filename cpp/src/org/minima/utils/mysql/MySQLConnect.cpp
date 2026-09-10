#include "MySQLConnect.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaIOExceptionText.hpp"
#include "support/JavaFloating.hpp"
#include "support/JavaUnicode.hpp"
#include <unicode/unum.h>
#include <bit>
#include <iostream>
namespace minima {
MySQLConnectServices MySQLConnect::runtimeServices;
namespace {
template<class T>const T& required(const T* value){if(!value)throw JavaNullPointerException("null MySQL object");return *value;}
template<class T>std::optional<std::vector<uint8_t>> serialized(const T* object,const std::function<void(const std::string&)>& log){
    try{return required(object).serialise();}catch(const wire::IOException& error){log(javaIOExceptionText(error));}return std::nullopt;
}
const std::vector<uint8_t>& bytes(const std::optional<std::vector<uint8_t>>& value){if(!value)throw JavaNullPointerException("null MySQL bytes");return *value;}
class HiddenText:public SqlObject{public:std::u16string toString() override{return u"_hidden_";}};
int64_t entryLong(const MMREntryNumber& entry){
    auto digits=entry.getUnscaled().toDecimalString();bool negative=digits.front()=='-';if(negative)digits.erase(0,1);
    int64_t scale=entry.getScale();if(scale>=int64_t(digits.size()))return 0;
    if(scale>0)digits.resize(digits.size()-size_t(scale));
    uint64_t value=0;for(char digit:digits)value=value*10+unsigned(digit-'0');
    if(scale<0){if(scale<=-64)return 0;for(int64_t i=0;i<-scale;++i)value*=10;}
    return std::bit_cast<int64_t>(negative?uint64_t(0)-value:value);
}
std::u16string fixedDouble(double number){
    if(!std::isfinite(number))return unicode::decodeUTF8(javaFloating(number));
    auto text=javaFloating(std::abs(number));auto e=text.find('E');int exponent=e==std::string::npos?0:std::stoi(text.substr(e+1));if(e!=std::string::npos)text.resize(e);
    auto point=text.find('.');int fraction=point==std::string::npos?0:int(text.size()-point-1);if(point!=std::string::npos)text.erase(point,1);
    int shift=exponent-fraction+6;std::string scaled;
    if(shift>=0)scaled=text+std::string(size_t(shift),'0');
    else{
        int keep=int(text.size())+shift;bool roundUp=keep>=0&&text[size_t(keep)]>='5';scaled=keep>0?text.substr(0,size_t(keep)):"0";
        if(roundUp){size_t i=scaled.size();while(i>0&&scaled[i-1]=='9'){scaled[--i]='0';}if(i>0)++scaled[i-1];else scaled.insert(0,1,'1');}
    }
    auto first=scaled.find_first_not_of('0');scaled=first==std::string::npos?"0":scaled.substr(first);
    if(scaled.size()<7)scaled.insert(0,7-scaled.size(),'0');scaled.insert(scaled.size()-6,1,'.');
    UErrorCode error=U_ZERO_ERROR;auto fmt=unum_open(UNUM_DECIMAL,nullptr,0,nullptr,nullptr,&error);
    if(U_FAILURE(error)||!fmt)throw std::runtime_error("Cannot obtain numeric locale");
    struct Close{UNumberFormat* value;~Close(){unum_close(value);}}close{fmt};
    UChar decimal[16],zero[16];int dl=unum_getSymbol(fmt,UNUM_DECIMAL_SEPARATOR_SYMBOL,decimal,16,&error);int zl=unum_getSymbol(fmt,UNUM_ZERO_DIGIT_SYMBOL,zero,16,&error);
    if(U_FAILURE(error)||dl<1||dl>=16||zl!=1)throw std::runtime_error("Unsupported numeric locale symbols");
    std::u16string result;if(std::signbit(number))result+='-';
    for(char digit:scaled){if(digit=='.')result.append(reinterpret_cast<char16_t*>(decimal),size_t(dl));else result+=char16_t(zero[0]+digit-'0');}return result;
}
}
MySQLConnect::MySQLConnect(Text host,Text database,Text username,Text password,bool readOnly,MySQLConnectServices services)
 :mMySQLHost(std::move(host)),mDatabase(std::move(database)),mUsername(std::move(username)),mPassword(std::move(password)),mReadOnly(readOnly),mServices(std::move(services)){}
void MySQLConnect::log(const std::string& message) const{if(mServices.log)mServices.log(message);else MinimaLogger::log(message);}
void MySQLConnect::init(){
    auto url="jdbc:mysql://"+mMySQLHost.value_or("null")+"/"+mDatabase.value_or("null")+"?autoReconnect=true";
    if(DEBUG){log("JDBC - "+url);log("Username:"+mUsername.value_or("null")+" Password:"+mPassword.value_or("null"));}
    if(!mServices.connect)throw SqlException("No native MySQL driver configured");
    mConnection=mServices.connect(url,mUsername,mPassword);
    if(!mReadOnly){auto statement=sqlRequired(mConnection).createStatement();
        sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `syncblock` (  `id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,  `txpowid` varchar(80) NOT NULL UNIQUE,  `block` bigint NOT NULL UNIQUE,  `timemilli` bigint NOT NULL,  `syncdata` mediumblob NOT NULL)");
        sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `cascadedata` (\t\t`id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,\t\t`cascadetip` BIGINT NOT NULL,\t\t`fulldata` mediumblob NOT NULL)");
        sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `coins` (\t `id` bigint NOT NULL AUTO_INCREMENT PRIMARY KEY,  `coinid` varchar(128) NOT NULL,  `address` varchar(128) NOT NULL,  `amount` varchar(128) NOT NULL,  `amountdouble` double NOT NULL,  `tokenid` varchar(128) NOT NULL,  `storestate` int NOT NULL,  `state` text,  `mmrentrynumber` bigint NOT NULL,  `spent` int NOT NULL,  `blockcreated` bigint NOT NULL,  `blockspent` bigint NOT NULL,  `date` varchar(128) NOT NULL,  `token` text,  `tokenamount` double NOT NULL)");
        sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `txpow` (  `id` bigint NOT NULL AUTO_INCREMENT PRIMARY KEY,  `txpowid` varchar(80) NOT NULL UNIQUE,  `txpowdata` mediumblob NOT NULL)");
        sqlRequired(statement).close();
    }
    SQL_INSERT_SYNCBLOCK=sqlRequired(mConnection).prepareStatement("INSERT IGNORE INTO syncblock ( txpowid, block, timemilli, syncdata ) VALUES ( ?, ? ,? ,? )");
    SQL_FIND_SYNCBLOCK_ID=sqlRequired(mConnection).prepareStatement("SELECT syncdata FROM syncblock WHERE txpowid=?");
    SQL_FIND_SYNCBLOCK_NUM=sqlRequired(mConnection).prepareStatement("SELECT syncdata FROM syncblock WHERE block=?");
    SQL_SELECT_RANGE=sqlRequired(mConnection).prepareStatement("SELECT syncdata FROM syncblock WHERE block>=? ORDER BY block ASC LIMIT 250");
    SQL_SELECT_LAST_BLOCK=sqlRequired(mConnection).prepareStatement("SELECT block FROM syncblock ORDER BY block ASC LIMIT 1");
    SQL_SELECT_FIRST_BLOCK=sqlRequired(mConnection).prepareStatement("SELECT block FROM syncblock ORDER BY block DESC LIMIT 1");
    SQL_COUNT=sqlRequired(mConnection).prepareStatement("SELECT Count(*) as tot FROM syncblock");
    SAVE_CASCADE=sqlRequired(mConnection).prepareStatement("INSERT INTO cascadedata ( cascadetip, fulldata ) VALUES ( ?, ? )");
    LOAD_CASCADE=sqlRequired(mConnection).prepareStatement("SELECT fulldata FROM cascadedata ORDER BY cascadetip ASC LIMIT 1");
    SQL_INSERT_COIN=sqlRequired(mConnection).prepareStatement("INSERT INTO coins(coinid,address,amount,amountdouble,tokenid,storestate,state,mmrentrynumber,spent,blockcreated,blockspent,date,token,tokenamount) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? )");
    SQL_LATEST_COIN=sqlRequired(mConnection).prepareStatement("SELECT MAX(blockcreated) as maxblock from coins");
    SQL_TOTAL_COIN=sqlRequired(mConnection).prepareStatement("SELECT COUNT(*) as tot from coins");
    SQL_INSERT_TXPOW=sqlRequired(mConnection).prepareStatement("INSERT IGNORE INTO txpow ( txpowid, txpowdata ) VALUES ( ?, ? )");
    SQL_GET_TXPOW=sqlRequired(mConnection).prepareStatement("SELECT * FROM txpow WHERE txpowid=?");
}
void MySQLConnect::shutdown(){try{if(!sqlRequired(mConnection).isClosed())mConnection->close();}catch(const SqlException& error){log(sqlExceptionText(error));}}
void MySQLConnect::wipeAll(){auto statement=sqlRequired(mConnection).createStatement();for(const auto* table:{"syncblock","cascadedata","coins","txpow"})sqlRequired(statement).execute(std::string("DROP TABLE ")+table);sqlRequired(statement).close();shutdown();init();}
void MySQLConnect::wipeCoinsDB(){auto statement=sqlRequired(mConnection).createStatement();sqlRequired(statement).execute("DROP TABLE coins");statement->close();shutdown();init();}
bool MySQLConnect::saveCascade(const Cascade* cascade){
    auto data=serialized(cascade,[this](const auto& text){log(text);});sqlRequired(SAVE_CASCADE).clearParameters();
    sqlRequired(SAVE_CASCADE).setLong(1,sqlRequired(required(cascade).getTip()).getTxPoW().getBlockNumber().getAsLong());sqlRequired(SAVE_CASCADE).setBytes(2,bytes(data));sqlRequired(SAVE_CASCADE).execute();return true;
}
std::shared_ptr<Cascade> MySQLConnect::loadCascade(){
    sqlRequired(LOAD_CASCADE).clearParameters();auto rows=sqlRequired(LOAD_CASCADE).executeQuery();if(sqlRequired(rows).next()){
        auto value=Cascade::convertMiniDataVersion(MiniData(bytes(rows->getBytes("fulldata"))),[this](const auto& error){log(javaIOExceptionText(error));});
        if(value)return std::make_shared<Cascade>(std::move(*value));
    }return nullptr;
}
int32_t MySQLConnect::getCount(){std::lock_guard lock(mMonitor);try{auto rows=sqlRequired(SQL_COUNT).executeQuery();if(sqlRequired(rows).next())return rows->getInt("tot");}catch(const SqlException& error){log(sqlExceptionText(error));}return -1;}
bool MySQLConnect::saveBlock(const TxBlock* block){std::lock_guard lock(mMonitor);try{
    auto data=serialized(block,[this](const auto& text){log(text);});sqlRequired(SQL_INSERT_SYNCBLOCK).clearParameters();
    sqlRequired(SQL_INSERT_SYNCBLOCK).setString(1,required(block).getTxPoW().getTxPoWID());sqlRequired(SQL_INSERT_SYNCBLOCK).setLong(2,block->getTxPoW().getBlockNumber().getAsLong());
    sqlRequired(SQL_INSERT_SYNCBLOCK).setLong(3,mServices.now());sqlRequired(SQL_INSERT_SYNCBLOCK).setBytes(4,bytes(data));sqlRequired(SQL_INSERT_SYNCBLOCK).execute();return true;
}catch(const SqlException& error){log(sqlExceptionText(error));}return false;}
std::shared_ptr<TxBlock> MySQLConnect::readBlock(SqlResultSet& rows){auto value=TxBlock::convertMiniDataVersion(MiniData(bytes(rows.getBytes("syncdata"))),[this](const auto& error){log(javaIOExceptionText(error));});return value?std::make_shared<TxBlock>(std::move(*value)):nullptr;}
std::shared_ptr<TxBlock> MySQLConnect::loadBlockFromID(const Text& id){std::lock_guard lock(mMonitor);try{
    sqlRequired(SQL_FIND_SYNCBLOCK_ID).clearParameters();sqlRequired(SQL_FIND_SYNCBLOCK_ID).setString(1,id);auto rows=sqlRequired(SQL_FIND_SYNCBLOCK_ID).executeQuery();if(sqlRequired(rows).next())return readBlock(*rows);
}catch(const SqlException& error){log(sqlExceptionText(error));}return nullptr;}
std::shared_ptr<TxBlock> MySQLConnect::loadBlockFromNum(int64_t number){std::lock_guard lock(mMonitor);try{
    sqlRequired(SQL_FIND_SYNCBLOCK_NUM).clearParameters();sqlRequired(SQL_FIND_SYNCBLOCK_NUM).setLong(1,number);auto rows=sqlRequired(SQL_FIND_SYNCBLOCK_NUM).executeQuery();if(sqlRequired(rows).next())return readBlock(*rows);
}catch(const SqlException& error){log(sqlExceptionText(error));}return nullptr;}
int64_t MySQLConnect::edge(const std::shared_ptr<SqlPreparedStatement>& statement){try{sqlRequired(statement).clearParameters();auto rows=sqlRequired(statement).executeQuery();if(sqlRequired(rows).next())return rows->getLong("block");}catch(const SqlException& error){log(sqlExceptionText(error));}return -1;}
int64_t MySQLConnect::loadFirstBlock(){std::lock_guard lock(mMonitor);return edge(SQL_SELECT_FIRST_BLOCK);}
int64_t MySQLConnect::loadLastBlock(){std::lock_guard lock(mMonitor);return edge(SQL_SELECT_LAST_BLOCK);}
std::vector<std::shared_ptr<TxBlock>> MySQLConnect::loadBlockRange(const MiniNumber* start){std::lock_guard lock(mMonitor);std::vector<std::shared_ptr<TxBlock>> blocks;try{
    sqlRequired(SQL_SELECT_RANGE).clearParameters();sqlRequired(SQL_SELECT_RANGE).setLong(1,required(start).getAsLong());auto rows=sqlRequired(SQL_SELECT_RANGE).executeQuery();while(sqlRequired(rows).next())blocks.push_back(readBlock(*rows));
}catch(const SqlException& error){log(sqlExceptionText(error));}return blocks;}
void MySQLConnect::insertCoin(const Coin* coin,int64_t blockSpent,const Text& date){std::lock_guard lock(mMonitor);try{
    sqlRequired(SQL_INSERT_COIN).clearParameters();sqlRequired(SQL_INSERT_COIN).setString(1,required(coin).getCoinID().to0xString());
    sqlRequired(SQL_INSERT_COIN).setString(2,coin->getAddress().to0xString());sqlRequired(SQL_INSERT_COIN).setString(3,coin->getAmount().toString());sqlRequired(SQL_INSERT_COIN).setDouble(4,coin->getAmount().getAsDouble());
    sqlRequired(SQL_INSERT_COIN).setString(5,coin->getTokenID().to0xString());sqlRequired(SQL_INSERT_COIN).setInt(6,coin->storeState()?1:0);sqlRequired(SQL_INSERT_COIN).setString(7,coin->getStateAsJSON().toString());
    sqlRequired(SQL_INSERT_COIN).setLong(8,entryLong(coin->getMMREntryNumber()));sqlRequired(SQL_INSERT_COIN).setInt(9,coin->getSpent()?1:0);sqlRequired(SQL_INSERT_COIN).setLong(10,coin->getBlockCreated().getAsLong());
    sqlRequired(SQL_INSERT_COIN).setLong(11,blockSpent);sqlRequired(SQL_INSERT_COIN).setString(12,date);sqlRequired(SQL_INSERT_COIN).setString(13,coin->getToken()?coin->getToken()->toJSON().toString():"");
    sqlRequired(SQL_INSERT_COIN).setDouble(14,coin->getTokenAmount().getAsDouble());sqlRequired(SQL_INSERT_COIN).execute();
}catch(const SqlException& error){log(sqlExceptionText(error));}}
int64_t MySQLConnect::getMaxCoinBlock(){try{auto rows=sqlRequired(SQL_LATEST_COIN).executeQuery();if(sqlRequired(rows).next())return rows->getLong("maxblock");}catch(const SqlException& error){log(sqlExceptionText(error));}return -1;}
int64_t MySQLConnect::getTotalCoins(){try{auto rows=sqlRequired(SQL_TOTAL_COIN).executeQuery();if(sqlRequired(rows).next())return rows->getLong("tot");}catch(const SqlException& error){log(sqlExceptionText(error));}return 0;}
json::JSONObject MySQLConnect::searchCoins(const Text& query,bool hideToken){std::lock_guard lock(mMonitor);json::JSONObject error;try{
    auto statement=sqlRequired(mConnection).createStatement();json::JSONObject results;results.put("sql",query?json::JSONValue(*query):json::JSONValue());error.put("sql",results.get("sql"));
    if(sqlRequired(statement).execute(query)){
        auto rows=statement->getResultSet();json::JSONArray allrows;auto meta=sqlRequired(rows).getMetaData();int32_t columns=sqlRequired(meta).getColumnCount();uint32_t count=0;
        while(rows->next()){
            ++count;json::JSONObject row;for(int64_t i=1;i<=columns;++i){auto column=meta->getColumnName(int32_t(i));auto value=rows->getObject(int32_t(i));bool hidden=hideToken&&column==u"token";if(hidden)value=std::make_shared<HiddenText>();
                if(value){auto type=meta->getColumnClassName(int32_t(i));if(!type)throw JavaNullPointerException("null SQL column class");
                    if(*type=="java.sql.Clob"&&!hidden){auto clob=std::dynamic_pointer_cast<SqlClob>(value);if(!clob)throw JavaClassCastException("SQL object is not a Clob");row.put(column,clob->getSubString(1,std::bit_cast<int32_t>(uint32_t(clob->length()))));}
                    else if(*type=="java.lang.Double"){auto number=std::dynamic_pointer_cast<SqlDouble>(value);if(!number)throw JavaClassCastException("SQL object is not a Double");row.put(column,fixedDouble(number->doubleValue()));}
                    else row.put(column,value->toString());
                }
            }allrows.add(row);
        }results.put("status",true);results.put("results",true);results.put("count",std::bit_cast<int32_t>(count));results.put("rows",allrows);
    }else{results.put("status",true);results.put("results",false);}sqlRequired(statement).close();return results;
}catch(const SqlException& exception){log(sqlExceptionText(exception));error.put("status",false);error.put("results",false);error.put("error",true);}return error;}
bool MySQLConnect::saveTxPoW(const TxPoW* txpow){std::lock_guard lock(mMonitor);try{
    auto data=serialized(txpow,[this](const auto& text){log(text);});sqlRequired(SQL_INSERT_TXPOW).clearParameters();sqlRequired(SQL_INSERT_TXPOW).setString(1,required(txpow).getTxPoWID());sqlRequired(SQL_INSERT_TXPOW).setBytes(2,bytes(data));sqlRequired(SQL_INSERT_TXPOW).execute();return true;
}catch(const SqlException& error){log(sqlExceptionText(error));}return false;}
std::shared_ptr<TxPoW> MySQLConnect::getTxPoW(const Text& id){std::lock_guard lock(mMonitor);try{
    sqlRequired(SQL_GET_TXPOW).clearParameters();sqlRequired(SQL_GET_TXPOW).setString(1,id);auto rows=sqlRequired(SQL_GET_TXPOW).executeQuery();if(sqlRequired(rows).next()){
        auto value=TxPoW::convertMiniDataVersion(MiniData(bytes(rows->getBytes("txpowdata"))),[this](const auto& error){log(javaIOExceptionText(error));});if(value)return std::make_shared<TxPoW>(std::move(*value));
    }
}catch(const SqlException& error){log(sqlExceptionText(error));}return nullptr;}
void MySQLConnect::main(const std::vector<std::string>&){
    MySQLConnect mysql("localhost:3306","mydatabase","myuser","myuser");mysql.init();TxPoW txpow;txpow.setBlockNumber(MiniNumber::ZERO);txpow.setTimeMilli(MiniNumber::ZERO);txpow.calculateTXPOWID();
    std::cout<<"ID:"<<txpow.getTxPoWID()<<'\n';mysql.saveTxPoW(txpow);auto value=mysql.getTxPoW("0xA04535D07E88F9AF7E9F8F706E4509526CB4E95203AB7730B7E4B7EF6B53D38B");MinimaLogger::log(sqlRequired(value).toJSON().toString());mysql.shutdown();
}
}
