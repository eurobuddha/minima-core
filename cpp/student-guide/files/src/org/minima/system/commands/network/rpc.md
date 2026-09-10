# rpc — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native body exists; live-enable and concurrency validation are unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/rpc.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java) | [Native body](../../../../../../../../src/org/minima/system/commands/network/rpc.cpp) |

## What this file does

Controls RPC enablement and the additional RPC-user records exposed through UserDB.

## Follow the logic

1. Obtain UserDB, then give the enable parameter priority over action. If enable exists, parse its boolean value, stop RPC, sleep one second and only then update RPC_ENABLED.
2. If password is present, store it and enable authentication; otherwise disable authentication without necessarily erasing the old password string. Start RPC when enabled, or call stopRPC again when disabled.
3. Although ssl appears in the advertised parameters/help, this runCommand body does not read or apply it.
4. For action adduser, require default RPC authentication, forbid exact username minima, read password, lowercase mode and require read or write. Append a user object without checking duplicate usernames.
5. For removeuser, build a new array excluding every exact matching username and replace UserDB’s array. listusers switches output from a count to the full stored user array. Unknown action falls through to status output.
6. The response masks the default password as ***. When listusers is selected, it includes the stored extra-user records, which contain their password fields.

## How the translation is approached

The native body uses the existing Main/NetworkManager methods and actual UserDB
arrays, with RawTxnCommands' checked object/string adapters and JavaLowerCase.
The existing Main sleep service is bound to the command's one-second pause.
Synthetic interruption propagates the native InterruptedException category at
that boundary. No Java thread registry or full interruption mechanism is implied.

## What the tests tell us

The [lab](../../../../../../../../student-guide/new-tests/rpc-command.md) was written
before the [comparison corpus](../../../../../../../../tools/rpc_commands_corpus.py).
The current cases check direct/runner dispatch, metadata, duplicate usernames,
reserved names, malformed array elements, unchanged versus replaced array identity,
retained list replies and locale-sensitive modes. An initial expected value treated
blank action as unknown; Command.getParam rejects blank strings before the action
switch, so the test now expects that rejection.

A malformed saved array revealed an actual shared-getter bug: native JsonDB exposed
bad_variant_access instead of Java ClassCastException. The corrected getJSONArray
keeps null/missing default creation and shared-array ownership, while explicitly
checking the stored variant type. Earlier preference tests reduced all exceptions
to ERROR, which could not detect this category mismatch; this command comparison
checks the category and state separately.

The stopped NetworkManager disable path and interrupted pause are exercised.
Successful live enable/authentication and safe concurrent mutation remain open;
this lesson does not claim a production-ready RPC implementation.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [UserDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/UserDB.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [rpc](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L17), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L22), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L69), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L74), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L181).

## Check your understanding

When enable:false is supplied, how many stopRPC calls occur on the successful path? Why is masking the default password not the same as masking all users in listusers?

## Limits and traceability

The source’s help text is not evidence that ssl is implemented here. The native command is bound to Main, but successful live enablement, concurrent authentication/configuration access, durable preferences and complete parity remain unfinished.

Pinned Java SHA-256: `aa904b05d470794d5c51a5040e56128b7f7726ee948b9de36d7e934b14469a4c`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Shared configuration follow-up

RPC_ENABLED and RPC_AUTHENTICATE now use atomic boolean storage. RPC_PASSWORD
reuses RPCClient's mutex-protected nullable string snapshot. The command still
performs its source assignments separately and in the same order; this is not
a transaction across all settings. Response booleans explicitly load their
values. User-array mutation and live enable integration are still open gates.
