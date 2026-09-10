# webhooks — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/webhooks.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/webhooks.java) | [src/org/minima/system/commands/network/webhooks.cpp](../../../../../../../../src/org/minima/system/commands/network/webhooks.cpp) |

## What this file does

Manages notification hooks and returns the current stored hook strings.

## Follow the logic

1. Default action to list, obtain the NotifyManager and create the reply’s response object.
2. For add, read optional filter and required hook, concatenate filter + # + hook, and pass that complete string to addHook.
3. For remove, pass the supplied hook string directly; it does not reconstruct the filter prefix. Exact stored-string removal therefore matters.
4. For clear, clear all hooks. For errorlogs, parse enable and obtain the manager again before changing WEBHOOKS_ERROR_LOGS.
5. For list or unknown actions, perform no mutation. Every path then retrieves the hook list and returns it under response.webhooks. A missing manager can fail after some parameter parsing, depending on the branch.

## How the translation is approached

Native code uses UTF-16 parameter helpers to preserve Java string behavior, including isolated surrogate data, and a required runtimeLookup callback. The callback is invoked a second time specifically in the errorlogs branch, matching the source’s receiver order.

## What the tests tell us

The [notification oracle](../../../../../../../../support/NotifyManagerOracle.java), [native harness](../../../../../../../../support/notify_manager.cpp) and [driver](../../../../../../../../tools/notify_manager_differential.py) execute real webhook commands directly and through CommandRunner with controlled manager/HTTP receivers. Command cases cover metadata, parameter types, missing manager, exact removal with or without # prefix, unknown actions, admin caller checks, Unicode filters and case-sensitive errorlogs values. They record reply/error plus hook/database state and manager lookup order. Null/cast expression descriptions are normalized; no remote hook is contacted.

## Dependencies and reading map

Direct Minima imports: [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [NotifyManager](../../../../../../../../../upstream-study/src/org/minima/system/network/webhooks/NotifyManager.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [webhooks](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/webhooks.java#L14), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/webhooks.java#L19), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/webhooks.java#L52), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/webhooks.java#L57), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/webhooks.java#L107).

## Check your understanding

After adding hook:http://fixture/a with no filter, what exact string must remove receive? Why can removing the unprefixed URL leave the hook present?

## Limits and traceability

The command fixture does not establish arbitrary real HTTP delivery, full Main binding or every logger/URL transport failure. NotifyManager’s separate lesson covers delivery logic. The counterpart remains partial and parity not_verified.

Pinned Java SHA-256: `8fbd385d0c8fee1c26e7594c7943dcff7bbab9030071f3e050e6d202dbda9802`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
