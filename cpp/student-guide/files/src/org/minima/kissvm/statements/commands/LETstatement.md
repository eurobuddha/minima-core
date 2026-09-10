# LETstatement — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/commands/LETstatement.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java) | [src/org/minima/kissvm/statements/commands/LETstatement.cpp](../../../../../../../../src/org/minima/kissvm/statements/commands/LETstatement.cpp) |

## What this file does

Assign a value to a named variable or an array-style numeric-index key.

## Follow the logic

1. For a normal name, evaluate the right-hand expression then store its Value.
2. For an array assignment, evaluate each index in order, require NUMBER and append its trimmed text and a comma.
3. Only after the key is formed, evaluate the right-hand value and store it. GET and EXISTS build the same key format.
4. toString renders the assignment for traces. The Java array formatter catches Error and logs its stack; that exceptional formatting branch is not reproduced by the current native body.

## How the translation is approached

C++ stores shared expressions and retains assigned Value pointers through Contract. Its trim helper removes bytes at most space, matching the source’s trim intent for numeric text. The missing Error catch in toString remains an explicit gap.

## What the tests tell us

The driver tests normal assignment, replacement, LET (1 2), an invalid Boolean index, an empty index list and malformed assignment syntax. Shared variable state and traces are compared; no injected serious-error formatter test is claimed.

The [KISS execution driver](../../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [Statement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [LETstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java#L50), [LETstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java#L62), [execute](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java#L69), [toString](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/LETstatement.java#L95).

## Check your understanding

Why should a failure evaluating an array index prevent evaluation of the right-hand expression?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `5bc22ab2d0a863e345c2c431e1025aaa7545f31594c8024c1d020c480b9c958d`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
