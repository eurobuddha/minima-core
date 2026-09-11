#include "org/minima/utils/MiniFile.hpp"
#include "MegaMMR.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <iostream>
#include "support/BlockMMR.hpp"
#include "support/JavaUnixFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
namespace minima {
void MegaMMR::addBlock(const TxBlock& block) {
    applyBlockMMR(*mMMR,block,[&](const Coin& coin){mAllUnspentCoins.remove(coin.getCoinID().to0xString());},
        [&](const std::shared_ptr<Coin>& coin){mAllUnspentCoins.put(coin->getCoinID().to0xString(),coin);},
        [&](const Coin& coin){return isPrunable(coin);});
    const auto& number=block.getTxPoW().getBlockNumber();
    auto root=mMMR->getRoot();if(!root) throw std::logic_error("MegaMMR block has no root");
    if(!root->getData().isEqual(block.getTxPoW().getMMRRoot())) std::clog<<"[!] MEGAMMR ROOT AND TXBLOCK ROOT DONT MATCH @ "<<number.toString()<<'\n';
}
bool MegaMMR::isPrunable(const Coin& coin) const {
    if(!GeneralParams::MEGAMMR_MEGAPRUNE) return false;
    return coin.getAddress().getLength()!=32 || (GeneralParams::MEGAMMR_MEGAPRUNE_STATE&&!coin.getState().empty()) || (GeneralParams::MEGAMMR_MEGAPRUNE_TOKENS&&!coin.getTokenID().isEqual(Token::TOKENID_MINIMA));
}
void MegaMMR::scanUnspendable() {
    for(const auto& coin:mAllUnspentCoins.elements()) {
        auto entry=mMMR->getEntry(0,coin->getMMREntryNumber());
        if(!entry->isEmpty()) entry->getMMRData()->setUnspendable(isPrunable(*coin));else std::clog<<"[!] Coin with no MMREntry in MegaMMR! @ "<<coin->toString()<<'\n';
    }
}
void MegaMMR::pruneUnspendable(bool scanMMR) {
    if(scanMMR) scanUnspendable();mMMR->scanUnspendableTree();JavaHashtable<std::shared_ptr<Coin>> remaining;
    for(const auto& coin:mAllUnspentCoins.elements()) if(!mMMR->getPrunedUnspendableCoins().contains(coin->getMMREntryNumber().toString())) remaining.put(coin->getCoinID().to0xString(),coin);
    mAllUnspentCoins=std::move(remaining);
}
void MegaMMR::clear() {mMMR=std::make_shared<MMR>();mAllUnspentCoins=JavaHashtable<std::shared_ptr<Coin>>();}
std::vector<uint8_t> MegaMMR::serialise() {
    if(GeneralParams::MEGAMMR_MEGAPRUNE) pruneUnspendable(false);
    auto out=wire::join(MiniNumber(1),*mMMR,MiniNumber(int64_t(mAllUnspentCoins.size())));
    for(const auto& coin:mAllUnspentCoins.elements()) wire::append(out,coin->serialise());return out;
}
MegaMMR MegaMMR::deserialise(std::span<const uint8_t> data,size_t& offset) {
    MegaMMR mega;mega.readDataStream(data,offset);return mega;
}
void MegaMMR::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    (void)MiniNumber::deserialise(data,offset).getAsInt();
    mMMR=std::make_shared<MMR>();mMMR->readDataStream(data,offset);mMMR->setFinalized(false);
    mAllUnspentCoins=JavaHashtable<std::shared_ptr<Coin>>();
    int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i) {auto coin=std::make_shared<Coin>(Coin::deserialise(data,offset));mAllUnspentCoins.put(coin->getCoinID().to0xString(),coin);}
    if(GeneralParams::MEGAMMR_MEGAPRUNE) pruneUnspendable(true);else scanUnspendable();
}
MMRData MegaMMR::getCoinData(const MiniNumber& number) {return MMRData::CreateMMRDataLeafNode(Coin(MiniData::fromHex("0x00"),number,MiniData::fromHex("0x00")),number);}
}

namespace minima {
void MegaMMR::loadMMR(const std::filesystem::path& file) {std::clog<<"Loading MegaMMR size : "<<MiniFormat::formatSize(javaUnixFileLength(file))<<'\n';MiniFile::loadObjectSlow(file,*this);}
void MegaMMR::saveMMR(const std::filesystem::path& file) {MiniFile::saveObjectDirect(file,*this);}
void MegaMMR::main(const std::vector<std::string>*) {
    MegaMMR mega;auto& mmr=mega.getMMR();
    for(int i=0;i<10;++i) {
        auto address=MiniData::getRandomData(32);address=MiniData::ZERO_TXPOWID;
        auto amount=MiniNumber::ONE;
        auto coin=std::make_shared<Coin>(MiniData::getRandomData(32),address,amount,Token::TOKENID_MINIMA);
        coin->setMMREntryNumber(mmr.getEntryNumber());
        mmr.addEntry(MMRData::CreateMMRDataLeafNode(*coin,amount));
        mega.getAllCoins().put(coin->getCoinID().to0xString(),coin);
    }
    MMR::printmmrtree(&mmr);mmr.pruneTree();mega.pruneUnspendable(true);
    MMR::printmmrtree(&mmr);mega.pruneUnspendable(true);MMR::printmmrtree(&mmr);
    for(int i=0;i<10;++i) {
        auto address=MiniData::getRandomData(32);if(i>4&&i<9)address=MiniData::ZERO_TXPOWID;
        auto amount=MiniNumber::ONE;if(i<=1)amount=MiniNumber::ZERO;if(i==9)amount=MiniNumber::ZERO;
        auto coin=std::make_shared<Coin>(MiniData::getRandomData(32),address,amount,Token::TOKENID_MINIMA);
        coin->setMMREntryNumber(mmr.getEntryNumber());
        mmr.addEntry(MMRData::CreateMMRDataLeafNode(*coin,amount));
        mega.getAllCoins().put(coin->getCoinID().to0xString(),coin);
    }
    MMR::printmmrtree(&mmr);mega.pruneUnspendable(true);mmr.pruneTree();
    MMR::printmmrtree(&mmr);mega.pruneUnspendable(true);MMR::printmmrtree(&mmr);
}

}
