# whitepaper — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/whitepaper.java](../../../../../../../../upstream-study/src/org/minima/system/commands/whitepaper.java) | `src/org/minima/system/commands/whitepaper.cpp` (planned) |

## What this file does

Exposes the white-paper text embedded as the class’s WP constant.

## Follow the logic

1. The WP field is a long compile-time concatenation of literal strings. It contains the document title, abstract, numbered sections and references.
2. runCommand creates the standard reply and a separate response object.
3. Put the unchanged WP string under response.whitepaper, then return. This differs from tutorial, which puts its text directly under response.
4. getFunction returns a fresh whitepaper command. The method does not fetch the referenced URLs, evaluate the paper’s arguments or execute protocol logic.

## How the translation is approached

This mapped command remains unported. Reuse Command reply formatting and the pinned literal text or command metadata. Text-preservation work should keep Java escape interpretation, UTF-16 length and reply nesting explicit rather than editing the content during translation.

## What the tests tell us

Proposed validation is an exact Java/native reply comparison after the counterpart exists. Include name/help metadata, fresh function instances, parameter acceptance and every branch described here. No native test pass is claimed for this absent command.

## Dependencies and reading map

Direct Minima imports: [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [whitepaper](../../../../../../../../upstream-study/src/org/minima/system/commands/whitepaper.java#L538), [runCommand](../../../../../../../../upstream-study/src/org/minima/system/commands/whitepaper.java#L543), [getFunction](../../../../../../../../upstream-study/src/org/minima/system/commands/whitepaper.java#L555).

## Check your understanding

A client expects response to be a string because tutorial returned one. What assertion catches its incorrect assumption when calling whitepaper?

## Limits and traceability

This lesson reviews the executable wrapper and the constant’s role. It does not independently fact-check the historical paper or present its claims as current protocol guarantees. The native counterpart is absent; these are source explanations and future validation plans.

Pinned Java SHA-256: `1a94c714ff2910d68ab638da024ad00e4de66b79fd5118467b5f1423193c2329`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
