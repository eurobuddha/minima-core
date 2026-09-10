#include "GenesisCoin.hpp"
#include "org/minima/objects/Address.hpp"
namespace minima {
const MiniData GenesisCoin::GENESIS_COINID=MiniData::fromHex("0x5350415254414355534C4F5645534D494E494D41");
GenesisCoin::GenesisCoin():Coin(GENESIS_COINID,Address::TRUE_ADDRESS.getAddressData(),MiniNumber::BILLION,Token::TOKENID_MINIMA){}
}
