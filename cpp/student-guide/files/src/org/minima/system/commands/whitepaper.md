# whitepaper — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/whitepaper.java](../../../../../../../../upstream-study/src/org/minima/system/commands/whitepaper.java) | [Native counterpart](../../../../../../../src/org/minima/system/commands/whitepaper.cpp) |

## What this file does

Exposes the white-paper text embedded as the class’s WP constant.

## Follow the logic

1. The WP field is a long compile-time concatenation of literal strings. It contains the document title, abstract, numbered sections and references.
2. runCommand creates the standard reply and a separate response object.
3. Put the unchanged WP string under response.whitepaper, then return. This differs from tutorial, which puts its text directly under response.
4. getFunction returns a fresh whitepaper command. The method does not fetch the referenced URLs, evaluate the paper’s arguments or execute protocol logic.

## How the translation is approached

The native counterpart reuses Command reply formatting and copies the exact pinned literal expressions. Java and C++ escapes are checked against the actual compiled Java output. The two tutorial classes retain distinct namespaces and text; only the root-package tutorial is registered, matching the source. The white-paper constant remains public and immutable.

## What the tests tell us

Both differential suites pass 71 cases and 428 independent assertions. It compares complete text, metadata, reply shape, the public WP constant, fresh factories, ignored direct-call parameters, parameter aliases, ordinary/non-admin callers, invalid runner parameters and the registered tutorial variant. Python independently decodes the pinned literal expressions and records exact lengths and SHA-256 hashes. Both full builds pass; the normal comparison took 2.618 seconds and the sanitizer comparison took 4.137 seconds, with identical transcript hashes and empty native stderr.

## Dependencies and reading map

Direct Minima imports: [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [whitepaper](../../../../../../../../upstream-study/src/org/minima/system/commands/whitepaper.java#L538), [runCommand](../../../../../../../../upstream-study/src/org/minima/system/commands/whitepaper.java#L543), [getFunction](../../../../../../../../upstream-study/src/org/minima/system/commands/whitepaper.java#L555).

## Check your understanding

A client expects response to be a string because tutorial returned one. What assertion catches its incorrect assumption when calling whitepaper?

## Limits and traceability

This lesson reviews the executable wrapper and the constant’s role. It does not independently fact-check the historical paper or present its claims as current protocol guarantees. The native text is compared with the source; this does not certify the rest of the node.

Pinned Java SHA-256: `1a94c714ff2910d68ab638da024ad00e4de66b79fd5118467b5f1423193c2329`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
