# tutorial — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/tutorial.java](../../../../../../../../upstream-study/src/org/minima/system/commands/tutorial.java) | [Native counterpart](../../../../../../../src/org/minima/system/commands/tutorial.cpp) |

## What this file does

Returns a fixed grammar and function-reference string for KISS VM scripting.

## Follow the logic

1. Create the standard reply and concatenate the source string literals into one tutorial text.
2. The text describes statement forms, expression precedence, values, globals and built-in function signatures. This method does not invoke the tokenizer/parser or check that the prose matches the current implementation.
3. Store the text directly under response, then return. There is no filesystem or network read.
4. Preserve the literal newlines, backslashes, spacing and source spellings. For example, SUMINPUT/SUMOUTPUT in the grammar and SUMINPUTS/SUMOUTPUTS in later descriptions are text in this file, not aliases implemented by this command.

## How the translation is approached

The native counterpart reuses Command reply formatting and copies the exact pinned literal expressions. Java and C++ escapes are checked against the actual compiled Java output. The two tutorial classes retain distinct namespaces and text; only the root-package tutorial is registered, matching the source. The white-paper constant remains public and immutable.

## What the tests tell us

Both differential suites pass 71 cases and 428 independent assertions. It compares complete text, metadata, reply shape, the public WP constant, fresh factories, ignored direct-call parameters, parameter aliases, ordinary/non-admin callers, invalid runner parameters and the registered tutorial variant. Python independently decodes the pinned literal expressions and records exact lengths and SHA-256 hashes. Both full builds pass; the normal comparison took 2.618 seconds and the sanitizer comparison took 4.137 seconds, with identical transcript hashes and empty native stderr.

## Dependencies and reading map

Direct Minima imports: [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [tutorial](../../../../../../../../upstream-study/src/org/minima/system/commands/tutorial.java#L7), [runCommand](../../../../../../../../upstream-study/src/org/minima/system/commands/tutorial.java#L12), [getFunction](../../../../../../../../upstream-study/src/org/minima/system/commands/tutorial.java#L240).

## Check your understanding

Why would a successful exact-text test say nothing about whether a script shown in that text executes correctly? Identify the separate parser/VM test needed for that claim.

## Limits and traceability

 The native text is compared with the source; this does not certify the rest of the node.

Pinned Java SHA-256: `ea36e30e00b5a8c0f040dc8dbba47d3faa3f330d3cca6391c210d03fc479d2d1`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
