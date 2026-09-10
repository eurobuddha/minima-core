# TreeKeyNode — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/keys/TreeKeyNode.java](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java) | [src/org/minima/objects/keys/TreeKeyNode.cpp](../../../../../../../src/org/minima/objects/keys/TreeKeyNode.cpp) |

## What this file does

One level of a deterministic signing-key tree: a collection of one-time keys committed by an MMR, plus lazily built child trees.

## Follow the logic

1. Remember the branching size and derive childSeed by hashing the serialized private seed. The child seed separates descendant derivation from this level’s individual key derivation.
2. For each index i, hash serialized MiniNumber(i) and the original private seed together. Use that derived seed to construct a Winternitz key, then add its public key as a zero-sum MMR leaf.
3. The MMR root becomes this node’s public key. Keys remain indexed so callers can retrieve the chosen one-time key and its membership proof.
4. getChild checks the indexed cache. If absent, hash the index with childSeed and build a new node of the same size. Later calls reuse that child.
5. A child also caches its parent-to-child SignatureProof. TreeKey can reuse this fixed signature of the child root while selecting different message keys below it.

## How the translation is approached

The native node uses unique_ptr children, a vector of Winternitz values, a mapped MMR and an optional cached parent signature. It creates descendants only when requested. Direct child/key array accesses now check bounds and throw the existing JavaArrayIndexOutOfBoundsException adapter, including index and array length. Negative/zero constructor sizes still need complete exception-parity review.

## What the tests tell us

The treekey fixture exercises this node indirectly through complete key construction, public-key equality, two successive signatures and verification, across branching sizes 1, 2 and 3 and several depths/use counts. This checks deterministic roots and selected descendant paths, but does not independently assert all cache object identities. Evidence comes from the actual [Java oracle](../../../../../../../support/ObjectOracle.java), [native harness](../../../../../../../support/objects.cpp) and [object differential driver](../../../../../../../tools/object_differential.py). The inspected full-object result is 9,244 matches with zero failures; it is not a per-file assertion count. Generic errors do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java), [MMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [MMREntryNumber](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java), [MMRProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [Crypto](../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java).

Method navigation (declarations with bodies found in this file): [TreeKeyNode](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java#L26), [getChild](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java#L69), [getPublicKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java#L83), [getWOTSKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java#L87), [getProof](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java#L91), [childSigExists](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java#L95), [setParentChildSig](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java#L99), [getParentChildSig](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKeyNode.java#L103).

## Vault/signing follow-up

The vault/signing comparison first failed for custom keyuses=-1: Java reported an array-bounds error while C++ reported RuntimeException: vector. Reusing the typed exception adapter repairs that mismatch. Twelve direct child/key cases cover INT_MIN, -1, 0, 1, 2 and INT_MAX on a two-key node; valid indices still execute real key construction. See the [experiment and results](../../../../../../new-tests/vault-and-signing.md).

## Check your understanding

Why use hash(index, childSeed) for child trees instead of hash(index, originalSeed), which already derives the keys at the current level?

## Limits and traceability

No direct proof of all child-cache alias behavior follows from matching signature bytes. A full lifetime review must also account for references returned to children and one-time keys. Source-map status remains partial and parity not_verified.

Pinned Java SHA-256: `73c0006c688955ea8d1f2b77d4ad04b394b1ff6aa422b9f9a7b6d45b667a7834`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
