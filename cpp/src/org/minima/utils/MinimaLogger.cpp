#include "MinimaLogger.hpp"
#include "MiniFormat.hpp"
#include "support/JavaCalendarFields.hpp"
#include "support/JavaExceptions.hpp"
#include <bit>
#include <chrono>
#include <iostream>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(__GLIBC__)
#include <malloc.h>
#endif
namespace minima {
namespace {
std::mutex runtimeLock,outputLock;
std::shared_ptr<const MinimaLogger::Runtime> runtime;
std::pair<uint64_t,uint64_t> heapMemory(){
#if defined(__APPLE__)
    malloc_statistics_t stats{};malloc_zone_statistics(nullptr,&stats);
    return {stats.size_allocated,stats.size_allocated-stats.size_in_use};
#elif defined(__GLIBC__)
    auto stats=mallinfo2();return {uint64_t(stats.arena)+stats.hblkhd,uint64_t(stats.fordblks)};
#else
    throw std::runtime_error("Native allocator memory statistics are not bound on this platform");
#endif
}
MinimaLogger::Runtime defaults(MinimaLogger::Runtime value){
    if(!value.totalMemory)value.totalMemory=[]{return std::bit_cast<int64_t>(heapMemory().first);};
    if(!value.freeMemory)value.freeMemory=[]{return std::bit_cast<int64_t>(heapMemory().second);};
    if(!value.currentTimeMillis)value.currentTimeMillis=[]{return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();};
    if(!value.println)value.println=[](const std::u16string& text){std::lock_guard lock(outputLock);std::cout<<unicode::encodeUTF8(text)<<std::endl;};
    if(!value.getMain)value.getMain=[]{return std::shared_ptr<MinimaLogger::MainReceiver>{};};
    if(!value.collectMemory)value.collectMemory=[]{};
    return value;
}
}
const MinimaLogger::DateFormat MinimaLogger::DATEFORMAT;
MinimaLogger::DateFormat::DateFormat(){
    UErrorCode error=U_ZERO_ERROR;int32_t count=ucal_getDefaultTimeZone(nullptr,0,&error);
    if(error!=U_BUFFER_OVERFLOW_ERROR&&U_FAILURE(error))throw std::runtime_error(u_errorName(error));
    error=U_ZERO_ERROR;mZone.resize(size_t(count));ucal_getDefaultTimeZone(reinterpret_cast<UChar*>(mZone.data()),count,&error);
    if(U_FAILURE(error))throw std::runtime_error(u_errorName(error));
}
std::string MinimaLogger::DateFormat::format(int64_t millis) const {
    auto date=javaCalendarFields(millis,&mZone);std::ostringstream output;output.imbue(std::locale::classic());
    output<<std::setfill('0')<<std::setw(2)<<date.day<<'/'<<std::setw(2)<<date.month<<'/'<<std::setw(4)<<date.year<<' '<<std::setw(2)<<date.local.tm_hour<<':'<<std::setw(2)<<date.local.tm_min<<':'<<std::setw(2)<<date.local.tm_sec;return output.str();
}
void MinimaLogger::setRuntime(Runtime value){
    auto replacement=std::make_shared<const Runtime>(defaults(std::move(value)));
    {std::lock_guard lock(runtimeLock);runtime.swap(replacement);}
    // Releasing a receiver may itself log. Never run its destructor under the
    // registry lock, and never copy user callbacks while holding that lock.
}
MinimaLogger::Runtime MinimaLogger::getRuntime(){
    std::shared_ptr<const Runtime> held;{std::lock_guard lock(runtimeLock);held=runtime;}
    return held?*held:defaults({});
}
void MinimaLogger::log(const String& text,bool notify){
    auto services=getRuntime();auto total=services.totalMemory();auto free=services.freeMemory();auto used=std::bit_cast<int64_t>(uint64_t(total)-uint64_t(free));
    auto date=DATEFORMAT.format(services.currentTimeMillis());auto full=unicode::decodeUTF8("Minima @ "+date+" ["+MiniFormat::formatSize(used)+"] : ")+text.value_or(u"null");
    services.println(full);
    if(notify&&services.getMain()){
        auto data=std::make_shared<json::JSONObject>();data->put("message",full);
        auto main=services.getMain();if(!main)throw JavaNullPointerException("null Main");main->PostNotifyEvent(MINIMA_LOG,std::move(data));
    }
}
void MinimaLogger::log(const std::string& text,bool notify){log(String(unicode::decodeUTF8(text)),notify);}
void MinimaLogger::log(const char* text,bool notify){log(text?String(unicode::decodeUTF8(text)):std::nullopt,notify);}
void MinimaLogger::log(const std::shared_ptr<Throwable>& error,bool notify){
    if(!error)throw JavaNullPointerException("null exception");log(error->toString(),notify);
    auto frames=error->getStackTrace();if(!frames)throw JavaNullPointerException("null stack trace");
    for(const auto& frame:*frames){if(!frame)throw JavaNullPointerException("null StackTraceElement");log(String(u"     "+*frame),notify);}
}
void MinimaLogger::logUncaught(const std::shared_ptr<Throwable>& error,bool notify){
    if(!error)throw JavaNullPointerException("null throwable");log(String(u"[!] UNCAUGHT EXCEPTION : "+error->toString().value_or(u"null")),notify);
    auto frames=error->getStackTrace();if(!frames)throw JavaNullPointerException("null stack trace");
    for(const auto& frame:*frames){if(!frame)throw JavaNullPointerException("null StackTraceElement");log(String(u"     "+*frame),notify);}
}
}
