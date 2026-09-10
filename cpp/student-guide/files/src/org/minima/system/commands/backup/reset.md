# reset — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/reset.java) · `src/org/minima/system/commands/backup/reset.cpp` (absent)

## Purpose

reset sequences archive import and optional wallet restore commands. It returns nested command records and does not propagate every nested failure into its own status.

## Follow the command

1. Require all keys created, then archivefile and action. chainsync builds archive action:import with file and stores the first nested result under response, without checking its status.
2. seedsync adds the quoted required phrase and optional keys/keyuses text to the archive import command. It similarly returns the first result directly.
3. restore runs restore shutdown:false with file and optional password. This first nested status is checked; failure throws an Error restoring message containing the nested JSON.
4. After successful restore, call restoreReadyForSync, then archive import. Return an object containing restore and chainsync results without checking the latter status. Unknown actions throw.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse restore/archive and CommandRunner. Preserve exact quoting and status handling; this wrapper coordinates existing actions rather than implementing file decoding.

## Understanding the tests

The runner fixture checks reset’s name/permission category only. Proposed tests record nested text and order, failure before/after restore, first-array-result handling, spaces/quotes in paths and phrases, and outer status when chainsync fails. Assert restoreReadyForSync occurs only after a successful restore result.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

There is no rollback if archive import fails after the wallet has been restored. No reset, deletion or nested command was executed for the lesson.

## Check your understanding

Why can outer status be true while response.chainsync.status is false?

Pinned Java SHA-256: `645ca7ebbb13ac649ad21330d618ac0705536a3c71c54a69ffc97544b5d4f6d5`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
