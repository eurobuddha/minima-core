# TxPoWDB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowdb/TxPoWDB.java](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java) | [src/org/minima/database/txpowdb/TxPoWDB.cpp](../../../../../../../src/org/minima/database/txpowdb/TxPoWDB.cpp) |

## What this file does

Coordinates the short-lived RAM mempool, longer-lived SQL TxPoW cache and on-chain transaction index.

## Follow the logic

1. Construct one RamDB and the two SQL stores. Loading opens the main file first, then an onchain sibling path. Save, hard-close and SQL-clean operations call the main store before the on-chain store.
2. addTxPoW first gets the object ID and attempts to post a backup message containing the original object. Posting failures are caught/logged and insertion continues. Even duplicate additions attempt a fresh backup message.
3. Insert into RAM only if that ID is absent. If SQL lacks the ID, resolve the wallet, evaluate TxPoW relevance and attempt SQL insertion. Return relevance, not a database-success flag. Existing SQL entries still trigger a fresh relevance calculation.
4. addSQLTxPoW skips the RAM and backup-message paths and evaluates relevance only on a SQL miss.
5. getTxPoW consults RAM first, then SQL on a miss. SQL retrieval is not automatically promoted into RAM. getAllTxPoW preserves request order and duplicates while omitting missing values.
6. Child-block lookup obtains IDs from SQL and resolves each through the same RAM-first path. Mempool flags, coin-conflict search, cleanup and the complete map are delegated to RamDB.
7. These operations are not one atomic transaction across all stores. A failure in relevance or SQL after RAM insertion can leave RAM changed; a retained backup message can observe subsequent mutation of its shared TxPoW.

## How the translation is approached

C++ composes the same mapped stores, requires Main-message and wallet-resolution services and retains shared TxPoW/Message/RamData/map ownership. Optional identifiers allow null-key failure paths. File paths use the existing Java Unix helper and SQL clocks are injectable for comparison.

## What the tests tell us

The [Java owning-store oracle](../../../../../../../support/TxPoWDBOracle.java), [native harness](../../../../../../../support/txpow_db.cpp) and [driver](../../../../../../../tools/txpow_db_differential.py) execute actual RAM/SQL/searcher code with recorded JDBC and owner receivers. They compare duplicate objects, held map identities, mutable IDs, wallet short-circuit order, null/failing services, SQL faults, two-store lifecycle, cleanup and seeded sequences. Clocks are validated against operation windows before normalization. The inspected [report](../../../../../../../build/txpow-db-results.json) has 1,355 programs, 7,407 operations and 80,960 matching recorded effects with no failures.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWOnChainDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java), [RamDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java), [RamData](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java), [TxPoWSqlDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java), [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Main](../../../../../../../../upstream-study/src/org/minima/system/Main.java), [TxPoWSearcher](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Message](../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java).

Method navigation (declarations with bodies found in this file): [TxPoWDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L50), [loadSQLDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L58), [hardCloseSQLDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L67), [saveDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L72), [addTxPoW](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L81), [addSQLTxPoW](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L115), [getTxPoW](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L130), [getAllTxPoW](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L144), [exists](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L168), [getChildBlocks](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L183), [getRamSize](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L194), [getSqlSize](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L198), [getSqlFile](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L202), [getSQLDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L206), [getOnChainDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L210), [cleanDBRAM](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L220), [wipeDBRAM](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L224), [cleanDBSQL](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L228), [clearMainChainTxns](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L236), [setOnMainChain](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L240), [setInCascade](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L244), [getAllUnusedTxns](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L248), [removeMemPoolTxPoW](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L255), [checkMempoolCoins](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java#L262).

## Check your understanding

If addTxPoW returns true after SQL insertion fails inside a caught SQL exception, what does true mean? Which fixture observations reveal whether RAM changed and a backup message was posted?

## Limits and traceability

JDBC and Main/wallet owners are fixture boundaries. Native H2 storage and the full node runtime remain incomplete, and NPE expression text/logger stacks are normalized. Return-value parity is therefore distinct from persistent-storage parity.

Pinned Java SHA-256: `f2f39efc4458f56f777af22e20ce6bf1d60c5bd282960070910d17968c5b4808`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
