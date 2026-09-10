# JSONObject — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/json/JSONObject.cpp) · [C++ header](../../../../../../../src/org/minima/utils/json/JSONObject.hpp)

## Purpose

JSONObject stores named values in insertion order because the actual superclass is LinkedHashMap, despite the comment describing keys as unordered. Its main job is compact JSON serialization plus a few typed getters.

## Follow the algorithm

1. Construct an empty map or shallow-copy a supplied map. A copied container is independent, but mutable values remain references to the same objects.
2. writeJSONString writes an opening brace, iterates entries in map order, inserts commas between entries, converts every key with String.valueOf, escapes and quotes that key, writes a colon, and delegates the value to JSONValue. A null top-level map writes null.
3. The instance serializers delegate to the static map serializer, which deliberately bypasses a custom JSONAware override on the top-level map and prevents self-recursion.
4. getString casts the stored value; getBoolean casts/unboxes and fails on missing/null values. The default string getter checks containsKey rather than nonnull value, so an explicit null does not select the default. The static key/value formatter returns one escaped property fragment without surrounding braces.

## How the C++ translation works

The native ordered storage of UTF-16 key/value pairs retains insertion order and replaces an existing value without moving its slot. Each access now locks and returns an owned value. Replacing a value leaves the structural modification count unchanged; inserting a new key changes it. Checked iterators retain their storage owner, so replacing the calling variable does not destroy the original iteration. JSONValue copies retain shared ownership of nested objects. This is a subset of Java Map: arbitrary/null object keys, the full removal/iteration APIs, arbitrary map-copy construction, and static map serializers are not all exposed. Native getString on null throws rather than returning Java null.

## Understanding the tests

The JSON driver parses duplicate-key input such as {,"a":::1,"a":2,} and compares the exact reserialized object, verifying both replacement and the permissive source parser. It also compares nested objects, escaped lone-surrogate keys, and Unicode. This is parser/serialization integration coverage; it does not directly test every typed getter or map method.

- [Java DOM, lexer, and event fixture](../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The current native container surface is partial. A proposed direct test should distinguish an absent key from a present-null key in getString(key, default), and verify replacement order independently of parsing. The guide does not treat the absence of those direct tests as a pass.

## Check your understanding

If keys A, B, then A are inserted, in what order are keys serialized, and which A value appears?

Pinned Java SHA-256: `0dcf342437c36cd2cbbd4d47a01598aa0988808a4b51e93c5763ab08029e5b7c`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).


## Shared-record experiment

The [shared JSON lab](../../../../../../new-tests/shared-json-rpc.md) explains the
new direct iterator/copy comparison and actual UserDB concurrent test. A copied
object has independent outer slots but shared children. The mutex is released
before serializing a child, avoiding locks held across a nested traversal. This
does not make an entire tree serialization a single atomic snapshot; structural
changes may raise ConcurrentModificationException, as with the source iterator.
