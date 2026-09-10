// Adapted from upstream test/org/minima/objects/TxHeaderTests.java.
#include "org/minima/objects/TxHeader.hpp"
#include <stdexcept>
using namespace minima;
int main() {
    TxHeader header;header.mTxBodyHash=Crypto::getInstance().hashObject(MiniData::fromHex("0x1234"));header.mBlockDifficulty=MiniData::fromHex("0xFFFF");header.mTimeMilli=MiniNumber(99999999);header.mBlockNumber=MiniNumber(123450);header.mNonce=MiniNumber(900000);
    if(!header.getBodyHash().isMore(MiniData::fromHex("0x00"))) throw std::runtime_error("Missing body hash");
    auto json=header.toJSON();auto bytes=header.serialise();size_t offset=0;auto decoded=TxHeader::deserialise(bytes,offset);
    if(offset!=bytes.size()||decoded.serialise()!=bytes||decoded.toJSON().toJSONString()!=json.toJSONString()) throw std::runtime_error("Header round trip mismatch");
}
