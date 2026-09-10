#include "GenesisTxPoW.hpp"
#include "GenesisCoin.hpp"
#include "GenesisMMR.hpp"
#include "org/minima/objects/Address.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/BlockMMR.hpp"
#include <iostream>
namespace minima {
GenesisTxPoW::GenesisTxPoW(const std::string& address) {
    auto genesis=std::make_shared<GenesisMMR>();setTxDifficulty(Crypto::MAX_HASH);setNonce(MiniNumber(256));setTimeMilli();setBlockNumber(MiniNumber(1));setBlockDifficulty(Crypto::MAX_HASH);
    for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i) setSuperParent(i,MiniData::fromHex("0x00"));
    getTransaction().addInput(GenesisCoin());getTransaction().addOutput(Coin(Coin::COINID_OUTPUT,MiniData::fromHex(address),MiniNumber::BILLION,Token::TOKENID_MINIMA));
    getWitness().addCoinProof(CoinProof(GenesisCoin(),genesis->getProofToPeak(MMREntryNumber(0))));getWitness().addScript(ScriptProof(Address::TRUE_ADDRESS.getScript()));
    setHeaderBodyHash();calculateTransactionID();TxBlock block(genesis,*this,{});auto mmr=constructBlockMMR(block);auto root=mmr.getRoot();setMMRRoot(root->getData());setMMRTotal(root->getValue());calculateTXPOWID();
    std::clog<<"Genesis block created : "<<getTxPoWID()<<'\n';mIsBlockPOW=true;mIsTxnPOW=true;
}
}
