# txndelete — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txndelete.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txndelete.java) | [src/org/minima/system/commands/txn/txndelete.cpp](../../../../../../../../src/org/minima/system/commands/txn/txndelete.cpp) |

## What this file does

Delete a named draft transaction, or clear the current list with the exact sentinel all.

## Follow the logic

1. Build the reply and obtain the database, then read id.
2. If id equals lowercase all, clear the current list and retain found=true. Otherwise call deleteTransaction and use its Boolean result.
3. Return response:"Deleted" or "Not found". Not finding a row is not a command exception.

## How the translation is approached

C++ obtains the actual shared TxnDB through its runtime lookup and uses existing TxnRow, Transaction and Witness types. The lookup is a runtime dependency boundary; it does not supply full Main startup. It preserves the exact sentinel spelling and success-shaped not-found reply.

## What the tests tell us

The driver supplies all, ALL, missing and ordinary IDs and issues list/export afterward. Sequences combine multiple inserts, deletion and import to expose ordering changes.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../validation/multisig-work-in-progress/normal/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txndelete](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txndelete.java#L13), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txndelete.java#L18), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txndelete.java#L35), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txndelete.java#L40), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txndelete.java#L66).

## Check your understanding

Why does deleting a missing ID differ from listing a missing ID?

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. 

Pinned Java SHA-256: `a75f4ed592358a51a58f1fbd26d6689cb0468911ef9996ebfac081381d3647bd`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
