# createtokenfrom — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native counterpart exists; exhaustive behavior and API review remain open.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/wallet/createtokenfrom.java](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createtokenfrom.java) | [createtokenfrom.cpp](../../../../../../../../../src/org/minima/system/commands/send/wallet/createtokenfrom.cpp) |

## What this file does

Create and post a new token by replacing a funded output with a token-creation coin. The wrapper delegates transaction operations to existing commands.

## Follow the logic

1. Read fromaddress, amount, script, privatekey, keyuses and mine in source order.
2. For a JSON token name, require the name key; its value may itself be null or another JSON type. Otherwise wrap the command String in a new name object.
3. Parse decimals directly from the original String without trimming, defaulting to 8. More than 16 raises a generic exception. There is no lower-bound check.
4. Floor the requested supply, reject more than one trillion or less than one, then compute MINI_UNIT × 10^decimals × supply. MINI_UNIT is 10^-44. The MiniNumber class truncates after each operation.
5. Compute scale as 44 minus decimals, then create the temporary transaction and request funding.
6. If funding returns Boolean false, delete the row and throw its nullable error. Missing or wrongly typed status fails during cast/unboxing before that deletion.
7. Construct a coin with the token-creation marker 0xFF and attach the real Token metadata. Fetch the stored transaction only now and replace output zero. An absent row or empty outputs fails here; additional change outputs are preserved.
8. Add the script.
9. Request MMR proofs, custom-key signing and posting with the mine flag. Returned failures from proof/sign/script children are ignored by this source wrapper.
10. Delete the temporary row, then inspect posting status. A returned failure now raises CommandException; a thrown earlier exception may have prevented deletion. Return the posting response on success.

## How the translation is approached

The C++ body preserves the pinned Java call sequence and reuses CommandResult for first-child selection and typed failure handling. Token, Coin, TxnDB and the mutable Transaction output list provide the actual stored mutation and serialization. MiniNumber gains the missing source MINI_UNIT constant; JavaInteger parses the raw decimals String. No cleanup-finally block or new child-status checks are introduced.

## What the tests tell us

[The custom-address corpus](../../../../../../../../../tools/custom_address_send_corpus.py), [the shared comparison driver](../../../../../../../../../tools/wallet_wrappers_differential.py) and [the pre-test lab](../../../../../../../../new-tests/custom-address-send-commands.md) exercise actual wrapper bodies. The fixture records child text, serialized rows and token JSON, injects returned failures separately from throws, and supplies explicit outputs through real txnoutput calls. Independent checks cover sequence, cleanup, first-output replacement and preserved change. The final custom-address group passes all 492 programs and 2,052 child calls in both normal and ASAN/UBSAN builds (13.69s/25.24s). Both full builds pass without warnings. The original number comparison also passes 99 source assertions plus eleven separately labelled exact error observations per runtime.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [Coin](../../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [StateVariable](../../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [Token](../../../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Command](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [createtokenfrom](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createtokenfrom.java#L24), [getValidParams](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createtokenfrom.java#L29), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createtokenfrom.java#L35), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createtokenfrom.java#L169), [getFunction](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createtokenfrom.java#L176).

## Check your understanding

What remains in the custom transaction database when funding returns false, posting returns false, or signing throws? Why can a negative decimal count produce zero token supply after fixed-precision truncation?

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. Fixture funding and posting replies are controlled input, not network acceptance. The real-child tests preserve the source fromaddress: spacing behavior. Native Main, durable SQL and full node integration are unfinished.

Pinned Java SHA-256: `958811e4f89c1a4efb21fd425e4b7abc7ea921356d7f6145f12c13e7e9502899`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
