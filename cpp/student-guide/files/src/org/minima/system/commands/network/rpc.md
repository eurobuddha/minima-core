# rpc — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/rpc.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java) | `src/org/minima/system/commands/network/rpc.cpp` (planned) |

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

The mapped rpc.cpp is absent. Reuse candidates for a future port are Command parameter parsing, GeneralParams, UserDB and NetworkManager start/stop boundaries; no new authentication backend is required to explain the pinned command.

## What the tests tell us

Proposed tests should record stop/sleep/global-update/start order, enable-over-action precedence, omitted password, unused ssl, duplicate usernames, reserved username, mode validation, replacement-array aliasing and listusers output. Inject interruption or manager failure at each stage to inspect partial changes. These are planned tests, not reported runs.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [UserDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/UserDB.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [rpc](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L17), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L22), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L69), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L74), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/rpc.java#L181).

## Check your understanding

When enable:false is supplied, how many stopRPC calls occur on the successful path? Why is masking the default password not the same as masking all users in listusers?

## Limits and traceability

The source’s help text is not evidence that ssl is implemented here. This unported command needs both behavioral tests and runtime binding before use. The mapped native file is absent; all validation proposed here is future work.

Pinned Java SHA-256: `aa904b05d470794d5c51a5040e56128b7f7726ee948b9de36d7e934b14469a4c`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
