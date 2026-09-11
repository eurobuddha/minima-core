#include "SendPollManager.hpp"
namespace minima {
SendPollManager::SendPollManager(std::function<json::JSONObject(const std::optional<std::string>&)> runCommand,std::function<void()> releaseLocks)
    :MessageProcessor("SENDPOLL_MANAGER",std::move(releaseLocks)),mRunCommand(std::move(runCommand)) {
    if(!mRunCommand)throw std::invalid_argument("SendPollManager requires CommandRunner binding");
}
std::shared_ptr<SendPollManager> SendPollManager::create(std::function<json::JSONObject(const std::optional<std::string>&)> runCommand,std::function<void()> releaseLocks) {
    auto manager=std::shared_ptr<SendPollManager>(new SendPollManager(std::move(runCommand),std::move(releaseLocks)));
    manager->schedulePoll();manager->startMessageProcessor();return manager;
}
void SendPollManager::addSendCommand(std::optional<std::string> command) {
    std::lock_guard lock(mSyncObject);mSendCommands.emplace_back(std::move(command));
}
std::vector<SendPollMessage> SendPollManager::listCommands() {
    std::lock_guard lock(mSyncObject);std::vector<SendPollMessage> copy;
    for(const auto& command:mSendCommands)copy.push_back(command.copy());return copy;
}
void SendPollManager::removeCommand(const std::optional<std::string>& uid) {
    std::lock_guard lock(mSyncObject);std::vector<SendPollMessage> copy;
    for(const auto& command:mSendCommands){if(!command.getUID())throw std::invalid_argument("Null command UID");if(command.getUID()!=uid)copy.push_back(command);}
    mSendCommands=std::move(copy);
}
void SendPollManager::pauseBetweenCommands() {nativeThreadSleep(1000);}
void SendPollManager::schedulePoll() {PostTimerMessage(std::make_shared<TimerMessage>(SENDPOLL_TIMER.load(),SENDPOLL_FUNCTION));}
void SendPollManager::processMessage(std::shared_ptr<Message> message) {
    if(!message||!message->getMessageType())throw std::invalid_argument("Null poll message type");
    if(*message->getMessageType()!=SENDPOLL_FUNCTION)return;
    auto commands=listCommands();
    for(const auto& command:commands){
        if(!isRunning())break;
        auto result=mRunCommand(command.getCommand());
        if(result.get("status").asBool())removeCommand(command.getUID());
        pauseBetweenCommands();
    }
    schedulePoll();
}
}
