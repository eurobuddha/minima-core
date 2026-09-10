# OperatorExpression — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/expressions/OperatorExpression.java](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java) | [src/org/minima/kissvm/expressions/OperatorExpression.cpp](../../../../../../../src/org/minima/kissvm/expressions/OperatorExpression.cpp) |

## What this file does

Implement numeric, string and hexadecimal operators with source-specific byte normalization.

## Follow the logic

1. Charge an instruction and evaluate both operands, including the dummy FALSE operand used by unary NEG and bitwise NOT.
2. ADD accepts matching NUMBER or STRING values and returns immediately, so it skips the common result trace. Other numeric operations enforce NUMBER; division checks zero explicitly.
3. Shifts require HEX on the left and NUMBER on the right, bound the absolute shift before integer conversion and delegate to MiniData.
4. Bitwise AND aligns the rightmost bytes to the shorter length; OR/XOR zero-pad on the left to the longer length. NOT inverts each byte. All these helpers strip leading zero bytes and represent an all-zero result as one zero byte.
5. Other successful paths emit the common expression/result trace.

## How the translation is approached

C++ reuses MiniNumber and MiniData arithmetic and implements byte loops over checked vectors. A shared stripZeros helper preserves the explicit zero representation. ADD’s early return is retained, even though tracing it consistently might seem cleaner.

## What the tests tell us

The driver crosses many value types with arithmetic/bitwise operators, tests divide-by-zero and fractional/negative/excessive shifts, and includes precedence examples. It compares values, number wire bytes and trace, so the special ADD trace omission is observable.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [BooleanValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [NumberValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [StringValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [OperatorExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java#L44), [OperatorExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java#L48), [getValue](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java#L58), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java#L248), [andFastHEX](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java#L300), [orFastHEX](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java#L370), [notFastHEX](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/OperatorExpression.java#L460).

## Check your understanding

Why is the zero result of bitwise operations different from BITSET clearing its final set bit? Compare their byte-normalization rules.

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `d71a3d87318bdf95e5ce286f28c3e7c021216aae99ab22a1ca2076fe9d544532`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
