# REV — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/hex/REV.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/REV.java) | [src/org/minima/kissvm/functions/hex/REV.cpp](../../../../../../../../src/org/minima/kissvm/functions/hex/REV.cpp) |

## What this file does

Reverse the order of bytes in HEX.

## Follow the logic

1. Require one HEX argument.
2. For each output position i, take input[length −i−1].
3. Return a new HEX value; this reverses bytes, not the bits inside a byte.

## How the translation is approached

C++ copies the byte vector then uses std::reverse.

## What the tests tell us

The unary matrix includes0x00,0x80,0xFFFF, longer HEX values and wrong types; comparison runs through actual expression evaluation.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [REV](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/REV.java#L11), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/REV.java#L16), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/REV.java#L41), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/REV.java#L46).

## Check your understanding

Predict REV(0x0102) and explain why REV(0x80) remains0x80.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `5b37cc20101aa4a7c21a42866fddbd3388da06acd7daf0753e1ae87b89286c5d`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Original test mapping

The [reverse/length lab](../../../../../../../new-tests/reverse-and-length.md)
now maps the original test's three methods and twelve executed assertions to
native code. It uses actual factories and expression parameters. The production
implementation is unchanged; all original-test comparisons pass in both builds.
