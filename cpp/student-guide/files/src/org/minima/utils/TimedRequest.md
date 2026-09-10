# TimedRequest — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/TimedRequest.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/TimedRequest.cpp) · [C++ header](../../../../../../src/org/minima/utils/TimedRequest.hpp)

## Purpose

Despite its name, this class currently performs a linear duplicate-ID check. RequestUnit records a construction timestamp, but sendNow neither adds a request nor checks elapsed time. Understanding what the code omits is essential to reproducing it.

## Follow the algorithm

1. A RequestUnit records currentTimeMillis and retains the supplied MiniData reference.
2. TimedRequest creates a publicly accessible, initially empty list of RequestUnit references.
3. sendNow walks that list in order and calls isEqual on each stored ID. The first equal ID returns false.
4. If no ID matches, return true. The list and timestamps remain unchanged; callers would have to record, remove, or replace entries themselves.

## How the C++ translation works

C++ uses shared_ptr for the public list, its units, and their IDs to retain the observable aliases and nullable states. RequestUnit takes time from TimerMessage::currentTimeMillis. sendNow preserves the source traversal and does not introduce an expiry policy. It explicitly rejects null list/unit/ID states using native runtime errors; exact Java exception text is not reproduced by this class.

## Understanding the tests

Each of 250 generated sequences calls send twice before adding anything, proving that sendNow does not automatically record. It then inserts ID 0x01 with the oldest possible signed 64-bit timestamp and checks it again, proving that age does not expire it. 0x0001 is included to exercise MiniData equality semantics. Further operations mutate IDs, clear/replace/null the list, and insert null units. The fixtures compare every returned result and list size. A separate boolean asserts the constructor timestamp lies between wall-clock reads immediately before and after construction.

- [Java fixture](../../../../../../support/SmallUtilitiesOracle.java)
- [Native fixture](../../../../../../support/small_utilities.cpp)
- [Differential driver](../../../../../../tools/small_utilities_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

All per-operation exceptions become ERROR in this fixture, so exception subtype/message parity remains unproven. The wall-clock bound is a fixture check, not a guarantee against clock adjustments. No locking or automatic request scheduling is implemented here.

## Check your understanding

Starting from an empty list, what do three identical sendNow calls return? How would the answer change if a caller manually inserted a matching RequestUnit after the first call?

Pinned Java SHA-256: `bd571213217894b0e06b5381393165c580144640b02d21bda5caf5a7bcbef807`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
