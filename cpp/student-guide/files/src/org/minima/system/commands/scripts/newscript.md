# newscript — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native command is implemented and its scoped Java/C++ comparison passes; full parity remains unverified.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/scripts/newscript.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/newscript.java) | [src/org/minima/system/commands/scripts/newscript.cpp](../../../../../../../../src/org/minima/system/commands/scripts/newscript.cpp) |

## What this file does

Adds or replaces a custom wallet script with an explicit coin-tracking preference.

## Follow the logic

1. Resolve the wallet before reading script and required trackall. Optional clean defaults false.
2. When clean is true, call Contract.cleanScript before deriving the Address. Cleaning can therefore change the address used as the database key.
3. Look up the derived address; if a script row already exists, remove it first.
4. Call addScript with the selected text, simple=false, default=false, publickey=0x00 and the tracking flag. Convert the returned row to JSON.
5. Removal and insertion are separate calls. A later insertion failure can follow successful removal; a test should inspect the intermediate persistent state rather than assuming an atomic replacement.

## How the translation is approached

The native command now uses the existing Command parameter conversion, actual MinimaDB wallet owner, Wallet methods, and Address/Contract/ScriptRow/Signature components at the source call boundaries. The four commands are registered for dispatch. Complete native H2 storage and Main lifecycle integration remain absent.

## What the tests tell us

The new wallet_commands_differential comparison runs these actual commands through the actual Wallet. Only the SQL driver is replaced with a recording receiver. It compares metadata, returned JSON, SQL call order, logs and selected cache membership, injecting failures at successive SQL operations. The normal and sanitizer comparisons each pass 675 programs, 1,907 operations and 50,275 SQL calls. These totals cover all four commands together, not this file alone. See the [worked wallet-command lab](../../../../../../../new-tests/wallet-commands.md) for the failing examples, fixes and validation limits.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [newscript](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/newscript.java#L16), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/newscript.java#L21), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/newscript.java#L48), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/newscript.java#L53), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/newscript.java#L90).

## Check your understanding

Compare clean:false and clean:true for whitespace-bearing source. Then inject addScript failure after a matching old row is removed: what state must your fixture observe?

## Limits and traceability

This command has a native counterpart and passing scoped direct comparisons. Recorded query rows do not prove durable persistence, crash-safe key-use updates or a fully integrated node. Native H2 and Main remain unfinished.

Pinned Java SHA-256: `43e79c5a9a9e58d4d13348409357329805f5d31f4b642b14d45cb65fdb6178ad`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
