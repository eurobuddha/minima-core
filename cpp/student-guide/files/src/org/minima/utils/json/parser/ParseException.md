# ParseException — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/json/parser/ParseException.java) · [Mapped C++ body](../../../../../../../../src/org/minima/utils/json/parser/ParseException.cpp) · [C++ header](../../../../../../../../src/org/minima/utils/json/parser/ParseException.hpp)

## Purpose

ParseException carries a mutable error category, position, and unexpected object. Its message is calculated from those fields, so mutating a field changes later diagnostic text.

## Follow the algorithm

1. Constructors default the position to -1 and unexpected object to null unless supplied. Categories distinguish unexpected character, token, and exception.
2. Getters/setters expose all three fields. Position is the zero-based character position reported by the lexer, not a byte offset.
3. getMessage selects a category-specific sentence and appends the unexpected object through Java string conversion. Unknown categories use the source spelling Unkown error at position ... .
4. Because the unexpected value is an Object reference, its toString may change or fail when getMessage is called later.

## How the C++ translation works

C++ retains mutable category/position and formats the same sentence templates. It stores unexpectedObject as a string snapshot rather than an arbitrary live Object. what() caches the rendered message and catches failures to satisfy noexcept; NativeStackTrace records a native boundary trace. These are meaningful exception-model adaptations.

## Understanding the tests

The JSON fixture catches ParseException separately and emits error type, position, exact getMessage text, and already-emitted lexer/events. Malformed braces, key types, invalid escapes, truncated input, and extra root values exercise both character and token categories. Non-ParseException failures become generic ERROR.

- [Java DOM, lexer, and event fixture](../../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Direct constructor/setter behavior, live unexpected-object mutation, unknown categories, and getMessage failure are not specifically asserted. A future test should mutate every field after construction and compare subsequent messages.

## Check your understanding

Why can an error after an emoji have a different UTF-16 character position from its UTF-8 byte offset?

Pinned Java SHA-256: `4368aadd36aa983c8ab24eb48d87b292af055988ceadcbe510bf6255869643f7`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
