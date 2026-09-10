# txnview — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txnview.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnview.java) | [src/org/minima/system/commands/txn/txnview.cpp](../../../../../../../../src/org/minima/system/commands/txn/txnview.cpp) |

## What this file does

Decode and display serialized transaction data without inserting it into TxnDB.

## Follow the logic

1. Choose file ahead of data, require a file to exist when selected, and decode a TxnRow.
2. Only the data branch honors an id parameter if it is directly present. The advertised valid-parameter list does not include id.
3. Return row JSON without adding the row to the database.

## How the translation is approached

C++ reuses the same row decoder and file helpers as txnimport but does not obtain a database owner. It retains the source’s data-only renaming branch even though CommandRunner may reject id before reaching it.

## What the tests tell us

The fixture invokes commands both directly and through CommandRunner. Direct txnview cases supply id with data, while dispatcher sequences test parameter filtering. Wire snapshots verify that viewing does not insert a row.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../build/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txnview](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnview.java#L16), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnview.java#L21), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnview.java#L40), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnview.java#L45), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnview.java#L89).

## Check your understanding

Why can direct runCommand and a command string expose different behavior for the same extra parameter?

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. 

Pinned Java SHA-256: `761062c8a386e39b84fc8df76b63a00a7da7bca62f0c2e7b5a5b6221d7d366ee`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
