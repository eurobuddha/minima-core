# JSONStreamAware — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONStreamAware.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/json/JSONStreamAware.cpp) · [C++ header](../../../../../../../src/org/minima/utils/json/JSONStreamAware.hpp)

## Purpose

JSONStreamAware lets an object append its representation to a supplied JSONWriter. It avoids requiring the object to return a separate complete string and allows IOExceptions to propagate in Java.

## Follow the algorithm

1. An implementation receives the existing writer and appends its representation at the current position.
2. JSONValue checks this interface before JSONAware and before generic map/list handling. When both awareness interfaces are implemented, this callback has priority.
3. The interface supplies no buffer allocation, escaping, rollback, or validation. If a callback writes a prefix and then throws, the prefix can remain in the writer.

## How the C++ translation works

The native interface declares a const pure virtual writeJSONString(JSONWriter&) and virtual destructor. JSONObject/JSONArray implement it. However JSONValue::writeJSONString currently renders its variant to a whole string before appending; it does not provide Java’s arbitrary stream-aware dispatch or all partial-write behavior.

## Understanding the tests

The JSON fixture tests supported container output through serialization but has no custom streaming object that deliberately fails after a prefix. Thus exact final strings provide useful indirect coverage while callback ordering, partial output, and exception propagation remain untested for this interface.

- [Java DOM, lexer, and event fixture](../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

A proposed test should start a writer with a sentinel, call a custom stream-aware object that writes then throws, and compare both exception and retained buffer. Also test precedence over JSONAware with an object implementing both.

## Check your understanding

Why can a full-string-first native implementation produce the same successful output but leave different state after a failure?

Pinned Java SHA-256: `4e431b9022acd69538b660c91b4f5a78a02de098aae21b178d611528cf2ea109`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
