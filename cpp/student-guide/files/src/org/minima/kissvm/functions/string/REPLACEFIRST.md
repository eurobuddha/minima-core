# REPLACEFIRST — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/string/REPLACEFIRST.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/REPLACEFIRST.java) | [src/org/minima/kissvm/functions/string/REPLACEFIRST.cpp](../../../../../../../../src/org/minima/kissvm/functions/string/REPLACEFIRST.cpp) |

## What this file does

Replace the first literal string match while retaining Java replacement syntax.

## Follow the logic

1. Require main, search and replacement strings in that order.
2. Quote the search pattern so dots and other regex characters are literal. The replacement string is not quoted: backslash escapes and group0 remain meaningful.
3. Find the first match and replace it.
4. Return StringValue, including its own encoding and length rules.

## How the translation is approached

C++ reuses the UTF-16 replaceQuoted/expandReplacement helper. It models backslash and group0 behavior and handles empty searches by advancing one code unit. It does not invoke an arbitrary regex engine.

## What the tests tell us

The driver crosses originals abcabc/emoji/a.b/empty, searches a/emoji/dot/empty, and replacements including $0,$00,$01,$1, dangling backslash and ${no}. Generic replacement-runtime diagnostics are normalized, but KISS errors and successful traces are compared.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [REPLACEFIRST](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/REPLACEFIRST.java#L13), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/REPLACEFIRST.java#L18), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/REPLACEFIRST.java#L40), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/REPLACEFIRST.java#L45).

## Check your understanding

Why is a literal-search replacement still different from an unconditional string.find()+insert loop?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `e2bc1dd55d4bcda3e9fdd7a5af0b99a44778675777b8595a2288db48a4d1f126`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
