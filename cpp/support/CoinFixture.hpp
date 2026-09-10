#pragma once
#include "org/minima/objects/Coin.hpp"
#include "org/minima/utils/BaseConverter.hpp"
namespace minima::support {
inline Coin coinFixture(int n) {
    Coin c(MiniData::fromHex(BaseConverter::numberToHex(n+1)),MiniData::fromHex("0x001122"),MiniNumber("0.125"),MiniData::fromHex("0x00"),n%2==0);
    BigInt entry(1);for(int i=0;i<n;++i) entry=entry*BigInt(2);c.setMMREntryNumber(MMREntryNumber(entry));
    c.setSpent(n%3==0);c.setBlockCreated(MiniNumber(n*123));
    std::vector<StateVariable> states;for(int i=0;i<n%5;++i) states.emplace_back(i%3,i%2==0?"[state 🐼]":"1.00");c.setState(std::move(states));
    if(n%2==1) {Token token(MiniData::fromHex("0xFF"),MiniNumber(3),MiniNumber("0.001"),MiniString("Test"),MiniString("RETURN TRUE"));c.setToken(token);c.resetTokenID(token.getTokenID().value());}
    return c;
}
}
