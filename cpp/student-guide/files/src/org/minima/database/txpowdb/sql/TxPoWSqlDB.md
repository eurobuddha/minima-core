# TxPoWSqlDB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java) | [src/org/minima/database/txpowdb/sql/TxPoWSqlDB.cpp](../../../../../../../../src/org/minima/database/txpowdb/sql/TxPoWSqlDB.cpp) |

## What this file does

Stores complete serialized TxPoW objects and searchable metadata, with retention that preserves relevant transactions.

## Follow the logic

1. Create a table with unique txpowid and an index over txpowid/parentid. Prepare exact-ID, block-child, count, relevance, newest-page and cleanup queries. The retention value is initialized at first class use from the configured days.
2. Before insertion, optional block/transaction logs can inspect the object and transaction JSON. Then open the connection, serialize the TxPoW and bind ID, flags, parent, current time, bytes and relevance. Serialization failure can be logged before later bindings encounter the missing result.
3. Reading retrieves bytes and uses TxPoW.convertMiniDataVersion. Malformed input can become null; list queries retain those null entries. A SQL failure after earlier rows returns the completed prefix.
4. Relevant and newest queries bind limit then offset and sort by descending insertion time. Latest-size uses strictly greater than the supplied timestamp. Count failures return -1; exists catches SQL errors silently and returns false.
5. customSizeQuery lowercases the entire condition, removes semicolons, then removes update, delete and insert in separate single passes before appending it to SQL. These textual edits can alter literals and are not a SQL parser or a proof of safe query construction.
6. Cleanup deletes only old rows with isrelevant=0. Even hard cleanup preserves relevant rows because it changes the timestamp threshold, not that predicate. It can call the clock twice and logs around the operation when enabled.
7. wipe recreates the table after DROP ALL OBJECTS without rebuilding the fast index or prepared statements. closeAndReopen saves without compaction, clears the connection and reopens through SqlDB.

## How the translation is approached

Native code reuses the real TxPoW codec, SqlDB service boundary and Java lowercase helper. Its token-removal helper deliberately scans each original input once so newly formed matches survive that pass. Function-local retention initialization models Java class loading, and unsigned arithmetic followed by bit conversion models signed Java wrap.

## What the tests tell us

The [Java oracle](../../../../../../../../support/TxPoWSqlDBOracle.java), [native harness](../../../../../../../../support/txpow_sql_db.cpp) and [driver](../../../../../../../../tools/txpow_sql_db_differential.py) compare schema and prepared-statement identities, pagination, filtering edge cases, null receivers, faults during calls, cleanup/reopen and every truncated prefix of four real TxPoW fixtures. Logging/serialization failure timing and a nonempty transaction are included. The inspected [report](../../../../../../../../validation/checkpoint-361/normal/txpow-sql-db-results.json) records 2,592 programs, 8,729 operations and 86,333 driver calls with zero failures. JDBC is recorded rather than executed by a native H2 engine. Clock arithmetic is validated before normalization; NPE expression text and logger prefixes/stacks are excluded.

## Dependencies and reading map

Direct Minima imports: [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniByte](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [SqlDB](../../../../../../../../../upstream-study/src/org/minima/utils/SqlDB.java).

Method navigation (declarations with bodies found in this file): [TxPoWSqlDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L59), [createSQL](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L67), [wipeDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L114), [customSizeQuery](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L144), [addTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L181), [getTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L234), [getChildBlocks](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L271), [getRelevantSize](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L304), [getAllRelevant](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L326), [getAllRelevant](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L330), [getLatestTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L371), [getLatestTxPoWSize](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L412), [getSize](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L438), [exists](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L460), [cleanDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L486), [cleanDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L490), [closeAndReopen](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L531), [main](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java#L549).

## Source main and durable-file validation

The source main is now translated using the existing SQL provider and an explicit
home path. It sets time and parent before calculating the ID, prints the TxPoW,
inserts it with relevant=true and saves without compaction. The real-H2 main corpus
opens every saved file with Java and C++, compares exact wire roundtrips and JSON,
and checks that printed ID/parent/flags/relevance match the stored row. Fresh PRNG
and time values are independently checked before cross-writer projection.

See the [database demonstration lab](../../../../../../../new-tests/database-main-methods.md) for the assertions, initial test-assumption corrections and limits.

## Check your understanding

Why does hard cleanup differ from wipe for a relevant transaction? Trace the exact DELETE predicate, then design one fixture that distinguishes the two operations.

## Limits and traceability

Recorded SQL text is not durable storage. The later main corpus adds real H2 execution and persistence for the demonstration. Every real driver failure, concurrency and full runtime ownership remain unverified; count/default results can hide caught errors. The counterpart remains partial and parity not_verified.

Pinned Java SHA-256: `19401817f8fd1a32c1bffd565eabe3677dc673f357516e6b45bde8b425221c69`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
