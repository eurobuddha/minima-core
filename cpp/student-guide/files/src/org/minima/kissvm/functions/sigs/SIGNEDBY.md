# SIGNEDBY — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/sigs/SIGNEDBY.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/SIGNEDBY.java) | [src/org/minima/kissvm/functions/sigs/SIGNEDBY.cpp](../../../../../../../../src/org/minima/kissvm/functions/sigs/SIGNEDBY.cpp) |

## What this file does

Ask whether a public key is present among the Contract’s accepted signatures.

## Follow the logic

1. Require one HEX public key.
2. Ask Contract.checkSignature whether the key is recognized.
3. Return the Boolean; this wrapper does not decode and cryptographically verify a new signature.

## How the translation is approached

C++ delegates to the same Contract signature membership operation.

## What the tests tell us

The rich fixture contains accepted keys 0xCA and0xFE; SIGNEDBY(0xCA) and shared unary invalid-type/arity cases run through the real evaluator.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [SIGNEDBY](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/SIGNEDBY.java#L12), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/SIGNEDBY.java#L17), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/SIGNEDBY.java#L31), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/SIGNEDBY.java#L36).

## Check your understanding

Explain the distinction between SIGNEDBY and CHECKSIG in a validation pipeline.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `f41ef00f7ee244666009f76f83545387c80588f61396d5dabaefc7911ceee002`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
