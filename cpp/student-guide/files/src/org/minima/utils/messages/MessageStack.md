# MessageStack — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageStack.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/messages/MessageStack.cpp) · [C++ header](../../../../../../../src/org/minima/utils/messages/MessageStack.hpp)

## Purpose

Despite its name, this class is a first-in, first-out queue. Producers append at the tail and a consumer removes the head. Filtering retains references and relative order.

## Follow the algorithm

1. PostMessage(String) creates a Message and delegates. PostMessage(Message) appends under the list monitor, releases that monitor, then notifies all threads waiting on a separate wake-up lock. Null messages can be queued.
2. getNextMessage returns null if empty; otherwise it reads the first entry and removes it. A queued null is removed but is indistinguishable from an empty return to the caller.
3. isNextMessage and getSize read under the list monitor. clear removes all entries under the same monitor.
4. clearExcept builds a temporary list of messages whose type appears in the supplied list. clearExceptString instead retains types containing a substring. Only after the entire scan succeeds does either replace contents, so a null-message/type failure can leave the original queue intact.
5. printAllMessages holds the list monitor while logging each indexed diagnostic with notification disabled. This is a callback boundary, not merely a read of bytes.

## How the C++ translation works

The native deque preserves FIFO and shared_ptr preserves message identity. One mutex plus condition_variable protects queue access and wakeups; this differs structurally from Java’s two monitors. Filtering builds a replacement deque before assignment, preserving failure-before-mutation behavior. Native printAllMessages currently writes std::clog rather than calling MinimaLogger, so notification/format/reentry parity is incomplete.

## Understanding the tests

Message operation programs compare queue size, availability, and popped object index after posts, clears, and both filters. A fixed program queues a normal message followed by null and then runs both filters, exercising failure without losing earlier entries. The worker test adds 2000 messages from four concurrent producers and requires each producer’s sequence to remain ordered and each message to be processed once; this checks a real native queue consumer as well as sequential value tests.

- [Java message and queue fixture](../../../../../../../support/MessageOracle.java)
- [Native message and queue fixture](../../../../../../../support/messages.cpp)
- [Differential operation programs](../../../../../../../tools/message_differential.py)
- [Native worker and timer assertions](../../../../../../../support/message_workers.cpp)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

ERROR normalization hides exception details, and the concurrency test is native-only. Java permits Message subclasses to override equals, which may affect LinkedList.remove(Object); native pop_front does not invoke a custom equality method. Logger callback reentry and all possible scheduling interleavings are not certified.

## Check your understanding

If the queue is [A, null, B], what happens to its size after one successful pop followed by a null pop? Why does filtering into a temporary list matter when a later element throws?

Pinned Java SHA-256: `270b293710c05a3a6a177507bd559b140d16e5b6f1f366902890b8ac2de3092b`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
