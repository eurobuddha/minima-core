# archive — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/archive.java) · `src/org/minima/system/commands/backup/archive.cpp` (absent)

## Purpose

archive coordinates archive inspection, export, import, resynchronization and historical address lookup. Its actions share static temporary sources, so the whole class is more than a stateless command wrapper.

## Follow the command

1. integrity starts at the archive’s oldest block and loads ascending ranges of roughly 256 heights. For each consecutive pair check height increment first, then parent hash; an else-if counts at most one error per pair. Separately decide whether genesis or the cascade tip aligns with the block range. valid describes this alignment, while errors reports linkage problems.
2. resync requires completed keys, resolves either a network host or the special archiverestore source, and probes a remote host at block -1 before resetting. Post MDS_RESYNC_START, clear MegaMMR when enabled, then either retain wallet keys or clean/derive a supplied phrase, reset the wallet, create keys and set their use count.
3. Start at block zero and repeatedly wipe RAM TxPoWs and request/create the next IBD. Install a cascade only on the initial zero request. Advance to last received height+1, post archive processing, then poll for a tip and exact end height with attempt limits. Null responses become empty IBDs. Empty batches and timeout errors both leave the loop; the code then reports completed, saves and shuts down.
4. export delegates H2 gzip SQL backup. exportraw writes a GZIP stream containing a MiniByte cascade flag, optional cascade, MiniNumber declared block count and consecutive TxBlock encodings. If the archive begins at one it omits the cascade; otherwise a cascade moves export start to tip+1. The count comes from height difference and optional maxexport, not actual loaded row count.
5. Raw export writes a block before testing whether the count limit has been reached, so zero/negative maxexport can still emit one block with an inconsistent declared count. Missing ranges can also make declared count differ from records. Stream-close exceptions are logged rather than necessarily failing the command.
6. import selects raw only for filenames ending exactly .dat; all others restore an H2 temporary database. Set the matching static source and run nested archive resync, forwarding phrase/keys/keyuses/anyphrase. Normal completion closes/deletes the temporary source and clears its pointer; exceptions can skip cleanup. importold uses the older H2 path but does not explicitly reset the static format flag.
7. The separate importraw action resets the chain, posts batches directly and waits only every twentieth batch with unbounded exact-height polling. It does not explicitly wait for the final short batch before shutdown. inspectraw checks sequential heights beginning from zero, even when a cascade exists; a mismatch is logged and the response only contains duration.
8. inspect reads a supplied H2 backup through a temporary database, or current archive when file is absent, and reports cascade/endpoints/size. It rejects .dat and deletes the temporary folder even when inspecting current state. addresscheck scans archive ascending then retained chain descending, appending matching created/spent events under coins without deduplicating overlap. Optional statecheck normalizes Mx except strings containing @.
9. NotifyListener synchronously sends NOTIFY_POST containing ARCHIVEUPDATE and message data when a listener exists. sendArchiveReq frames an MSG_ARCHIVE_REQ in MiniData, uses ten-second connect/read limits, records payload byte counts, then reads a MiniByte and IBD without validating the response type byte. Exceptions retry up to the supplied count, sleeping ten seconds between attempts; exhausted/zero attempts return null.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse ArchiveManager, RawArchiveInput, IBD/TxBlock framing, NIO messages and TxPoWProcessor. H2 SQL archive portability and Java thread/socket lifecycle require explicit native work; a raw archive reader alone is not the entire command.

## Understanding the tests

The inspected runner fixture checks archive permission classification only. Proposed tests must separate actions: linkage versus cascade-alignment flags; raw header/count/record bytes and maxexport 0; format suffix case; nested command forwarding/static cleanup; address overlap order; wrong response type; retries and first/later null IBD. Controlled processor/clock fixtures should exercise both timeout breaks and final-short-batch behavior without real waits. Cross-language raw fixtures and isolated H2 restore tests are separate requirements.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No archive file, database or host was used. Integrity checks adjacency/alignment, not all consensus rules. Static temporary sources and cleanup without finally make concurrent/repeated invocation important. Several success messages survive incomplete processing; they cannot be used as parity evidence.

## Check your understanding

Construct an archive with consecutive heights and a wrong parent hash: how can integrity.valid be true while integrity.errors is nonzero?

Pinned Java SHA-256: `a7bdbe6083655474292fc8782885efd98ec37fa87c92b89864f07696b223ae31`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
