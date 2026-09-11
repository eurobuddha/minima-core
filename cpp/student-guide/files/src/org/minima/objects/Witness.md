# Witness — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/Witness.java](../../../../../../../upstream-study/src/org/minima/objects/Witness.java) | [src/org/minima/objects/Witness.cpp](../../../../../../src/org/minima/objects/Witness.cpp) |

## What this file does

Collects the signatures, input coin proofs and script proofs supplied alongside a transaction. It stores evidence; it does not decide whether that evidence is valid.

## Follow the logic

1. Initialize three independent lists. Each clear method removes only its corresponding category.
2. addSignature rejects Java null and deduplicates by the root public key’s canonical hex string. isSignedBy compares that exact string, so this is a membership query rather than cryptographic signature verification.
3. Coin proofs are appended without deduplication. Script proofs are deduplicated by address bytes; getScript returns the first matching proof or null.
4. The list getters expose Java’s mutable lists. Callers can therefore bypass add-method deduplication by modifying a returned collection.
5. JSON emits signatures, mmrproofs and scripts. Binary output uses the same category order, prefixing each list with a MiniNumber count.
6. Reading replaces each category list immediately before reading its count, then appends decoded entries directly. It does not call the deduplicating add methods. Duplicate entries on the wire are preserved, and a failed read can leave earlier categories replaced while later ones retain old contents.

## How the translation is approached

Native Witness now reuses Signature’s shared record and owned list-handle pattern. readDataStream replaces each list immediately before its count; clear methods still mutate the current list. Copies of Witness share the record, so containing TxPoW objects observe replacements. Streamable writers append nested child fields directly. A pointer returned by getScript can still be invalidated by later vector growth, unlike a Java object reference.

## What the tests tell us

The witness fixture uses counts 0, 1, 2 and 5. Each iteration adds the same signature twice and the TRUE script twice but appends one coin proof, so matching output directly checks insertion deduplication and coin accumulation. Java-produced wire, truncated and trailing variants exercise reconstruction. This fixture does not by itself authenticate signatures or prove alias lifetime behavior.

Evidence: [Java oracle](../../../../../../support/ObjectOracle.java), [native harness](../../../../../../support/objects.cpp) and [object differential driver](../../../../../../tools/object_differential.py). The inspected object report records 9,464 matching observations across the complete corpus. That total is not this file’s test count; generic ERROR normalization does not prove exact exception classes.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Signature](../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Witness](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L35), [clearSignatures](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L44), [addSignature](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L48), [getAllSignatures](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L59), [getAllSignatureKeys](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L63), [isSignedBy](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L73), [clearCoinProofs](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L86), [addCoinProof](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L90), [getAllCoinProofs](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L94), [clearScriptProofs](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L104), [addScript](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L108), [getScript](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L114), [getAllScripts](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L124), [scriptExists](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L128), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L132), [toString](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L163), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L168), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/Witness.java#L189).

## Vault/signing follow-up

The vault/signing fixture directly tests addSignature(null) and isSignedBy(null), both with and without existing signatures. The new nullable-signature overload reuses the existing deduplicating value overload after rejecting null. isSignedBy still collects existing signature roots before comparing the optional input, preserving failure order. Signature now shares its own record; this earlier fixture does not by itself establish complete collection or child identity. See the [experiment and results](../../../../../new-tests/vault-and-signing.md).

## Check your understanding

Why can reading a serialized witness produce duplicate scripts even though addScript prevents them? Which API path would a test need to use to distinguish those behaviors?

## Limits and traceability

A native Signature passed by value cannot express Java null. Ownership, retained references and arbitrary direct list mutation need focused tests beyond the ordinary object fixture. Consensus checking remains in TxPoWChecker and related code. Counterpart status remains partial and full parity not_verified.

Pinned Java SHA-256: `6f4807e19f59bc2d9025f6737e61ada542e18e02d807955e898b0193717fc6b9`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Retained coin objects

The [coin ownership lab](../../../../../new-tests/coin-ownership.md) follows a single coin retained in transaction inputs, outputs and a witness proof. Ordinary native Coin copies now share field storage, while explicit deep copies reconstruct independent records. The new experiment checks mutation visibility, container growth/removal/clear and handle reassignment. Other collection and element identities remain separate fidelity questions; consult the lab for measured results and exclusions.

## Signing calls versus stored signatures

The [offline-file lab](../../../../../new-tests/offline-send-files.md) demonstrates a caller interaction: sendsign signs each repeated input key, but addSignature retains only the first proof for that root in each witness. We verify the retained signature and count SQL key-use writes separately. Deduplicating signing calls would silently change wallet effects even if the final witness bytes matched. Witness production code is reused unchanged in this batch.

## Streamable and list ownership follow-up

The [Witness lab](../../../../../new-tests/streamable-witness.md) explains owned list replacement, retained complete contents, source-ordered partial reads and writes, containing TxPoW aliases, direct-wire duplicates and clear-versus-replace behavior. It extends the common interface differential suite and independently predicts partial list contents. Full class parity remains open.
