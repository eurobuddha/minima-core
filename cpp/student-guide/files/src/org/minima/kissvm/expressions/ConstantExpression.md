# ConstantExpression — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/expressions/ConstantExpression.java](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java) | [src/org/minima/kissvm/expressions/ConstantExpression.cpp](../../../../../../../src/org/minima/kissvm/expressions/ConstantExpression.cpp) |

## What this file does

Return a literal value while charging for its evaluation.

## Follow the logic

1. Retain the Value supplied at construction.
2. On getValue, increment the instruction count before returning that same stored Value.
3. Display the stored Value text without another evaluation.

## How the translation is approached

C++ retains a shared Value pointer and returns it unchanged. Removing the instruction increment as an optimization would change resource accounting even though the numerical result is identical.

## What the tests tell us

The driver evaluates Boolean, numeric, HEX and string literals directly and as operands, and varies script instruction limits. The harness compares instruction counts, values and failure state.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [ConstantExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java#L18), [getValue](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java#L23), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/ConstantExpression.java#L32).

## Check your understanding

Why can evaluating a constant fail when the contract has exhausted its instruction budget?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `71cf9867c723b6c35d3f3e9ba20ad857ecaf52291a7bdabff91c15db1453545c`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
