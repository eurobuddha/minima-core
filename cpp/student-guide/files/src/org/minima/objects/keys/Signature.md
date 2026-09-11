# Signature — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/keys/Signature.java](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java) | [src/org/minima/objects/keys/Signature.cpp](../../../../../../../src/org/minima/objects/keys/Signature.cpp) |

## What this file does

An ordered chain of SignatureProof objects linking a tree’s root public key down to a signature of the final message.

## Follow the logic

1. Start with an empty mutable list. addSignatureProof appends without deduplication or cryptographic checks.
2. getRootPublicKey reads the first proof’s derived root. An empty Signature is serializable and has valid empty JSON, but asking it for a root fails on index zero.
3. JSON contains a signatures array. Binary output writes a MiniNumber count followed by each proof in order.
4. Java reading replaces the existing list before reading the count, then appends complete proofs. The convenience MiniData converter catches IOException, logs and returns null.
5. Verification of the chain belongs to TreeKey.verify. Merely being able to decode this container does not authenticate its contents.

## How the translation is approached

Native storage is a vector with mutable access. Its root getter explicitly throws a Java-typed IndexOutOfBoundsException for an empty list. Static deserialization returns a fresh object; the optional-returning converter now catches only wire::IOException, logs through MinimaLogger and returns an empty optional. Other exceptions propagate as in Java.

## What the tests tell us

The signature branch creates 0, 1, 2 and 5 proof entries and compares bytes/JSON. It intentionally uses arbitrary short signature bytes, so this is container testing. Tree-key-derived verification cases separately check valid and altered signature chains. Evidence comes from the actual [Java oracle](../../../../../../../support/ObjectOracle.java), [native harness](../../../../../../../support/objects.cpp) and [object differential driver](../../../../../../../tools/object_differential.py). The inspected full-object result is 9,244 matches with zero failures; it is not a per-file assertion count. Generic errors do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [Signature](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L23), [addSignatureProof](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L25), [getAllSignatureProofs](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L29), [getRootPublicKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L33), [toJSON](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L37), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L50), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L58), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L68), [convertMiniDataVersion](../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java#L77).

## Check your understanding

Why can an empty Signature round-trip successfully but fail when added to Witness? Trace the root-key lookup used by Witness.addSignature.

## Limits and traceability

The [token-validation lab](../../../../../../../student-guide/new-tests/token-validation.md) now checks converter I/O logging and the escaping NumberFormatException for a zero-length BigInteger. Mutable list identity across reads, inherited APIs and exhaustive logger failure behavior remain open. The empty-list getter’s typed fix does not establish every Signature error path. Source-map status remains partial and parity not_verified.

Pinned Java SHA-256: `5fd453e5f2df679cf33afc68d993cc8c0b10d781418c78a024b5e7fe9e65059c`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## Incremental output for the SPHINCS envelope

The native writeDataStream(vector&) now appends the existing encoding to a supplied
buffer. Signature writes its count and delegates each SignatureProof; SignatureProof
writes the public key and signature, then delegates the shared MMR proof writer.
A bad later proof hash leaves the already-written prefix intact. serialise wraps
this writer with a fresh vector, preserving successful output bytes.
The [envelope lab](../../../../../../../student-guide/new-tests/sphincs-signature-envelope.md)
checks this failure boundary, and the existing object corpus regresses normal wire
behavior. This does not add representation of Java null proof-list entries.
