# restoresync — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/restoresync.java) · `src/org/minima/system/commands/backup/restoresync.cpp` (absent)

## Purpose

restoresync restores the same encrypted backup format, then optionally catches an old chain up through an archive node before shutting down.

## Follow the command

1. Perform the six-section restoration with restoreReady(false), temporary wallet SQL, direct node-file writes, disabled save state and replacement relevant TxPoW SQL. Restore/close wallet SQL, delete archive SQL and reopen services for sync. Like restore, this has no global rollback.
2. Compare restored tip time with now minus two days. Read keyuses default 256 even though keyuses is absent from getValidParams. If the backup is newer than two days or host is absent, increment key uses, save and stop the node directly.
3. For an older backup with host, start from tip.getParent(128) and call performResync. Its connection probe requests block -1 before archiveResetReady. A failed probe logs and returns an empty object, and the caller still proceeds to shutdown.
4. After a successful probe, reset archive state and increment or set key uses according to the helper argument. Repeatedly wipe RAM TxPoWs, request a batch, install an initial cascade only at start zero, advance next request to last block+1 and post archive IBD processing.
5. A null batch is replaced by an empty IBD and treated as end-of-data. Poll for the first tip and then exact batch-end height with bounded attempt counts; timeouts log and break. Every fifth batch can trigger memory reset above 250 MiB.
6. The helper emits a completed message even after those loop-break errors. The caller saves and shuts down, returning the ordinary restore restart message rather than the helper’s detailed result.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse backup decoding, archive network request framing and asynchronous TxPoW processing. A native synchronous batch loop must preserve the observable wait/error paths or document an intentional change.

## Understanding the tests

The runner fixture checks restoresync classification only. Proposed tests use fixed clock/files and controlled archive replies: just below/exactly two days, no host, failed probe, null later batch, cascade at zero/nonzero start, timeouts and key-use increment versus set. Assert saved state and shutdown even when helper reports no data; never treat its completed text as proof of full sync.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No restoration or network request ran. Old backups can be partly restored before host parsing/connection fails. Exact-height polling can fail if the processor passes the expected end height, and late errors do not necessarily become status:false.

## Check your understanding

Why can a failed archive connection still lead to the caller returning a restart message?

Pinned Java SHA-256: `4eaff23d3afc7e26d2e15df39bb97613787e03a21dc0030fb26246d0dfe9832c`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
