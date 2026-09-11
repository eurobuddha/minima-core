# TreeKey — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/keys/TreeKey.java](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java) | [src/org/minima/objects/keys/TreeKey.cpp](../../../../../../../src/org/minima/objects/keys/TreeKey.cpp) |

## What this file does

A stateful hierarchy of one-time signing keys. It selects a path from a usage counter and produces a chain of signatures ending at the message.

## Follow the logic

1. Store levels and keys per level, rejecting only levels above eight at this initial check. Compute maxUses by Math.pow followed by Java int conversion, initialize usage to zero and build the root TreeKeyNode.
2. Before signing, if uses is at least maxUses, log a serious error and reset uses to zero. This is the pinned source’s observable exhaustion behavior; the class itself does not refuse key reuse.
3. Convert uses into base keysPerLevel digits, pad to the requested levels and reverse to get the root-to-leaf path.
4. At each selected key, obtain its public key and MMR proof. At intermediate levels, sign the child root once and cache that parent-child proof. At the final level, sign the actual message.
5. Append proofs in root-to-leaf order and increment uses only after the loop succeeds. Errors before completion can leave lazily created children or cached signatures while leaving uses unchanged.
6. Verification rejects more than eight proofs and rejects a first derived root different from the configured public key. Each intermediate signature must verify the next proof’s derived root; the final signature must verify the supplied message. An empty proof chain returns false.
7. setPublicKey permits verification with a default-constructed public-only object. setUses directly replaces the counter without range validation; callers are responsible for persistent usage policy.

## How the translation is approached

C++ preserves Java double-to-int saturation when computing maxUses and explicit integer wraparound when incrementing uses. It stores optional private/public values and unique ownership of the root, while an injectable logger supports testing. The nullable signing overload can reproduce a null message error at the final node.

## What the tests tell us

The driver uses a fixed seed, branch/depth/use combinations including the last available uses, zero depth, negative usage, zero keys and excessive depth. Successful cases produce two signatures and compare public key, maxUses, resulting usage counts, complete signature bytes and verification. Derived tests alter the message, public key and signature length, and separately test an empty chain. Evidence comes from the actual [Java oracle](../../../../../../../support/ObjectOracle.java), [native harness](../../../../../../../support/objects.cpp) and [object differential driver](../../../../../../../tools/object_differential.py). The inspected full-object result is 9,244 matches with zero failures; it is not a per-file assertion count. Generic errors do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MMRProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [MiniFormat](../../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [createDefault](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L25), [TreeKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L42), [TreeKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L44), [setPublicKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L68), [getPublicKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L72), [getPrivateKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L76), [getMaxUses](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L80), [getUses](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L84), [setUses](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L88), [getSize](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L92), [getDepth](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L96), [sign](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L100), [verify](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L186), [baseConversion](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L234), [main](../../../../../../../../upstream-study/src/org/minima/objects/keys/TreeKey.java#L258).

## Vault/signing follow-up

The vault/signing experiment also exercises default 64-by-64-by-64 trees with use indices -1, 0, 1 and 262144. Successful custom signatures are independently verified. Exhaustion logging now uses the actual MinimaLogger by default, preserving its observable output and receiver effects; existing explicit logger callbacks remain supported. The object-only fixture captures signing logs as its Java counterpart does. See the [experiment and results](../../../../../../new-tests/vault-and-signing.md).

## Check your understanding

For two keys per level and three levels, write the path for uses=3. Which parent-child signatures can be reused when signing the next message at uses=4?

## Limits and traceability

The exhaustion reset must be documented faithfully but is not a safe application key-use policy. Wallet persistence and backup/restore must prevent unintended one-time key reuse. These fixtures establish selected parity behavior, not a cryptographic audit or complete durability guarantee. Source-map status remains partial and parity not_verified.

Pinned Java SHA-256: `3483c44a628382707674b615ae9f0cdfb9998d6dd1663848421ea200b27fabdd`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## SPHINCS integration follow-up

The SPHINCS integration adds a nullable public-key setter and preserves Java null-comparison failure after computing the first proof root. Empty and overlong signature lists still return false before that comparison. See the [signer lab](../../../../../../../student-guide/new-tests/sphincs-signer.md) for validation in progress.

## SPHINCS transaction error follow-up

A nullable Signature overload now preserves the source null-dereference boundary, including the observed pinned-JDK diagnostic. Valid references delegate to the existing verifier. See the [transaction lab](../../../../../../new-tests/sphincs-transactions.md).

## Nullable verification data

The pointer-data overload now preserves source rejection order for a null transaction ID: empty/overlong proof lists and mismatching roots return false first; only the final matching proof dereferences the message. A null signature still fails first. The existing value overload delegates to the same verifier. See the [Transaction lab](../../../../../../new-tests/streamable-transaction.md).
