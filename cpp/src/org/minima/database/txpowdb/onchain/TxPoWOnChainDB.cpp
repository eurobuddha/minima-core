#include "TxPoWOnChainDB.hpp"
#include "support/JavaDateString.hpp"
#include <bit>

namespace minima {
namespace {
const std::string createTable = "CREATE TABLE IF NOT EXISTS `onchaintxpow` ("
    "  `id` bigint auto_increment,"
    "  `blockid` varchar(80) NOT NULL,"
    "  `block` bigint NOT NULL,"
    "  `txpowid` varchar(80) NOT NULL,"
    "  `timemilli` bigint NOT NULL"
    ")";
json::JSONValue value(const std::optional<std::string>& text) {
    return text ? json::JSONValue(*text) : json::JSONValue();
}
}
TxPoWOnChainDB::TxPoWOnChainDB(SqlDBServices services, std::function<int64_t()> clock)
    : SqlDB(std::move(services)), mClock(std::move(clock)) {
    if (!mClock) throw std::invalid_argument("TxPoWOnChainDB requires a clock");
}
void TxPoWOnChainDB::createSQL() {
    auto statement = sqlRequired(mSQLConnection).createStatement();
    sqlRequired(statement).execute(createTable);
    sqlRequired(statement).close();
    SQL_INSERT_ONCHAINTXPOW = sqlRequired(mSQLConnection).prepareStatement(
        "INSERT IGNORE INTO onchaintxpow ( blockid, block, txpowid, timemilli ) VALUES ( ?, ? ,? ,? )");
    SQL_SELECT_ONCHAINTXPOW = sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM onchaintxpow WHERE txpowid=?");
    SQL_DELETE_ONCHAINTXPOW = sqlRequired(mSQLConnection).prepareStatement("DELETE FROM onchaintxpow WHERE timemilli < ?");
    SQL_SELECT_ONBLOCKTXPOW = sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM onchaintxpow WHERE blockid=?");
    SQL_SELECT_ONBLOCKNUMTXPOW = sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM onchaintxpow WHERE block=?");
    SQL_TOTAL_TXPOW = sqlRequired(mSQLConnection).prepareStatement("SELECT COUNT(*) AS tot FROM onchaintxpow");
    SQL_SELECT_FIRSTTXPOW = sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM onchaintxpow ORDER BY timemilli ASC LIMIT 1");
    SQL_SELECT_LASTTXPOW = sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM onchaintxpow ORDER BY timemilli DESC LIMIT 1");
}
void TxPoWOnChainDB::wipeDB() {
    checkOpen();
    auto statement = sqlRequired(mSQLConnection).createStatement();
    sqlRequired(statement).execute("DROP ALL OBJECTS");
    sqlRequired(statement).execute(createTable);
    sqlRequired(statement).close();
}
bool TxPoWOnChainDB::addOnChainTxPoW(const std::optional<std::string>& blockID,
    const MiniNumber* block, const std::optional<std::string>& txPoWID) {
    std::lock_guard lock(mSQLLock);
    try {
        checkOpen();
        sqlRequired(SQL_INSERT_ONCHAINTXPOW).clearParameters();
        sqlRequired(SQL_INSERT_ONCHAINTXPOW).setString(1, blockID);
        // Java evaluates the receiver before the MiniNumber argument.
        auto& statement = sqlRequired(SQL_INSERT_ONCHAINTXPOW);
        if (!block) throw SqlRuntimeException("java.lang.NullPointerException", "null block number");
        statement.setLong(2, block->getAsLong());
        sqlRequired(SQL_INSERT_ONCHAINTXPOW).setString(3, txPoWID);
        sqlRequired(SQL_INSERT_ONCHAINTXPOW).setLong(4, mClock());
        sqlRequired(SQL_INSERT_ONCHAINTXPOW).execute();
        return true;
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return false;
}
json::JSONObject TxPoWOnChainDB::getOnChainTxPoW(const std::optional<std::string>& txPoWID) {
    std::lock_guard lock(mSQLLock);
    json::JSONObject result;
    result.put("found", false);
    result.put("txpowid", value(txPoWID));
    try {
        checkOpen();
        sqlRequired(SQL_SELECT_ONCHAINTXPOW).clearParameters();
        sqlRequired(SQL_SELECT_ONCHAINTXPOW).setString(1, txPoWID);
        auto rows = sqlRequired(SQL_SELECT_ONCHAINTXPOW).executeQuery();
        if (sqlRequired(rows).next()) {
            result.put("found", true);
            result.put("blockid", value(sqlRequired(rows).getString("blockid")));
            result.put("block", sqlRequired(rows).getLong("block"));
            result.put("timemilli", sqlRequired(rows).getLong("timemilli"));
        }
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return result;
}
json::JSONArray TxPoWOnChainDB::getInBlockRows(const std::shared_ptr<SqlPreparedStatement>& statement,
    const std::optional<std::string>& blockID, int64_t block, bool numeric) {
    std::lock_guard lock(mSQLLock);
    json::JSONArray result;
    try {
        checkOpen();
        sqlRequired(statement).clearParameters();
        if (numeric) sqlRequired(statement).setLong(1, block);
        else sqlRequired(statement).setString(1, blockID);
        auto rows = sqlRequired(statement).executeQuery();
        while (sqlRequired(rows).next()) {
            json::JSONObject row;
            row.put("txpowid", value(sqlRequired(rows).getString("txpowid")));
            row.put("blockid", value(sqlRequired(rows).getString("blockid")));
            row.put("block", sqlRequired(rows).getLong("block"));
            row.put("timemilli", sqlRequired(rows).getLong("timemilli"));
            result.add(row);
        }
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return result;
}
json::JSONArray TxPoWOnChainDB::getInBlockTxPoW(const std::optional<std::string>& blockID) {
    return getInBlockRows(SQL_SELECT_ONBLOCKTXPOW, blockID, 0, false);
}
json::JSONArray TxPoWOnChainDB::getInBlockTxPoW(int64_t block) {
    return getInBlockRows(SQL_SELECT_ONBLOCKNUMTXPOW, std::nullopt, block, true);
}
json::JSONObject TxPoWOnChainDB::getEndTxPoW(const std::shared_ptr<SqlPreparedStatement>& statement) {
    std::lock_guard lock(mSQLLock);
    json::JSONObject result;
    result.put("found", false);
    try {
        checkOpen();
        sqlRequired(statement).clearParameters();
        auto rows = sqlRequired(statement).executeQuery();
        if (sqlRequired(rows).next()) {
            result.put("found", true);
            result.put("txpowid", value(sqlRequired(rows).getString("txpowid")));
            result.put("blockid", value(sqlRequired(rows).getString("blockid")));
            result.put("block", sqlRequired(rows).getLong("block"));
            auto time = sqlRequired(rows).getLong("timemilli");
            result.put("timemilli", time);
            result.put("date", javaDateString(time));
        }
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return result;
}
json::JSONObject TxPoWOnChainDB::getFirstTxPoW() { return getEndTxPoW(SQL_SELECT_FIRSTTXPOW); }
json::JSONObject TxPoWOnChainDB::getLastTxPoW() { return getEndTxPoW(SQL_SELECT_LASTTXPOW); }
int32_t TxPoWOnChainDB::getSize() {
    std::lock_guard lock(mSQLLock);
    try {
        checkOpen();
        auto rows = sqlRequired(SQL_TOTAL_TXPOW).executeQuery();
        if (sqlRequired(rows).next()) return sqlRequired(rows).getInt("tot");
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return -1;
}
int32_t TxPoWOnChainDB::cleanDB() {
    std::lock_guard lock(mSQLLock);
    return cleanDB(false);
}
int32_t TxPoWOnChainDB::cleanDB(bool hard) {
    std::lock_guard lock(mSQLLock);
    try {
        checkOpen();
        auto now = mClock();
        auto time = std::bit_cast<int64_t>(uint64_t(now) - uint64_t(MAX_ONCHAINSQL_MILLI.load()));
        if (hard) time = std::bit_cast<int64_t>(uint64_t(mClock()) + 100000u);
        sqlRequired(SQL_DELETE_ONCHAINTXPOW).clearParameters();
        sqlRequired(SQL_DELETE_ONCHAINTXPOW).setLong(1, time);
        return sqlRequired(SQL_DELETE_ONCHAINTXPOW).executeUpdate();
    } catch (const SqlException& error) { log(sqlExceptionText(error)); }
    return 0;
}
}
