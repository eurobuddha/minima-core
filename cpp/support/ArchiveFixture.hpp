#pragma once
#include "org/minima/objects/TxPoW.hpp"
namespace minima::support {
inline auto archiveFixtureTxPoW(int i,int mode){auto t=std::make_shared<TxPoW>();t->setTimeMilli(MiniNumber(i*1000));t->setNonce(MiniNumber(i));t->setBlockNumber(MiniNumber(i));t->getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x12"));for(int n=0;n<32;++n)t->setSuperParent(n,MiniData::fromHex("0x00"));t->setHeaderBodyHash();if((mode&1)&&i%2==0)t->clearBody();t->calculateTXPOWID();return t;}
}
