# ContentHandler — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/json/parser/ContentHandler.java) · [Mapped C++ body](../../../../../../../../src/org/minima/utils/json/parser/ContentHandler.cpp) · [C++ header](../../../../../../../../src/org/minima/utils/json/parser/ContentHandler.hpp)

## Purpose

ContentHandler describes stoppable, event-based JSON parsing. Instead of building a DOM, the parser reports boundaries, keys, and primitive values to a callback object.

## Follow the algorithm

1. startJSON and endJSON signal the whole document and return void. The parser does not use them as pause requests.
2. Object/array start/end, object-entry start/end, and primitive callbacks return boolean. False asks the parser to return after that callback; it is a pause, not inherently a parse failure.
3. A caller resumes through JSONParser.parse(..., true), which continues the retained lexer/state and ignores the new input argument when a session exists.
4. The parser changes its internal state before many callbacks. In particular, pausing from a primitive object value returns before endObjectEntry and resumes in the parent state; a consumer must reproduce the source’s actual sequence rather than assume every entry-end event is replayed.

## How the C++ translation works

The native interface passes UTF-16 keys and const JSONValue references, with pure virtual callbacks and a virtual destructor. JSONParser preserves its source state machine and boolean stopping points. The interface itself contains no event buffering or automatic resumption.

## Understanding the tests

The JSON fixtures record every event as [name,value]. The driver runs each input with uninterrupted mode and pause intervals 1 and 3, then repeatedly resumes with the literal ignored on resume until endJSON. Exact event arrays and final positions are compared, including events emitted before malformed input fails. This checks callback order and pause semantics more precisely than comparing only a final parsed value.

- [Java DOM, lexer, and event fixture](../../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The handler fixture does not throw custom IO/parse/runtime exceptions or switch handlers during a pause. Native stream input is buffered in full, so event equivalence for supplied strings does not prove Reader consumption timing.

## Check your understanding

Why should a pause test compare the event sequence from each resume call, not just whether parsing eventually ends?

Pinned Java SHA-256: `77d18ff983b308da8912fc32d4877b62dab151b99b09a2598bf3e2ebdc429f40`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
