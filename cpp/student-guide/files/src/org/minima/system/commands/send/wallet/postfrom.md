# postfrom — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/wallet/postfrom.java](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/postfrom.java) | [src/org/minima/system/commands/send/wallet/postfrom.cpp](../../../../../../../../../src/org/minima/system/commands/send/wallet/postfrom.cpp) |

## What this file does

Import a serialized custom transaction, optionally add proofs, post it and delete the temporary row.

## Follow the logic

1. Fetch the custom database before parsing data. Decode a TxnRow and optionally override its ID; id is absent from the public valid-parameter list.
2. Parse mmr before dereferencing the decoded row and inserting it. A wrong parameter type here leaves the database unchanged.
3. Insert the row, replacing an existing matching ID, then parse mine. A failure at this later stage leaves the imported row.
4. Optionally run txnmmr, then txnpost with the requested mine value, then txndelete. Child status values are not checked.
5. Return the posting child’s response, including null. A successful outer status is not independent proof that posting succeeded.

## How the translation is approached

The native wrapper reuses Command, CommandRunner, MiniData/MiniNumber and the actual TxnDB/TxnRow and transaction commands. Its public string overload remains virtual for source-like subclassing. CommandResult shares the exact first-result selection, typed Boolean unboxing and nullable rejection logic. Java’s checked exceptions have no C++ compile-time equivalent. The method sequence and intentionally unescaped command strings remain source-specific.

## What the tests tell us

[Read the Java/native comparison](../../../../../../../../../tools/wallet_wrappers_differential.py) and [the student lab](../../../../../../../../new-tests/wallet-command-orchestration.md) compare metadata, parameters, exact child-call text/order, database snapshots, cleanup and errors. Controlled child results isolate orchestration; real children also construct/export rows, exercise ignored failures, sign and cryptographically verify a decoded signature. Random IDs are checked and consistently renamed; only verified serialized ID bytes are normalized. Both normal and ASAN/UBSAN builds pass 659 programs and 2,060 child calls (14.29s/58.31s). All five selected checks pass per build, including the command, runner, runner-error and source-mapping regressions.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Command](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandRunner](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [postfrom](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/postfrom.java#L17), [getValidParams](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/postfrom.java#L22), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/postfrom.java#L27), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/postfrom.java#L68), [getFunction](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/postfrom.java#L75).

## Check your understanding

Which of mmr and mine is parsed before insertion, and why does that change cleanup evidence?

## Limits and traceability

The counterpart is implemented_partial; parity remains not_verified. No live node or durable SQL runs in this fixture. Actual posting success and confirmation, funded chains, arbitrary Java null/reference APIs and all concurrent interleavings remain unfinished or separately scoped. NPE/CCE runtime prose is normalized; explicit errors and observed logs are compared.

Pinned Java SHA-256: `4ba46d9af55a35f465e5ca5de3261cf50ac526480111e518e0d0afa19302dddf`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
