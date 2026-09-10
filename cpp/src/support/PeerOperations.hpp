#pragma once
#include "org/minima/system/network/p2p/P2PFunctions.hpp"
#include "org/minima/system/network/p2p/P2PState.hpp"
namespace minima::p2p::peer_detail {
template<class T>T& required(const std::shared_ptr<T>& value){if(!value)throw std::runtime_error("Null peer receiver");return *value;}
inline void put(const PeerMapPtr& map,const std::optional<std::string>& uid,SocketAddressPtr address){if(uid)required(map).put(*uid,std::move(address));else required(map).put(nullptr,std::move(address));}
inline void remove(const PeerMapPtr& map,const std::optional<std::string>& uid){if(uid)required(map).remove(*uid);else required(map).remove(nullptr);}
inline SocketAddressList values(const PeerMapPtr& map){return std::make_shared<std::vector<SocketAddressPtr>>(required(map).elements());}
inline std::shared_ptr<Message> send(const std::optional<std::string>& uid,json::JSONObject data){auto message=std::make_shared<Message>("P2P_SEND_MSG");message->addString("uid",uid).addObject("json",MessageObject(std::make_shared<json::JSONObject>(std::move(data))));return message;}
inline int32_t count(const PeerMapPtr& map){return int32_t(required(map).size());}
inline SocketAddressPtr get(const PeerMapPtr& map,const std::optional<std::string>& uid){auto value=uid?required(map).get(*uid):required(map).get(nullptr);return value.value_or(nullptr);}
}
