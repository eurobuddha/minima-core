# megammr — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/megammr.java) · [C++ counterpart](../../../../../../../../../src/org/minima/system/commands/backup/mmrsync/megammr.cpp)

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

The native class now maps the source methods and is registered in the real command runner. It reuses the existing MegaMMR and IBD codecs, database and wallet services, buffered file output, coin commands, and shutdown processing. The two commands retain their distinct reset and key-use rules. Main now supplies the serving helper as its default incoming MegaMMR callback. Full parity remains unverified.

## Understanding the tests

The [pre-test lab](../../../../../../../../../student-guide/new-tests/mega-mmr-commands.md)
explains why a backup check is a mutation, not a read-only assertion. The new
[driver](../../../../../../../../../tools/mega_commands_differential.py) reuses the
archive Java/native fixtures and real temporary H2 stores. It compares metadata,
parameter/type gates, enabled/disabled behavior, export bytes and filesystem effects.
Default export timestamps are checked against measured time before normalization.

The fixture creates real MMR leaves for synthetic coins and varies starting time,
IBD heights, cascade alignment and a changed coin ID. Changing the coin after
hashing its leaf makes the generated proof fail against that changed data.
After every attempt, compare the MegaMMR bytes, block time and finalized flag.
Repeated checks of a nonempty IBD must fail at its first old height after the first
check advances the MegaMMR. These are algorithm fixtures, not valid mined chains.

An empty-tree export revealed a shared IBD discrepancy: the file was already fully
written before getTreeTip failed. Java threw IndexOutOfBoundsException; native code
exposed a generic vector error. Both IBD endpoint methods now preserve Java's index
exception. A separate initial observer error mislabeled CommandException and was
fixed only in the fixture. Both failed reports are retained.

Both builds pass 579 combined command/helper comparisons and ten loopback cases.
The existing container tests remain separate evidence for wire and partial reads.
Direct EOF/NPE/class-cast diagnostic prose is normalized by the reused observer;
other responses, mutation and file contents remain compared.

## Limits

The separate [live-node lab](../../../../../../../../../student-guide/new-tests/mega-mmr-live-node.md)
and [driver](../../../../../../../../../tools/native_mega_startup.py) use actual saved
genesis data. Java and C++ export identical 903-byte backups. The file-overload
integrity helper succeeds, and both runtimes import the other's backup, shut down,
and preserve the owned coin proof and all eight wallet keys on opposite-runtime
reopen. The test sets key uses to 100 before import; signing afterward changes only
one key to 101 and a modified message fails verification.

The demonstration main, a full integrity report with a nonempty cascade,
empty-MMR failure categories, and exact buffered partial-write boundaries still
need validation. The live test covers genesis, not recovery of pruned coins.
A finished import response does not prove every coinimport succeeded. The integrity
helper checks all coins present in its map; that alone is not proof the map omitted none.

## Check your understanding

Why can checking the same in-memory MegaMMRBackup twice fail on the second attempt?

Pinned Java SHA-256: `5ec2aedf311d44ba2836aaaa64dd9cdbac38780c84929b39d5145608d78e1cac`. [Mapping](../../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../../REVIEW.md).

## Backup/checker interruption follow-up

Import's 250 ms IBD-completion wait now uses NativeThread. Interruption exits before memory cleanup, proof transfer and final shutdown, matching the source statement order. Existing MegaMMR comparisons remain intact. A direct interrupted import against durable storage is not newly claimed. See the [interruption lab](../../../../../../../../new-tests/backup-checker-interruption.md) for results and remaining limits. Full parity stays not_verified.
