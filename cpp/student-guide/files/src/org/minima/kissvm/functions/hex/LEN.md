# LEN — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/hex/LEN.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/LEN.java) | [src/org/minima/kissvm/functions/hex/LEN.cpp](../../../../../../../../src/org/minima/kissvm/functions/hex/LEN.cpp) |

## What this file does

Measure HEX byte length or string UTF-16 length.

## Follow the logic

1. Require one argument and evaluate it.
2. Return byte-array length for HEX.
3. Return Java String.length() for STRING; reject numbers and booleans.

## How the translation is approached

C++ uses the byte vector size or StringValue.getUTF16().size(), preserving code-unit counting instead of counting UTF-8 bytes or visible characters.

## What the tests tell us

The unary matrix includes empty and emoji strings. Explicit SUBSTR/LEN cases split surrogate pairs, and LEN(SETLEN(...)) checks byte lengths at size boundaries.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [LEN](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/LEN.java#L13), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/LEN.java#L18), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/LEN.java#L43), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/LEN.java#L48).

## Check your understanding

Predict LEN([a🐼b]) and explain each counted unit.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `ee5d287101ce7e3793639506c695c42d69e9466ebf9cdb20393590e0ed532e56`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Original test mapping

The [reverse/length lab](../../../../../../../new-tests/reverse-and-length.md)
now maps the original test's three methods and twelve executed assertions to
native code. It uses actual factories and expression parameters. The production
implementation is unchanged; all original-test comparisons pass in both builds.
