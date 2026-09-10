# createfrom — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/wallet/createfrom.java](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createfrom.java) | [src/org/minima/system/commands/send/wallet/createfrom.cpp](../../../../../../../../../src/org/minima/system/commands/send/wallet/createfrom.cpp) |

## What this file does

Compose an unsigned transaction from a source address using the existing transaction commands.

## Follow the logic

1. Parse source and recipient addresses, amount, token and burn. Reject positive token burns before reading script.
2. Generate a 32-byte random transaction ID and issue txncreate. Preserve the literal space after fromaddress: in the txnaddamount text.
3. Read the funding reply status by Boolean cast/unboxing. On false, delete the row and throw the child error, which may be null.
4. Call txnscript, txnmmr, txnexport showtxn:true, then txndelete. These child statuses are not checked.
5. Return export.response. Exceptions before the final delete can leave a row; no finally block repairs that source behavior.

## How the translation is approached

The native wrapper reuses Command, CommandRunner, MiniData/MiniNumber and the actual TxnDB/TxnRow and transaction commands. Its public string overload remains virtual for source-like subclassing. CommandResult shares the exact first-result selection, typed Boolean unboxing and nullable rejection logic. Java’s checked exceptions have no C++ compile-time equivalent. The method sequence and intentionally unescaped command strings remain source-specific.

## What the tests tell us

[Read the Java/native comparison](../../../../../../../../../tools/wallet_wrappers_differential.py) and [the student lab](../../../../../../../../new-tests/wallet-command-orchestration.md) compare metadata, parameters, exact child-call text/order, database snapshots, cleanup and errors. Controlled child results isolate orchestration; real children also construct/export rows, exercise ignored failures, sign and cryptographically verify a decoded signature. Random IDs are checked and consistently renamed; only verified serialized ID bytes are normalized. Both normal and ASAN/UBSAN builds pass 659 programs and 2,060 child calls (14.29s/58.31s). All five selected checks pass per build, including the command, runner, runner-error and source-mapping regressions.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Command](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [createfrom](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createfrom.java#L16), [getValidParams](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createfrom.java#L21), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createfrom.java#L27), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createfrom.java#L81), [getFunction](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/createfrom.java#L88).

## Check your understanding

Why can an empty exported transaction have status:true in the pinned offline experiment?

## Limits and traceability

The counterpart is implemented_partial; parity remains not_verified. No live node or durable SQL runs in this fixture. Actual posting success and confirmation, funded chains, arbitrary Java null/reference APIs and all concurrent interleavings remain unfinished or separately scoped. NPE/CCE runtime prose is normalized; explicit errors and observed logs are compared.

Pinned Java SHA-256: `836ff3fb5cf2780f8133d54ce2c717d27e9210858238c657a9e1dee7314ccfb8`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
