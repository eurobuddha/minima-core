#pragma once
#include "org/minima/objects/base/MiniByte.hpp"
#include <array>
namespace minima {
// Pinned NIOMessage.convertMessageType shared with the pending NIOMessage port.
inline std::string nioMessageType(const MiniByte& type){
    static constexpr std::array<const char*,24> names={"GREETING","IBD","TXPOWID","TXPOWREQ","TXPOW","GENMESSAGE","PULSE","P2P","PING","MAXIMA_CTRL","MAXIMA","","MSG_SINGLE_PONG","MSG_IBD_REQ","MSG_IBD_RESP","MSG_ARCHIVE_REQ","MSG_ARCHIVE_DATA","MSG_ARCHIVE_SINGLE_REQ","TXBLOCKID","TXBLOCKREQ","TXBLOCK","TXBLOCKMINE","MSG_MEGAMMRSYNC_REQ","MSG_MEGAMMRSYNC_RESP"};
    auto value=type.getValue();if(value<int(names.size())&&names[size_t(value)][0])return names[size_t(value)];return "UNKNOWN_"+type.toString();
}
}
