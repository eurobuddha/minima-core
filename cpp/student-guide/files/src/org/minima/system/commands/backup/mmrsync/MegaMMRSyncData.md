# MegaMMRSyncData — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/MegaMMRSyncData.java) · [Mapped native body](../../../../../../../../../src/org/minima/system/commands/backup/mmrsync/MegaMMRSyncData.cpp)

## Purpose

MegaMMRSyncData serializes the public addresses and keys a syncing node wants searched. The two ordered lists have separate counts.

## Follow the command

1. The populated constructor stores the caller’s address and key lists without copying. The default constructor leaves both null and getters return live lists.
2. Write MiniNumber version 1, address count and each MiniData address, followed by key count and each MiniData key.
3. Read and ignore version. Replace the address list before reading its count/elements; only after it completes replace the key list and read its count/elements.
4. Counts narrow through MiniNumber.getAsInt. A negative count yields an empty list because the Java for loop does not execute, then decoding continues at the next count rather than rejecting the packet.

## Translation and dependencies

The native class models each list as shared_ptr<vector<shared_ptr<MiniData>>> to retain aliases and null possibilities. The two-field read loop assigns a new list before its count, preserving Java’s staged replacement and partial decoded contents.

## Understanding the tests

The Java/native fixtures construct a wrapper around empty lists and then append variable-length zero-filled data to those same lists, checking constructor aliases through emitted bytes. The driver compares null/default serialization, sizes 0/1/2/5/17, altered version, a crafted negative-count packet, extra trailing byte and deterministic truncation positions. After reads into existing/default objects it compares both reference-equality flags, consumed offsets and every surviving list element.

- [Java serialization/state oracle](../../../../../../../../../support/MMRSyncOracle.java)
- [Native comparison fixture](../../../../../../../../../support/mmrsync.cpp)
- [Concrete mutation and truncation cases](../../../../../../../../../tools/mmrsync_differential.py)
- [Deterministic Java TxPoW fixture](../../../../../../../../../support/ArchiveCacheOracle.java)
- [Native TxPoW fixture](../../../../../../../../../support/ArchiveFixture.hpp)
- [Shared parameter parsing and reply envelope](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

These are format/state tests, not address or public-key validity checks. A negative or extreme count can have surprising framing effects; the source imposes no explicit list-size bound here. Exact exception messages and network privacy are outside these cases.

## Check your understanding

If a packet fails at the key count, why is the key-list reference already new even though it contains no decoded keys?

Pinned Java SHA-256: `dbfdd43c0951c1594bf66c3bd068528ed0c37a52f74a5f2fed027331a8d1b3ac`. [Mapping](../../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../../REVIEW.md).
