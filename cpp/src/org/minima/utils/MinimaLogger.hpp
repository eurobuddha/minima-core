#pragma once
#include "org/minima/utils/json/JSONObject.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
namespace minima {
class MinimaLogger {
public:
    using String=std::optional<std::u16string>;
    struct MainReceiver {
        virtual ~MainReceiver()=default;
        virtual void PostNotifyEvent(const std::string& event,std::shared_ptr<json::JSONObject> data)=0;
    };
    // The native throwable boundary supplies the original exception text and
    // its recorded frames; the logger must not manufacture a catch-site stack.
    struct Throwable {
        enum class Kind { Exception, Error, OutOfMemory, MVStore, Timeout };
        virtual ~Throwable()=default;
        virtual String toString() const=0;
        virtual std::optional<std::vector<String>> getStackTrace() const=0;
        virtual Kind kind() const noexcept{return Kind::Exception;}
    };
    struct Runtime {
        std::function<int64_t()> totalMemory,freeMemory,currentTimeMillis;
        std::function<void(const std::u16string&)> println;
        std::function<std::shared_ptr<MainReceiver>()> getMain;
    };
    class DateFormat {
        std::u16string mZone;
    public:
        DateFormat();
        std::string format(int64_t millis) const;
    };
    inline static const std::string MINIMA_LOG="MINIMALOG";
    static const DateFormat DATEFORMAT;
    // Fill omitted platform callbacks with native defaults. The Main receiver
    // is absent before startup, just as Main.getInstance() initially is null.
    static void setRuntime(Runtime runtime);
    static Runtime getRuntime();
    static void log(const String& text,bool notify=true);
    static void log(const std::string& text,bool notify=true);
    static void log(const char* text,bool notify=true);
    static void log(const std::shared_ptr<Throwable>& error,bool notify=true);
    static void logUncaught(const std::shared_ptr<Throwable>& error,bool notify);
};
}
