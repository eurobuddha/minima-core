# megammr — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/megammr.java) · `src/org/minima/system/commands/backup/mmrsync/megammr.cpp` (absent)

## Purpose

megammr reports, exports, imports or checks a full MegaMMR backup. Its integrity helper mutates the supplied backup while verifying it.

## Follow the command

1. info returns enabled, MMR JSON and coin count even when global MegaMMR mode is false. export/import require it enabled; unknown actions simply return the common envelope.
2. export chooses a dated path, deletes/creates the destination before locking, then takes a database read lock, creates complete IBD and writes MegaMMRBackup through buffered output. It releases the lock on normal/caught failure, but does not remove a partial file or guarantee stream cleanup after an exception.
3. import loads the complete backup object before resetting services, clears/replaces the live MegaMMR and posts its IBD with override true. Poll the processor’s finished flag every 250 ms without a timeout.
4. After IBD processing, collect wallet public details and obtain matching CoinProofs. Run coinimport track:true for each proof but ignore returned status, then report finished, save and shut down. This path does not call checkMegaMMR first.
5. integrity loads a backup and calls checkMegaMMR. That helper first requires IBD.checkValidData, then demands each IBD block height be exactly current MegaMMR time+1 and adds each block to that MegaMMR. Finalize the MMR and verify every stored coin’s generated proof against its leaf and entry.
6. Return IBD total weight and report cascade/date/derived chain tip. The report assumes an existing cascade tip. The file overload and standalone main merely load and invoke the same mutating helper.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  The three native serialization containers cover the data envelope only. Import, chain processing, proof transfer, shutdown and the mutating integrity algorithm remain unported and require MegaMMR/IBD and command services.

## Understanding the tests

The inspected MMR-sync differential driver tests MegaMMRBackup bytes and partial read state, not this command. Proposed direct tests must add disabled-mode guards, export locks/partial files, delayed/never-finished IBD, ignored coinimport failures and integrity sequences. Call checkMegaMMR twice on the same object to observe why applying IBD blocks again is not idempotent.

- [Java serialization/state oracle](../../../../../../../../../support/MMRSyncOracle.java)
- [Native comparison fixture](../../../../../../../../../support/mmrsync.cpp)
- [Concrete mutation and truncation cases](../../../../../../../../../tools/mmrsync_differential.py)
- [Deterministic Java TxPoW fixture](../../../../../../../../../support/ArchiveCacheOracle.java)
- [Native TxPoW fixture](../../../../../../../../../support/ArchiveFixture.hpp)
- [Shared parameter parsing and reply envelope](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No backup was imported or checked operationally. A successful file decode does not establish integrity, and a finished import response does not prove every coinimport succeeded. The integrity method checks all coins present in its map; that alone is not proof the map omitted none.

## Check your understanding

Why can checking the same in-memory MegaMMRBackup twice fail on the second attempt?

Pinned Java SHA-256: `5ec2aedf311d44ba2836aaaa64dd9cdbac38780c84929b39d5145608d78e1cac`. [Mapping](../../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../../REVIEW.md).
