# ContainerFactory — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/json/parser/ContainerFactory.java) · [Mapped C++ body](../../../../../../../../src/org/minima/utils/json/parser/ContainerFactory.cpp) · [C++ header](../../../../../../../../src/org/minima/utils/json/parser/ContainerFactory.hpp)

## Purpose

ContainerFactory allows the parser to ask a caller which object/map and array/list containers to allocate. Returning null requests the parser’s normal JSONObject or JSONArray.

## Follow the algorithm

1. When JSONParser sees an opening brace, it calls createObjectContainer if a factory exists. A null result is replaced with a fresh JSONObject.
2. An opening square bracket similarly calls creatArrayContainer, whose misspelling is part of the source method name, and falls back to JSONArray on null.
3. The returned container is inserted into its parent before nested parsing continues. The factory can therefore observe allocation order and may retain references to partially populated containers.

## How the C++ translation works

The native abstract interface returns shared_ptr<JSONObject> and shared_ptr<JSONArray>, preserving nullable fallback and shared container identity. Java permits arbitrary Map/List implementations; those cannot all fit the native concrete return types. Native destructors are virtual, but container mutation methods are not a complete polymorphic Java collections surface.

## Understanding the tests

The inspected JSON fixture uses the default factory path only. It compares nested container output, which exercises fallback allocation internally but does not invoke a custom factory. A future fixture should record allocation order, return null selectively, prepopulate containers, and throw on a chosen nested allocation.

- [Java DOM, lexer, and event fixture](../../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

An existing mapped interface and successful default parsing do not establish custom Map/List compatibility or factory failure state. No such pass is claimed.

## Check your understanding

For {"a":[{}]}, what order should object/array factory callbacks occur, and when could a retained outer object first contain the array?

Pinned Java SHA-256: `8bbfc455209779d5a2e7c56fe6123907f79a457de8fdb4ab4990d1fa42351ff5`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
