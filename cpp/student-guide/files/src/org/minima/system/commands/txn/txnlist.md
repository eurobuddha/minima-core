# txnlist — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txnlist.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnlist.java) | [src/org/minima/system/commands/txn/txnlist.cpp](../../../../../../../../src/org/minima/system/commands/txn/txnlist.cpp) |

## What this file does

Display all draft transactions or one named row, optionally omitting witness details.

## Follow the logic

1. Read id with an absent-key default of empty text and read transactiononly with default false. A present blank id still fails Command validation.
2. For an absent id, iterate the live row list in order and call toJSON(!transactiononly) for each.
3. For a named id, look up the row and throw Transaction not found if absent.
4. Return an array for all rows or an object for one row. Row JSON generation can precompute coin IDs.

## How the translation is approached

C++ obtains the actual shared TxnDB through its runtime lookup and uses existing TxnRow, Transaction and Witness types. The lookup is a runtime dependency boundary; it does not supply full Main startup. It holds the shared row list while iterating and reuses TxnRow.toJSON, including its state effects.

## What the tests tell us

Cases list all/specific/missing rows, toggle transactiononly and compare complete wire state after listing. Random operation sequences expose list ordering after replacement and deletion.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../build/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txnlist](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnlist.java#L16), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnlist.java#L21), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnlist.java#L39), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnlist.java#L44), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnlist.java#L77).

## Check your understanding

Can a read-looking display command change cached or precomputed transaction data? Follow TxnRow.toJSON.

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. 

Pinned Java SHA-256: `1dfaf4ff15a34567d50e395738641b184c48fdf37ebf61ce0ac660ffa88d08bc`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
