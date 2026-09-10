# TimerMessage — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/messages/TimerMessage.cpp) · [C++ header](../../../../../../../src/org/minima/utils/messages/TimerMessage.hpp)

## Purpose

A TimerMessage is an ordinary Message with an absolute deadline and a destination processor. Constructing it from another message shares the payload map while copying the current type label.

## Follow the algorithm

1. The type constructor initializes Message, stores the original delay, and computes currentTimeMillis+delay using signed long arithmetic. Negative delays and overflow are not rejected.
2. The message constructor first reads the source type and then assigns the same contents map. Later map edits are shared, while replacing either message’s type does not replace the other type.
3. setProcessor stores the destination; getProcessor and getTimer return the stored values. This object does not start a thread or schedule itself.
4. Formatting subtracts the current wall clock from the deadline and prepends Timer/deadline and remaining Delay to Message.toString. The displayed Delay is a changing difference, not simply the original constructor delay.

## How the C++ translation works

The native implementation uses explicit unsigned addition/subtraction then bit-casts to int64_t, avoiding undefined C++ signed overflow while matching Java wraparound. Shared map ownership preserves aliasing. The destination is a weak_ptr, a deliberate ownership adaptation that can expire where Java’s strong reference would keep a processor reachable. Explicit-clock constructors and toStringAt are deterministic native test adapters.

## Understanding the tests

The Java fixture replaces the private deadline by reflection with 1000+delay and normalizes displayed remaining time to a clock of 100; the native fixture uses explicit clock adapters. Operation programs include zero, negative, and signed-long-extreme delays and mutate both original/timer maps. Worker assertions separately create deadlines 990, 1000, and 1010, polling at 1000 and 1011 to exercise the scheduler’s strict comparison.

- [Java message and queue fixture](../../../../../../../support/MessageOracle.java)
- [Native message and queue fixture](../../../../../../../support/messages.cpp)
- [Differential operation programs](../../../../../../../tools/message_differential.py)
- [Native worker and timer assertions](../../../../../../../support/message_workers.cpp)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The deterministic fixture does not prove live clock construction timing or processor lifetime equivalence. The weak destination is a meaningful garbage-collection/ownership difference. A future test should release all ordinary references to the receiver while a timer remains and compare what keeps it alive.

## Check your understanding

After wrapping a message, why does adding a field to the original affect the timer while changing the original type does not? What is the wrapped deadline when now+delay exceeds the signed-long maximum?

Pinned Java SHA-256: `278bd800df235442dd02d165584321dbba05b42d8f48fa4210ed354283410cf3`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
