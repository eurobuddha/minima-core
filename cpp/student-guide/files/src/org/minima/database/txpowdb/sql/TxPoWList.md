# TxPoWList — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowdb/sql/TxPoWList.java](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWList.java) | [src/org/minima/database/txpowdb/sql/TxPoWList.cpp](../../../../../../../../src/org/minima/database/txpowdb/sql/TxPoWList.cpp) |

## What this file does

A serializable wrapper around a mutable list of TxPoW references, used when storing or transferring groups of proofs of work.

## Follow the logic

1. The public constructor stores the supplied ArrayList reference directly. Adding to that list afterward changes what the wrapper will serialize.
2. Write a four-byte signed int count, followed by each TxPoW. This count is not encoded as MiniNumber, unlike many other Minima list formats.
3. Reading immediately replaces the list, reads the signed count and appends complete decoded TxPoWs. A negative count executes zero loop iterations and leaves the rest of the input unread.
4. If decoding fails partway through, the receiver retains the successfully appended prefix and the stream has advanced. The convenience converter instead returns null for IOException after logging it.
5. The list can contain Java null through its public field/API, but serializing such an element fails; there is no add-method validation in this wrapper.

## How the translation is approached

Native code uses shared_ptr<vector<shared_ptr<TxPoW>>> to preserve both the caller’s list alias and element references. A mutating read method replaces the shared list before reading the count and appends incrementally. The converter catches wire::IOException and logs through std::clog, with different diagnostic formatting from MinimaLogger.

## What the tests tell us

The make fixture constructs the wrapper before adding rows, directly testing caller-list aliasing. Lists of 0/1/2/5/17 objects include body-present and body-cleared TxPoWs. Derived reads compare bytes, consumed offset and retained list size for truncations, trailing bytes, negative counts and malformed nested objects; both direct read and convenience conversion are tested. Evidence: [Java oracle](../../../../../../../../support/ArchiveCacheOracle.java), [native harness](../../../../../../../../support/archive_cache.cpp), [driver](../../../../../../../../tools/archive_cache_differential.py). The inspected existing report records 734 matching cases with zero failures for the complete suite. Generic ERROR/IO_ERROR categories do not prove all exact exception names/messages.

## Dependencies and reading map

Direct Minima imports: [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [TxPoWList](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWList.java#L18), [TxPoWList](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWList.java#L20), [writeDataStream](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWList.java#L25), [readDataStream](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWList.java#L33), [convertMiniDataVersion](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWList.java#L45), [ReadFromStream](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/sql/TxPoWList.java#L65).

## Check your understanding

Why would replacing the shared native list with a copied vector break the make fixture? What should remain in a receiver after the third TxPoW fails to decode?

## Limits and traceability

The wrapper’s package name does not mean these tests exercise SQL storage. Exact diagnostics, null-element errors and all alias lifetimes still need parity review. The counterpart remains partial and full parity not_verified.

Pinned Java SHA-256: `e97b74ec259dc332dba8c13d5d6c371fc0a901c4352f54f35a28f6ba07fcbd16`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
