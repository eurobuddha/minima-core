#pragma once
#include "org/minima/objects/Coin.hpp"
namespace minima::sphincs {
class TransactionTest {
public:
    static void log(const std::optional<std::string>& message);
    static std::string getOutCoinString(const Coin* coin);
    static std::string getOutCoinString(const Coin& coin) {return getOutCoinString(&coin);}
    static void main();
};
}
