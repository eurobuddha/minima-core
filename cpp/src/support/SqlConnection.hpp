#pragma once
#include "SqlRow.hpp"
#include "JavaExceptions.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace minima {
// Native driver boundary for the JDBC calls used by SqlDB. No alternate SQL
// dialect, storage format, or successful no-op driver is supplied here.
class SqlException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
class SqlRuntimeException : public std::runtime_error {
    std::string mClass;
public:
    SqlRuntimeException(std::string type, const std::string& message)
        : std::runtime_error(message), mClass(std::move(type)) {}
    const std::string& className() const { return mClass; }
};
inline std::string sqlExceptionText(const std::exception& error) {
    if (dynamic_cast<const SqlException*>(&error))
        return std::string("java.sql.SQLException: ") + error.what();
    if (auto typed = dynamic_cast<const SqlRuntimeException*>(&error))
        return typed->className() + ": " + error.what();
    return javaExceptionText(error);
}
template<class T> T& sqlRequired(const std::shared_ptr<T>& value) {
    if (!value) throw SqlRuntimeException("java.lang.NullPointerException", "null SQL reference");
    return *value;
}
class SqlObject {
public:
    virtual ~SqlObject() = default;
    virtual std::u16string toString() = 0;
};
class SqlDouble : public SqlObject {
public:
    virtual double doubleValue() const = 0;
};
class SqlClob : public SqlObject {
public:
    virtual int64_t length() = 0;
    virtual std::u16string getSubString(int64_t position, int32_t length) = 0;
};
class SqlResultSetMetaData {
public:
    virtual ~SqlResultSetMetaData() = default;
    virtual int32_t getColumnCount() = 0;
    virtual std::u16string getColumnName(int32_t column) = 0;
    virtual std::optional<std::string> getColumnClassName(int32_t column) = 0;
};
class SqlResultSet : public SqlRow {
public:
    virtual bool next() = 0;
    virtual int64_t getLong(const std::string& column) = 0;
    virtual std::optional<std::vector<uint8_t>> getBytes(const std::string& column) = 0;
    virtual std::shared_ptr<SqlResultSetMetaData> getMetaData() = 0;
    virtual std::shared_ptr<SqlObject> getObject(int32_t column) = 0;
};
class SqlStatement {
public:
    virtual ~SqlStatement() = default;
    virtual bool execute(const std::optional<std::string>& sql) = 0;
    virtual std::shared_ptr<SqlResultSet> executeQuery(const std::optional<std::string>& sql) = 0;
    virtual std::shared_ptr<SqlResultSet> getResultSet() = 0;
    virtual void close() = 0;
};
class SqlPreparedStatement : public SqlStatement {
public:
    using SqlStatement::execute;
    using SqlStatement::executeQuery;
    virtual void clearParameters() = 0;
    virtual void setString(int32_t parameter, const std::optional<std::string>& value) = 0;
    virtual void setLong(int32_t parameter, int64_t value) = 0;
    virtual void setInt(int32_t parameter, int32_t value) = 0;
    virtual void setDouble(int32_t, double) { throw SqlException("SQL driver does not support double bindings"); }
    virtual void setBytes(int32_t parameter, const std::optional<std::vector<uint8_t>>& value) = 0;
    virtual bool execute() = 0;
    virtual std::shared_ptr<SqlResultSet> executeQuery() = 0;
    virtual int32_t executeUpdate() = 0;
};
class SqlConnection {
public:
    virtual ~SqlConnection() = default;
    virtual bool isClosed() = 0;
    virtual void setAutoCommit(bool enabled) = 0;
    virtual std::shared_ptr<SqlStatement> createStatement() = 0;
    virtual std::shared_ptr<SqlPreparedStatement> prepareStatement(const std::string& sql) = 0;
    virtual void close() = 0;
};
using SqlConnect = std::function<std::shared_ptr<SqlConnection>(
    const std::string& url, const std::string& user, const std::string& password)>;
}
