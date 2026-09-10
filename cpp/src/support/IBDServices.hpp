#pragma once
#include "org/minima/database/txpowtree/TxPowTree.hpp"
#include "org/minima/database/cascade/Cascade.hpp"
namespace minima {
// MinimaDB/ArchiveManager/Main bindings. Required callbacks fail when unbound;
// offline tests use the same boundary to supply deterministic archive results.
struct IBDArchive {
    std::function<MiniNumber(const std::string&)> exists;
    std::function<std::vector<std::shared_ptr<TxBlock>>(const MiniNumber&,const MiniNumber&,bool)> loadBlockRange;
    std::function<std::vector<std::shared_ptr<TxBlock>>(const MiniNumber&)> loadSyncBlockRange;
    std::function<std::shared_ptr<TxBlock>()> loadLastBlock;
    std::function<std::shared_ptr<Cascade>()> loadCascade;
};
struct IBDServices {
    std::function<std::shared_ptr<TxPowTree>()> getTree;
    std::function<std::shared_ptr<Cascade>()> getCascade;
    std::function<std::shared_ptr<IBDArchive>()> getArchive;
    std::function<void(bool)> readLock;
    std::function<bool()> isShuttingDown;
};
}
