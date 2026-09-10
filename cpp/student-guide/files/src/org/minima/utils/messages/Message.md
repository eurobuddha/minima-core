# Message — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/messages/Message.cpp) · [C++ header](../../../../../../../src/org/minima/utils/messages/Message.hpp)

## Purpose

Message combines a nullable type label with a mutable map of named object references. Typed convenience methods box or cast values; they do not coerce strings, numbers, and booleans into one another.

## Follow the algorithm

1. Construction stores the supplied type or defaults to the empty string, then creates a new HashMap. setMessageType replaces the label. isMessageType calls equals on its argument, so a null argument fails even if the stored type is also null.
2. addObject replaces a map entry and returns this for chaining. Primitive add methods box their exact Java type. addString constructs a String, so null fails there even though addObject can store null.
3. getAllContents returns the actual mutable map, not a copy. exists tests whether the retrieved value is nonnull: an explicit null and a missing key both report false.
4. getBoolean returns false for a null/missing value; its default overload uses the supplied fallback in that case. Integer and Float getters unbox exact types and fail on null or wrong type. getString preserves null but still rejects a nonstring.
5. Formatting walks HashMap keys, appends key:value plus comma/space for every entry, and wraps the type and contents in brackets. Null keys and values render as null. Rendering an arbitrary payload can itself execute user code or fail.

## How the C++ translation works

MessageObject uses std::any to retain primitive types and shared_ptr plus a rendering callback for object identity. The native JavaHashMap compatibility layer supplies key iteration and nullable keys. getAllContents shares that map. C++ optional strings represent nullable type/key/string values; typed getters now translate a missing integer/float box to JavaNullPointerException and a wrong box to JavaClassCastException. Boolean/string getters retain their different null defaults. The original message fixture still collapses exception details.

## Understanding the tests

The differential driver checks raw Float bit patterns, including signed zero, NaN, infinities, subnormals, and 750 random patterns, through every typed getter. It also generates 140 mixed operation programs with null/colliding/Unicode keys, direct map writes/removal, type mutation, queueing, and timer copies. After each operation both fixtures emit the result, queue flags, and every message diagnostic. A copied timer shares its original map, so later mutations must change both renderings. Exceptions are collapsed to ERROR.

- [Java message and queue fixture](../../../../../../../support/MessageOracle.java)
- [Native message and queue fixture](../../../../../../../support/messages.cpp)
- [Differential operation programs](../../../../../../../tools/message_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Main runtime follow-up

The [Main lab](../../../../../../new-tests/main-runtime.md) supplies wrong starting/timer/key-use/host boxes to actual handlers. It compares exception classes instead of the original message fixture's generic ERROR, and verifies whether earlier handler effects occurred. This drove the shared typed-getter correction. All 336 Main programs pass in both builds; this is not exhaustive Float-box or arbitrary-object exception coverage.

## Limits and a useful next test

This covers the exercised primitive boxes and map operations, not every Java Object or HashMap API. Exact failures, arbitrary custom toString callbacks, and concurrent payload mutation remain outside these assertions.

## Check your understanding

If addObject("x", null) has run, what do exists("x"), getBoolean("x"), and getBoolean("x", true) return? Why should getInteger reject a Float containing 3.0?

Pinned Java SHA-256: `f5be6e7ede96339eb58ab1e6e55189b65738f1ec04dc4221d0dd6982ba400973`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
