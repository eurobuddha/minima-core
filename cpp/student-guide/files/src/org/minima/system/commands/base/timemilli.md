# timemilli — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/timemilli.java) · `src/org/minima/system/commands/base/timemilli.cpp` (absent)

## Purpose

timemilli reports the current wall clock, optionally shifted backward by a requested number of minutes or hours.

## Follow the command

1. Read System.currentTimeMillis once.
2. If minutesback exists, parse it as MiniNumber and narrow with getAsInt; otherwise, if hoursback exists, do the corresponding hours conversion. Minutes takes precedence when both are supplied.
3. The multiplication 1000*60*value or 1000*60*60*value occurs in Java int arithmetic before assignment to long. Large values can wrap at 32 bits, then that wrapped offset is subtracted from the long timestamp.
4. Construct Date from the shifted timestamp, emit numeric timemilli and date text, and return the common reply.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse MiniNumber narrowing and the native Java date adapter. Explicit unsigned-width arithmetic is needed to preserve source int overflow without undefined signed C++ overflow.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests freeze the clock and compare absent parameters, both parameters together, negative/fractional values and offsets just across 32-bit multiplication overflow. Date formatting should be checked separately across zones.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

A normal current-time result does not establish overflow, parameter precedence or timezone behavior. Negative offsets move the timestamp forward.

## Check your understanding

Why does storing minback in a long fail to prevent overflow in the multiplication that initializes it?

Pinned Java SHA-256: `cc3b6cfe6d1ddfa751d7776ed9f267a97b8bb02f8e10ea5f914649ea780ba6c6`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
