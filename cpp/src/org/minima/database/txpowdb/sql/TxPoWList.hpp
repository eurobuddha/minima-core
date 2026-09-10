#pragma once
#include "org/minima/objects/TxPoW.hpp"
namespace minima {
class TxPoWList {
    TxPoWList()=default;
public:
    using Rows=std::vector<std::shared_ptr<TxPoW>>;
    std::shared_ptr<Rows> mTxPoWs=std::make_shared<Rows>();
    explicit TxPoWList(std::shared_ptr<Rows> rows):mTxPoWs(std::move(rows)){}
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static std::shared_ptr<TxPoWList> deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::shared_ptr<TxPoWList> convertMiniDataVersion(const MiniData& data);
};
}
