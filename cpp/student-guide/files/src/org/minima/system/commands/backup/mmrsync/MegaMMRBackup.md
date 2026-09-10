# MegaMMRBackup — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/MegaMMRBackup.java) · [Mapped native body](../../../../../../../../../src/org/minima/system/commands/backup/mmrsync/MegaMMRBackup.cpp)

## Purpose

MegaMMRBackup pairs a MegaMMR snapshot with an IBD containing the cascade and chain data that follows it. It is a wire-format container, not the backup command itself.

## Follow the command

1. The populated constructor stores references to MegaMMR and IBD, and getters return those same references. The default constructor leaves both null.
2. Write MiniNumber version 1, followed by MegaMMR bytes and IBD bytes without an additional wrapper length around each object.
3. Read the version and narrow it to int but never validate its value. Replace the MegaMMR field with a new instance before reading its body; assign IBD only after IBD.ReadFromStream completes.
4. Consequently a malformed read can replace/partially fill MegaMMR while preserving the old IBD reference. This replacement timing is observable to callers holding references.

## Translation and dependencies

The native implementation uses shared_ptr to preserve constructor/getter aliases, writes the same component ordering and replaces MegaMMR before its read. IBD is assigned from a successfully deserialized value, matching Java’s later assignment. Null serialization raises a native argument exception; the fixture compares its broad error outcome, not exact Java exception text.

## Understanding the tests

The inspected driver builds default/null and populated objects with 0, 1, 2, 5 and 17 deterministic entries/blocks. It compares exact bytes, then reads valid, appended, version-mutated, negative-count and many truncated variants into default and already-populated objects. Each result includes error category, consumed offset, whether both original references survived, and serialized partial state. This specifically tests the early MegaMMR versus late IBD replacement distinction.

- [Java serialization/state oracle](../../../../../../../../../support/MMRSyncOracle.java)
- [Native comparison fixture](../../../../../../../../../support/mmrsync.cpp)
- [Concrete mutation and truncation cases](../../../../../../../../../tools/mmrsync_differential.py)
- [Deterministic Java TxPoW fixture](../../../../../../../../../support/ArchiveCacheOracle.java)
- [Native TxPoW fixture](../../../../../../../../../support/ArchiveFixture.hpp)
- [Shared parameter parsing and reply envelope](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The fixture’s blocks/MMR entries test format and mutation mechanics, not a valid synchronized chain or recovery from a live backup. Exact arbitrary exception diagnostics and externally visible partially written output streams are not established by vector-return serialization.

## Check your understanding

If decoding fails in IBD after MegaMMR was read, which old reference remains and which has been replaced?

Pinned Java SHA-256: `aae85009ef9ee328def0fcf61829bbcd0369fb811c3b7ecb9ee2b3755cff0ba6`. [Mapping](../../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../../REVIEW.md).
