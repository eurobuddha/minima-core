# JSONValue — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONValue.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/json/JSONValue.cpp) · [C++ header](../../../../../../../src/org/minima/utils/json/JSONValue.hpp)

## Purpose

JSONValue is the dispatch layer between arbitrary Java objects and JSON text. It also provides parsing conveniences with different failure behavior. The native class additionally serves as the tagged representation of supported JSON values.

## Follow the algorithm

1. parse(Reader) catches Exception and returns null; parseWithException propagates parsing/IO failures. In parse(String), StringReader construction happens before the catching Reader wrapper, so null input can still fail. A returned null is ambiguous between valid JSON null and ignored failure.
2. Serialization checks types in order: null, String, Double/Float, other Number, Boolean, JSONStreamAware, JSONAware, Map, Collection, primitive arrays, Object[], then fallback toString. Stream-aware output wins over string-aware output when both interfaces exist.
3. Strings are escaped and quoted; nonfinite boxed floats/doubles become null. Other numbers and fallback object text are written without JSON validity checks.
4. escape walks UTF-16 code units. It uses short escapes for quote, slash, backslash and standard controls, and uppercase four-digit Unicode escapes for U+0000–001F, U+007F–009F, and U+2000–20FF. Other code units are retained.

## How the C++ translation works

C++ uses a variant for null, bool, signed 64-bit integer, double, UTF-8/UTF-16 strings, RawJSON, and shared JSONObject/JSONArray. javaFloating preserves observed Java number rendering. RawJSON permits deliberate raw text but is not the full arbitrary-object/interface dispatch mechanism. Generic JSONAware/JSONStreamAware values, Float-specific identity, primitive arrays, and Java Reader overload behavior remain incomplete.

## Understanding the tests

The JSON driver compares reserialized values and final token positions for fixed numeric boundaries, hundreds of random finite doubles, control characters, malformed escapes, and nested structures. Infinity reached through 1e999 is serialized to null, while signed zero and subnormal cases check exact text. However, the fixture invokes JSONParser directly, so it does not directly establish the catch-and-return-null parse wrapper behavior.

- [Java DOM, lexer, and event fixture](../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Complete object dispatch and exception semantics need direct tests. A useful fixture should implement both awareness interfaces with different output and show stream precedence, then test malformed input through both parsing convenience methods.

## Check your understanding

Why can parse return null for two entirely different reasons? Why must a custom collection serializer avoid calling JSONValue.writeJSONString(this) recursively?

Pinned Java SHA-256: `53b218fe11a2f551527555024ed0dfa74559a9ed0d0e57cfddfe7fa317700942`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
