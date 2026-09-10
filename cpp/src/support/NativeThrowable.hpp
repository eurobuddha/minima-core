#pragma once
#include "JavaExceptions.hpp"
#include "JavaIOExceptionText.hpp"
#include "SqlConnection.hpp"
#include "JavaUnicode.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/utils/json/parser/ParseException.hpp"
#include <ostream>

namespace minima {
inline std::string nativeExceptionText(const std::exception& error) {
    if(dynamic_cast<const SqlException*>(&error)||dynamic_cast<const SqlRuntimeException*>(&error))
        return sqlExceptionText(error);
    if(auto io=dynamic_cast<const wire::IOException*>(&error))return javaIOExceptionText(*io);
    if(auto parse=dynamic_cast<const json::parser::ParseException*>(&error))
        return "org.minima.utils.json.parser.ParseException: "+parse->getMessage();
    return javaExceptionText(error);
}
class NativeThrowable final:public MinimaLogger::Throwable {
    using String=MinimaLogger::String;
    String mText;
    std::vector<String> mFrames;
    Kind mKind=Kind::Exception;
public:
    explicit NativeThrowable(const std::exception& error):mText(unicode::decodeUTF8(nativeExceptionText(error))) {
        if(dynamic_cast<const JavaOutOfMemoryError*>(&error)||dynamic_cast<const std::bad_alloc*>(&error))mKind=Kind::OutOfMemory;
        else if(dynamic_cast<const JavaMVStoreException*>(&error))mKind=Kind::MVStore;
        else if(dynamic_cast<const JavaTimeoutException*>(&error))mKind=Kind::Timeout;
        else if(dynamic_cast<const JavaError*>(&error))mKind=Kind::Error;
        if(auto trace=dynamic_cast<const NativeStackTrace*>(&error))
            for(const auto& frame:trace->nativeStackTrace())mFrames.emplace_back(unicode::decodeUTF8(frame));
    }
    String toString() const override{return mText;}
    Kind kind() const noexcept override{return mKind;}
    std::optional<std::vector<String>> getStackTrace() const override{return mFrames;}
    void printStackTrace(std::ostream& output) const {
        output<<unicode::encodeUTF8(*mText)<<'\n';
        for(const auto& frame:mFrames)output<<"\tat "<<unicode::encodeUTF8(*frame)<<'\n';
        output.flush();
    }
};
}
