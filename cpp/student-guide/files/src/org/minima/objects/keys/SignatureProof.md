# SignatureProof — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/keys/SignatureProof.java](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java) | [src/org/minima/objects/keys/SignatureProof.cpp](../../../../../../../src/org/minima/objects/keys/SignatureProof.cpp) |

## What this file does

Stores a one-time public key, its signature bytes, and a path connecting that key to a root public key.

## Follow the logic

1. Keep the public key and signature as MiniData and the path as MMRProof. The constructor does not verify the signature.
2. Compute a zero-sum MMR leaf from the public key, then fold the proof and return its root hash as the root public key.
3. JSON presents the leaf public key, derived root key, proof and signature. The derived root key is calculated rather than serialized independently.
4. Write public key, signature and proof in that order. Read the same sequence to rebuild the wrapper.

## How the translation is approached

The native class uses value members and move construction. It reuses the common MMRData and MMRProof implementations for root derivation, keeping signature cryptography in the separate signature/key classes.

## What the tests tell us

The sigproof fixture builds explicit key/signature bytes with an empty proof at time 17 and compares wire data, JSON and the derived root key. Tree-key and signature cases elsewhere in the corpus exercise larger structures; this wrapper test by itself does not authenticate signature bytes.

The [Java object oracle](../../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [MMRProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [SignatureProof](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L22), [SignatureProof](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L24), [getPublicKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L30), [getSignature](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L34), [getProof](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L38), [getRootPublicKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L42), [toJSON](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L50), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L62), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L69), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/keys/SignatureProof.java#L75).

## Check your understanding

Why can arbitrary signature bytes still produce a well-formed SignatureProof JSON object? Identify the additional operation needed before trusting the signature.

## Limits and traceability

 The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `4c06b05f28612d1327f70a02f86886cead02d7c2b032d605e6cfae2188f8dc6b`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
