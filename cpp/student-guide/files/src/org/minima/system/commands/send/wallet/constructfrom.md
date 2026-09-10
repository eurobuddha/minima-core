# constructfrom — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/wallet/constructfrom.java](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/constructfrom.java) | [src/org/minima/system/commands/send/wallet/constructfrom.cpp](../../../../../../../../../src/org/minima/system/commands/send/wallet/constructfrom.cpp) |

## What this file does

Compose an unsigned transaction from a comma-separated list of coin IDs and explicit recipient/change amounts.

## Follow the logic

1. Parse coinlist and script, recipient address/amount, then changeamount before changeaddress. Parse tokenid as a string.
2. Generate a 32-byte ID and create the row. StringTokenizer discards empty comma tokens; trim each remaining token with Java’s <= U+0020 rule.
3. Issue one txninput per token. A false input status deletes then throws; a missing or wrongly typed status throws before cleanup.
4. Issue the recipient output and add change only for a positive change amount. Output statuses are ignored.
5. Add script, request proofs, export with showtxn:true, delete and return export.response. Empty coinlist is rejected by Command; commas-only input reaches this body with zero tokens.

## How the translation is approached

The native wrapper reuses Command, CommandRunner, MiniData/MiniNumber and the actual TxnDB/TxnRow and transaction commands. Its public string overload remains virtual for source-like subclassing. CommandResult shares the exact first-result selection, typed Boolean unboxing and nullable rejection logic. Java’s checked exceptions have no C++ compile-time equivalent. The method sequence and intentionally unescaped command strings remain source-specific.

## What the tests tell us

[Read the Java/native comparison](../../../../../../../../../tools/wallet_wrappers_differential.py) and [the student lab](../../../../../../../../new-tests/wallet-command-orchestration.md) compare metadata, parameters, exact child-call text/order, database snapshots, cleanup and errors. Controlled child results isolate orchestration; real children also construct/export rows, exercise ignored failures, sign and cryptographically verify a decoded signature. Random IDs are checked and consistently renamed; only verified serialized ID bytes are normalized. Both normal and ASAN/UBSAN builds pass 659 programs and 2,060 child calls (14.29s/58.31s). All five selected checks pass per build, including the command, runner, runner-error and source-mapping regressions.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Command](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [constructfrom](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/constructfrom.java#L17), [getValidParams](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/constructfrom.java#L22), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/constructfrom.java#L28), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/constructfrom.java#L96), [getFunction](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/constructfrom.java#L103).

## Check your understanding

How do an empty string, three commas, and a comma-separated whitespace token differ?

## Limits and traceability

The counterpart is implemented_partial; parity remains not_verified. No live node or durable SQL runs in this fixture. Actual posting success and confirmation, funded chains, arbitrary Java null/reference APIs and all concurrent interleavings remain unfinished or separately scoped. NPE/CCE runtime prose is normalized; explicit errors and observed logs are compared.

Pinned Java SHA-256: `c2c0b4763e81ab80d61d1aa7b34882fb79d410f019bb6392e5fd6a03bd00690e`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
