# test — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/test.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/test.cpp)

## Purpose

The test command’s active runCommand refreshes SQL databases and logs around that operation. A separate main method experiments with generated address lengths; neither is an automated assertion suite.

## Follow the command

1. Create the common reply, log About to close and reopen DBs, and call MinimaDB.refreshSQLDB.
2. Only after refresh returns, log DBs reopened.. and return the envelope without a response field. Accepted show/action options are not read.
3. A private resource helper asks the class loader for a stream and logs when it returns null; runCommand does not invoke it.
4. The standalone main generates 100000 random 32-byte values, converts each to a Minima address, and prints deviations from length 63. It does not throw on a deviation or return a failed test status.

## Translation and dependencies

The native command reuses Command replies, MinimaDB.refreshSQLDB, MinimaLogger, MiniData random bytes and Address encoding. It is registered in the actual native runner. The full100000-iteration demonstration is a separate native entry point. The unused private class-loader resource helper remains unrepresented because there is no native classpath resource service; this is an explicit remaining fidelity gap.

## Understanding the tests

Both differential runs pass274programs/616operations and114independent assertions. It reuses the existing database fixture to compare actual SQL refresh calls, identities, locks, logs and retained state;50SQL failure points are exercised for both checked and runtime exceptions. Cases cover loaded/unloaded databases, repeated calls, ignored typed parameters, direct/registered calls and non-admin callers. Both actual mains complete all100000iterations; the driver requires only the expected start/finish output, since a32-byte input has a fixed63-character address length.

Both full builds and actual-node H2/restart checks pass. Each build runs four Java/native node processes,12refresh commands and verifies eight wallet keys before/after restart. Both complete100000-iteration demonstrations pass. See the [lab](../../../../../../../../student-guide/new-tests/database-test-command.md) and [evidence](../../../../../../../../validation/base-test-command-work-in-progress/progress.json) for completed phases.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

The test plan was written before execution. Exact source/main behavior is tested separately from successful compilation.

## Limits

Invoking this command can close/reopen databases; its name does not imply harmless observation. Validation uses isolated fixture or temporary-node databases. The unused private resource helper remains outside represented behavior.

## Check your understanding

Which output proves that refresh returned normally, and why is printing a non-63 address length not an assertion failure?

Pinned Java SHA-256: `b15b044f3f158eaf4453e5b2364ff4ff55c4616d1a1dd841dc50eadcec9e6820`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
