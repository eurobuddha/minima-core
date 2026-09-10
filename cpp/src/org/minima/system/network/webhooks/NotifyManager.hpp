#pragma once
#include "org/minima/database/userprefs/UserDB.hpp"
#include "org/minima/utils/messages/MessageProcessor.hpp"
#include "org/minima/utils/messages/MessageListener.hpp"
namespace minima {
struct NotifyManagerServices {
    std::function<void()> releaseLocks;
    std::function<std::shared_ptr<UserDB>()> getUserDB;
    std::function<std::shared_ptr<MessageListener>()> getListener;
    std::function<std::string(const std::string&,const std::string&)> sendPOST;
    std::function<void(const std::string&,bool)> log;
};
class NotifyManager : public MessageProcessor {
    NotifyManagerServices mServices;
    mutable std::recursive_mutex mHooksLock;
    MiniUtil::UTF16StringList mHooks;
    std::shared_ptr<UserDB> userDB() const;
protected:
    explicit NotifyManager(NotifyManagerServices services);
    void processMessage(std::shared_ptr<Message> message) override;
public:
    inline static const std::string NOTIFY_POST="NOTIFY_POST";
    std::atomic<bool> WEBHOOKS_ERROR_LOGS{false};
    static std::shared_ptr<NotifyManager> create(NotifyManagerServices services);
    void shutDown(){stopMessageProcessor();}
    void PostEvent(std::shared_ptr<json::JSONObject> event);
    MiniUtil::UTF16StringList getAllWebHooks() const;
    void addHook(const std::optional<std::string>& hook);
    void removeHook(const std::optional<std::string>& hook);
    void addHookUTF16(const std::optional<std::u16string>& hook);
    void removeHookUTF16(const std::optional<std::u16string>& hook);
    void clearHooks();
};
}
