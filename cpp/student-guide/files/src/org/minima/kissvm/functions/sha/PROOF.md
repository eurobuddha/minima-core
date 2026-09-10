# PROOF — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/sha/PROOF.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/PROOF.java) | [src/org/minima/kissvm/functions/sha/PROOF.cpp](../../../../../../../../src/org/minima/kissvm/functions/sha/PROOF.cpp) |

## What this file does

Verify an MMR membership proof against an expected hash and sum.

## Follow the logic

1. Require data, leaf sum, root hash, root sum and serialized proof. Data must be HEX or STRING.
2. Build the domain-separated MMR leaf using the matching Streamable type and supplied sum.
3. Decode the proof; an IOException becomes the source-specific execution error.
4. Fold the leaf through the proof and compare both hash and sum with the expected root.

## How the translation is approached

C++ reuses MMRData and MMRProof. Its current decoder catch accepts std::exception broadly; the KISS harness normalizes generic runtime failures, so full exception-category parity is still open.

## What the tests tell us

KissVMOracle builds real3-leaf trees for both HEX and STRING leaves. It emits a valid proof and an altered leaf-sum case for each; the driver also supplies a malformed proof.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MMRData](../../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [MMRProof](../../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java).

Method navigation (declarations with bodies found in this file): [PROOF](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/PROOF.java#L18), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/PROOF.java#L23), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/PROOF.java#L73), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/PROOF.java#L78).

## Check your understanding

Why is a matching root hash alone insufficient for an MMRData equality check?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `bd025be435fce1514b7e325cc683138b68ebaf97be3ef6f390d3ee39a2942070`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
