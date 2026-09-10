#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include <atomic>
#include <bit>
namespace minima {
// java.util.Random's specified 48-bit LCG and bounded-int rejection rule.
class JavaRandom48 {
    static constexpr uint64_t multiplier=0x5deece66dULL,mask=(uint64_t(1)<<48)-1;
    std::atomic<uint64_t> mSeed;
    uint32_t next31(){uint64_t previous=mSeed.load(),next;do{next=(previous*multiplier+11)&mask;}while(!mSeed.compare_exchange_weak(previous,next));return uint32_t(next>>17);}
public:
    JavaRandom48():JavaRandom48([]{auto data=MiniData::getRandomData(8);uint64_t seed=0;for(uint8_t byte:data.bytes())seed=(seed<<8)|byte;return seed;}()){}
    explicit JavaRandom48(uint64_t seed):mSeed((seed^multiplier)&mask){}
    void setSeed(uint64_t seed){mSeed=(seed^multiplier)&mask;}
    int32_t nextInt(int32_t bound){if(bound<=0)throw std::invalid_argument("bound must be positive");uint32_t b=uint32_t(bound),r=next31(),m=b-1;if((b&m)==0)return int32_t((uint64_t(b)*r)>>31);uint32_t u=r;while(std::bit_cast<int32_t>(u-(r=u%b)+m)<0)u=next31();return int32_t(r);}
};
}
