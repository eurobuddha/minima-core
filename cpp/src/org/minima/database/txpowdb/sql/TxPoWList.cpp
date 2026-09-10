#include "TxPoWList.hpp"
#include <bit>
#include <iostream>
namespace minima {
std::vector<uint8_t> TxPoWList::serialise() const {if(!mTxPoWs)throw std::invalid_argument("Null TxPoW list");std::vector<uint8_t> out;auto count=uint32_t(mTxPoWs->size());for(int shift=24;shift>=0;shift-=8)out.push_back(uint8_t(count>>shift));for(const auto& txpow:*mTxPoWs){if(!txpow)throw std::invalid_argument("Null TxPoW");wire::append(out,txpow->serialise());}return out;}
void TxPoWList::readDataStream(std::span<const uint8_t> data,size_t& offset){mTxPoWs=std::make_shared<Rows>();auto count=std::bit_cast<int32_t>(wire::integer(data,offset));for(int i=0;i<count;++i)mTxPoWs->push_back(std::make_shared<TxPoW>(TxPoW::deserialise(data,offset)));}
std::shared_ptr<TxPoWList> TxPoWList::deserialise(std::span<const uint8_t> data,size_t& offset){auto list=std::shared_ptr<TxPoWList>(new TxPoWList);list->readDataStream(data,offset);return list;}
std::shared_ptr<TxPoWList> TxPoWList::convertMiniDataVersion(const MiniData& data){try{size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException& error){std::clog<<"TxPoWList read error: "<<error.what()<<'\n';return nullptr;}}
}
