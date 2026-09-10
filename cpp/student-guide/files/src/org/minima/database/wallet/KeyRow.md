# KeyRow — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/wallet/KeyRow.java](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java) | [src/org/minima/database/wallet/KeyRow.cpp](../../../../../../../src/org/minima/database/wallet/KeyRow.cpp) |

## What this file does

Stores a tree key’s parameters, usage counters, modifier and private/public key strings as a wallet database row.

## Follow the logic

1. Read size, depth, uses and maxuses as SQL ints, then modifier, privatekey and publickey as strings. Preserve that sequence because a failing getter stops construction.
2. The direct constructor simply stores its arguments. It does not enforce uses <= maxuses, a valid depth or consistency between private and public key.
3. Getters include access to the private key. JSON emits size/depth/uses/maxuses/modifier/publickey; the privatekey insertion is commented out and therefore absent from JSON.
4. Usage counters are stored facts, not an atomic allocation protocol. This record alone does not reserve a one-time signing key or persist its next use.

## How the translation is approached

Native int32_t matches Java int width and optional strings represent null. The SqlRow interface delegates JDBC-equivalent conversion to the eventual backend. Native public fields remain mutable, as in Java.

## What the tests tell us

Inputs include null strings, integer extrema, negative counters and Unicode. Both constructor routes are compared, all seven SQL failure positions are injected, and the harness separately captures private-key getter output and public JSON, proving the deliberate omission rather than merely checking successful JSON serialization. The [Java row oracle](../../../../../../../support/WalletRowsOracle.java) uses a ResultSet proxy that logs each getter and can throw SQLException at a chosen call. The [native row harness](../../../../../../../support/wallet_rows.cpp) supplies the corresponding SqlRow callbacks. The [driver](../../../../../../../tools/wallet_rows_differential.py) compares direct and SQL constructors, getter results, JSON and failure order. Its inspected report has 1,710 matches across all three row types, with zero failures. SQL driver conversion and persistence are explicitly excluded.

## Dependencies and reading map

Direct Minima imports: [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [KeyRow](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L21), [KeyRow](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L31), [getSize](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L41), [getDepth](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L45), [getUses](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L49), [getMaxUses](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L53), [getModifier](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L57), [getPrivateKey](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L61), [getPublicKey](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L65), [toJSON](../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java#L69).

## Check your understanding

Why does absence of privatekey from JSON not imply that a KeyRow cannot reveal its private key? Identify the other public access paths.

## Limits and traceability

No validity or safe key-use policy is established by this permissive record. Database transaction semantics, crash recovery and durable counter advancement require Wallet/backend review. The source-map counterpart is partial and parity not_verified.

Pinned Java SHA-256: `8aec906da9c45465d36a019da41592069c263df07c0d40d6df197ad22d3c7896`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
