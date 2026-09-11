#pragma once
#include "../Command.hpp"
#include "org/minima/database/archive/ArchiveManager.hpp"
#include "org/minima/database/archive/RawArchiveInput.hpp"
#include "org/minima/utils/messages/MessageListener.hpp"
namespace minima::commands {
class archive:public Command {
    friend struct ArchiveCommandFixture;
    const std::string LOCAL_ARCHIVE="archiverestore";
    inline static std::atomic<bool> H2_TEMPARCHIVE{true};
    inline static std::shared_ptr<ArchiveManager> STATIC_TEMPARCHIVE;
    inline static std::shared_ptr<RawArchiveInput> STATIC_RAW;
    inline static std::mutex mSourceLock;
    template<class T> static std::shared_ptr<T> source(const std::shared_ptr<T>& value){std::lock_guard lock(mSourceLock);return value;}
    template<class T> static void replaceSource(std::shared_ptr<T>& field,std::shared_ptr<T> value){std::lock_guard lock(mSourceLock);field.swap(value);}
public:
    archive();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"action","host","phrase","anyphrase","keys","keyuses","file","address","statecheck","logs","maxexport"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<archive>();}
    static void NotifyListener(const std::shared_ptr<MessageListener>& listener,const std::optional<std::string>& message);
    static std::shared_ptr<IBD> sendArchiveReq(const std::optional<std::string>& host,int32_t port,const MiniNumber* start,int32_t attempts);
    static std::shared_ptr<IBD> sendArchiveReq(const std::optional<std::string>& host,int32_t port,const MiniNumber& start,int32_t attempts=3){return sendArchiveReq(host,port,&start,attempts);}
    static std::shared_ptr<IBD> sendArchiveReq(const std::optional<std::string>& host,int32_t port,const MiniNumber* start){return sendArchiveReq(host,port,start,3);}
};
}
