# signfrom — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/wallet/signfrom.java](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/signfrom.java) | [src/org/minima/system/commands/send/wallet/signfrom.cpp](../../../../../../../../../src/org/minima/system/commands/send/wallet/signfrom.cpp) |

## What this file does

Import a custom transaction, sign with a supplied key, optionally post, export and delete it.

## Follow the logic

1. Fetch the database, decode data, optionally replace ID and insert the row before parsing privatekey, keyuses and post.
2. Invoke txnsign publickey:custom. With post:true append txnpostauto:true and txnpostmine:true exactly as the source does.
3. Export without showtxn:true, then delete. Neither signing nor export status is checked before these next steps.
4. Cast export.response to an object and add post after deletion. Missing or wrongly typed export responses fail at this point.
5. For post:true, try to copy txpow from the signing response. Only exceptions inside this small extraction block are logged and swallowed.

## How the translation is approached

The native wrapper reuses Command, CommandRunner, MiniData/MiniNumber and the actual TxnDB/TxnRow and transaction commands. Its public string overload remains virtual for source-like subclassing. CommandResult shares the exact first-result selection, typed Boolean unboxing and nullable rejection logic. Java’s checked exceptions have no C++ compile-time equivalent. The method sequence and intentionally unescaped command strings remain source-specific.

## What the tests tell us

[Read the Java/native comparison](../../../../../../../../../tools/wallet_wrappers_differential.py) and [the student lab](../../../../../../../../new-tests/wallet-command-orchestration.md) compare metadata, parameters, exact child-call text/order, database snapshots, cleanup and errors. Controlled child results isolate orchestration; real children also construct/export rows, exercise ignored failures, sign and cryptographically verify a decoded signature. Random IDs are checked and consistently renamed; only verified serialized ID bytes are normalized. Both normal and ASAN/UBSAN builds pass 659 programs and 2,060 child calls (14.29s/58.31s). All five selected checks pass per build, including the command, runner, runner-error and source-mapping regressions.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Command](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandRunner](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [MinimaLogger](../../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [signfrom](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/signfrom.java#L19), [getValidParams](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/signfrom.java#L24), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/signfrom.java#L29), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/signfrom.java#L87), [getFunction](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/signfrom.java#L94).

## Check your understanding

Why can a key parsing failure retain the row while an export response cast failure happens after deletion?

## Limits and traceability

The counterpart is implemented_partial; parity remains not_verified. No live node or durable SQL runs in this fixture. Actual posting success and confirmation, funded chains, arbitrary Java null/reference APIs and all concurrent interleavings remain unfinished or separately scoped. NPE/CCE runtime prose is normalized; explicit errors and observed logs are compared.

Pinned Java SHA-256: `ae498850012f2de72071bf453fd7ff56534fa5e21851a979ab7f20b1f1254dca`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
