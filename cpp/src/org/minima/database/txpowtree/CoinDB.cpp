#include "CoinDB.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaIOExceptionText.hpp"

namespace minima {
std::shared_ptr<CoinDB> CoinDB::mCoinDB;
std::mutex CoinDB::mInstanceLock;
void CoinDB::createCoinDB(const File& file, SqlDBServices services) {
    // Publish only after construction returns, including its caught SQL failures.
    auto replacement = std::shared_ptr<CoinDB>(new CoinDB(file, std::move(services)));
    std::lock_guard lock(mInstanceLock);
    mCoinDB = std::move(replacement);
}
std::shared_ptr<CoinDB> CoinDB::getTxPoWTreeCoinDB() {
    std::lock_guard lock(mInstanceLock);
    return mCoinDB;
}
CoinDB::CoinDB(const File& file, SqlDBServices services) : SqlDB(std::move(services)) {
    try {
        auto normalized = javaUnixFile(file);
        if (!normalized) throw SqlRuntimeException("java.lang.NullPointerException", "null coin database file");
        auto old = javaUnixAbsoluteFile(*normalized) + ".mv.db";
        std::error_code error;
        if (old.find('\0') == std::string::npos) std::filesystem::remove(old, error);
        loadDB(normalized);
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
}
void CoinDB::createSQL() {
    auto statement = sqlRequired(mSQLConnection).createStatement();
    sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `coins` ("
        "  `id` bigint auto_increment,"
        "  `txpowtreeid` varchar(80) NOT NULL,"
        "  `blockheight` int NOT NULL,"
        "  `coindata` blob NOT NULL"
        ")");
    sqlRequired(statement).close();
    SQL_INSERT_COIN = sqlRequired(mSQLConnection).prepareStatement("INSERT INTO coins ( txpowtreeid, blockheight, coindata ) VALUES ( ?, ?, ? )");
    SQL_SELECT_ALLCOINS = sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM coins WHERE txpowtreeid=?");
    SQL_CLEAR_COINS = sqlRequired(mSQLConnection).prepareStatement("DELETE FROM coins WHERE blockheight<?");
    SQL_SIZE = sqlRequired(mSQLConnection).prepareStatement("SELECT Count(*) as tot FROM coins");
}
void CoinDB::closeAndReopen() {
    std::lock_guard lock(mSQLLock);
    try {
        saveDB(true);
        mSQLConnection.reset();
        checkOpen(false);
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
}
bool CoinDB::insertCoin(const MiniData* treeID, const Coin* coin, const MiniNumber* block) {
    std::lock_guard lock(mSQLLock);
    try {
        if (checkOpen()) log("insertCoin CoinDB reopen required");
        if (!coin) throw SqlRuntimeException("java.lang.NullPointerException", "null coin");
        std::optional<std::vector<uint8_t>> data;
        try { data = coin->serialise(); }
        catch (const wire::IOException& error) { log(javaIOExceptionText(error)); }
        sqlRequired(SQL_INSERT_COIN).clearParameters();
        auto& first = sqlRequired(SQL_INSERT_COIN);
        if (!treeID) throw SqlRuntimeException("java.lang.NullPointerException", "null tree ID");
        first.setString(1, treeID->to0xString());
        auto& second = sqlRequired(SQL_INSERT_COIN);
        if (!block) throw SqlRuntimeException("java.lang.NullPointerException", "null block");
        second.setLong(2, block->getAsLong());
        auto& third = sqlRequired(SQL_INSERT_COIN);
        if (!data) throw SqlRuntimeException("java.lang.NullPointerException", "null serialized coin");
        third.setBytes(3, data);
        sqlRequired(SQL_INSERT_COIN).execute();
        return true;
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return false;
}
CoinDB::Coins CoinDB::getAllCoins(const MiniData* treeID) {
    std::lock_guard lock(mSQLLock);
    Coins coins;
    try {
        if (checkOpen()) log("getAllCoins CoinDB reopen required");
        sqlRequired(SQL_SELECT_ALLCOINS).clearParameters();
        auto& statement = sqlRequired(SQL_SELECT_ALLCOINS);
        if (!treeID) throw SqlRuntimeException("java.lang.NullPointerException", "null tree ID");
        statement.setString(1, treeID->to0xString());
        auto rows = sqlRequired(SQL_SELECT_ALLCOINS).executeQuery();
        while (sqlRequired(rows).next()) {
            auto data = sqlRequired(rows).getBytes("coindata");
            if (!data) throw SqlRuntimeException("java.lang.NullPointerException", "null coin bytes");
            auto coin = Coin::convertMiniDataVersion(MiniData(std::move(*data)),
                [this](const wire::IOException& error) { log(javaIOExceptionText(error)); });
            coins.push_back(coin ? std::make_shared<Coin>(std::move(*coin)) : nullptr);
        }
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return coins;
}
void CoinDB::clearOldCoins(int64_t minimumBlock) {
    std::lock_guard lock(mSQLLock);
    try {
        if (checkOpen()) log("clearOldCoins CoinDB reopen required");
        sqlRequired(SQL_CLEAR_COINS).clearParameters();
        sqlRequired(SQL_CLEAR_COINS).setLong(1, minimumBlock);
        sqlRequired(SQL_CLEAR_COINS).execute();
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
}
int32_t CoinDB::getSize() {
    std::lock_guard lock(mSQLLock);
    try {
        if (checkOpen()) log("getSize CoinDB reopen required");
        sqlRequired(SQL_SIZE).clearParameters();
        auto rows = sqlRequired(SQL_SIZE).executeQuery();
        if (sqlRequired(rows).next()) return sqlRequired(rows).getInt("tot");
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return -1;
}
}
