#pragma once
#include "InetSocketAddress.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include <algorithm>
namespace minima::p2p {
// Extracted from P2PGreeting: Collections.shuffle's independently seeded
// permutation is compared as a permutation, not by coincidental random choices.
inline void shufflePeers(const SocketAddressList& peers){
    if(!peers)throw std::runtime_error("Null peer list");
    for(size_t count=peers->size();count>1;--count){uint64_t bound=count,threshold=(uint64_t(0)-bound)%bound,random;do{random=0;auto entropy=MiniData::getRandomData(8);for(auto byte:entropy.bytes())random=(random<<8)|byte;}while(random<threshold);std::swap(peers->at(count-1),peers->at(random%bound));}
}
}
