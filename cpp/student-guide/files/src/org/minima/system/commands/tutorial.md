# tutorial — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/tutorial.java](../../../../../../../../upstream-study/src/org/minima/system/commands/tutorial.java) | `src/org/minima/system/commands/tutorial.cpp` (planned) |

## What this file does

Returns a fixed grammar and function-reference string for KISS VM scripting.

## Follow the logic

1. Create the standard reply and concatenate the source string literals into one tutorial text.
2. The text describes statement forms, expression precedence, values, globals and built-in function signatures. This method does not invoke the tokenizer/parser or check that the prose matches the current implementation.
3. Store the text directly under response, then return. There is no filesystem or network read.
4. Preserve the literal newlines, backslashes, spacing and source spellings. For example, SUMINPUT/SUMOUTPUT in the grammar and SUMINPUTS/SUMOUTPUTS in later descriptions are text in this file, not aliases implemented by this command.

## How the translation is approached

This mapped command remains unported. Reuse Command reply formatting and the pinned literal text or command metadata. Text-preservation work should keep Java escape interpretation, UTF-16 length and reply nesting explicit rather than editing the content during translation.

## What the tests tell us

Proposed validation is an exact Java/native reply comparison after the counterpart exists. Include name/help metadata, fresh function instances, parameter acceptance and every branch described here. No native test pass is claimed for this absent command.

## Dependencies and reading map

Direct Minima imports: [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [tutorial](../../../../../../../../upstream-study/src/org/minima/system/commands/tutorial.java#L7), [runCommand](../../../../../../../../upstream-study/src/org/minima/system/commands/tutorial.java#L12), [getFunction](../../../../../../../../upstream-study/src/org/minima/system/commands/tutorial.java#L240).

## Check your understanding

Why would a successful exact-text test say nothing about whether a script shown in that text executes correctly? Identify the separate parser/VM test needed for that claim.

## Limits and traceability

 The native counterpart is absent; these are source explanations and future validation plans.

Pinned Java SHA-256: `ea36e30e00b5a8c0f040dc8dbba47d3faa3f330d3cca6391c210d03fc479d2d1`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
