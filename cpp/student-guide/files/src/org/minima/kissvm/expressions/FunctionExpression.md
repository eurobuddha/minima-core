# FunctionExpression — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/expressions/FunctionExpression.java](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/FunctionExpression.java) | [src/org/minima/kissvm/expressions/FunctionExpression.cpp](../../../../../../../src/org/minima/kissvm/expressions/FunctionExpression.cpp) |

## What this file does

Evaluate one built-in function with shared instruction, stack and trace bookkeeping.

## Follow the logic

1. Charge one instruction and increment Contract stack depth.
2. Call the retained MinimaFunction, whose arguments are expressions evaluated by the function as needed.
3. On normal return, decrement depth, trace function name/parameters and returned value, then return it.
4. If runFunction throws, the decrement and result trace are skipped because the source has no finally block.

## How the translation is approached

C++ retains a shared function instance and keeps the explicit sequencing. Its formatter reconstructs Java list-style parameter text used by trace comparisons.

## What the tests tell us

All built-in expression cases pass through this wrapper. Wrong argument types, arithmetic failures and nested functions are compared with final stack depth and instruction count, exposing premature cleanup or missing bookkeeping.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [FunctionExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/FunctionExpression.java#L22), [getValue](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/FunctionExpression.java#L28), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/FunctionExpression.java#L49).

## Check your understanding

Would an unconditional scope guard to restore stack depth match the source after a function throws?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `0fa4846bab8fdb0d22d61966e2c5839c6ecc15ee84d0e4cd6c8d417adb4ddb19`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
