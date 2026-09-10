# newaddress — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/newaddress.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/newaddress.cpp)

## Purpose

newaddress asks the wallet to create a fresh nondefault simple address and reports its row plus the total key count.

## Follow the command

1. Get the wallet from MinimaDB.
2. Call createNewSimpleAddress(false); false marks this as a nondefault address.
3. After creation, call getAllKeys and take its size, then serialize the returned ScriptRow and add total.
4. Attach the row as response and return. A later key-count or JSON failure can occur after the address was created; the wrapper has no rollback.

## Translation and dependencies

The native command now uses the actual MinimaDB owner and existing Command parameter/reply helpers, and is registered for dispatch. It reuses Wallet address creation and ScriptRow serialization. This command changes wallet state and cannot be replaced by generating an unrelated random address string.

## Understanding the tests

The tests execute actual Wallet and TreeKey creation, compare resulting key/script bytes and ordered SQL calls, and inspect the returned address and post-creation query count. Checked and runtime SQL failures exercise partial effects. A success guard checks the nondefault, tracked simple row, public-key size and supplied-row count.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both native builds and all six selected regression suites pass. The expanded address group passes 134 programs with 276 operations and 3,693 SQL calls in each build. See the [address-command lab](../../../../../../../new-tests/address-commands.md) for inputs, operation order and limitations.

## Limits

The fixture runs actual native/Java Wallet and default-sized TreeKey generation. Only SQL is a recording driver, so resulting key/script bytes and call order are compared but durable wallet inserts are not proved.

## Check your understanding

Why is total read after createNewSimpleAddress instead of incrementing an assumed earlier count?

Pinned Java SHA-256: `b7f38eff1c8c89f9b6503151ae0cfe8063b720f6bdd1dbf952a9fa412d0e4257`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
