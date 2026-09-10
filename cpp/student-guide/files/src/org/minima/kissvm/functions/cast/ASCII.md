# ASCII — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/cast/ASCII.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/ASCII.java) | [src/org/minima/kissvm/functions/cast/ASCII.cpp](../../../../../../../../src/org/minima/kissvm/functions/cast/ASCII.cpp) |

## What this file does

Turn a hexadecimal byte sequence into an ASCII string.

## Follow the logic

1. Require one argument and ask Contract to evaluate it as HEX.
2. Decode each byte using Java ASCII: bytes outside the 0–127 range become the replacement character.
3. Wrap the decoded text in StringValue; its string-size and encoding rules still apply.

## How the translation is approached

The C++ loop appends the byte as a UTF-16 code unit below 128 and U+FFFD otherwise. It then reuses StringValue.

## What the tests tell us

The driver supplies 0x00, 0x80, 0xFFFF, empty/string/number/boolean inputs, and wrong argument counts. The harness compares value type and text as well as instruction count and errors.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [ASCII](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/ASCII.java#L19), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/ASCII.java#L24), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/ASCII.java#L37), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/ASCII.java#L42).

## Check your understanding

Predict how ASCII(0x418042) differs from UTF8(0x418042), and trace where replacement occurs.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `d215a774bee3c39caf624caa35c7b1848d497c742e7b86398f97cfad04f7119f`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
