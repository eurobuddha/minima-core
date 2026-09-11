# CoinDB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowtree/CoinDB.java](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java) | [src/org/minima/database/txpowtree/CoinDB.cpp](../../../../../../../src/org/minima/database/txpowtree/CoinDB.cpp) |

## What this file does

Stores serialized coins against a tree-node identifier and block height, using the project SQL boundary.

## Follow the logic

1. Creating the singleton first constructs a replacement. Its constructor attempts to delete the old path plus .mv.db, then calls loadDB. A caught SQL failure still permits publication; an uncaught runtime failure prevents replacement.
2. createSQL defines an auto-increment id, tree identifier, integer height and coin blob, then prepares insert, select-by-tree, strict-height-delete and count queries. It does not add a uniqueness constraint on tree identifiers or an ORDER BY for selection.
3. insertCoin ensures the connection is open, serializes the coin, clears prepared parameters and binds tree ID, narrowed block long and bytes in that order. SQL errors are logged and return false; other failures can escape after some bindings.
4. getAllCoins streams result rows and converts each blob back into a Coin. A conversion may return null, which is retained in the list. A SQL failure after earlier rows returns the accumulated prefix.
5. clearOldCoins deletes only heights strictly below the threshold. getSize returns the SQL count narrowed to int, or -1 when no row or a caught SQL failure supplies a value.
6. closeAndReopen saves compactly, clears the connection and opens it again. Synchronized methods serialize access to shared prepared-statement parameter state.

## How the translation is approached

C++ reuses SqlDB and its injected connection/statement interfaces, preserves optional null coins and adds a mutex for singleton publication. The constructor uses the Java Unix file-normalization helper. This implements call ordering and codecs against a SQL service boundary; it does not include a native H2 database engine.

## What the tests tell us

The [Java oracle](../../../../../../../support/CoinDBOracle.java), [native harness](../../../../../../../support/coin_db.cpp) and [driver](../../../../../../../tools/coin_db_differential.py) record SQL calls and real temporary-file deletion while using actual Coin codecs. They exercise held old singleton owners, replacement after SQL/runtime failure, null receivers, failing calls at each statement position, partial rows, every truncated prefix of selected coin wires, trailing bytes, invalid hashes, path normalization and seeded operation sequences. The inspected [report](../../../../../../../validation/student-linked-evidence/build/coin-db-results.json) has 1,350 matching programs, 4,318 operations and 28,803 recorded effects. JDBC is a recording fixture; NPE expression details and logger prefixes/stacks are normalized.

## Dependencies and reading map

Direct Minima imports: [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [SqlDB](../../../../../../../../upstream-study/src/org/minima/utils/SqlDB.java).

Method navigation (declarations with bodies found in this file): [createCoinDB](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L19), [getTxPoWTreeCoinDB](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L22), [CoinDB](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L34), [createSQL](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L51), [closeAndReopen](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L80), [insertCoin](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L98), [getAllCoins](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L130), [clearOldCoins](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L173), [getSize](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/CoinDB.java#L196).

## Check your understanding

Why can a getAllCoins result contain both valid Coin objects and null? Design a three-row fixture that distinguishes a codec failure from a later SQL failure.

## Limits and traceability

No native H2 persistence or full-node integration is proven. SQL result ordering without ORDER BY, concurrency interleavings and real driver resource failures remain outside the recorded fixture. The counterpart is partial and parity not_verified.

Pinned Java SHA-256: `2015f3f39659cbe82e08b046a2870b240e5cc03018e9548c303eb5123dc64620`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
