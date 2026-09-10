# MiniUtil — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/MiniUtil.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/MiniUtil.cpp) · [C++ header](../../../../../../src/org/minima/utils/MiniUtil.hpp)

## Purpose

This class supplies a fixed date formatter and two conversions between a string list and a JSON array. The conversions create new containers but preserve element order and null values. They do not parse arbitrary JSON values into strings.

## Follow the algorithm

1. The static Java formatter is created with pattern dd_MM_yyyy_HHmmss and English locale. Its time zone and calendar behavior remain part of the DateFormat implementation.
2. convertArrayList allocates a JSONArray, walks strings in order, appends each reference, and returns the new array.
3. convertJSONArray allocates an ArrayList, visits each JSON element, casts it to String, and appends it. Null survives the cast, but a number or object does not become text automatically.

## How the C++ translation works

C++ uses vector<optional<string>> and additional UTF-16 variants so null and Java text can remain distinguishable. A new JSON container is allocated in each conversion. Date formatting uses ICU for dates representable accurately by a double; extreme millisecond values use an integer/calendar path and localtime_r. This is a native approximation of the observable formatter surface, not a clone of every mutable SimpleDateFormat method.

## Understanding the tests

The preferences fixture has a dedicated date request that directly invokes MiniUtil.DATEFORMAT in both languages. Its driver compares exact strings under TZ=UTC for the Unix epoch, negative milliseconds, the Gregorian calendar transition, dates beyond 2^53 milliseconds, and both signed 64-bit extremes. Native preference-list rendering also calls convertArrayList; source array/list setters include strings, null, and emoji. This gives concrete date and list integration coverage, but it is not a standalone exhaustive test of both conversion overload families.

- [Java preferences fixture](../../../../../../support/PrefsOracle.java)
- [Native preferences fixture](../../../../../../support/prefs.cpp)
- [Preferences differential driver](../../../../../../tools/prefs_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The Java formatter is a shared mutable object; the C++ surface exposes a formatting wrapper, so mutation, parsing, concurrency, and arbitrary-zone equivalence are not established here. A useful direct conversion test would round-trip ["a",null,"🐼"], verify order and null preservation, and separately require a numeric JSON element to fail.

## Check your understanding

Why is creating a new list different from converting every element with toString? Which additional inputs would you choose to test the difference between UTF-8 bytes and Java UTF-16 text?

Pinned Java SHA-256: `d9b33775242506c7ef93b10d7df3305eeaad7f125d0d29bcfb847c7459ade9a8`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
