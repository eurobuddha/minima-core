#include "GenesisMMR.hpp"
#include "GenesisCoin.hpp"
namespace minima {
GenesisMMR::GenesisMMR() {setBlockTime(MiniNumber(0));GenesisCoin coin;addEntry(MMRData::CreateMMRDataLeafNode(coin,coin.getAmount()));finalizeSet();}
}
