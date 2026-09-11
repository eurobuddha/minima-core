# TimerProcessor — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerProcessor.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/messages/TimerProcessor.cpp) · [C++ header](../../../../../../../src/org/minima/utils/messages/TimerProcessor.hpp)

## Purpose

This singleton-style service scans pending timers once per loop and posts overdue messages to running processors. It uses a linear insertion-order list, not a priority queue.

## Follow the algorithm

1. createTimerProcessor constructs and starts a new instance and replaces the static reference. It does not stop an older instance. get returns the current instance; stopTimerProcessor dereferences it and stops it.
2. PostMessage appends nonnull timers under the messages lock. Null input is logged with a captured stack instead of being appended. getSize and printing use the same lock.
3. Each run iteration locks the list, reads wall time once, and visits timers in insertion order. A deadline must be strictly less than that time to be due; equality is retained for a later scan.
4. For a due timer, obtain its processor. If running, post the same TimerMessage reference; otherwise log and discard it. Future timers are added to a new list. Replace the old list only after traversal succeeds.
5. After unlocking, sleep for one second. An interrupt during sleep clears running. stop clears running and interrupts; this service does not promise exact-millisecond delivery.

## How the C++ translation works

The native implementation reuses the same pollAt body for deterministic and real worker paths. NativeThread provides interruptible sleep and routes escaping failures. The timer retains its worker descriptor so stop delivers an observable pending interrupt before sleep consumes it. A registry keeps previous created instances alive until teardown, matching the fact that replacing the singleton does not stop old Java workers. Shared timer objects and weak processor targets require explicit lifetime handling; null expired receivers throw.

## Understanding the tests

The native worker fixture posts deadlines 990, 1000, and 1010 and polls at 1000: exactly one receiver message and two pending timers are required. Polling at 1011 must deliver the other two. A due timer for a stopped receiver must disappear without increasing its queue. It also posts null, exercises the real background factory with a latch, and explicitly stops/joins workers. These assertions cover strict deadlines and basic live delivery, not exact scheduling latency.

- [Native worker and timer assertions](../../../../../../../support/message_workers.cpp)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The original worker fixture has no Java trace comparison; the later interruption follow-up below adds three actual Java/native timer cases. Clock jumps, repeated singleton replacement, logger failure during traversal, and expired weak destinations require further cases. A useful failure test would throw after one due delivery and inspect whether the unchanged original list permits that message to be delivered again on retry.

## Check your understanding

If two timers are inserted in reverse deadline order and both are overdue at the next scan, which is posted first? Why is replacing the list only at the end observable if delivery throws?

Pinned Java SHA-256: `384d1125acc9516fc4853ebdcc904e17d5e4eaef85d5e5295b2b206fe9ae90e3`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).

## Interruption follow-up

The [executor/timer lab](../../../../../../new-tests/executor-interruption.md) explains
three actual worker tests: stop during polling, self-interrupt during polling, and
an internal null queue entry. A logger callback observes pending=false before the
action and true afterward. The unchanged one-second sleep consumes the flag, exits,
and leaves an empty timer queue. The null entry is logged and skipped as in Java.
The expanded thread comparison passes 12 cases / 194 assertions in both normal
and sanitizer builds; message_workers also passes. These are fresh scoped results.
Java synchronized-list reentrancy, clock jumps and full timer API parity remain open.
