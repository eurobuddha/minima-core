# GETOUTKEEPSTATE — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.java](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.java) | [src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.cpp](../../../../../../../../../src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.cpp) |

## What this file does

Read the state-retention flag of a selected transaction output.

## Follow the logic

1. Require one numeric index, convert it to int, obtain the transaction’s output list and reject an index outside0..size−1.
2. Read the selected coin’s state-retention flag.
3. Return BooleanValue for keep-state.

## How the translation is approached

C++ retains the bounds check before selecting the vector entry and reuses Coin/Token/Witness. No new transaction search is introduced.

## What the tests tell us

The rich fixture has 3 outputs, a Minima coin, a scaled token coin and a coin with missing token metadata. The driver queries indices −1/0/1/2/3/0.5/4294967296 and TRUE. Input proof IDs are deliberately different from transaction IDs.

Existing test path: [kissvm_differential.py](../../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [Coin](../../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java).

Method navigation (declarations with bodies found in this file): [GETOUTKEEPSTATE](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.java#L15), [runFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.java#L20), [requiredParams](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.java#L43), [getNewFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/GETOUTKEEPSTATE.java#L48).

## Check your understanding

Use the rich fixture to predict index 1. Explain why testing only index 0 would miss token or witness behavior.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `1c19cb939a3575db8712a9958d0b15060114436d1e01c60f134b9bd15a6728d2`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
