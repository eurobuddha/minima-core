# txnclear — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txnclear.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnclear.java) | [src/org/minima/system/commands/txn/txnclear.cpp](../../../../../../../../src/org/minima/system/commands/txn/txnclear.cpp) |

## What this file does

Clear selected witness material and invalidate cached monotonic checking.

## Follow the logic

1. Read id, then scripts/mmr/signatures flags, each defaulting true.
2. Find the row or throw before mutation.
3. Always call transaction.clearIsMonotonic, even if all three flags are false.
4. Clear script proofs, coin proofs and signatures in that order when enabled, then return row JSON.

## How the translation is approached

C++ obtains the actual shared TxnDB through its runtime lookup and uses existing TxnRow, Transaction and Witness types. The lookup is a runtime dependency boundary; it does not supply full Main startup. Native code explicitly checks nullable transaction/witness values and mutates the existing shared objects rather than replacing the entire row.

## What the tests tell us

The fixture starts with scripts, a coin proof, a signature and a checked-monotonic flag. The driver crosses true/false/TRUE/null flags and exports the result. The separate checked snapshot detects unconditional invalidation when all clearing flags are false.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../build/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txnclear](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnclear.java#L15), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnclear.java#L20), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnclear.java#L43), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnclear.java#L48), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnclear.java#L85).

## Check your understanding

Why is clearing the validation cache still meaningful when no witness list is cleared?

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. 

Pinned Java SHA-256: `ed31c841c009910efd90fcad39bc3fd4dd758e4a0ca144c527b077254f1bde07`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
