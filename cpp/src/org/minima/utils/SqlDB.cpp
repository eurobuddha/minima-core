#include "SqlDB.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "support/JavaUnixFile.hpp"
#include <bit>
#include <iostream>

namespace minima {
namespace {
const std::filesystem::path& requiredFile(const SqlDB::File& file) {
    if (!file) throw SqlRuntimeException("java.lang.NullPointerException", "null SQL file");
    return *file;
}
bool invalidFile(const std::filesystem::path& file) {
    return file.string().find('\0') != std::string::npos;
}
std::string absolute(const SqlDB::File& file) {
    const auto& path = requiredFile(file);
    return javaUnixAbsoluteFile(path);
}
void makeParent(const SqlDB::File& file) {
    const auto& path = requiredFile(file);
    auto parent = path == path.root_path() ? std::filesystem::path() : path.parent_path();
    if (parent.empty()) throw SqlRuntimeException("java.lang.NullPointerException", "null parent file");
    std::error_code error;
    if (!invalidFile(parent)) std::filesystem::create_directories(parent, error); // File.mkdirs() result is ignored.
}
std::string url(const std::string& path, bool encrypted) {
    return "jdbc:h2:" + path + ";MODE=MySQL;DB_CLOSE_ON_EXIT=FALSE" + (encrypted ? ";CIPHER=AES" : "");
}
}
SqlDB::SqlDB(SqlDBServices services) : mServices(std::move(services)) {
    if (!mServices.connect) throw std::invalid_argument("SqlDB requires an H2-compatible native connection provider");
}
void SqlDB::log(const std::string& text, bool notify) const {
    if (mServices.log) mServices.log(text, notify);
    else std::clog << text << '\n';
}
void SqlDB::connect(const std::string& path, bool encrypted) {
    mSQLConnection = mServices.connect(url(path, encrypted), "SA",
        encrypted ? mEncryptedPassword.value_or("null") + " userpasswd" : "");
    sqlRequired(mSQLConnection).setAutoCommit(true);
}
void SqlDB::loadDB(const File& file) {
    mSQLDBNoMV = javaUnixFile(file);
    makeParent(mSQLDBNoMV);
    auto path = absolute(mSQLDBNoMV);
    mSQLFile = path + ".mv.db";
    if (mServices.startupDebugLogs()) log("SQLDB Connect.. " + url(path, false));
    connect(path, false);
    if (mServices.startupDebugLogs()) log("SQLDB Connected.. " + url(path, false));
    createSQL();
}
void SqlDB::loadEncryptedSQLDB(const File& file, const std::optional<std::string>& password) {
    mEncrypted = true;
    mEncryptedPassword = password;
    mSQLDBNoMV = javaUnixFile(file);
    makeParent(mSQLDBNoMV);
    auto path = absolute(mSQLDBNoMV);
    mSQLFile = path + ".mv.db";
    connect(path, true);
    createSQL();
}
bool SqlDB::isOpen() {
    return mSQLConnection && !mSQLConnection->isClosed();
}
bool SqlDB::checkOpen(bool logs) {
    bool reopen = !mSQLConnection || mSQLConnection->isClosed();
    if (reopen) {
        if (logs) log("SqlDB requires re-open : " + requiredFile(mSQLDBNoMV).filename().string());
        // The JVM GC hint has no native side effect; ownership is RAII.
        connect(absolute(mSQLDBNoMV), mEncrypted);
        createSQL();
    }
    return reopen;
}
void SqlDB::hardCloseDB() {
    try {
        if (!mSQLConnection || mSQLConnection->isClosed()) return;
        mSQLConnection->close();
    } catch (const std::exception& error) { log(sqlExceptionText(error)); }
}
void SqlDB::saveDB(bool compact) {
    try {
        if (!mSQLConnection) { log("Trying to saveDB on NULL SQLDB"); return; }
        if (mSQLConnection->isClosed()) return;
        auto statement = mSQLConnection->createStatement();
        sqlRequired(statement).execute(compact ? "SHUTDOWN COMPACT" : "SHUTDOWN");
        mSQLConnection->close();
    } catch (const std::exception& error) { log(sqlExceptionText(error)); }
}
void SqlDB::backupToFile(const File& file, bool gzip) {
    auto normalized = javaUnixFile(file);
    const auto& path = requiredFile(normalized);
    std::error_code error;
    if (!invalidFile(path) && std::filesystem::exists(path.empty() ? "." : path, error))
        std::filesystem::remove(path.empty() ? "." : path, error);
    auto statement = sqlRequired(mSQLConnection).createStatement();
    auto sql = "SCRIPT TO '" + absolute(normalized) + "'" + (gzip ? " COMPRESSION GZIP" : "");
    sqlRequired(statement).executeQuery(sql);
    sqlRequired(statement).close();
}
void SqlDB::restoreFromFile(const File& file, bool gzip) {
    auto statement = sqlRequired(mSQLConnection).createStatement();
    sqlRequired(statement).execute("DROP ALL OBJECTS");
    auto sql = "RUNSCRIPT FROM '" + absolute(javaUnixFile(file)) + "'" + (gzip ? " COMPRESSION GZIP" : "");
    sqlRequired(statement).execute(sql);
    sqlRequired(statement).close();
}
std::shared_ptr<json::JSONObject> SqlDB::convertDataToJSONObject(const MiniData& data) {
    auto parsed = json::parser::JSONParser().parse(MiniString(data.bytes()).toString());
    if (parsed.isNull()) return {};
    if (auto object = std::get_if<std::shared_ptr<json::JSONObject>>(&parsed.value)) return *object;
    throw SqlRuntimeException("java.lang.ClassCastException", "JSON value is not a JSONObject");
}
MiniData SqlDB::convertJSONObjectToData(const json::JSONObject* object) {
    if (!object) throw SqlRuntimeException("java.lang.NullPointerException", "null JSONObject");
    return MiniData(MiniString(object->toString()).getData());
}
json::JSONObject SqlDB::executeGenericSQL(const std::optional<std::string>& sql) {
    std::lock_guard lock(mSQLLock);
    json::JSONObject result;
    result.put("sql", sql ? json::JSONValue(*sql) : json::JSONValue());
    try {
        checkOpen();
        auto statement = sqlRequired(mSQLConnection).createStatement();
        if (sqlRequired(statement).execute(sql)) {
            auto rows = sqlRequired(statement).getResultSet();
            json::JSONArray allRows;
            auto metadata = sqlRequired(rows).getMetaData();
            int32_t columns = sqlRequired(metadata).getColumnCount();
            uint32_t count = 0;
            while (sqlRequired(rows).next()) {
                ++count;
                json::JSONObject row;
                for (int64_t index = 1; index <= columns; ++index) {
                    auto column = sqlRequired(metadata).getColumnName(int32_t(index));
                    auto object = sqlRequired(rows).getObject(int32_t(index));
                    if (object) {
                        auto className = sqlRequired(metadata).getColumnClassName(int32_t(index));
                        if (!className) throw SqlRuntimeException("java.lang.NullPointerException", "null column class name");
                        if (*className == "java.sql.Clob") {
                            auto clob = std::dynamic_pointer_cast<SqlClob>(object);
                            if (!clob) throw SqlRuntimeException("java.lang.ClassCastException", "SQL object is not a Clob");
                            auto length = std::bit_cast<int32_t>(uint32_t(clob->length()));
                            row.put(column, clob->getSubString(1, length));
                        } else row.put(column, object->toString());
                    }
                }
                allRows.add(row);
            }
            result.put("status", true);
            result.put("results", true);
            result.put("count", std::bit_cast<int32_t>(count));
            result.put("rows", allRows);
        } else {
            result.put("status", true);
            result.put("results", false);
        }
        sqlRequired(statement).close();
    } catch (const std::exception& error) {
        auto text = sqlExceptionText(error);
        log("ExecuteSQL sql:" + sql.value_or("null") + " error:" + text, false);
        result.put("status", false);
        result.put("count", 0);
        result.put("rows", json::JSONArray());
        result.put("results", false);
        result.put("error", text);
    }
    return result;
}
}
