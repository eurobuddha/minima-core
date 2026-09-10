#pragma once
#include "org/minima/objects/TxPoW.hpp"
namespace minima {
class GenesisTxPoW : public TxPoW {
public:
    explicit GenesisTxPoW(const std::string& genesisAddress);
};
}
