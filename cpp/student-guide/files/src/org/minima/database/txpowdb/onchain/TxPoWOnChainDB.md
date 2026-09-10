# TxPoWOnChainDB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java) | [src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.cpp](../../../../../../../../src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.cpp) |

## What this file does

Indexes the block in which a TxPoW became permanently on-chain, storing identifiers, height and insertion time.

## Follow the logic

1. Create an auto-increment table and prepare insertion, transaction lookup, block-ID/height lookup, earliest/latest timestamp, count and cleanup statements. The schema does not make txpowid unique; INSERT IGNORE alone therefore does not establish identifier deduplication.
2. The default retention expression multiplies Java int literals before widening to long. Its stored value is 500,654,080 milliseconds, not the apparent thousand-day mathematical product. A faithful translation must reproduce that evaluated value.
3. Insertion clears parameters, binds block ID, MiniNumber narrowed to long, TxPoW ID and current time in order. SQL failures are logged and return false; runtime failures escape.
4. A transaction lookup initially returns found:false with the requested ID. It changes found to true before reading other fields, so a later SQL failure can return a partially populated found:true object.
5. Block lookups append only fully decoded row objects. Earliest/latest reads add a date string; their ordering is by insertion timestamp, not block height.
6. Cleanup deletes timestamps strictly below now minus retention. Hard mode calls the clock a second time and substitutes now plus 100,000. Wipe drops all objects and recreates the table, but does not rebuild prepared statements in this method.

## How the translation is approached

C++ retains SQL text/call order through SqlDB services, uses atomic retention storage, Java-compatible signed wrap arithmetic and the existing date renderer. Shared prepared-statement references allow helper methods to observe replacements during reopen. This remains a storage-boundary implementation.

## What the tests tell us

The [Java oracle](../../../../../../../../support/OnChainDBOracle.java), [native harness](../../../../../../../../support/onchain_db.cpp) and [driver](../../../../../../../../tools/onchain_db_differential.py) exercise all query overloads, null/partial rows, count narrowing, checked versus runtime failures at early JDBC calls, wipe/recreate, retention extremes, two clock calls in hard cleanup and seeded sequences. The inspected [report](../../../../../../../../build/onchain-db-results.json) records 855 programs, 3,639 operations and 29,167 driver calls with zero failures. JDBC is recorded rather than executed by a native H2 engine. Clock arithmetic is validated before normalization; NPE expression text and logger prefixes/stacks are excluded.

## Dependencies and reading map

Direct Minima imports: [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [SqlDB](../../../../../../../../../upstream-study/src/org/minima/utils/SqlDB.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [TxPoWOnChainDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L39), [createSQL](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L47), [wipeDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L82), [addOnChainTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L109), [getOnChainTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L136), [getInBlockTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L174), [getInBlockTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L213), [getFirstTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L252), [getLastTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L290), [getSize](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L330), [cleanDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L356), [cleanDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L360), [main](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java#L391).

## Check your understanding

A row is found, but getLong(block) fails after found and blockid were written. What JSON survives? Why must the test compare the entire object rather than only found?

## Limits and traceability

Actual H2 execution, duplicate-row behavior under a real engine, durable files and full runtime integration remain absent. UTC fixture date comparisons do not establish every process timezone. Partial counterpart; parity not_verified.

Pinned Java SHA-256: `8d767abbf13c56896a3eeabdc53384be0894c3392eed45ce6c32c7ed0e152255`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
