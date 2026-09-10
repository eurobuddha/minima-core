# JSONArray — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/json/JSONArray.cpp) · [C++ header](../../../../../../../src/org/minima/utils/json/JSONArray.hpp)

## Purpose

JSONArray is an ordered list with compact JSON serialization. Its many static overloads also serialize Java primitive arrays, whose behavior is not always the same as serializing boxed values in a Collection.

## Follow the algorithm

1. Construction creates an empty ArrayList or shallow-copies collection elements in iterator order. List mutation behavior otherwise comes from ArrayList.
2. Collection serialization writes brackets, inserts commas between elements, emits null directly for null entries, and delegates other elements to JSONValue. A null collection writes null; an empty one writes [].
3. Primitive numeric and boolean array overloads write String.valueOf for each element directly. Thus a primitive floating array can emit NaN or Infinity, while JSONValue normally maps boxed nonfinite floating values to null.
4. The char-array overload wraps each raw character in quotes without JSON escaping, so a quote/backslash character can produce invalid JSON. Object-array serialization delegates each element to JSONValue. These source peculiarities are part of the translation task, not automatically corrected behavior.

## How the C++ translation works

The native vector<JSONValue> implements the ordinary ordered collection surface and shared nested values. It exposes add/get/size plus selected string search/removal helpers and serializers. The source’s full ArrayList inheritance and static primitive/char/Object array overload set are absent, so one mapped file does not mean complete API parity.

## Understanding the tests

The JSON fixture compares parsed arrays containing numbers, booleans, null, nested objects, escaped strings, and permissive comma syntax. Exact output preserves order, brackets, and null positions. Event-mode comparisons independently observe start/end array and each primitive. These tests exercise ordinary parsed JSONArray serialization, not the source’s primitive-array overloads.

- [Java DOM, lexer, and event fixture](../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

A proposed direct fixture should compare float[] containing NaN against a JSONArray containing boxed NaN, and char[] containing a quote against a list containing a one-character string. No assertion for those overloads is claimed here.

## Check your understanding

Why can two Java containers holding conceptually identical characters serialize differently depending on whether they are char[] or a Collection?

Pinned Java SHA-256: `c37502757d18942d282ef3d53f572e6bf6ab7396151bb4b64788b51233b15073`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
