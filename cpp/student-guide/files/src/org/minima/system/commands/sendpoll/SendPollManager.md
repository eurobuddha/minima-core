# SendPollManager — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/sendpoll/SendPollManager.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollManager.java) | [src/org/minima/system/commands/sendpoll/SendPollManager.cpp](../../../../../../../../src/org/minima/system/commands/sendpoll/SendPollManager.cpp) |

## What this file does

Retry queued commands periodically, remove successful entries and retain failures for a later poll.

## Follow the logic

1. Construction schedules SENDPOLL_FUNCTION after the mutable default thirty seconds. SENDPOLL_LIST and SENDPOLL_CLEAR have transposed string values, and neither has a handling branch.
2. Add creates a new SendPollMessage under the queue lock. listCommands creates copied messages; removeCommand constructs a replacement list omitting every matching UID.
3. On SENDPOLL_FUNCTION, iterate a copied snapshot and check isRunning before each entry. New commands inserted while processing wait for a later snapshot.
4. Run each command through CommandRunner, cast status to Boolean and remove it only for true. Sleep one second after every normally completed attempt, including success.
5. After the loop schedule another poll, even when stopping broke the loop. An exception in runner/status/sleep escapes before that final schedule; no retry timer is posted by this method on that path.
6. Unknown message types do nothing; null message/type fails. Removing a null requested UID normally matches no generated UID, while a null stored UID fails when equality is called.

## How the translation is approached

C++ reuses MessageProcessor, TimerMessage and SendPollMessage with a mutex-protected vector and atomic delay. Factory creation establishes ownership before scheduling/starting the worker. A required command runner callback and overridable timing seams allow controlled tests; the new command fixture binds it to the real CommandRunner for offline hash/failure processing. Live wallet ownership remains a separate integration step.

## What the tests tell us

[Read the native polling checks](../../../../../../../../support/sendpoll.cpp). The harness records success/retry removal, detached snapshots, a reentrant insertion, missing-status failure before pause/reschedule, stop during polling, null/unknown types and 1000 concurrent additions/removals. One native worker/timer experiment invokes only a controlled offline callback and checks release/join cleanup. It does not send funds or compare Java scheduling. Those checks are native assertions. The new [command comparison](../../../../../../../../tools/sendpoll_command_differential.py) also runs 367 programs and 3,216 operations against compiled Java in normal and ASAN/UBSAN builds. It checks stable UID shape/identity, copied queue snapshots, additions/removals and actual worker success/failure through CommandRunner. Timer publication is recorded; delivery and logs are outside that comparison.

## Dependencies and reading map

Direct Minima imports: [CommandRunner](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageProcessor](../../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java), [TimerMessage](../../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java).

Method navigation (declarations with bodies found in this file): [SendPollManager](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollManager.java#L23), [addSendCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollManager.java#L29), [listCommands](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollManager.java#L35), [removeCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollManager.java#L47), [processMessage](../../../../../../../../../upstream-study/src/org/minima/system/commands/sendpoll/SendPollManager.java#L62).

## Check your understanding

If a command result omits status, does the next poll get scheduled by this function? Identify the exact exception boundary.

## Limits and traceability

The mapped file is partial and parity remains not_verified. The inspected tests have a finite native scope and do not certify full wallet command execution or every thread schedule.

Pinned Java SHA-256: `57679e54ba260ccb626b91d77d6c9487f9e0312c5ea5063134755e31b35440ef`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
