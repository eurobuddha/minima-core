# Expression — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/expressions/Expression.java](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java) | [src/org/minima/kissvm/expressions/Expression.cpp](../../../../../../../src/org/minima/kissvm/expressions/Expression.cpp) |

## What this file does

Define the operation that evaluates a syntax-tree node into a typed Value.

## Follow the logic

1. The Java interface declares getValue(Contract), which can throw ExecutionException. There is no evaluation algorithm in the interface itself.
2. Concrete nodes retain their operands and evaluate them against the caller’s Contract. Parsing builds this tree before execution.

## How the translation is approached

C++ uses an abstract base with a virtual destructor and shared ExpressionPtr/ValuePtr ownership. It makes toString explicit because traces need polymorphic expression display.

## What the tests tell us

Every expression-mode program in the KISS driver invokes getValue on a parsed tree. This exercises concrete nodes behind the interface; it is not an independent proof of every possible subclass implementation.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

## Check your understanding

Why should parsing an expression avoid evaluating its variable values immediately?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `5ede06c9090515511076aac7875d8c38772642d9a9c72fca49d6e3fec98ed5aa`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
