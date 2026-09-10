# Yytoken — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/json/parser/Yytoken.java) · [Mapped C++ body](../../../../../../../../src/org/minima/utils/json/parser/Yytoken.cpp) · [C++ header](../../../../../../../../src/org/minima/utils/json/parser/Yytoken.hpp)

## Purpose

Yytoken is the small record passed from lexical analysis to parsing. A numeric type identifies punctuation, a primitive value, or EOF; the value field stores primitive data when relevant.

## Follow the algorithm

1. The constructor copies the supplied type and value into public mutable fields. No validation rejects unknown types or inconsistent type/value pairs.
2. For TYPE_VALUE, toString writes VALUE(...) using the value’s ordinary string conversion. String values are not JSON-quoted in this diagnostic.
3. Punctuation types use fixed diagnostic labels such as LEFT BRACE({); EOF uses END OF FILE. Unknown types return the empty string.
4. The parser creates an EOF token when the lexer returns null, so lexer end-of-input and parser token diagnostics use different representations.

## How the C++ translation works

The native record retains public type and JSONValue payload. Its diagnostic switch mirrors the named types. Supported string payloads render raw, while other values use JSON serialization; this can differ from Java Object.toString for arbitrary objects or nonfinite doubles, which Java permits in a manually constructed token.

## Understanding the tests

The JSON lexer fixture records token type/value/position directly; parse-error fixtures also include Yytoken.toString in exact error messages. Inputs cover punctuation, booleans/null, integer/double distinctions, strings and malformed grammar. The fixture constructs tokens through the lexer, not arbitrary public field mutation.

- [Java DOM, lexer, and event fixture](../../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Unknown type numbers, nonfinite diagnostic payloads, arbitrary objects, and mutation need direct record tests. A lexer match does not establish every public Yytoken construction.

## Check your understanding

Why does a string token diagnostic VALUE(abc) differ from serializing the same primitive as JSON text?

Pinned Java SHA-256: `c2d66fe99d41a37cbecbec5d5814fe05bf276ca315dc3d6dbb62ab47e72af8e7`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
