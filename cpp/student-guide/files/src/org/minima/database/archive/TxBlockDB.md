# TxBlockDB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/archive/TxBlockDB.java](../../../../../../../../upstream-study/src/org/minima/database/archive/TxBlockDB.java) | [src/org/minima/database/archive/TxBlockDB.cpp](../../../../../../../src/org/minima/database/archive/TxBlockDB.cpp) |

## What this file does

A synchronized in-memory index of TxBlock objects keyed by their TxPoW IDs, with parent lookup and height-based cleanup.

## Follow the logic

1. addTxBlock inserts or replaces the object under its current TxPoW ID. findTxBlock returns the stored object reference or null; it does not copy the block.
2. getChildBlocks scans map values and selects blocks whose current parent-ID hex string exactly equals the requested string. Ordering follows ConcurrentHashMap enumeration rather than block height.
3. clearOld retains blocks with block number greater than or equal to the minimum. It builds a new map keyed from each retained block’s current ID, then replaces the original map.
4. This rebuilding can reindex a block whose mutable TxPoW changed its ID after insertion. Before cleanup the old key may still point to the changed object.
5. clearAll empties the current map. Every public method is synchronized in Java, so each complete operation holds the object monitor.

## How the translation is approached

C++ uses a mutex around complete operations and a shared JavaConcurrentMap of shared_ptr<TxBlock>. The optional parent query represents Java null equality returning no children. Reusing the map adapter preserves the tested collision/enumeration behavior and retaining pointers preserves row identity.

## What the tests tell us

The cache fixture creates deliberately colliding IDs, inserts duplicates, checks returned pointer identity, queries five parent groups, mutates every third TxPoW ID, cleans at thresholds -6/0/1/17/32 and checks reindexing and final clear. Sizes around collision thresholds and up to 256 exercise ordering and replacement. Evidence: [Java oracle](../../../../../../../support/ArchiveCacheOracle.java), [native harness](../../../../../../../support/archive_cache.cpp), [driver](../../../../../../../tools/archive_cache_differential.py). The inspected existing report records 734 matching cases with zero failures for the complete suite. Generic ERROR/IO_ERROR categories do not prove all exact exception names/messages.

## Dependencies and reading map

Direct Minima imports: [TxBlock](../../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [TxBlockDB](../../../../../../../../upstream-study/src/org/minima/database/archive/TxBlockDB.java#L14), [addTxBlock](../../../../../../../../upstream-study/src/org/minima/database/archive/TxBlockDB.java#L18), [findTxBlock](../../../../../../../../upstream-study/src/org/minima/database/archive/TxBlockDB.java#L22), [getChildBlocks](../../../../../../../../upstream-study/src/org/minima/database/archive/TxBlockDB.java#L26), [clearAll](../../../../../../../../upstream-study/src/org/minima/database/archive/TxBlockDB.java#L45), [clearOld](../../../../../../../../upstream-study/src/org/minima/database/archive/TxBlockDB.java#L49).

## Check your understanding

If a stored TxPoW ID changes from A to B, what can findTxBlock(A) and findTxBlock(B) return before and after clearOld retains that row?

## Limits and traceability

This is a cache, not an H2/MVStore replacement. Concurrent mutation of the TxBlock itself is separate from the cache mutex and remains outside the quiescent fixture. The counterpart remains partial and full parity not_verified.

Pinned Java SHA-256: `67deaf88609d3a3072e745eef40bb6fd9f297eeca6e36cd294955376f4942519`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
