# test — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/test.java) · `src/org/minima/system/commands/base/test.cpp` (absent)

## Purpose

The test command’s active runCommand refreshes SQL databases and logs around that operation. A separate main method experiments with generated address lengths; neither is an automated assertion suite.

## Follow the command

1. Create the common reply, log About to close and reopen DBs, and call MinimaDB.refreshSQLDB.
2. Only after refresh returns, log DBs reopened.. and return the envelope without a response field. Accepted show/action options are not read.
3. A private resource helper asks the class loader for a stream and logs when it returns null; runCommand does not invoke it.
4. The standalone main generates 100000 random 32-byte values, converts each to a Minima address, and prints deviations from length 63. It does not throw on a deviation or return a failed test status.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse MinimaDB refresh and logger behavior. The command and standalone address experiment are separate entry points and require separate validation.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed command tests record log→refresh→log order and inject refresh failure to require absence of the final success log. Proposed main coverage compares address encoding properties and explicitly asserts the intended invariant rather than treating diagnostic prints as a pass.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

Invoking this command can close/reopen databases; its name does not imply harmless observation. No database was touched while writing the lesson.

## Check your understanding

Which output proves that refresh returned normally, and why is printing a non-63 address length not an assertion failure?

Pinned Java SHA-256: `b15b044f3f158eaf4453e5b2364ff4ff55c4616d1a1dd841dc50eadcec9e6820`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
