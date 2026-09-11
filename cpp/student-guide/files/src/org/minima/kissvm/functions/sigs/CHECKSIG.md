# CHECKSIG — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/sigs/CHECKSIG.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/CHECKSIG.java) | [src/org/minima/kissvm/functions/sigs/CHECKSIG.cpp](../../../../../../../../src/org/minima/kissvm/functions/sigs/CHECKSIG.cpp) |

## What this file does

Cryptographically verify a serialized TreeKey signature over supplied data.

## Follow the logic

1. Require three HEX arguments and charge31 extra instructions before extracting them.
2. Reject a zero-length public key or signature.
3. Create a TreeKey verifier with the supplied public key, decode Signature and verify the data.
4. Return the verification Boolean.

## How the translation is approached

C++ reuses the native TreeKey and Signature rather than introducing an unrelated signing scheme. A missing decoded signature currently uses a generic native exception; the harness normalizes this category.

## What the tests tell us

KissVMOracle signs0x1234 with an actual TreeKey, then emits the valid case, modified data and a wrong public key. The driver adds empty/malformed signature cases and compares instruction accounting.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Signature](../../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java), [TreeKey](../../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java).

Method navigation (declarations with bodies found in this file): [CHECKSIG](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/CHECKSIG.java#L18), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/CHECKSIG.java#L23), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/CHECKSIG.java#L63), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/CHECKSIG.java#L68).

## Check your understanding

Why should the additional instruction charge be checked even for an invalid signature?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `6fa710f6bff484a228fb95a577b6a2bca48bdceef5bf44b3280415637ef2cf8e`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## SPHINCS transaction error follow-up

Malformed serialized signatures can convert to null after I/O logging. CHECKSIG now passes that null through the actual TreeKey verifier instead of substituting a generic native error. The full SPHINCS script mutation test found and verifies this correction. See the [transaction lab](../../../../../../../new-tests/sphincs-transactions.md).
