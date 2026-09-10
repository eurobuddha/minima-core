# logs — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`. Finite command-specific comparison passes in both builds.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/logs.java) · [Native C++ body](../../../../../../../../src/org/minima/system/commands/base/logs.cpp)

## Purpose

logs selectively changes global logging flags and returns their current values. Omitted options preserve prior settings; supplied strings enable a flag only when exactly equal to true after shared trimming.

## Follow the command

1. Process scripts, mining, maxima, blocks, networking, ibd and peerschecker in source order. Each present option sets the corresponding GeneralParams boolean to the result of an exact true comparison.
2. Process txpowdb last; it changes both TxPoWSqlDB.LOG_ADD_BLOCKS and LOG_ADD_TRANSACTION together.
3. Build response fields from current globals, reporting txpowdb using only LOG_ADD_BLOCKS. No persistence call occurs in this command.
4. Parameter parsing and mutation are interleaved: a later invalid/blank/null parameter can fail after earlier flags have already changed.

## Translation and dependencies

The wrapper reuses the existing Command getters and global flag owners. Each present parameter is parsed immediately before its assignment, preserving the source's fixed order and partial changes. The seven GeneralParams logging fields now use the same std::atomic<bool> storage pattern already used by TxPoWSqlDB. Reads and writes of each field are safe for native worker access; the whole group is still not an atomic transaction.

The parameter fixture snapshots use load() to copy bool values, and the processor fixture assigns its two affected flags directly instead of storing bool pointers. These changes adapt test access to the actual production storage without substituting test-only flags. A source search found MAXIMA_LOGS only in its declaration, reset, and this command; storing a setting alone does not demonstrate an active logging consumer.

## Understanding the tests

The [logging corpus](../../../../../../../../tools/logs_commands_corpus.py) contains 327 programs and 982 operations. It seeds all nine actual flags, including the transaction flag omitted from the command response. Each parameter is crossed with eighteen possible inputs and two mixed prior states. The independent checker calculates which earlier assignments survive, where parsing fails, and which later values must remain untouched.

An omitted txpowdb preserves deliberately different block and transaction flags. A supplied value updates both. Unicode whitespace and embedded control characters distinguish Java trimming from broader whitespace rules. Metadata, fresh factories, runner permissions and unknown parameters are also compared. Finally, Main.setSyncIBD provides a real consumer: its log appears only when the command enabled ibd.

The [lab](../../../../../../../new-tests/logging-flags-command.md) was written before the tests. Both full builds pass, and all 327 programs / 982 operations match Java in normal and ASAN/UBSAN builds. All 25 affected consumer regressions also pass in both builds (194.27s / 457.64s). All 42 saved result reports were read without failures; the lab links to their scopes and limitations.

## Limits

A reply is not a transactional snapshot. Individual atomics prevent native data races for these fields, but do not reproduce every racy Java schedule. Full-node startup and every subsystem's logging remain outside this finite command comparison. Existing consumer tests are rerun as separate evidence.

## Check your understanding

If scripts:true succeeds but txpowdb is blank, does the source roll back SCRIPTLOGS?

Pinned Java SHA-256: `4f3cd7cae9824d9c4bbad52b345db83d4268279fef5766464b2822f8b7ada43e`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
