# JSONParser — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/json/parser/JSONParser.java) · [Mapped C++ body](../../../../../../../../src/org/minima/utils/json/parser/JSONParser.cpp) · [C++ header](../../../../../../../../src/org/minima/utils/json/parser/JSONParser.hpp)

## Purpose

JSONParser consumes Yytoken records either into mutable containers or into callbacks. Its grammar is deliberately as permissive as the pinned implementation: extra commas and repeated/omitted colons can be accepted in certain states.

## Follow the algorithm

1. DOM parsing resets the lexer and uses status/value stacks. At the root, a primitive becomes a finished value; an opening brace/bracket creates a container. After one root value, only EOF is accepted.
2. Inside an object, comma tokens are ignored and a string token becomes the pending key. In the pending-key state colon tokens are ignored, while a primitive or new container becomes the value. Thus the state machine does not require exactly one colon.
3. Inside an array, commas are ignored and values/containers are appended. Closing a nested container removes its frame and resumes its parent; closing the root moves to the finished-value state. Wrong closing tokens or nonstring keys produce an error.
4. ContainerFactory controls allocation with null fallback. DOM parsing inserts child containers into parents before descending, retaining shared references during incremental population.
5. Event parsing uses a persistent status stack, reports callbacks, and returns whenever a boolean callback is false. Resume ignores new input if a session already exists. S_IN_PAIR_VALUE delays endObjectEntry after a nested value without consuming another token; caught failures mark the parser in error.

## How the C++ translation works

The native DOM path uses a vector of frames containing state, value and pending key; shared containers retain the same relationships. The event path closely follows the source states and stopping order. It operates on UTF-16 text through the reused lexer. std::istream overloads read the entire input before parsing, so they do not reproduce Java Reader consumption or failure timing.

## Understanding the tests

The differential driver sends every corpus text through DOM, lexer and three event modes. It compares compact output, exact event sequences, parse category/message/position, and partial events. Inputs explicitly include [1 2,,,3,], duplicate object keys with repeated colons, missing closures, extra root values, raw controls, Unicode/surrogates, number limits, random invalid strings and nested valid documents. Resume calls deliberately pass unrelated text to prove it is ignored.

- [Java DOM, lexer, and event fixture](../../../../../../../../support/JSONOracle.java)
- [Native JSON fixture](../../../../../../../../support/json.cpp)
- [Exact value/event/error comparison driver](../../../../../../../../tools/json_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Exact corpus matches do not establish all strings, custom factories, callback exceptions, Reader errors, memory bounds, or thread safety. A stricter third-party JSON parser would not be a compatible replacement without addressing the accepted nonstandard syntax.

## Check your understanding

Trace the states for {"a":::1}. Which branch consumes each colon, and why does the source accept the final value?

Pinned Java SHA-256: `800d372a8f447b6269e0f647743b55fe5d2d94fdfe5095ec0f27c775576e117c`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
