# Address — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/Address.java](../../../../../../../upstream-study/src/org/minima/objects/Address.java) | [src/org/minima/objects/Address.cpp](../../../../../../src/org/minima/objects/Address.cpp) |

## What this file does

Connects a script or raw address bytes to a human-readable Mx address with an error-detecting checksum.

## Follow the logic

1. The script constructor wraps text in MiniString, builds a zero-sum single-leaf MMR and derives the root. That root hash becomes the address bytes. The raw-data constructor instead records empty script text and accepts the supplied address bytes.
2. For Mx encoding, hash the raw bytes and take the first four hash bytes as checksum. Build an envelope containing sentinel byte 1, a two-byte length, the address bytes and checksum, then Base32 encode it.
3. The nonzero sentinel prevents numeric Base32 conversion from losing the beginning of the envelope. It is structural data rather than part of the address itself.
4. Decoding first checks that sentinel, reads a signed Java short length, reads the requested payload and four checksum bytes, and compares them with a newly calculated checksum. It does not require end-of-stream after the checksum.
5. Equality compares only address bytes. JSON also exposes script and Mx text, but these are not additional equality conditions.
6. Serialize hash-encoded address bytes followed by script. Reading rebuilds Mx text from the address bytes; it does not recompute the bytes from the script or validate their consistency.

## How the translation is approached

The native script constructor reuses the mathematical equivalence that a one-leaf proof is empty, directly returning its leaf hash. Mx encoding reuses BaseConverter and SHA3; decoding reproduces the signed-short boundary with an explicit JavaNegativeArraySizeException when the high bit is set. Native TRUE_ADDRESS is const, unlike Java’s mutable static field.

## What the tests tell us

Script cases include empty, boolean and Unicode text and compare full address bytes/JSON. Seeded raw byte cases of lengths up to 64 are encoded to Mx, decoded again, and mutated in the final character to test checksum rejection. These sizes do not cover the signed-short length boundary or arbitrary trailing-envelope bytes.

Evidence: [Java oracle](../../../../../../support/ObjectOracle.java), [native harness](../../../../../../support/objects.cpp) and [object differential driver](../../../../../../tools/object_differential.py). The inspected object report records 9,244 matching observations across the complete corpus. That total is not this file’s test count; generic ERROR normalization does not prove exact exception classes.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [MMR](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java), [MMRData](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [MMREntry](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java), [MMRProof](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [BaseConverter](../../../../../../../upstream-study/src/org/minima/utils/BaseConverter.java), [Crypto](../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Address](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L43), [Address](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L45), [Address](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L73), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L79), [toString](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L88), [getScript](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L95), [getAddressData](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L99), [getMinimaAddress](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L103), [isEqual](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L107), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L112), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L118), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L124), [makeMinimaAddress](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L133), [convertMinimaAddress](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L178), [main](../../../../../../../upstream-study/src/org/minima/objects/Address.java#L228).

## Check your understanding

Why can a deserialized address retain a script that does not hash to its stored bytes? Which separate validation would be needed if a caller wanted that invariant?

## Limits and traceability

The checksum detects accidental changes; it does not establish ownership or signature validity. Default Java Address fields are null before reading, whereas the native default contains empty values. Exact null/default behavior and all oversized envelope cases remain open. Counterpart status remains partial and full parity not_verified.

Pinned Java SHA-256: `5fc5d8e77fe88c97804d78b6640d6c2b9a41df779414dadcdae871c44461f881`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Transaction-object Streamable integration

The in-place reader replaces address bytes and then script text. Only after both succeed does it rebuild the Mx string. Failed script input can therefore expose new address bytes alongside the previous cached Mx text. The append writer preserves hash bytes if the later script write fails. Public default/null field behavior remains a separate gap.

The [transaction-object interface lab](../../../../../new-tests/streamable-transaction-objects.md) explains the source-order tests and existing generic callers. Both complete builds pass. The expanded interface suite passes 2,771 comparisons and 20,914 assertions per build across 20 types, with matching transcripts and empty native stderr. All seven affected regressions also pass in both builds. Full nullable/reference/API and node parity remain unverified.
