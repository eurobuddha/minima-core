#include "RamData.hpp"
#include <chrono>
namespace minima {
RamData::RamData(std::shared_ptr<TxPoW> txpow):mTxPoW(std::move(txpow)) {updateLastAccess();}
void RamData::updateLastAccess() {mLastAccess=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
}
