# systemcheck — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/systemcheck.java) · `src/org/minima/system/commands/base/systemcheck.cpp` (absent)

## Purpose

systemcheck reports internal processor queues and locks, and actively posts diagnostic checker messages. It also has a details mode that writes queue contents to logs.

## Follow the command

1. With default action list, call getInfo for Main, TxPoWProcessor, miner, NIO, P2P, send-poll and notification processors. getInfo reports queue size and null or lastMessage.toString.
2. Add timer queue size, database read/write lock details/current writer state, and Main’s shutdown flag.
3. Then log and call TxPoWProcessor.postCheckCall, log/post Main.MAIN_CALLCHECKER, and create a one-second TimerMessage carrying the same type plus timer=true. Post it through Main after a third log.
4. With action details, require processor and compare recognized names ignoring case. Message processors log their name then print queued messages; timerprocessor has its own branch. The source spelling senpollmanager is the accepted send-poll selector.
5. Details always returns Sent to Minima Log even for an unrecognized processor. Unknown actions return an empty response object. There is no general null-service protection.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse MessageProcessor/TimerProcessor information and Main/MinimaDB routing. A status-only endpoint would omit authorized source side effects, and queue sizes are not collected under one global snapshot lock.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed fixtures assert all list fields and log/post order, the timer payload/delay, null last messages, unknown action/processor behavior, spelling/case selectors and failure after earlier posts. Dependency worker tests do not execute this command.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The assembled report is not an atomic runtime snapshot. A returned response does not mean the asynchronously posted checker calls have completed.

## Check your understanding

Why must a list-mode test inspect posted messages in addition to the returned JSON?

Pinned Java SHA-256: `0b7d0e2fdc50b744cf3d38b2620d477d5672b2e0957b542cce37d00ffe89eab0`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
