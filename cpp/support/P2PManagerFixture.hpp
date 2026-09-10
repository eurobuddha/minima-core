#pragma once
#include "PeerRuntimeFixture.hpp"
#include "org/minima/system/network/p2p/P2PManager.hpp"
class IdleChecker : public P2PPeersChecker {
    IdleChecker(const std::shared_ptr<MessageStack>& parent):P2PPeersChecker(parent,[]{return std::shared_ptr<TxPoWTreeNode>{};},[]{}){}
public:
    static std::shared_ptr<IdleChecker> create(const std::shared_ptr<MessageStack>& parent){return std::shared_ptr<IdleChecker>(new IdleChecker(parent));}
    using MessageStack::getNextMessage;
};
namespace minima::p2p {
class P2PManagerFixture : public P2PManager {
    P2PManagerFixture(P2PManagerServices services):P2PManager(std::move(services)){}
    std::shared_ptr<P2PPeersChecker> createPeersChecker()override{return IdleChecker::create(shared_from_this());}
public:
    std::vector<std::shared_ptr<TimerMessage>> timers;
    static std::shared_ptr<P2PManagerFixture> create(P2PManagerServices services){auto value=std::shared_ptr<P2PManagerFixture>(new P2PManagerFixture(std::move(services)));value->initialize();return value;}
    void PostTimerMessage(const std::shared_ptr<TimerMessage>& timer)override{timers.push_back(timer);}
    void process(const std::shared_ptr<Message>& message){processMessage(message);}
    auto peerState(){return state;}
    void send(const Messages& messages){sendMessages(messages);}
    void first(bool value){firstgo=value;}
    JSONArray queued(){std::lock_guard lock(mLock);JSONArray rows;for(const auto& message:mMessages)rows.add(message?nullable(message->getMessageType()):JSONValue{});return rows;}
    static void seed(uint64_t value){rand.setSeed(value);}
    using MessageStack::getNextMessage;
};
}
