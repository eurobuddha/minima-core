# sendpoll — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/sendpoll.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpoll.java) | [src/org/minima/system/commands/send/sendpoll.cpp](../../../../../../../../src/org/minima/system/commands/send/sendpoll.cpp) |

## What this file does

Manage retryable command text through add, list and remove; actual execution belongs to the polling worker.

## Follow the logic

1. Read action, defaulting to add, then obtain Main’s manager before testing any action branch.
2. For add, replace only the first case-sensitive sendpoll occurrence in the complete command text. Reject only exact send. Empty direct-call text is accepted.
3. Add the text through the existing locked manager. Return response.command without executing or validating a transaction.
4. For list, turn the copied message list into response.commands and count total. For remove, require uid and report it even when absent from the queue.
5. An unknown action returns an empty response, although the Main lookup still occurs. A null manager only fails if the chosen branch dereferences it.

## How the translation is approached

The C++ wrapper reuses SendPollManager, SendPollMessage, Command and CommandRunner. The fixed literal replacement uses find/replace once. An explicit getManager binding stands in for the unfinished native Main owner; its default throws a typed null-receiver exception. The manager is dereferenced after branch-specific parameter validation, matching source order.

## What the tests tell us

[Read the Java/C++ comparison](../../../../../../../../tools/sendpoll_command_differential.py) and [the lab](../../../../../../../new-tests/send-polling-command.md) cover 367 programs and 3,216 operations in both normal and ASAN/UBSAN builds. Compare exact metadata, queue contents, detached snapshots, removal, errors and public runner dispatch. Actual worker processing removes a successful hash command and retains a failed command. Stopping keeps queued work but still schedules a timer. Random UIDs are shape-checked and consistently renamed; NPE/CCE types are compared while runtime prose is normalized.

## Dependencies and reading map

Direct Minima imports: [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [SendPollManager](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollManager.java), [SendPollMessage](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollMessage.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [sendpoll](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpoll.java#L16), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpoll.java#L21), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpoll.java#L47), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpoll.java#L55), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpoll.java#L110).

## Check your understanding

Why does action:unknown fail when Main is missing, yet succeed when Main exists with a null polling manager?

## Limits and traceability

The counterpart is implemented_partial and parity remains not_verified. The fixture does not start Main, deliver background timers or compare logger output. Generic C++ cast prose differs from Java class/loader details. Existing worker tests cover additional native concurrency behavior; full runtime, storage and exhaustive scheduling remain unfinished.

Pinned Java SHA-256: `4d04da99c9280dfe61b8d8ee1f9ee51f08176294ce4ed859de1416b681713b95`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
