# JSONWriter — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONWriter.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/json/JSONWriter.cpp) · [C++ header](../../../../../../../src/org/minima/utils/json/JSONWriter.hpp)

## Purpose

JSONWriter is an in-memory append buffer used by the serializers. Its write(int) overload writes one Java char, not the decimal digits of an integer.

## Follow the algorithm

1. Construction creates an empty StringBuffer.
2. write(String) appends the supplied text. Java StringBuffer.append(String) turns null into the literal null.
3. write(int) narrows the integer to a 16-bit char before appending. For example 65 appends A and 65536 appends a zero code unit.
4. getFullString and toString return a snapshot String of the current buffer; neither clears it.

## How the C++ translation works

The native class appends std::string bytes and converts write(int) through a 16-bit mask into UTF-8-form bytes. It returns a const reference to its buffer instead of a Java immutable snapshot. It does not accept nullable strings, reproduce StringBuffer synchronization, or combine successive surrogate halves as a complete UTF-16 buffer would.

## Understanding the tests

The JSON differential indirectly tests writer punctuation and string assembly because both fixtures serialize DOM values/events/errors into text for exact comparison. It does not directly call write with arbitrary integer code units, null strings, concurrent writers, or retained getFullString results.

- [Java DOM, lexer, and event fixture](../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

A proposed direct test should retain the result of getFullString, append again, and examine snapshot-versus-reference behavior. Another should write the two surrogate halves of an emoji separately and compare final encoding. Those are uncovered native adaptations.

## Check your understanding

What does write(49) append, and why is it different from write("49")?

Pinned Java SHA-256: `b77cc1ce18e2f984b6b6e2b1a53d33533ed60799e46eb2468e89ceab37a77181`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
