# systemcheck — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/systemcheck.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/systemcheck.cpp)

## Purpose

systemcheck reports internal processor queues and locks, and actively posts diagnostic checker messages. It also has a details mode that writes queue contents to logs.

## Follow the command

1. With default action list, call getInfo for Main, TxPoWProcessor, miner, NIO, P2P, send-poll and notification processors. getInfo reports queue size and null or lastMessage.toString.
2. Add timer queue size, database read/write lock details/current writer state, and Main’s shutdown flag.
3. Then log and call TxPoWProcessor.postCheckCall, log/post Main.MAIN_CALLCHECKER, and create a one-second TimerMessage carrying the same type plus timer=true. Post it through Main after a third log.
4. With action details, require processor and compare recognized names ignoring case. Message processors log their name then print queued messages; timerprocessor has its own branch. The source spelling senpollmanager is the accepted send-poll selector.
5. Details always returns Sent to Minima Log even for an unrecognized processor. Unknown actions return an empty response object. There is no general null-service protection.

## Translation and dependencies

The native counterpart maps every declared source method and is registered in the
real runner. It reuses Command parsing/replies, Main ownership, processor queues,
TimerMessage, MinimaLogger and MinimaDB diagnostics. Unicode case comparison uses
the existing Java-compatible case helper. The writer-thread diagnostic is read
through the existing synchronized native accessor. The report is not collected
under one global snapshot lock.

## Understanding the tests

The [pre-test lab](../../../../../../../../student-guide/new-tests/system-check.md)
and [corpus](../../../../../../../../tools/systemcheck_commands_corpus.py) extend
the existing Main runtime fixture. Seven actual processor objects have unstarted
workers, so queued messages cannot disappear while we inspect them. Seed different
queue sizes and last messages, call list twice, and check that each reply describes
the queues before that call's new posts. Verify the three log messages, both
immediate checker messages, and timer type, true payload, and bounded 1000 ms delay.

Tests cover all selectors, Unicode case variants, source spelling mistakes,
unknown actions/selectors, typed failures, missing receivers, helpers, factories,
runner gates, and a timer-post failure after earlier posts. Nested write/read locks
exercise the report's lock counts. Details must log through the common logger and
leave notification queues unchanged. Platform-specific exception stack frames are
classified and removed from comparisons; they naturally differ between runtimes.

The shared MessageStack print path now uses MinimaLogger with notification false.
The queue lock is recursive, preserving Java monitor reentry. A modification
counter detects changes before fetching another message, without holding a native
deque iterator across a callback. Callback tests read, append, clear, retain all,
or retain only the first message. Appending must fail before the next item; clearing
can simply end iteration because Java checks size before its modification counter.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

The [live driver](../../../../../../../../tools/native_systemcheck_startup.py)
starts actual Java and C++ nodes, checks report shapes and every details selector,
and waits for the TxPoWProcessor, immediate Main and delayed Main checker logs.
Exact live queue counts are deliberately not compared because workers run concurrently.
Both builds pass 262 programs (718 operations), two live nodes, and seven affected
regression groups. See [saved evidence](../../../../../../../../validation/systemcheck-work-in-progress).

## Limits

The assembled report is not an atomic runtime snapshot. A returned response does
not mean the asynchronously posted checker calls have completed. The fixture's
manual timer intercept records posting; the actual-node test verifies delivery.
TimerProcessor's own logging-callback reentry, arbitrary custom Message subclasses,
and exhaustive concurrent owner replacement remain outside these assertions.

## Check your understanding

Why must a list-mode test inspect posted messages in addition to the returned JSON?

Pinned Java SHA-256: `0b7d0e2fdc50b744cf3d38b2620d477d5672b2e0957b542cce37d00ffe89eab0`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
