# ScriptRow — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/wallet/ScriptRow.java](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java) | [src/org/minima/database/wallet/ScriptRow.cpp](../../../../../../../src/org/minima/database/wallet/ScriptRow.cpp) |

## What this file does

Stores a wallet script, its address, associated public key and simple/default/track flags, and derives the human-readable address for JSON.

## Follow the logic

1. The SQL constructor reads script/address strings, simple and defaultaddress ints, publickey string, then track int. Every nonzero integer, including a negative value, becomes true.
2. The direct constructor stores supplied strings and booleans without checking that script and address correspond.
3. Getters return those values. toJSON also parses address as MiniData and generates an Mx address each time it is called.
4. Consequently a row can construct successfully with an invalid or null address and fail later during JSON conversion. Null script or public key is independently permitted and can serialize as JSON null.
5. The JSON key for the default flag is default, whereas the SQL column name is defaultaddress. Matching those names at the correct boundary is part of the translation.

## How the translation is approached

Native optional strings retain null and !=0 reproduces Java integer booleans. Mx generation reuses the mapped Address and MiniData implementations. A null address reaches optional.value failure in C++, which is not the same exception type as Java’s null path.

## What the tests tell us

The fixture varies invalid/empty/hex/Mx-invalid addresses, null strings and integer boolean extrema. It observes delayed JSON failures and SQL getter order, distinguishing row construction from address formatting. Generic ERROR matching does not prove exact invalid-address/null error classes. The [Java row oracle](../../../../../../../support/WalletRowsOracle.java) uses a ResultSet proxy that logs each getter and can throw SQLException at a chosen call. The [native row harness](../../../../../../../support/wallet_rows.cpp) supplies the corresponding SqlRow callbacks. The [driver](../../../../../../../tools/wallet_rows_differential.py) compares direct and SQL constructors, getter results, JSON and failure order. Its inspected report has 1,710 matches across all three row types, with zero failures. SQL driver conversion and persistence are explicitly excluded.

## Dependencies and reading map

Direct Minima imports: [Address](../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [ScriptRow](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L23), [ScriptRow](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L51), [getScript](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L60), [getAddress](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L64), [isSimple](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L68), [isDefault](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L72), [getPublicKey](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L76), [isTrack](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L80), [toJSON](../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java#L84).

## Check your understanding

What happens for SQL simple=-1? Why does a successfully constructed ScriptRow not guarantee that calling toJSON will succeed?

## Limits and traceability

The record performs no script ownership or address-consistency validation. Real JDBC conversions, database storage and all exact failure messages are outside the controlled row fixture. The source-map counterpart is partial and parity not_verified.

Pinned Java SHA-256: `14ba711469fe6041eb2659a71f8613e54ac361a5c4c4646747f111a2d7ca957a`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
