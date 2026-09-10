# sign — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native command is implemented and its scoped Java/C++ comparison passes; full parity remains unverified.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/signatures/sign.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/signatures/sign.java) | [src/org/minima/system/commands/signatures/sign.cpp](../../../../../../../../src/org/minima/system/commands/signatures/sign.cpp) |

## What this file does

Asks the wallet to sign raw data with the private key corresponding to a supplied public key.

## Follow the logic

1. Parse required data first, then required publickey as MiniData. Their order determines the first error when both inputs are malformed.
2. Resolve the wallet and pass the public key’s canonical 0x string and data object to signData.
3. The wallet owns key lookup and stateful signing-use updates; this command does not implement either policy itself.
4. Serialize the resulting Signature through MiniData.getMiniDataVersion and return its hex bytes.
5. Signing can mutate wallet key usage before a later serialization failure. Proposed failure tests must observe both the returned error and retained usage state.

## How the translation is approached

The native command now uses the existing Command parameter conversion, actual MinimaDB wallet owner, Wallet methods, and Address/Contract/ScriptRow/Signature components at the source call boundaries. The four commands are registered for dispatch. Complete native H2 storage and Main lifecycle integration remain absent.

## What the tests tell us

The new wallet_commands_differential comparison runs these actual commands through the actual Wallet. Only the SQL driver is replaced with a recording receiver. It compares metadata, returned JSON, SQL call order, logs and selected cache membership, injecting failures at successive SQL operations. The normal and sanitizer comparisons each pass 675 programs, 1,907 operations and 50,275 SQL calls. These totals cover all four commands together, not this file alone. See the [worked wallet-command lab](../../../../../../../new-tests/wallet-commands.md) for the failing examples, fixes and validation limits.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Signature](../../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [sign](../../../../../../../../../upstream-study/src/org/minima/system/commands/signatures/sign.java#L15), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/signatures/sign.java#L20), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/signatures/sign.java#L36), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/signatures/sign.java#L41), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/signatures/sign.java#L59).

## Check your understanding

Why is verifying the returned signature insufficient to prove correct key-use persistence? What should a two-call wallet fixture record in addition to signature bytes?

## Limits and traceability

This command has a native counterpart and passing scoped direct comparisons. Recorded query rows do not prove durable persistence, crash-safe key-use updates or a fully integrated node. Native H2 and Main remain unfinished.

Pinned Java SHA-256: `edd6bc26b857db900a51dce34e491ad06488bfa04a4ec4d179f51c7afd3d72c4`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
