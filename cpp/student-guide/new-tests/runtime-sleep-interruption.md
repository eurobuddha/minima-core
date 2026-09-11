# Following an interrupt through the node

A sleep is part of a control-flow contract. Java can leave it by throwing
`InterruptedException`. The caller decides whether to continue, retry or stop.
Replacing it with an ordinary C++ delay preserves elapsed time but loses that
exception path.

## Read the caller before replacing the sleep

This change reuses [NativeThread](../../src/support/NativeThread.hpp), introduced
in the [worker interruption lab](thread-interruption.md). Its pending bit is shared
with the worker's owner. A valid sleep consumes that bit and throws the Java
exception category. This lab adds no new thread library or dependencies.

| File | What happens when the relevant sleep is interrupted |
|---|---|
| Minima.cpp | The daemon's 250 ms polling loop catches the interruption and continues |
| Main.cpp | General shutdown and restart polling continue; restart's 5-second pause also continues to create the replacement network |
| Main.cpp, MAIN_NETRESTART | Its separate 10-second delay propagates interruption before calling restartNIO |
| rpc.cpp | The old RPC server has been stopped, but enablement and authentication assignments have not happened yet |
| P2PManager.cpp | A failed discovery ping's pause catches Exception, then advances the attempt counter and can retry |
| NIOManager.cpp | Its server-completion sleep propagates interruption; stopMessageProcessor has not yet executed |
| ArchiveServer.cpp | The retry pause ignores interruption and retries once; the separate post-response pause reaches the outer exception handler |

Main previously lacked three of the source's local InterruptedException catches.
Its new private helper catches only that category. Broadening it to every error
would hide failures which the source sends to a different handler.

## The experiment

[RuntimeSleepOracle.java](../../support/RuntimeSleepOracle.java) executes the
unchanged pinned Main methods using the existing constructor-free Java fixture.
[runtime_sleep.cpp](../../support/runtime_sleep.cpp) uses the corresponding native
Main fixture and default sleep. Database objects exist in memory, and the existing
recording SQL connector satisfies Main's service contract without loading a wallet.
Neither fixture replaces sleep or directly throws InterruptedException.

Five Main/RPC cases observe shutdown polling, restart polling, restart pause,
MAIN_NETRESTART and RPC configuration. The old network's completion is controlled:
a helper thread waits for the caller's interrupt bit to be consumed before marking
it stopped. This forces the polling branch without relying on a short timing delay.
For restart polling, the existing log boundary delivers a second real interrupt
before the five-second pause. The replacement NetworkManager is the actual class:
it starts its listener on kernel-assigned port 0, with P2P/P2P2 disabled and an empty
connection list. It is shut down after observation. No preview or mainnet data is used.

The test checks the escaping exception category, pending bit, network identity,
RPC enable/authentication booleans and release of Main's database read lock.
A successful return without a replacement network would fail the restart cases.

Ten archive cases reuse the actual ArchiveHandler, MySQLConnect and recording SQL
driver. A null block number or escaping runtime error triggers a retry; the pending
interrupt is consumed by its pause. SQLExceptions caught inside loadBlockRange do
not reach that pause, so the bit stays pending. We compare complete normalized
SQL-event, log and result records, not just return codes.

Four P2P cases use the existing real TCP responder: P2P_INIT and P2P_LOOP each receive
either a valid greeting immediately or a dropped response followed by a valid one.
The former retains the interrupt bit because no sleep occurs. The latter consumes
it and performs exactly two pings. Exact request bytes, known/verified peers and
completion state are checked using the existing canonicalizer for unrelated random
fields. The peer address inside the reply is test data, never a contacted endpoint.

[The driver](../../tools/runtime_sleep_differential.py) passes 19 comparisons and
167 assertions in both normal and address/undefined sanitizer builds. Seven focused
CTests pass in each configuration: this lab, Main ownership, lifecycle commands,
RPC commands, P2P manager native/differential tests and launcher boundaries.
Both builds also compile the actual MinimaCoreC++ executable.

## Failures, scope and next exercise

The first native fixture failed because it omitted Main's required SQL connector.
The existing recording connector fixed the setup. A sandbox run then exited before
producing JSON while starting real networking. Repeating with local socket access
passed; the fixture now retains diagnostics on stderr. Both failed runs remain in
[the evidence directory](../../validation/runtime-sleep-work-in-progress/progress.json).

This is additional coverage. Existing archive response waits and discovery retry
corpora were not removed, shortened or counted as newly run. Direct interruption
of NIO's shutdown wait, a complete daemon run, archive post-response interruption,
I/O waits, monitor semantics and all possible schedules still need work. P2P's
existing native catch is broader for Java Error categories; that separate gap is
explicitly retained. Seven raw native sleep sites remain in other mapped files.

Student exercise: explain why an interrupted RPC enable command can leave the old
server stopped while its configuration flag retains the old value. Then contrast
that with Main.restartNIO: why must its InterruptedException catch continue to the
replacement constructor? Use the source order of statements in each explanation.

## Subsequent work

The [backup/checker lab](backup-checker-interruption.md) migrates the seven raw sleep sites listed above and extends this driver to 26 comparisons. Those earlier counts describe the previous checkpoint; direct I/O and full lifecycle parity remain open.
