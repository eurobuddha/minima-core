# IFstatement — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/commands/IFstatement.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/IFstatement.java) | [src/org/minima/kissvm/statements/commands/IFstatement.cpp](../../../../../../../../src/org/minima/kissvm/statements/commands/IFstatement.cpp) |

## What this file does

Run the first branch whose condition evaluates to Boolean TRUE.

## Follow the logic

1. Keep paired condition and action lists in insertion order.
2. Evaluate each condition and require BOOLEAN.
3. Run the matching StatementBlock, then break. Later conditions are not evaluated. An ELSE branch is supplied by the parser as an always-true condition.

## How the translation is approached

C++ uses parallel vectors of shared expressions and blocks. It preserves first-match control flow and delegates instruction/stack behavior to StatementBlock.

## What the tests tell us

The driver tests TRUE/FALSE branches, ELSEIF, an invalid numeric condition, an empty branch, a RETURN inside a branch and nested IF depths 1, 63, 64, 65 and 70.

The [KISS execution driver](../../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [Statement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java), [StatementBlock](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [IFstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/IFstatement.java#L40), [addCondition](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/IFstatement.java#L45), [execute](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/IFstatement.java#L54), [toString](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/IFstatement.java#L88).

## Check your understanding

Construct a program where the first condition is true and the second would reference a missing variable. Why should it still succeed?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `4dd1ec7debcdc87df985f94f3f54f0ab35849c08392d3714f75ea17ec7b722f0`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
