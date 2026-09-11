#pragma once
#include "org/minima/utils/SqlDB.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <bit>
#include <map>
#include "support/JavaFloating.hpp"
#include "support/JavaUnicode.hpp"
namespace minima::sql_fixture {
using namespace json;
inline const JSONObject* config;
inline JSONArray events, logs;
inline std::map<std::string,size_t> queryBatches;
inline int calls = 0, connectionID = 0, preparedID = 0;
inline bool flag(const std::string& key) { return config->get(key).isNull() ? false : config->get(key).asBool(); }
inline int64_t integer(const JSONObject& object,const std::string& key,int64_t fallback=0) {
    return object.get(key).isNull()?fallback:std::get<int64_t>(object.get(key).value);
}
inline const JSONArray& array(const JSONValue& v) { return *std::get<std::shared_ptr<JSONArray>>(v.value); }
inline const JSONObject& object(const JSONValue& v) { return *std::get<std::shared_ptr<JSONObject>>(v.value); }
inline std::optional<std::string> string(const JSONValue& value) { return value.isNull()?std::nullopt:std::optional(value.asString()); }
inline void event(const std::string& method,std::initializer_list<JSONValue> args={}) {
    JSONArray e;e.add(method);for(const auto& a:args)e.add(a);events.add(e);
    if(++calls==integer(*config,"fail",-1)) {
        if(flag("runtimeFailure"))throw SqlRuntimeException("java.lang.IllegalStateException", "fixture failure");
        throw SqlException("fixture failure");
    }
}
class Text:public SqlObject {
    std::u16string mText;
public:
    explicit Text(std::u16string value):mText(std::move(value)){}
    std::u16string toString() override {
        try { event("object.toString"); } catch(const SqlException& error) { throw std::runtime_error(error.what()); }
        return mText;
    }
};
class Double:public SqlDouble {
    double mValue;
public:
    explicit Double(double value):mValue(value){}
    double doubleValue() const override{return mValue;}
    std::u16string toString() override{return unicode::decodeUTF8(javaFloating(mValue));}
};
class Clob:public SqlClob {
    JSONObject mCell;
public:
    explicit Clob(JSONObject cell):mCell(std::move(cell)){}
    int64_t length() override {event("clob.length");return integer(mCell,"length",int64_t(mCell.get("clob").asUTF16().size()));}
    std::u16string getSubString(int64_t start,int32_t size) override {event("clob.getSubString",{start,size});return mCell.get("clob").asUTF16();}
    std::u16string toString() override {event("object.toString");return mCell.get("text").asUTF16();}
};
class Metadata:public SqlResultSetMetaData {
    const JSONObject& column(int32_t index) {return object(array(config->get("columns")).get(index-1));}
public:
    int32_t getColumnCount() override {event("meta.getColumnCount");return int32_t(array(config->get("columns")).size());}
    std::u16string getColumnName(int32_t index) override {event("meta.getColumnName",{index});return column(index).get("name").asUTF16();}
    std::optional<std::string> getColumnClassName(int32_t index) override {event("meta.getColumnClassName",{index});return string(column(index).get("class"));}
};
class Rows:public SqlResultSet {
    int mRow=-1;
    std::string mSQL;
    std::optional<JSONArray> mBatch;
    const JSONArray& rows() const {
        if(mBatch)return *mBatch;
        if(config->containsKey("rowsBySQL")){const auto& choices=object(config->get("rowsBySQL"));if(choices.containsKey(mSQL))return array(choices.get(mSQL));}
        return array(config->get("rows"));
    }
public:
    explicit Rows(std::string sql=""):mSQL(std::move(sql)){
        if(config->containsKey("rowsBySQLBatches")){const auto& choices=object(config->get("rowsBySQLBatches"));if(choices.containsKey(mSQL)){const auto& batches=array(choices.get(mSQL));auto index=queryBatches[mSQL]++;mBatch=index<batches.size()?array(batches.get(index)):JSONArray();}}
    }
    bool next() override {event("rows.next");return ++mRow<int(rows().size());}
    std::shared_ptr<SqlResultSetMetaData> getMetaData() override {event("rows.getMetaData");return flag("nullMeta")?nullptr:std::make_shared<Metadata>();}
    std::shared_ptr<SqlObject> getObject(int32_t index) override {
        event("rows.getObject",{index});const auto& value=array(rows().get(mRow)).get(index-1);
        if(value.isNull())return {};
        if(std::holds_alternative<std::shared_ptr<JSONObject>>(value.value)){const auto& cell=object(value);if(cell.containsKey("doubleBits"))return std::make_shared<Double>(std::bit_cast<double>(std::stoull(cell.getString("doubleBits"),nullptr,16)));return std::make_shared<Clob>(cell);}
        return std::make_shared<Text>(value.asUTF16());
    }
    int32_t getInt(const std::string& column) override {event("rows.getInt",{column});return std::bit_cast<int32_t>(uint32_t(integer(object(rows().get(mRow)),column)));}
    int64_t getLong(const std::string& column) override {event("rows.getLong",{column});return integer(object(rows().get(mRow)),column);}
    std::optional<std::vector<uint8_t>> getBytes(const std::string& column) override {event("rows.getBytes",{column});auto value=string(object(rows().get(mRow)).get(column));return value?std::optional(MiniData::fromHex(*value).bytes()):std::nullopt;}
    std::optional<std::string> getString(const std::string& column) override {event("rows.getString",{column});return string(object(rows().get(mRow)).get(column));}
};
class Prepared:public SqlPreparedStatement {
    int mID=++preparedID;
    std::string mSQL;
public:
    explicit Prepared(std::string sql=""):mSQL(std::move(sql)){}
    void clearParameters() override {event("prepared.clearParameters",{mID});}
    void setString(int32_t index,const std::optional<std::string>& value) override {event("prepared.setString",{mID,index,value?JSONValue(*value):JSONValue()});}
    void setLong(int32_t index,int64_t value) override {event("prepared.setLong",{mID,index,value});}
    void setInt(int32_t index,int32_t value) override {event("prepared.setInt",{mID,index,value});}
    void setDouble(int32_t index,double value) override {event("prepared.setDouble",{mID,index,javaFloating(value)});}
    void setBytes(int32_t index,const std::optional<std::vector<uint8_t>>& value) override {event("prepared.setBytes",{mID,index,value?JSONValue(MiniData(*value).to0xString()):JSONValue()});}
    bool execute() override {event("prepared.execute",{mID});return flag("results");}
    std::shared_ptr<SqlResultSet> executeQuery() override {event("prepared.executeQuery",{mID});return flag("nullRows")?nullptr:std::make_shared<Rows>(mSQL);}
    int32_t executeUpdate() override {event("prepared.executeUpdate",{mID});return std::bit_cast<int32_t>(uint32_t(integer(*config,"updated")));}
    bool execute(const std::optional<std::string>&) override {throw std::logic_error("unused fixture method");}
    std::shared_ptr<SqlResultSet> executeQuery(const std::optional<std::string>&) override {throw std::logic_error("unused fixture method");}
    std::shared_ptr<SqlResultSet> getResultSet() override {throw std::logic_error("unused fixture method");}
    void close() override {event("prepared.close",{mID});}
};
class Statement:public SqlStatement {
public:
    bool execute(const std::optional<std::string>& sql) override {event("statement.execute",{sql?JSONValue(*sql):JSONValue()});return flag("results");}
    std::shared_ptr<SqlResultSet> executeQuery(const std::optional<std::string>& sql) override {event("statement.executeQuery",{sql?JSONValue(*sql):JSONValue()});return flag("nullRows")?nullptr:std::make_shared<Rows>();}
    std::shared_ptr<SqlResultSet> getResultSet() override {event("statement.getResultSet");return flag("nullRows")?nullptr:std::make_shared<Rows>();}
    void close() override {event("statement.close");}
};
class Connection:public SqlConnection {
    int mID=++connectionID;
    bool mClosed=false;
public:
    bool isClosed() override {event("connection.isClosed",{mID});return mClosed;}
    void setAutoCommit(bool enabled) override {event("connection.setAutoCommit",{mID,enabled});}
    std::shared_ptr<SqlStatement> createStatement() override {event("connection.createStatement",{mID});return flag("nullStatement")?nullptr:std::make_shared<Statement>();}
    std::shared_ptr<SqlPreparedStatement> prepareStatement(const std::string& sql) override {event("connection.prepareStatement",{mID,sql});return flag("nullPrepared")?nullptr:std::make_shared<Prepared>(sql);}
    void close() override {event("connection.close",{mID});mClosed=true;}
};
}
