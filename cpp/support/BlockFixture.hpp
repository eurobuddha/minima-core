#pragma once
#include "CoinFixture.hpp"
#include "org/minima/objects/TxBlock.hpp"
namespace minima::support {
inline TxPoW fixedPow(int nonce) {TxPoW t;t.setTimeMilli(MiniNumber(1788883200000LL));t.setNonce(MiniNumber(nonce));t.getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x000102"));return t;}
inline TxBlock blockFixture(int size,int flags,int time) {
    auto parent=std::make_shared<MMR>();parent->setBlockTime(MiniNumber(time));std::vector<Coin> coins;
    for(int i=0;i<size;++i) {auto c=coinFixture(i);c.setSpent(false);c.setMMREntryNumber(parent->getEntryNumber());parent->addEntry(MMRData::CreateMMRDataLeafNode(c,c.getAmount()));coins.push_back(c);}
    std::vector<TxPoW> txns;
    for(int i=0;i<3;++i) {
        auto t=fixedPow(i);bool burn=(flags&4)&&i==1;auto& txn=burn?t.getBurnTransaction():t.getTransaction();auto& witness=burn?t.getBurnWitness():t.getWitness();
        const auto& input=coins.at(size_t(i%size));txn.addInput(input);if(!(flags&8)) witness.addCoinProof(CoinProof(input,parent->getProof(input.getMMREntryNumber())));
        for(int j=0;j<2;++j) {Coin c(MiniData::fromHex("0x1122"),MiniNumber("0.0125"),Token::TOKENID_MINIMA,!(flags&1)||j==0);if((flags&2)&&j==1) {c.resetTokenID(Token::TOKENID_CREATE);c.setToken(Token(MiniData::fromHex("0x01"),MiniNumber(3),c.getAmount(),MiniString("Created"),MiniString("RETURN TRUE")));}txn.addOutput(c);}
        txn.addStateVariable(StateVariable(0,"[transaction "+std::to_string(i)+"]"));t.setHeaderBodyHash();t.calculateTXPOWID();txns.push_back(std::move(t));
    }
    auto main=txns.front();main.setBlockNumber(MiniNumber(time+1));main.addBlockTxPOW(txns[2].getTxPoWIDData());main.addBlockTxPOW(txns[1].getTxPoWIDData());main.setHeaderBodyHash();main.calculateTXPOWID();
    if(flags&16) txns.erase(txns.begin()+1);return TxBlock(parent,std::move(main),txns);
}
}
