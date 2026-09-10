#pragma once
#include "org/minima/system/brains/TxPoWSearcher.hpp"
namespace minima {
struct TxPoWSearcherFixture {
    static void reset(){std::lock_guard lock(TxPoWSearcher::mImportedMutex);TxPoWSearcher::mImportedTokens.clear();}
};
}
