# MessageListener — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageListener.java) · [Mapped C++ body](../../../../../../../src/org/minima/utils/messages/MessageListener.cpp) · [C++ header](../../../../../../../src/org/minima/utils/messages/MessageListener.hpp)

## Purpose

This interface is the smallest callback contract for receiving a Message: a listener supplies processMessage and may throw an Exception. It supplies no queue, worker, filtering, or retry policy.

## Follow the algorithm

1. A caller holds a MessageListener reference and invokes processMessage with a Message reference, which may be null unless the caller rejects it.
2. The implementing object chooses all behavior. The interface declaration permits an exception to propagate; the caller must decide whether to catch it.
3. Because there is no method body or mutable field, there is no serialization format or algorithm to duplicate in this file.

## How the C++ translation works

The C++ abstract class declares a pure virtual processMessage taking shared_ptr<Message> and a virtual destructor. The mapped .cpp only includes the header. Shared ownership replaces Java object reachability; C++ has no Java checked-exception declaration. This is a real interface counterpart but does not establish any listener implementation’s behavior.

## Understanding the tests

No dedicated MessageListener assertion was found in the inspected message fixtures; MessageProcessor has its own protected handler and does not inherit this interface. The linked message tests exercise Message payloads that a listener may receive, which is indirect dependency coverage only. A future focused fixture should invoke a concrete listener through the base interface, retain/mutate the same payload, accept null, and propagate a deliberate failure.

- [Java message and queue fixture](../../../../../../../support/MessageOracle.java)
- [Native message and queue fixture](../../../../../../../support/messages.cpp)
- [Differential operation programs](../../../../../../../tools/message_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Compiling a pure virtual interface checks its C++ signature, not callback semantics, exception categories, ownership lifetime, or callers. The proposed listener fixture is not an existing pass result.

## Check your understanding

Why would a successful MessageProcessor worker test not by itself prove that MessageListener is used correctly?

Pinned Java SHA-256: `9e0092cb09231923245536c7c480d4c5020f358e17f2f660f840bcb08226f646`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
