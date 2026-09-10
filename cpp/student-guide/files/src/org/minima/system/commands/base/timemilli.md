# timemilli — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`. The native command compiles and has finite differential coverage.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/timemilli.java) · [Native C++ body](../../../../../../../../src/org/minima/system/commands/base/timemilli.cpp)

## Purpose

timemilli reports the current wall clock, optionally shifted backward by a requested number of minutes or hours.

## Follow the command

1. Read System.currentTimeMillis once.
2. If minutesback exists, parse it as MiniNumber and narrow with getAsInt; otherwise, if hoursback exists, do the corresponding hours conversion. Minutes takes precedence when both are supplied.
3. The multiplication 1000*60*value or 1000*60*60*value occurs in Java int arithmetic before assignment to long. Large values can wrap at 32 bits, then that wrapped offset is subtracted from the long timestamp.
4. Construct Date from the shifted timestamp, emit numeric timemilli and date text, and return the common reply.

## Translation and dependencies

The wrapper reuses Command, TimerMessage.currentTimeMillis, MiniNumber.getAsInt and the existing JavaDateString adapter. Multiplication uses unsigned 32-bit arithmetic followed by a bit-preserving signed conversion; subtraction uses the corresponding 64-bit pattern. This preserves Java wraparound while avoiding undefined signed C++ overflow.

## Understanding the tests

The shared trace/clock corpus runs 209 programs and 652 operations per build. Time cases include absent parameters, null and wrong types, malformed numbers, negative and fractional offsets, integer narrowing, and values crossing multiplication overflow. Crossed minute/hour parameters prove that minutes takes priority, including when the ignored hours value is invalid.

The fixture samples the real clock immediately before and after each operation. The Python checker independently truncates and wraps the offset, adds it back to the returned timestamp, and requires that reconstructed clock to lie between those samples. It also checks the exact UTC date against the returned milliseconds. Only after both assertions succeed are separately sampled timestamps normalized for comparison. This avoids requiring Java and C++ to run in the same millisecond.

Both full builds and all eleven affected regressions pass in normal and ASAN/UBSAN builds. Metadata also preserves the leading tab in Java's full help. See the [trace and clock lab](../../../../../../../new-tests/trace-and-clock-commands.md).

## Limits

UTC date coverage does not certify every timezone, locale, historical calendar or external clock adjustment. Negative offsets move the timestamp forward. Finite arithmetic cases are evidence for the translation, not an exhaustive proof of the whole runtime.

## Check your understanding

Why does storing minback in a long fail to prevent overflow in the multiplication that initializes it?

Pinned Java SHA-256: `cc3b6cfe6d1ddfa751d7776ed9f267a97b8bb02f8e10ea5f914649ea780ba6c6`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).

