#pragma once
#include "org/minima/objects/Coin.hpp"
namespace minima {
class GenesisCoin : public Coin {
public:
    static const MiniData GENESIS_COINID;
    GenesisCoin();
};
}
