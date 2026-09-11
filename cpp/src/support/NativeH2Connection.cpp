#include "NativeH2Connection.hpp"
#include "ByteIO.hpp"
#include "JavaUnicode.hpp"
#include "libminima_h2.h"
#include <bit>
#include <limits>

namespace minima {
namespace {
using String=std::optional<std::u16string>;
struct Packet {
    wire::Bytes data;
    size_t position=0;
    void putByte(uint8_t value){data.push_back(value);}
    void putInt(int32_t value){auto bits=std::bit_cast<uint32_t>(value);for(int shift=24;shift>=0;shift-=8)putByte(uint8_t(bits>>shift));}
    void putLong(int64_t value){auto bits=std::bit_cast<uint64_t>(value);for(int shift=56;shift>=0;shift-=8)putByte(uint8_t(bits>>shift));}
    void putDouble(double value){putLong(std::bit_cast<int64_t>(value));}
    static int32_t length(size_t size){if(size>size_t(INT32_MAX))throw SqlException("Native H2 argument exceeds the JDBC array limit");return int32_t(size);}
    void putString(const std::optional<std::string>& value){
        if(!value){putInt(-1);return;}auto chars=unicode::decodeUTF8(*value);putInt(length(chars.size()));
        for(char16_t c:chars){putByte(uint8_t(c>>8));putByte(uint8_t(c));}
    }
    void putBytes(const std::optional<wire::Bytes>& value){putInt(value?length(value->size()):-1);if(value)wire::append(data,*value);}
    uint8_t byte(){return wire::byte(data,position);}
    int32_t integer(){return std::bit_cast<int32_t>(wire::integer(data,position));}
    int64_t number(){uint64_t value=0;for(int i=0;i<8;++i)value=(value<<8)|byte();return std::bit_cast<int64_t>(value);}
    double real(){return std::bit_cast<double>(number());}
    String string(){
        int32_t size=integer();if(size==-1)return std::nullopt;
        if(size<0||size_t(size)>(data.size()-position)/2)throw SqlException("Invalid native H2 string response");
        std::u16string value;value.reserve(size_t(size));for(int32_t i=0;i<size;++i){uint16_t c=uint16_t(byte())<<8;c|=byte();value+=char16_t(c);}return value;
    }
    std::optional<wire::Bytes> bytes(){
        int32_t size=integer();if(size==-1)return std::nullopt;if(size<0)throw SqlException("Invalid native H2 byte response");
        auto value=wire::readFully(data,position,size_t(size));return wire::Bytes(value.begin(),value.end());
    }
};
std::u16string required(String value){if(!value)throw JavaNullPointerException("Null native H2 string result");return std::move(*value);}
std::optional<std::string> utf8(String value){return value?std::optional(unicode::encodeUTF8(*value)):std::nullopt;}
class H2SqlException final:public SqlException {
    std::string mText;
public:
    H2SqlException(const std::string& type,const std::optional<std::string>& message):SqlException(message.value_or("")),mText(type+(message?": "+*message:"")){}
    std::string toString() const override{return mText;}
};
}
class NativeH2Engine {
    graal_isolate_t* mIsolate=nullptr;
    class Thread {
        bool mAttached=false;
    public:
        graal_isolatethread_t* value=nullptr;
        explicit Thread(graal_isolate_t* isolate){
            value=graal_get_current_thread(isolate);
            if(!value){int status=graal_attach_thread(isolate,&value);if(status)throw SqlException("Cannot attach native H2 thread: "+std::to_string(status));mAttached=true;}
        }
        ~Thread(){if(mAttached)graal_detach_thread(value);}
    };
public:
    NativeH2Engine(){
        graal_isolatethread_t* thread=nullptr;
        int status=graal_create_isolate(nullptr,&mIsolate,&thread);
        if(status)throw SqlException("Cannot create native H2 isolate: "+std::to_string(status));
        graal_detach_thread(thread);
    }
    ~NativeH2Engine(){
        graal_isolatethread_t* thread=nullptr;
        if(graal_attach_thread(mIsolate,&thread)==0)graal_tear_down_isolate(thread);
    }
    Packet call(int op,int64_t handle,Packet input={}){
        Thread thread(mIsolate);
        int size=minima_h2_call(thread.value,op,handle,reinterpret_cast<char*>(input.data.data()),Packet::length(input.data.size()));
        if(size<0)throw JavaOutOfMemoryError("Native H2 could not allocate its response");
        struct Discard {graal_isolatethread_t* thread;~Discard(){minima_h2_discard(thread);}}discard{thread.value};
        Packet result;result.data.resize(size_t(size));
        if(minima_h2_copy(thread.value,reinterpret_cast<char*>(result.data.data()),size)!=size)throw SqlException("Invalid native H2 response size");
        auto category=result.byte();
        if(category){
            auto type=unicode::encodeUTF8(required(result.string()));auto message=utf8(result.string());
            if(category==1)throw H2SqlException(type,message);
            if(type=="org.h2.mvstore.MVStoreException")throw JavaMVStoreException(message.value_or(""));
            if(category==4)throw JavaOutOfMemoryError(message.value_or(""));
            if(category==3)throw JavaError(type,message.value_or(""));
            throw SqlRuntimeException(type,message.value_or(""));
        }
        return result;
    }
};
namespace {
struct Handle {
    std::shared_ptr<NativeH2Engine> engine;
    std::shared_ptr<Handle> parent;
    int64_t id;
    Handle(std::shared_ptr<NativeH2Engine> engine,int64_t id,std::shared_ptr<Handle> parent={}):engine(std::move(engine)),parent(std::move(parent)),id(id){}
    ~Handle(){try{engine->call(0,id);}catch(...){/* Destructors cannot propagate JDBC close failures. */}}
    Packet call(int op,Packet input={}) const{return engine->call(op,id,std::move(input));}
    static std::shared_ptr<Handle> adopt(const std::shared_ptr<NativeH2Engine>& engine,int64_t value,const std::shared_ptr<Handle>& owner={}){
        if(!value)return nullptr;
        try{return std::make_shared<Handle>(engine,value,owner);}catch(...){try{engine->call(0,value);}catch(...){}throw;}
    }
    std::shared_ptr<Handle> child(int64_t value,const std::shared_ptr<Handle>& owner){return adopt(engine,value,owner);}
};
Packet column(const std::string& value){Packet packet;packet.putString(value);return packet;}
Packet index(int32_t value){Packet packet;packet.putInt(value);return packet;}
class Object:public SqlObject {
    std::shared_ptr<Handle> mHandle;
public:
    explicit Object(std::shared_ptr<Handle> handle):mHandle(std::move(handle)){}
    std::u16string toString() override{return required(mHandle->call(50).string());}
};
class Double:public SqlDouble {
    std::shared_ptr<Handle> mHandle;
public:
    explicit Double(std::shared_ptr<Handle> handle):mHandle(std::move(handle)){}
    std::u16string toString() override{return required(mHandle->call(50).string());}
    double doubleValue() const override{return mHandle->call(51).real();}
};
class Clob:public SqlClob {
    std::shared_ptr<Handle> mHandle;
public:
    explicit Clob(std::shared_ptr<Handle> handle):mHandle(std::move(handle)){}
    std::u16string toString() override{return required(mHandle->call(50).string());}
    int64_t length() override{return mHandle->call(52).number();}
    std::u16string getSubString(int64_t start,int32_t length) override{Packet packet;packet.putLong(start);packet.putInt(length);return required(mHandle->call(53,std::move(packet)).string());}
};
class MetaData:public SqlResultSetMetaData {
    std::shared_ptr<Handle> mHandle;
public:
    explicit MetaData(std::shared_ptr<Handle> handle):mHandle(std::move(handle)){}
    int32_t getColumnCount() override{return mHandle->call(40).integer();}
    std::u16string getColumnName(int32_t col) override{return required(mHandle->call(41,index(col)).string());}
    std::optional<std::string> getColumnClassName(int32_t col) override{return utf8(mHandle->call(42,index(col)).string());}
};
class ResultSet:public SqlResultSet {
    std::shared_ptr<Handle> mHandle;
public:
    explicit ResultSet(std::shared_ptr<Handle> handle):mHandle(std::move(handle)){}
    bool next() override{return mHandle->call(30).byte()!=0;}
    std::optional<std::string> getString(const std::string& col) override{return utf8(mHandle->call(31,column(col)).string());}
    int32_t getInt(const std::string& col) override{return mHandle->call(32,column(col)).integer();}
    int64_t getLong(const std::string& col) override{return mHandle->call(33,column(col)).number();}
    std::optional<wire::Bytes> getBytes(const std::string& col) override{return mHandle->call(34,column(col)).bytes();}
    std::shared_ptr<SqlResultSetMetaData> getMetaData() override{
        auto handle=mHandle->child(mHandle->call(35).number(),mHandle);return handle?std::make_shared<MetaData>(handle):nullptr;
    }
    std::shared_ptr<SqlObject> getObject(int32_t col) override{
        auto result=mHandle->call(36,index(col));auto handle=mHandle->child(result.number(),mHandle);int kind=result.byte();if(!handle)return nullptr;
        if(kind==1)return std::make_shared<Double>(handle);if(kind==2)return std::make_shared<Clob>(handle);return std::make_shared<Object>(handle);
    }
};
// JDBC PreparedStatement also exposes Statement's SQL-taking overloads; H2 is
// responsible for accepting/rejecting them, not a native approximation.
template<class Base> class Statement:public Base {
protected:
    std::shared_ptr<Handle> mHandle;
    std::shared_ptr<SqlResultSet> rows(int op,Packet input={}){
        auto handle=mHandle->child(mHandle->call(op,std::move(input)).number(),mHandle);return handle?std::make_shared<ResultSet>(handle):nullptr;
    }
public:
    explicit Statement(std::shared_ptr<Handle> handle):mHandle(std::move(handle)){}
    bool execute(const std::optional<std::string>& sql) override{Packet packet;packet.putString(sql);return mHandle->call(10,std::move(packet)).byte()!=0;}
    std::shared_ptr<SqlResultSet> executeQuery(const std::optional<std::string>& sql) override{Packet packet;packet.putString(sql);return rows(11,std::move(packet));}
    std::shared_ptr<SqlResultSet> getResultSet() override{return rows(12);}
    void close() override{mHandle->call(13);}
};
class PreparedStatement final:public Statement<SqlPreparedStatement> {
public:
    using Statement<SqlPreparedStatement>::Statement;
    void clearParameters() override{mHandle->call(20);}
    void setString(int32_t i,const std::optional<std::string>& value) override{auto packet=index(i);packet.putString(value);mHandle->call(21,std::move(packet));}
    void setLong(int32_t i,int64_t value) override{auto packet=index(i);packet.putLong(value);mHandle->call(22,std::move(packet));}
    void setInt(int32_t i,int32_t value) override{auto packet=index(i);packet.putInt(value);mHandle->call(23,std::move(packet));}
    void setDouble(int32_t i,double value) override{auto packet=index(i);packet.putDouble(value);mHandle->call(24,std::move(packet));}
    void setBytes(int32_t i,const std::optional<wire::Bytes>& value) override{auto packet=index(i);packet.putBytes(value);mHandle->call(25,std::move(packet));}
    bool execute() override{return mHandle->call(26).byte()!=0;}
    std::shared_ptr<SqlResultSet> executeQuery() override{return rows(27);}
    int32_t executeUpdate() override{return mHandle->call(28).integer();}
};
class Connection:public SqlConnection {
    std::shared_ptr<Handle> mHandle;
public:
    explicit Connection(std::shared_ptr<Handle> handle):mHandle(std::move(handle)){}
    bool isClosed() override{return mHandle->call(2).byte()!=0;}
    void setAutoCommit(bool enabled) override{Packet packet;packet.putByte(enabled);mHandle->call(3,std::move(packet));}
    std::shared_ptr<SqlStatement> createStatement() override{return std::make_shared<Statement<SqlStatement>>(mHandle->child(mHandle->call(4).number(),mHandle));}
    std::shared_ptr<SqlPreparedStatement> prepareStatement(const std::string& sql) override{Packet packet;packet.putString(sql);return std::make_shared<PreparedStatement>(mHandle->child(mHandle->call(5,std::move(packet)).number(),mHandle));}
    void close() override{mHandle->call(6);}
};
}
NativeH2Runtime::NativeH2Runtime():mEngine(std::make_shared<NativeH2Engine>()){}
SqlConnect NativeH2Runtime::connector() const{
    return nullableConnector();
}
NativeH2Runtime::NullableConnect NativeH2Runtime::nullableConnector() const{
    return [engine=mEngine](const std::string& url,const std::optional<std::string>& user,const std::optional<std::string>& password)->std::shared_ptr<SqlConnection>{
        Packet packet;packet.putString(url);packet.putString(user);packet.putString(password);
        auto id=engine->call(1,0,std::move(packet)).number();return id?std::make_shared<Connection>(Handle::adopt(engine,id)):nullptr;
    };
}
std::string NativeH2Runtime::version() const{return unicode::encodeUTF8(required(mEngine->call(61,0).string()));}
int32_t NativeH2Runtime::liveHandles() const{return mEngine->call(60,0).integer();}
}
