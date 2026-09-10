# RamDB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowdb/ram/RamDB.java](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java) | [src/org/minima/database/txpowdb/ram/RamDB.cpp](../../../../../../../../src/org/minima/database/txpowdb/ram/RamDB.cpp) |

## What this file does

The in-memory TxPoW cache and mempool index. It tracks recent access, chain flags, candidate transactions and input coin references.

## Follow the logic

1. Index rows by TxPoW ID in a ConcurrentHashMap. addTxPoW refreshes an existing row’s timestamp and returns false; it inserts a new RamData only when the ID is absent. getTxPoW also refreshes access, while exists does not.
2. cleanDB computes now - MAX_TIME and retains rows whose lastAccess is strictly greater than that cutoff. It creates a new map reusing surviving RamData objects, then replaces the map reference. Old map handles remain distinct.
3. getAllUnusedTxns selects rows that are not on the main chain, are classified as transactions, and are not in the cascade. clearMainChainTxns resets only the main-chain flags.
4. cleanUnusedTxn performs a stable descending sort by burn amount, then removes entries when counter > MAX_UNUSED_TXNS. Since counter starts at zero, a nonnegative limit N retains up to N+1 candidates, not N.
5. setInCascade marks a matching row permanently for the purpose of ordinary unused-transaction selection unless another caller directly alters it. A missing ID is ignored by either flag setter.
6. checkForCoinID scans transaction rows outside the cascade, including rows on the main chain. It checks coin IDs in both normal and burn witness coin proofs rather than inspecting only the Transaction input arrays.
7. wipeRamDB clears the current map in place. This differs from cleanDB’s replacement and is visible to retained map references.

## How the translation is approached

C++ uses the shared JavaConcurrentMap adapter to retain the tested quiescent enumeration order. A mutex protects replacement/access of the map pointer, row fields are atomic, and stable_sort preserves tie order. cleanDBAt is an additional native hook for controlled time; ordinary cleanDB still reads the system clock.

## What the tests tell us

The RAM fixture tests sizes 0 through 513, limits -1/0/1/7/500 and combinations of main-chain/cascade flags. It inserts each TxPoW twice, queries unused transactions and coin references, cleans the unused pool, clears flags, refreshes lookups, performs zero-age eviction and checks an old map handle before wiping. The same suite also drives many collision-heavy map insert/get/remove/clear programs. Its report explicitly limits claims to quiescent ordering and leaves concurrency/timing unverified. See [Java oracle](../../../../../../../../support/RamOracle.java), [native harness](../../../../../../../../support/ram.cpp) and [differential driver](../../../../../../../../tools/ram_differential.py). The inspected existing report contains 315 matching cases with zero failures across this suite. Generic ERROR normalization does not prove exact exception classes.

## Dependencies and reading map

Direct Minima imports: [CoinProof](../../../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java), [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java).

Method navigation (declarations with bodies found in this file): [RamDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L28), [addTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L32), [exists](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L52), [getTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L56), [remove](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L65), [cleanDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L69), [cleanUnusedTxn](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L93), [compare](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L101), [getSize](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L115), [wipeRamDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L119), [clearMainChainTxns](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L126), [setOnMainChain](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L134), [getAllUnusedTxns](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L141), [setInCascade](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L157), [checkForCoinID](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamDB.java#L167).

## Check your understanding

With MAX_UNUSED_TXNS=1 and four equally ranked candidates, how many survive? Why can an old map returned by getCompleteMemPool still contain rows after cleanDB replaces the current map?

## Limits and traceability

Compound get-then-put operations are not made atomically equivalent merely by using a concurrent map. Actual concurrent enumeration, replacement races, exact age boundaries and mutable TxPoW IDs need focused review. Port status remains partial and parity not_verified.

Pinned Java SHA-256: `3c5b764fe2b5b852cc4f9291d9dd73393c279f483e85cc9acb7e1781e27db586`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
