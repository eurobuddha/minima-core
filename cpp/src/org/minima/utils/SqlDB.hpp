#pragma once
#include "support/SqlConnection.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include <filesystem>
#include <mutex>

namespace minima {
struct SqlDBServices {
    SqlConnect connect;
    std::function<bool()> startupDebugLogs = [] { return false; };
    std::function<void(const std::string&, bool)> log;
};
class SqlDB {
public:
    using File = std::optional<std::filesystem::path>;
    explicit SqlDB(SqlDBServices services);
    virtual ~SqlDB() = default;
    virtual File getSQLFile() const { return mSQLFile; }
    virtual void loadDB(const File& file);
    virtual void loadEncryptedSQLDB(const File& file, const std::optional<std::string>& password);
    virtual bool isOpen();
    virtual bool checkOpen() { return checkOpen(true); }
    virtual bool checkOpen(bool logs);
    virtual void hardCloseDB();
    virtual void saveDB(bool compact);
    virtual void backupToFile(const File& file) { backupToFile(file, false); }
    virtual void backupToFile(const File& file, bool gzip);
    virtual void restoreFromFile(const File& file) { restoreFromFile(file, false); }
    virtual void restoreFromFile(const File& file, bool gzip);
    static std::shared_ptr<json::JSONObject> convertDataToJSONObject(const MiniData& data);
    static MiniData convertJSONObjectToData(const json::JSONObject* object);
    static MiniData convertJSONObjectToData(const json::JSONObject& object) {
        return convertJSONObjectToData(&object);
    }
    virtual json::JSONObject executeGenericSQL(const std::optional<std::string>& sql);
protected:
    std::shared_ptr<SqlConnection> mSQLConnection;
    File mSQLFile, mSQLDBNoMV;
    // Java's synchronized subclass methods share the same reentrant monitor.
    std::recursive_mutex mSQLLock;
    virtual void createSQL() = 0;
    void log(const std::string& text, bool notify = true) const;
private:
    SqlDBServices mServices;
    bool mEncrypted = false;
    std::optional<std::string> mEncryptedPassword = std::string();
    void connect(const std::string& path, bool encrypted);
};
}
