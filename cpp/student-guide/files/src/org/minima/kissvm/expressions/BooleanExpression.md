# BooleanExpression — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/expressions/BooleanExpression.java](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java) | [src/org/minima/kissvm/expressions/BooleanExpression.cpp](../../../../../../../src/org/minima/kissvm/expressions/BooleanExpression.cpp) |

## What this file does

Evaluate logical and comparison operators with eager operand evaluation.

## Follow the logic

1. Charge one instruction, then evaluate left and right operands in that order. AND/OR do not short-circuit this evaluation. Unary NOT carries a dummy FALSE ConstantExpression as the right operand, which is still evaluated.
2. Except for NOT, require matching operand types. Logical operators require BOOLEAN; ordering comparisons require NUMBER; EQ/NEQ handle matching supported types.
3. Apply the operator, create the Boolean result, then trace the expression and result.
4. Reject an unknown operator code with ExecutionException.

## How the translation is approached

C++ uses shared operand nodes and explicit evaluation before its operator switch. It must not translate operand evaluation directly into C++ && or ||, which would skip the right side. Generic type-error categories remain normalized in the current suite.

## What the tests tell us

The driver crosses literal/value categories with every Boolean operator and includes RETURN FALSE AND missing and RETURN TRUE OR missing. Those two programs expose eager evaluation. Unary cases expose the extra dummy operand’s instruction charge.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [BooleanValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [NumberValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [StringValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [BooleanExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java#L46), [BooleanExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java#L50), [getBooleanValue](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java#L56), [getValue](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java#L61), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/BooleanExpression.java#L241).

## Check your understanding

Why does TRUE OR missing still fail in this VM? Count the expression evaluations involved.

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `1c7a5d1a3a63be4e38f7dddb42fcb68d6fc465b016d9332ccb985b3cd108062b`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
