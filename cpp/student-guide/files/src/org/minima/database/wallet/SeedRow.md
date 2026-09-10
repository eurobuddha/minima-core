# SeedRow — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/wallet/SeedRow.java](../../../../../../../../upstream-study/src/org/minima/database/wallet/SeedRow.java) | [src/org/minima/database/wallet/SeedRow.cpp](../../../../../../../src/org/minima/database/wallet/SeedRow.cpp) |

## What this file does

A two-field wallet record containing a seed phrase and its seed representation.

## Follow the logic

1. The ResultSet constructor reads phrase first, then seed using getString. Any SQL failure stops construction at that point.
2. The direct constructor stores the two supplied strings, including null. It does not validate the mnemonic, derive a seed or check correspondence between fields.
3. Getters expose each value. toJSON includes both phrase and seed without redaction; that differs from KeyRow’s JSON omission of its private key.

## How the translation is approached

C++ uses optional<string> for Java nullable strings and an abstract SqlRow for column access. JSON preserves null explicitly. This is a data adapter, not a BIP39 implementation or a native database driver.

## What the tests tell us

The seeded fixture varies null, empty, ordinary and Unicode strings through both constructors. Injected failures after the first or second SQL call verify field-read order; later configured failure numbers are not reached by this two-column constructor. The [Java row oracle](../../../../../../../support/WalletRowsOracle.java) uses a ResultSet proxy that logs each getter and can throw SQLException at a chosen call. The [native row harness](../../../../../../../support/wallet_rows.cpp) supplies the corresponding SqlRow callbacks. The [driver](../../../../../../../tools/wallet_rows_differential.py) compares direct and SQL constructors, getter results, JSON and failure order. Its inspected report has 1,710 matches across all three row types, with zero failures. SQL driver conversion and persistence are explicitly excluded.

## Dependencies and reading map

Direct Minima imports: [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [SeedRow](../../../../../../../../upstream-study/src/org/minima/database/wallet/SeedRow.java#L13), [SeedRow](../../../../../../../../upstream-study/src/org/minima/database/wallet/SeedRow.java#L18), [getPhrase](../../../../../../../../upstream-study/src/org/minima/database/wallet/SeedRow.java#L23), [getSeed](../../../../../../../../upstream-study/src/org/minima/database/wallet/SeedRow.java#L27), [toJSON](../../../../../../../../upstream-study/src/org/minima/database/wallet/SeedRow.java#L31).

## Check your understanding

Can a SeedRow contain a phrase and seed that do not correspond? Which component, outside this class, must establish that relation?

## Limits and traceability

This class intentionally exposes its stored fields through JSON. Persistence, mnemonic validation and application decisions about displaying seed material remain outside these row tests. The source-map counterpart is partial and parity not_verified.

Pinned Java SHA-256: `acdda96345e335286254c5c63f985a89d33a6a3d6a87c50d2a52afec690873e4`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
