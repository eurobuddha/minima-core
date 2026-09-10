# sendfrom — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native counterpart exists; exhaustive behavior and API review remain open.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/wallet/sendfrom.java](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/sendfrom.java) | [sendfrom.cpp](../../../../../../../../../src/org/minima/system/commands/send/wallet/sendfrom.cpp) |

## What this file does

Create and post a custom-address transfer with optional split, burn and state. The wrapper delegates transaction operations to existing commands.

## Follow the logic

1. Read fromaddress, destination, amount and token ID, then burn. A positive burn is allowed only for the exact token text 0x00. Read script, private key and key-use number next.
2. Generate the random temporary ID before reading mine and split. A wrong optional state object type is caught and ignored.
3. Create the temporary transaction and request funding, preserving split, burn and the literal space after fromaddress: in the child command.
4. If funding returns Boolean false, delete the row and throw its nullable error. Missing or wrongly typed status fails during cast/unboxing before that deletion.
5. Add the script and normalized state entries in insertion order. The shared StateVariable constructor validates each port and value.
6. Request MMR proofs, custom-key signing and posting with the mine flag. Returned failures from proof/sign/script children are ignored by this source wrapper.
7. Delete the temporary row, then inspect posting status. A returned failure now raises CommandException; a thrown earlier exception may have prevented deletion. Return the posting response on success.

## How the translation is approached

The C++ body preserves the pinned Java call sequence and reuses CommandResult for first-child selection and typed failure handling. RawTxnCommands.state shares the identical state loop. Existing Command getters, MiniNumber and child transaction commands supply validation and behavior. No cleanup-finally block or new child-status checks are introduced.

## What the tests tell us

[The custom-address corpus](../../../../../../../../../tools/custom_address_send_corpus.py), [the shared comparison driver](../../../../../../../../../tools/wallet_wrappers_differential.py) and [the pre-test lab](../../../../../../../../new-tests/custom-address-send-commands.md) exercise actual wrapper bodies. The fixture records child text, serialized rows and token JSON, injects returned failures separately from throws, and supplies explicit outputs through real txnoutput calls. Independent checks cover sequence, cleanup, first-output replacement and preserved change. The final custom-address group passes all 492 programs and 2,052 child calls in both normal and ASAN/UBSAN builds (13.69s/25.24s). Both full builds pass without warnings. The original number comparison also passes 99 source assertions plus eleven separately labelled exact error observations per runtime.

## Dependencies and reading map

Direct Minima imports: [StateVariable](../../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Command](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [sendfrom](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/sendfrom.java#L17), [getValidParams](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/sendfrom.java#L22), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/sendfrom.java#L28), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/sendfrom.java#L135), [getFunction](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/sendfrom.java#L142).

## Check your understanding

What remains in the custom transaction database when funding returns false, posting returns false, or signing throws? Why does an invalid state object get ignored while an invalid port inside a valid object can retain a row?

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. Fixture funding and posting replies are controlled input, not network acceptance. The real-child tests preserve the source fromaddress: spacing behavior. Native Main, durable SQL and full node integration are unfinished.

Pinned Java SHA-256: `ad7740d96b2d873744847995b7fbad034e210d4f87def42377204be430009432`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
