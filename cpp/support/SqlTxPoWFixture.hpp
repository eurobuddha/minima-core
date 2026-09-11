#pragma once
#include "SqlRecordingFixture.hpp"
#include "ArchiveFixture.hpp"
namespace minima::sql_fixture {
inline auto transaction(int i){auto t=std::make_shared<TxPoW>();t->setTimeMilli(MiniNumber(0));t->setNonce(MiniNumber(i));t->getTxBody()->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x12"));if(i%5!=0){Coin c(MiniData(BigInt(i+1)),MiniData::fromHex("0x1122"),MiniNumber(i%13+1),Token::TOKENID_MINIMA);t->getTransaction().addInput(c);t->getWitness().addCoinProof(CoinProof(c,MMRProof{}));}t->setHeaderBodyHash();t->calculateTXPOWID();return t;}
inline std::shared_ptr<TxPoW> fixture(){
    auto txpow=flag("transaction")?transaction(int(integer(*config,"index"))):support::archiveFixtureTxPoW(int(integer(*config,"index")),int(integer(*config,"mode")));
    if(!config->get("state").isNull()){txpow->getTransaction().addStateVariable(StateVariable(2,config->get("state").asString()));txpow->setHeaderBodyHash();txpow->calculateTXPOWID();}
    if(flag("invalidHash"))txpow->setSuperParent(0,MiniData(std::vector<uint8_t>(65)));return txpow;
}
inline JSONValue render(const std::shared_ptr<TxPoW>& txpow){
    if(!txpow)return {};JSONObject result;result.put("wire",MiniData(txpow->serialise()).to0xString());result.put("id",txpow->getTxPoWID());result.put("isblock",txpow->isBlock());result.put("istransaction",txpow->isTransaction());return result;
}
inline JSONValue render(const std::vector<std::shared_ptr<TxPoW>>& txpows){JSONArray result;for(const auto& txpow:txpows)result.add(render(txpow));return result;}
}
