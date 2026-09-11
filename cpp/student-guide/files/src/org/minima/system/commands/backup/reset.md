# reset — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/reset.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/backup/reset.cpp)

## Purpose

reset sequences archive import and optional wallet restore commands. It returns nested command records and does not propagate every nested failure into its own status.

## Follow the command

1. Require all keys created, then archivefile and action. chainsync builds archive action:import with file and stores the first nested result under response, without checking its status.
2. seedsync adds the quoted required phrase and optional keys/keyuses text to the archive import command. It similarly returns the first result directly.
3. restore runs restore shutdown:false with file and optional password. This first nested status is checked; failure throws an Error restoring message containing the nested JSON.
4. After successful restore, call restoreReadyForSync, then archive import. Return an object containing restore and chainsync results without checking the latter status. Unknown actions throw.

## Translation and dependencies

The native body preserves the source sequence and reuses CommandResult.first/status,
CommandRunner, vault's key gate and Main.restoreReadyForSync. The helper selects the
first nested result exactly as the Java code does. Status unboxing preserves null
and class-cast failures. Source help, valid parameters and fresh factory are mapped.
Main.restoreReadyForSync is now virtual to match Java's override boundary; its
processor creation and database reopening body is unchanged.

Paths, passwords and numeric-looking strings are concatenated without new escaping.
Only the seedsync phrase is quoted. As a result, spaces or semicolons can alter how
the actual runner parses the nested command. Preserving that behavior is part of
this port; the wrapper does not validate the archive file itself.

## Understanding the tests

The [pre-test lab](../../../../../../../../student-guide/new-tests/reset.md) explains
what the fixture observes. The reused
[Java oracle](../../../../../../../../support/RestoreOracle.java) and
[native fixture](../../../../../../../../support/restore.cpp) install scoped archive
and restore probes in the actual runner. They record complete child text, parsed
parameters, caller ID and readiness order. They restore the original registry after
each request, so a fake archive command is never part of the production registry.

The [driver](../../../../../../../../tools/reset_differential.py) covers all actions,
required/optional parameters, key-gate ordering, null and wrong types, Unicode,
spaces/quotes/semicolons, nested failures, malformed statuses, metadata/factory and
admin permission. It compares complete replies and file/store observations. Direct
NPE/class-cast prose is normalized. Embedded Error restoring JSON is parsed and
compared structurally, retaining all keys and values.

Four integration cases execute the actual restore and Main.restoreReadyForSync,
using both Java- and C++-generated backups and real H2. They require wallet uses
5→261 and 100→356, six relevant history rows, source preferences, a newly created
processor and replaced/reopened wallet/archive/history stores. They include a
failed archive result after successful restore. Test-owned workers and both old
and new SQL connections are closed after observations.

The archive probe returns a controlled result. These cases establish reset's
control flow and restore/reopen integration. Archive is now mapped and its actual
local synchronization has separate runtime evidence.
See [validation](../../../../../../../../validation/reset-work-in-progress) for
final build/test counts and coverage limits. Both normal and ASAN/UBSAN comparisons
pass 224 cases, including all four real restore/reopen cases.

The [live archive lab](../../../../../../../../student-guide/new-tests/archive-live-node.md)
adds actual reset action:restore in both runtimes with a genuine genesis archive.
It checks both nested statuses, clean shutdown, opposite-runtime reopen, exact
chain/coin state, all eight key-use counts at 263 and restored-key signing. Both
normal and sanitizer builds pass. This covers a real reset path without the archive
probe; cascades, multi-batches and seed replacement still need validation.

## Limits

There is no rollback if archive import fails after the wallet has been restored.
Complete reset/chain synchronization, concurrent mutation and full
public API parity remain unverified. All filesystem mutations in these tests occur
in isolated fixture directories.

## Check your understanding

Why can outer status be true while response.chainsync.status is false?

Pinned Java SHA-256: `645ca7ebbb13ac649ad21330d618ac0705536a3c71c54a69ffc97544b5d4f6d5`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
