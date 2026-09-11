# MessageProcessor — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/messages/MessageProcessor.cpp) · [C++ header](../../../../../../../src/org/minima/utils/messages/MessageProcessor.hpp)

## Purpose

MessageProcessor owns a worker loop that consumes queued messages, records the current message, logs processing errors, and releases database locks after every attempt. Subclasses implement the actual handler.

## Follow the algorithm

1. Java construction starts its thread immediately. Trace settings and filter are static across processors. Filtering lowercases both text and filter using the current locale; it does not run the handler itself.
2. The loop fetches a message and processes while it is nonnull and running. It always calls toString before assigning lastMessage, even when tracing is disabled. The trace predicate is evaluated before and after the handler, so a handler can change tracing between those checks.
3. Errors and exceptions take different logging branches. A finally clause always asks MinimaDB to release held read/write locks, including when error logging itself fails; a cleanup failure can replace the earlier failure.
4. After each attempt the next message is fetched before the loop tests running again. If the handler stops the processor, one additional already-fetched message can be discarded. When idle, the worker checks availability while holding the wake-up lock before waiting.
5. stopMessageProcessor clears running and wakes the worker. waitToShutDown polls every 250 ms and uses waited>timeout, then forcibly marks completion and interrupts the Java thread. The completion flag can therefore become true while a handler has not returned. Normal loop exit sets running false and completion true.

## How the C++ translation works

Native factories start workers only after the most-derived object exists under shared ownership, avoiding virtual calls during C++ construction. A required callback represents MinimaDB lock cleanup; atomics/mutexes protect flags and trace data. NativeThread supplies uncaught failure delivery. joinMessageProcessor is a separate real thread-exit barrier; waitToShutDown now signals the worker interrupt state as well as changing flags; nativeThreadSleep consumes that signal and throws. Arbitrary external C++ handlers and raw sleeps still require separate integration. The worker retains itself while active.

## Understanding the tests

The native worker fixture requires 2000 successful handler entries and 2000 cleanup calls despite injected handler exceptions. It asserts per-producer FIFO, normal shutdown flags, and that stopping during A from [A,B,C] processes only A while B is fetched/discarded and C remains. A blocked handler plus timeout zero demonstrates early completion flags while shared ownership remains alive; releasing its promise and joining must eventually destroy the worker. A Greek-case filter assertion checks the locale adapter.

- [Native worker and timer assertions](../../../../../../../support/message_workers.cpp)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

These are native concurrency/assertion tests, not a full Java thread-interleaving differential. They do not establish Java interrupt equivalence, all trace/logging failure ordering, or a full MinimaDB/Main node. Exact stack frames and arbitrary Java Error hierarchies need separate boundary tests.

## Check your understanding

Why must callers use a real join before destroying resources used by a blocked handler, even when isShutdownComplete is true? Which queued message is lost when the current handler stops the loop?

Pinned Java SHA-256: `f4cd66df973edee1e4f7374f955e2b602b76de8cee274652b77bae1976906f64`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).

## Interruptible shutdown checkpoint

The [thread interruption lab](../../../../../../new-tests/thread-interruption.md) compares real Java/native timeout shutdown, ordinary shutdown, an interrupted waiting caller, blocked transaction locks, delayed unlocks and actual send-poll processing. The cleanup recorder runs exactly once after the interrupted handler. This extends the earlier tests without claiming every thread interleaving.
