# STATE — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/state/STATE.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/STATE.java) | [src/org/minima/kissvm/functions/state/STATE.cpp](../../../../../../../../src/org/minima/kissvm/functions/state/STATE.cpp) |

## What this file does

Read a current transaction state port.

## Follow the logic

1. Require exactly one numeric port.
2. Convert the number using MiniNumber.getAsInt.
3. Delegate to Contract.getState and return its Value; missing-port validation belongs there.

## How the translation is approached

The C++ wrapper preserves the shared Value result and delegates lookup to Contract.

## What the tests tell us

The fixture installs current ports 0=17 and1=[hello], and previous ports 0=12 and1=[old]. Unary cases query existing and missing ports and wrong types; result/error and monotonic state are compared.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [STATE](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/STATE.java#L10), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/STATE.java#L15), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/STATE.java#L26), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/STATE.java#L31).

## Check your understanding

Predict STATE(0) versus PREVSTATE(0), then trace how missing port2 is handled.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `dfe66ffad50f797a225fb2dbeb0d9e9b5cbbefcbd443838c8dda52d449c0c3bc`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
