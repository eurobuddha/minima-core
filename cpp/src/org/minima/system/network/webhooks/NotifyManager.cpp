#include "NotifyManager.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/utils/RPCClient.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaIOExceptionText.hpp"
#include "support/JavaUnicode.hpp"
#include <algorithm>
namespace minima {
namespace {
std::string errorText(const std::exception& error){
    if(auto io=dynamic_cast<const wire::IOException*>(&error))return javaIOExceptionText(*io);
    return javaExceptionText(error);
}
}
NotifyManager::NotifyManager(NotifyManagerServices services)
    :MessageProcessor("NOTIFYMANAGER",services.releaseLocks),mServices(std::move(services)) {
    if(!mServices.getListener)
        throw std::invalid_argument("NotifyManager requires a listener receiver");
    if(!mServices.log)mServices.log=[](const auto& text,bool notify){MinimaLogger::log(text,notify);};
    if(!mServices.sendPOST)mServices.sendPOST=[](const auto& url,const auto& body){return RPCClient::sendPOST(url,body);};
    if(!mServices.getUserDB)mServices.getUserDB=[]{auto db=MinimaDB::getDB();if(!db)throw JavaNullPointerException("null MinimaDB");return db->getUserDB();};
    mHooks=userDB()->getWebHooksUTF16();
}
std::shared_ptr<UserDB> NotifyManager::userDB() const {
    auto db=mServices.getUserDB();if(!db)throw JavaNullPointerException("null UserDB");return db;
}
std::shared_ptr<NotifyManager> NotifyManager::create(NotifyManagerServices services){
    auto result=std::shared_ptr<NotifyManager>(new NotifyManager(std::move(services)));
    result->startMessageProcessor();return result;
}
void NotifyManager::PostEvent(std::shared_ptr<json::JSONObject> event){
    auto message=std::make_shared<Message>(NOTIFY_POST);message->addObject("notify",MessageObject(std::move(event)));PostMessage(message);
}
MiniUtil::UTF16StringList NotifyManager::getAllWebHooks() const {
    std::lock_guard lock(mHooksLock);MiniUtil::UTF16StringList result;
    for(const auto& hook:mHooks){if(!hook)throw JavaNullPointerException("null hook");result.push_back(*hook);}return result;
}
void NotifyManager::addHook(const std::optional<std::string>& hook){addHookUTF16(hook?std::optional(unicode::decodeUTF8(*hook)):std::nullopt);}
void NotifyManager::removeHook(const std::optional<std::string>& hook){removeHookUTF16(hook?std::optional(unicode::decodeUTF8(*hook)):std::nullopt);}
void NotifyManager::addHookUTF16(const std::optional<std::u16string>& hook){
    std::lock_guard lock(mHooksLock);
    if(std::find(mHooks.begin(),mHooks.end(),hook)==mHooks.end()){if(!hook)throw JavaNullPointerException("null hook");if(!hook->empty())mHooks.push_back(hook);}
    userDB()->setWebHooksUTF16(mHooks);
}
void NotifyManager::removeHookUTF16(const std::optional<std::u16string>& hook){
    std::lock_guard lock(mHooksLock);auto found=std::find(mHooks.begin(),mHooks.end(),hook);
    if(found!=mHooks.end())mHooks.erase(found);userDB()->setWebHooksUTF16(mHooks);
}
void NotifyManager::clearHooks(){std::lock_guard lock(mHooksLock);mHooks.clear();userDB()->setWebHooksUTF16(mHooks);}
void NotifyManager::processMessage(std::shared_ptr<Message> message){
    if(!message)throw JavaNullPointerException("null message");
    if(!message->isMessageType(NOTIFY_POST))return;
    auto value=message->getObject("notify");if(value.isNull())throw JavaNullPointerException("null notification");
    std::shared_ptr<json::JSONObject> notify;
    try{notify=value.as<std::shared_ptr<json::JSONObject>>();}catch(const std::bad_any_cast&){throw JavaClassCastException("Cannot cast notification to JSONObject");}
    const auto& eventValue=notify->get("event");std::optional<std::u16string> event;
    if(!eventValue.isNull()){if(!eventValue.isString())throw JavaClassCastException("Cannot cast notification event to String");event=eventValue.asUTF16();}
    auto listener=mServices.getListener();
    if(listener){try{listener->processMessage(message);}catch(const std::exception& error){mServices.log(errorText(error)+" : "+message->toString(),true);}}
    auto postmsg=notify->toString();auto hooks=getAllWebHooks();
    for(const auto& hook:hooks){
        const auto& text=*hook;auto index=text.find(u'#');if(index==text.npos)return;
        auto filter=text.substr(0,index);auto webhook=unicode::encodeUTF8(text.substr(index+1));
        if(!isRunning())return;
        try{
            if(!filter.empty()&&!event)throw JavaNullPointerException("null event");
            if(filter.empty()||event->find(filter)!=event->npos)mServices.sendPOST(webhook,postmsg);
        }catch(const std::exception& error){if(WEBHOOKS_ERROR_LOGS)mServices.log("ERROR webhook : "+unicode::encodeUTF8(*hook)+" "+errorText(error),false);}
    }
}
}
