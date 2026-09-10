# RawArchiveInput — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/archive/RawArchiveInput.java](../../../../../../../../upstream-study/src/org/minima/database/archive/RawArchiveInput.java) | [src/org/minima/database/archive/RawArchiveInput.cpp](../../../../../../../src/org/minima/database/archive/RawArchiveInput.cpp) |

## What this file does

Reads a gzip-compressed raw archive incrementally and returns groups of at most 256 blocks for synchronization.

## Follow the logic

1. connect opens file, buffered, gzip and data-input layers. Read a MiniByte cascade-presence flag; when true, read the Cascade. Then read the declared block count as MiniNumber narrowed to int and reset totalAdded to zero.
2. The Java method does not clear an existing cascade when a later connection has a false presence flag. Reconnection and partial-connect errors can therefore retain earlier state.
3. getNextIBD creates a fresh result. If declared blocks remain, attach the cascade only when totalAdded is zero, then read up to 256 TxBlocks, incrementing the counter after each successful addition.
4. Once totalAdded reaches totalFound, subsequent calls return empty IBD objects. A zero or negative declared count also yields no blocks and does not attach even a present cascade.
5. A failed block read throws before returning the local batch; earlier successful reads in that batch may already have advanced totalAdded. Retrying is therefore not equivalent to rewinding.
6. stop closes the nested Java stream layers. The API does not automatically validate every unread block or gzip trailer merely because the declared block count has been satisfied.

## How the translation is approached

Native GzipInput reuses zlib raw inflate with explicit gzip header, CRC and size handling, deferring trailer validation to a subsequent read like the Java stream. A 64 KiB object buffer retries static deserialization only on EOF, removes consumed bytes after success or other errors, and preserves stream progression. This adapter is part of the behavior being tested, not just an interchangeable decompression call.

## What the tests tell us

The Java oracle creates actual raw block/cascade payloads. The driver gzip-compresses them and checks block counts 0/1/2/255/256/257/513, no/empty/nonempty cascade, exact/overstated/negative declarations, repeated next/stop/connect calls, missing files, object/gzip truncation, corrupted CRC, concatenated members and trailing junk. Returned batch size, cascade presence/hash and IBD hash are compared. Evidence: [Java oracle](../../../../../../../support/RawArchiveOracle.java), [native harness](../../../../../../../support/raw_archive.cpp), [driver](../../../../../../../tools/raw_archive_differential.py). The inspected existing report records 613 matching cases with zero failures for the complete suite. Generic ERROR/IO_ERROR categories do not prove all exact exception names/messages.

## Dependencies and reading map

Direct Minima imports: [Cascade](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java), [IBD](../../../../../../../../upstream-study/src/org/minima/objects/IBD.java), [TxBlock](../../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [MiniByte](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [RawArchiveInput](../../../../../../../../upstream-study/src/org/minima/database/archive/RawArchiveInput.java#L31), [connect](../../../../../../../../upstream-study/src/org/minima/database/archive/RawArchiveInput.java#L35), [stop](../../../../../../../../upstream-study/src/org/minima/database/archive/RawArchiveInput.java#L54), [getCascade](../../../../../../../../upstream-study/src/org/minima/database/archive/RawArchiveInput.java#L61), [getNextIBD](../../../../../../../../upstream-study/src/org/minima/database/archive/RawArchiveInput.java#L65).

## Check your understanding

With 257 declared blocks, how many blocks do the first three successful getNextIBD calls contain? Why can a corrupt trailer remain unnoticed if no additional decompressed bytes are requested?

## Limits and traceability

Passing these fixtures establishes selected incremental/error behavior, not atomic archive import or full chain acceptance. Native pre-connect/null errors and exact logging differ; failed batch recovery and all gzip header combinations need full review. The counterpart remains partial and full parity not_verified.

Pinned Java SHA-256: `070d351589cc450093976ae070421598e19db1ee6294b0a752e42941c61d1b94`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
