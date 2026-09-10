# JSONAware — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONAware.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/json/JSONAware.cpp) · [C++ header](../../../../../../../src/org/minima/utils/json/JSONAware.hpp)

## Purpose

JSONAware is a callback interface allowing an object to supply its own complete JSON text. It declares one method and does not guarantee that the returned text is valid JSON.

## Follow the algorithm

1. An implementing class defines toJSONString and returns the entire representation as a String.
2. JSONValue dispatches to this method after checking scalar types and JSONStreamAware, but before its Map and Collection fallbacks.
3. JSONObject/JSONArray use their direct map/list serialization internally so their awareness methods do not recursively redispatch to themselves.

## How the C++ translation works

The native abstract class has a virtual const toJSONString method and virtual destructor defined in its mapped .cpp. JSONObject, JSONArray, and JSONValue implement it. The native tagged JSONValue does not accept an arbitrary JSONAware object, so general Java callback dispatch remains narrower.

## Understanding the tests

The JSON fixture exercises toJSONString on supported native containers when producing DOM and event outputs, comparing their exact strings with Java. No custom awareness implementation is supplied by that fixture; it therefore cannot establish interface dispatch precedence or propagation of a custom callback failure.

- [Java DOM, lexer, and event fixture](../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

A future fixture should implement only JSONAware and another object implementing both awareness interfaces, giving each method visibly different text. It should verify which callback runs and how null/invalid returned text is handled.

## Check your understanding

Why is satisfying an interface signature different from proving that every serializer dispatches to it at the right point?

Pinned Java SHA-256: `8d25a84fe95d3946ae09dfbf47c59d10cfc75dafe9dd9125668b09f5f12e13bd`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
