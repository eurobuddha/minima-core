# scripts — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native command is implemented and its scoped Java/C++ comparison passes; full parity remains unverified.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/scripts/scripts.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/scripts.java) | [src/org/minima/system/commands/scripts/scripts.cpp](../../../../../../../../src/org/minima/system/commands/scripts/scripts.cpp) |

## What this file does

Lists tracked wallet scripts or retrieves one exact address row.

## Follow the logic

1. Resolve the wallet and read optional address, defaulting to empty.
2. A case-insensitive mx prefix triggers Address.convertMinimaAddress. Only IllegalArgumentException from that conversion is wrapped as CommandException with the original exception text.
3. An empty address requests getAllAddresses and preserves returned row order while converting each row to JSON.
4. Otherwise perform an exact wallet lookup on the supplied or converted address. A missing row raises Script with that address not found.
5. Ordinary hex-looking text is not passed through getAddressParam here. Direct comparison with removescript should therefore include malformed non-Mx text and case differences.

## How the translation is approached

The native command now uses the existing Command parameter conversion, actual MinimaDB wallet owner, Wallet methods, and Address/Contract/ScriptRow/Signature components at the source call boundaries. The four commands are registered for dispatch. Complete native H2 storage and Main lifecycle integration remain absent.

## What the tests tell us

The new wallet_commands_differential comparison runs these actual commands through the actual Wallet. Only the SQL driver is replaced with a recording receiver. It compares metadata, returned JSON, SQL call order, logs and selected cache membership, injecting failures at successive SQL operations. The normal and sanitizer comparisons each pass 675 programs, 1,907 operations and 50,275 SQL calls. These totals cover all four commands together, not this file alone. See the [worked wallet-command lab](../../../../../../../new-tests/wallet-commands.md) for the failing examples, fixes and validation limits.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [scripts](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/scripts.java#L17), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/scripts.java#L22), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/scripts.java#L40), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/scripts.java#L45), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/scripts.java#L91).

## Check your understanding

Why can the empty-address result be an array while a successful address lookup returns one object? Design a test that catches accidental normalization of ordinary hex text.

## Limits and traceability

This command has a native counterpart and passing scoped direct comparisons. Recorded query rows do not prove durable persistence, crash-safe key-use updates or a fully integrated node. Native H2 and Main remain unfinished.

Pinned Java SHA-256: `10b1c311dbcc6a86bfa532889fc946089a887a115b09a34e35e41b2b88f27a77`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
