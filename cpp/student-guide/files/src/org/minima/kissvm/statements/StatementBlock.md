# StatementBlock — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/StatementBlock.java](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java) | [src/org/minima/kissvm/statements/StatementBlock.cpp](../../../../../../../src/org/minima/kissvm/statements/StatementBlock.cpp) |

## What this file does

Execute an ordered list of statements while updating instruction and stack state.

## Follow the logic

1. Increment Contract stack depth on entry.
2. Before each statement, return immediately if success has already been set.
3. Increment instruction count, log the statement text, then execute it. Each step can fail before the next occurs.
4. Decrement stack depth only after normal completion of the loop. The source has no finally block: early return or an exception skips this decrement.

## How the translation is approached

C++ preserves this explicit ordering, including the skipped decrement. A scope guard that always decremented would be a tidy-looking but behavior-changing rewrite.

## What the tests tell us

The driver varies script instruction limits 0, 1, 2, 8 and 1024, nested block depth, RETURN before later statements and failing assertions. The harness compares both instruction count and final stack depth, making bookkeeping observable.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java).

Method navigation (declarations with bodies found in this file): [StatementBlock](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java#L27), [run](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java#L37).

## Check your understanding

Trace RETURN TRUE RETURN FALSE: which statements execute, and does this block reach its final decrement?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `9661ce67b0821ac7fc864a92a2138092409557e8c114620f1001af8de059e9d8`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
