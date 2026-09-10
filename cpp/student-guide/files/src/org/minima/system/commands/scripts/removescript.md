# removescript — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native command is implemented and its scoped Java/C++ comparison passes; full parity remains unverified.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/scripts/removescript.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/removescript.java) | [src/org/minima/system/commands/scripts/removescript.cpp](../../../../../../../../src/org/minima/system/commands/scripts/removescript.cpp) |

## What this file does

Removes the wallet script identified by a normalized address.

## Follow the logic

1. Create the standard reply and resolve the wallet.
2. Read address through getAddressParam, which performs the established Minima-address conversion.
3. Call removeScript without first checking whether a row exists.
4. After that call returns, report Script removed. This wording does not establish that a record previously existed.
5. The help string starts with a carriage-return escape from the source spelling; exact metadata comparison should retain actual characters rather than a visually corrected string.

## How the translation is approached

The native command now uses the existing Command parameter conversion, actual MinimaDB wallet owner, Wallet methods, and Address/Contract/ScriptRow/Signature components at the source call boundaries. The four commands are registered for dispatch. Complete native H2 storage and Main lifecycle integration remain absent.

## What the tests tell us

The new wallet_commands_differential comparison runs these actual commands through the actual Wallet. Only the SQL driver is replaced with a recording receiver. It compares metadata, returned JSON, SQL call order, logs and selected cache membership, injecting failures at successive SQL operations. The normal and sanitizer comparisons each pass 675 programs, 1,907 operations and 50,275 SQL calls. These totals cover all four commands together, not this file alone. See the [worked wallet-command lab](../../../../../../../new-tests/wallet-commands.md) for the failing examples, fixes and validation limits.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [removescript](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/removescript.java#L13), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/removescript.java#L18), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/removescript.java#L34), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/removescript.java#L39), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/scripts/removescript.java#L58).

## Check your understanding

Test absent and existing addresses with the same wallet return behavior. Should the command replies differ? Why must malformed address input be tested before the delete callback?

## Limits and traceability

This command has a native counterpart and passing scoped direct comparisons. Recorded query rows do not prove durable persistence, crash-safe key-use updates or a fully integrated node. Native H2 and Main remain unfinished.

Pinned Java SHA-256: `d6784341ba024aa436687336e534eacf61158cc3700fb51f51c3644419858877`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
