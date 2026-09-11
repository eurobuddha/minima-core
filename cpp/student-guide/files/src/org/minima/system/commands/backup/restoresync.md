# restoresync — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/restoresync.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/backup/restoresync.cpp)

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

The native class maps the full runCommand, metadata/factory, both private section
readers and public performResync method. It reuses restore.cpp's proven six-section
sequence and stream adapters while preserving restoresync's distinct close/reopen
order. It reuses archive.NotifyListener/sendArchiveReq and the existing real
processor, wallet and archive methods. The 128-parent lookup stops at the oldest
available ancestor; it does not require exactly 128 stored ancestors.

Clock and used-memory checks use existing MinimaLogger Runtime callbacks, backed
by the native allocator and wall clock. Polling and network retry delays remain
real. Integer counter increments and time subtraction preserve Java wraparound.
Nullable host/start arguments remain representable in the native public method.
Missing-file exceptions use the existing length-aware JavaTypedException carrier.

## Understanding the tests

The [pre-test lab](../../../../../../../../student-guide/new-tests/restore-sync.md)
describes distinct restoration and remote requirements. The extended
[Java oracle](../../../../../../../../support/RestoreOracle.java) and
[native fixture](../../../../../../../../support/restore.cpp) use real H2 stores.
They add old/future tree tips, run actual restoreReadyForSync and stop the resulting
processor. The fixture marks MDS as already stopped; actual MDS/node shutdown is
covered separately with running nodes. Restore readiness itself is recorded.

The [driver](../../../../../../../../tools/restore_sync_differential.py) reuses
restore's malformed encrypted corpus and file snapshots, adding direct keyuses,
old/future/empty trees, ignored invalid hosts for recent backups, runner rejection
of keyuses, reopen failure and invalid performResync inputs. It compares SQL rows,
cache values, tree/cascade bytes, closure/replacement flags and partial files.
Direct EOF/NPE/class-cast prose is normalized; categories and effects stay compared.

Reopening a tree adds its TxPoW to SQL history. The independent assertion therefore
requires six relevant backup rows and permits an extra non-relevant row only if it
is the restored tip. Malformed SQL can delete the seed table; the fixture supplies
GeneralParams.SEED_PHRASE so both runtimes initialize the same new seed. Neither
correction changes production behavior. Initial failed reports are retained in
[validation evidence](../../../../../../../../validation/restoresync-work-in-progress).

The reused [live harness](../../../../../../../../tools/native_restore_startup.py)
selects restoresync through --restore-command. It runs real nodes in both restore
directions, checks chain/keys after opposite-runtime reopen and signs with a restored
key. One recent-backup direction supplies an invalid host that must be ignored.
Both builds pass 181 differential cases, including 39 success replies and 49
failures that leave saving disabled. Success replies include source quirks on
malformed inputs; they do not all mean the original wallet was preserved. The
source-state and failure observations in the saved report distinguish these cases.
Both builds also pass six actual node processes for this command, preserving
eight keys at uses 263 and signing once at 264. A shared-helper null-host mismatch
was corrected in connect; the existing 532-case network-startup suite also passes.

The [old-backup lab](../../../../../../../../student-guide/new-tests/restore-sync-remote.md)
and [remote driver](../../../../../../../../tools/native_restore_sync_remote.py)
extend this to actual nodes talking to a loopback archive. The fixture retimes a
saved solo genesis to timestamp zero, recomputes its ID, and verifies unchanged
transaction body and MMR bytes plus a valid block difficulty. Both runtimes produce
identical saved tree and IBD encodings. The real backup command writes each input.

The server records the exact requests. A nonempty retained-block reply advances
requests from -1 (preflight) to 1 and then 2. An empty archive stops at request 1.
Failed preflight tries -1 three times and leaves restored key uses at seven;
failure at request 2 comes after the increment and leaves them at 263. Both return
the same restart message. Thus checking only that message would miss an essential
state difference. Opposite-runtime reopen checks every key, chain and coin status,
valid signing, altered-data rejection and exactly one signing-use increment.
All eight workflows pass with 19 actual node processes in each build. See the
[saved remote results](../../../../../../../../validation/restoresync-remote-work-in-progress).

## Limits

Old backups can be partly restored before host parsing/connection fails. Exact-height
polling can fail if the processor passes the expected end height; late errors may
still lead to a success response. The retained genesis exercises duplicate-block
processing; it does not prove advancement to a new height. Memory-threshold
resets, cascades, multiple nonempty batches, exact age-boundary timing, partial stream failure
boundaries and full public API parity still require validation.

## Check your understanding

Why can a failed archive connection still lead to the caller returning a restart message?

Pinned Java SHA-256: `4eaff23d3afc7e26d2e15df39bb97613787e03a21dc0030fb26246d0dfe9832c`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
