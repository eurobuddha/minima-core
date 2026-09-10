# Winternitz — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/keys/Winternitz.java](../../../../../../../../upstream-study/src/org/minima/objects/keys/Winternitz.java) | [src/org/minima/objects/keys/Winternitz.cpp](../../../../../../../src/org/minima/objects/keys/Winternitz.cpp) |

## What this file does

The Minima wrapper around Winternitz one-time signatures using SHA3-256 and the Bouncy Castle GMSS parameter w=8.

## Follow the logic

1. The Java seeded constructor passes private seed bytes, SHA3Digest(256) and parameter eight to WinternitzOTSignature, then caches its public key. Its default constructor leaves the signing engine and public key null.
2. Java sign delegates message bytes to the library and returns signature bytes as MiniData. Java verify asks WinternitzOTSVerify to reconstruct a public key and compares it with the supplied key.
3. The inspected native algorithm hashes the message to 32 byte-valued digits and computes two checksum digits from 8192 minus their sum. These give 34 hash-chain positions.
4. Native private elements are generated from the first 32 seed bytes. Each iteration hashes the current seed, then updates the seed as a little-endian addition of seed + hash + 1 modulo 2^256.
5. To derive the native public key, advance every private element through 255 SHA3 rounds, concatenate endpoints and hash them. Signing advances each chain only by its message/checksum digit.
6. Verification requires 34×32 signature bytes, advances each received chain through its remaining 255-digit rounds, hashes the endpoints and compares the resulting public key. This explains how verification can finish a chain without knowing its starting secret.

## How the translation is approached

The C++ counterpart implements the required hash chains with existing MiniData.sha3 and stores optional private/public state. These detailed steps describe the native body; the Java wrapper delegates to bundled Bouncy Castle classes whose source bodies were not available in this repository search. Compatibility evidence therefore comes from executing the Java library through the oracle, not from claiming that its implementation was read.

## What the tests tell us

The wots branch crosses seed lengths 0, 31, 32, 33 and 64 with message lengths 0, 1, 32 and 65. Successful outputs compare public key and every signature byte, then verify. Derived cases mutate the message/public key and shorten or extend the signature, testing rejection/error behavior. Matching those vectors does not constitute a security proof. Evidence comes from the actual [Java oracle](../../../../../../../support/ObjectOracle.java), [native harness](../../../../../../../support/objects.cpp) and [object differential driver](../../../../../../../tools/object_differential.py). The inspected full-object result is 9,244 matches with zero failures; it is not a per-file assertion count. Generic errors do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java).

Method navigation (declarations with bodies found in this file): [Winternitz](../../../../../../../../upstream-study/src/org/minima/objects/keys/Winternitz.java#L24), [Winternitz](../../../../../../../../upstream-study/src/org/minima/objects/keys/Winternitz.java#L26), [getPublicKey](../../../../../../../../upstream-study/src/org/minima/objects/keys/Winternitz.java#L36), [sign](../../../../../../../../upstream-study/src/org/minima/objects/keys/Winternitz.java#L40), [verify](../../../../../../../../upstream-study/src/org/minima/objects/keys/Winternitz.java#L48).

## Check your understanding

Why does a signature reveal a point partway along each hash chain rather than the private chain start? Explain why the usage counter belongs in TreeKey/wallet handling rather than this wrapper.

## Limits and traceability

Short seeds, default construction and malformed signature sizes have Java null/library failure behavior that the native std::invalid_argument/optional failures do not exactly reproduce. This lesson describes a one-time scheme; repeated signing with the same private key requires explicit application policy. Source-map status remains partial and parity not_verified.

Pinned Java SHA-256: `b25af05c370428a13a9c19bf3e91b2dbfff663ba18e90f5d72f91dda55d228d2`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
