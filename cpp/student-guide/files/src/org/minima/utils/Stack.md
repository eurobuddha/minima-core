# Stack — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/Stack.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/Stack.cpp) · [C++ header](../../../../../../src/org/minima/utils/Stack.hpp)

## Purpose

This is a last-in, first-out container for object references. Think of plates: adding and removing happen at the same end. A null payload is allowed, so a null result alone cannot distinguish an empty stack from a stack containing null.

## Follow the algorithm

1. Construction creates an empty list. push appends the supplied object reference; it does not clone the object.
2. pop first checks emptiness. An empty stack returns null. Otherwise it removes and returns the final list element.
3. peek uses the same empty check but only reads the final element, leaving the stack unchanged.
4. isEmpty checks the collection size. Observe this alongside pop/peek when null is a possible value.

## How the C++ translation works

The native vector stores MessageObject, the existing type-erased object wrapper used by the messaging code. push moves the wrapper into the vector; pop moves it out before removing the slot. peek copies the wrapper, preserving the shared object identity rather than making a deep copy. An empty wrapper represents Java null.

## Understanding the tests

The driver generates 250 sequences, each beginning with empty peek/pop and a null push/peek/pop. It then adds 100 operations chosen from push, pop, peek, and mutation. Values include null, booleans, a number, a string, and mutable JSON objects. Both fixtures record the returned value and empty flag after every operation; the Python driver compares the complete result arrays. Mutation changes an object through the original value list, so a later peek detects an incorrect deep copy.

- [Java fixture](../../../../../../support/SmallUtilitiesOracle.java)
- [Native fixture](../../../../../../support/small_utilities.cpp)
- [Differential driver](../../../../../../tools/small_utilities_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The fixture collapses exceptions to ERROR, so it does not establish exact exception classes or messages. It is sequential and does not claim thread safety. A useful additional explicit case is pushing the same object twice, mutating it once, then checking that both pops observe the mutation.

## Check your understanding

After push(null), why can peek return null while isEmpty returns false? Explain why moving a shared wrapper in C++ does not mean moving the underlying object.

Pinned Java SHA-256: `ef95c1dae302f23e503bba34f808baa4daccff77bc4142b60f903c979530fce7`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
