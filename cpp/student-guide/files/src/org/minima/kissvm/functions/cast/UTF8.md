# UTF8 — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/cast/UTF8.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/UTF8.java) | [src/org/minima/kissvm/functions/cast/UTF8.cpp](../../../../../../../../src/org/minima/kissvm/functions/cast/UTF8.cpp) |

## What this file does

Decode hexadecimal bytes as UTF-8 text.

## Follow the logic

1. Require one HEX argument.
2. Decode its raw bytes with the UTF-8 decoder, including replacement of malformed sequences.
3. Construct a StringValue from the decoded text.

## How the translation is approached

C++ reuses MiniString’s byte-span decoder rather than interpreting a byte as a character. UTF-16 storage is supplied by StringValue.

## What the tests tell us

The shared unary-function cases include 0x80, 0xFFFF, large HEX integers and wrong types/counts. These selected cases check decoder interaction; they are not an exhaustive proof of every malformed UTF-8 sequence.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [UTF8](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/UTF8.java#L19), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/UTF8.java#L24), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/UTF8.java#L37), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/UTF8.java#L42).

## Check your understanding

Explain why one visible emoji occupies four UTF-8 bytes but two UTF-16 code units.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `e70cbfc88275fdad1e3d9d47c9c9cb1cb765e6cdf3db3d66cc44488231575f9a`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
