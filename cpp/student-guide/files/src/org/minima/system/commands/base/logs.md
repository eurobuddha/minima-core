# logs — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/logs.java) · `src/org/minima/system/commands/base/logs.cpp` (absent)

## Purpose

logs selectively changes global logging flags and returns their current values. Omitted options preserve prior settings; supplied strings enable a flag only when exactly equal to true after shared trimming.

## Follow the command

1. Process scripts, mining, maxima, blocks, networking, ibd and peerschecker in source order. Each present option sets the corresponding GeneralParams boolean to the result of an exact true comparison.
2. Process txpowdb last; it changes both TxPoWSqlDB.LOG_ADD_BLOCKS and LOG_ADD_TRANSACTION together.
3. Build response fields from current globals, reporting txpowdb using only LOG_ADD_BLOCKS. No persistence call occurs in this command.
4. Parameter parsing and mutation are interleaved: a later invalid/blank/null parameter can fail after earlier flags have already changed.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse the global flag owners and shared Command getters, preserving partial mutation order rather than prevalidating the entire request.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed cases initialize mixed flag values, omit all parameters, update one at a time, compare true versus TRUE/false, and force a late parsing failure after an earlier successful change. Assert both TxPoWSqlDB flags even though only one is returned.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

Logging behavior of each subsystem is separate from this flag-setting wrapper. A response alone cannot reveal a mismatch in the unreported transaction flag.

## Check your understanding

If scripts:true succeeds but txpowdb is blank, does the source roll back SCRIPTLOGS?

Pinned Java SHA-256: `4f3cd7cae9824d9c4bbad52b345db83d4268279fef5766464b2822f8b7ada43e`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
