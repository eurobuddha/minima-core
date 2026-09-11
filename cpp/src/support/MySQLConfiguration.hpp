#pragma once
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/mysql/MySQLConnect.hpp"
#include "NativeThrowable.hpp"
namespace minima::mysql_detail {
// Body of pinned mysql.convertMySQLParams, shared by Main startup and the mysql command.
inline void configure(const std::optional<std::string>& details,const std::function<void(int64_t)>& sleep){
    try {
        if(!details)throw JavaNullPointerException("null MySQL details");
        auto split=[](const std::string& text,char delimiter){auto at=text.find(delimiter);if(at==std::string::npos)throw JavaTypedException("java.lang.StringIndexOutOfBoundsException","Range [0, -1) out of bounds for length "+std::to_string(unicode::decodeUTF8(text).size()));return std::pair(text.substr(0,at),text.substr(at+1));};
        auto [user,db]=split(*details,'@');auto [username,password]=split(user,':');auto [host,name]=split(db,'/');
        MinimaLogger::log("MYSQL Database Setup : "+username+":***@"+host+" / "+name);
        auto prefs=sqlRequired(MinimaDB::getDB()).getUserDB();GeneralParams::MYSQL_STORE_ALLTXPOW=true;
        sqlRequired(prefs).setAutoMySQLHost(host);prefs->setAutoMySQLDB(name);prefs->setAutoMySQLUser(username);prefs->setAutoMySQLPassword(password);prefs->setAutoLoginDetailsMySQL(true);prefs->setAutoBackupMySQL(true);
        if(GeneralParams::MYSQL_DB_COINS)prefs->setAutoBackupMySQLCoins(true);
        if(GeneralParams::MYSQL_DB_DELAY!=0){MinimaLogger::log("Waiting "+std::to_string(GeneralParams::MYSQL_DB_DELAY)+" ms before attempting first MySQL connection..");sleep(GeneralParams::MYSQL_DB_DELAY);}
        MySQLConnect connection(host,name,username,password,true);connection.init();connection.shutdown();
    }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log("Failed to connect to MySQL DB - MUST be username:password@host:port/database "+nativeExceptionText(error));}
}
}
