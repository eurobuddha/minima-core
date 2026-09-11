# MiniFormat — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/MiniFormat.cpp) · [C++ header](../../../../../../src/org/minima/utils/MiniFormat.hpp)

## Purpose

These are display helpers for JSON, byte counts, durations, and padded numbers. Their output is an observable interface. Some rules are intentionally unusual, so a more conventional pretty printer or duration library would not be a 1:1 substitution.

## Follow the algorithm

1. convertToJSON parses text and casts the result to JSONArray. JSONPretty(array) assumes its elements are objects; it adds square brackets only when there is more than one element. Thus an empty array formats as empty text and a one-object array loses its outer brackets.
2. JSONPretty(object) scans its serialized text for the next quote, opening brace, closing brace, or comma. Braces change indentation, commas add newlines, and quoted spans are copied. It then replaces escaped slashes and newline sequences. The quote search simply finds the next quote; it is not a complete JSON lexer. On caught failure it returns the original text.
3. formatSize returns integer bytes below 1024. Otherwise it chooses a 1024-based unit from the bit length and prints one decimal place. zeroPad first floors the MiniNumber, then prepends zero characters until the requested text length is reached, including any minus sign in that length.
4. ConvertMilliToTime decomposes whole days into 365-day years,30-day months,7-day weeks, and remaining days, then derives hours/minutes/seconds from the original milliseconds. This is a display decomposition, not calendar arithmetic.
5. createRandomString fills bytes from java.util.Random, treats them as an unsigned integer, and formats uppercase radix 32. Leading zero bytes may shorten the output; length is an input byte count, not a promised character count.

## How the C++ translation works

The native port follows the same scanning and bracket quirks rather than using a general formatter. Integer division preserves truncation toward zero for durations. Size formatting explicitly rounds to one decimal and uses the classic C++ locale. Random-string generation reuses the Java 48-bit generator recurrence after obtaining a native seed. Debug logging uses std::clog and differs from the Java MinimaLogger calls.

## Understanding the tests

The utility driver compares size/time output for negative values, zero,1023/1024 boundaries, signed 64-bit extremes, and random large values. It tests padding with negative/fractional numbers and varied target lengths. JSON examples include empty arrays/objects, one and two objects, a primitive array, nested data, escaped quotes/newlines, and Unicode. Each output is encoded as hex by the fixtures and compared exactly, preserving whitespace differences. Random strings and debug=true are not part of that inspected driver.

- [Java utility fixture](../../../../../../support/UtilityOracle.java)
- [Native utility fixture](../../../../../../support/utility.cpp)
- [Utility differential driver](../../../../../../tools/utility_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Locale-sensitive String.format behavior, debug logger failures, all malformed pretty-print corner cases, and random-seed semantics remain outside this fixture. Display output should not be fed back into a JSON parser on the assumption that it preserves valid JSON. Proposed direct tests should cover negative zero-padding and a string containing an escaped quote followed by a comma.

## Check your understanding

Predict zeroPad(5,−1.1) after the floor operation. Why should a test compare literal whitespace for JSONPretty rather than parse both results and compare the resulting objects?

Pinned Java SHA-256: `580c7aee6fcd3539b3cf1d3953a1d9e53143c2af7866f9fb9976f3c9716f014f`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).

## Original entry-point follow-up

The original main keeps its full 9954-byte sample, including the data URL. JSONPretty now trims work text, logs UTF-16 length plus a second serialization through MinimaLogger, and logs caught failure without notification. A leading unmatched quote leaves the Java cursor at zero; native code preserves that loop and tests isolate it in child processes.

The [utility main lab](../../../../../new-tests/utility-main-methods.md) records five passing entry-point comparisons and
36 formatter cases (31 finite, five bounded loop observations) in both builds.
Existing utility and original POW regressions also pass. This is scoped evidence;
complete file API and behavior parity remain unverified.
