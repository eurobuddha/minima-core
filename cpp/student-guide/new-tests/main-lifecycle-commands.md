# Lab: quit and rescue commands

Written before the new tests. These small commands connect user input to Main's
lifecycle, so a successful reply is not enough to prove the requested effect.

`quit` parses compact before checking whether Main exists. With no Main it still
returns "Shutdown complete". With Main it calls the actual shutdown method, which
can catch a partial-shutdown error itself. The command then returns its ordinary
reply. Test absent Main, repeated shutdown, compact types and validation order.

`rescue` prefers an explicitly present host parameter; otherwise it uses the
configured rescue node. Explicit blank input fails Command's validation rather
than falling back. An empty selected host raises CommandException. A valid host
is stored in MAIN_DO_RESCUE and posted to Main, then returned in the reply. Do
not process that queued message in this fixture: the command promises to start an
attempt, while the separate Main handler can eventually halt the process.

Reuse the Main fixture with unstarted workers and actual database children. Add
metadata/direct-command/runner observations and drain the real Main queue only
after recording its size. Compare exact host and type, source replies, flags,
logs, exceptions, clone metadata, invalid parameters and administrator checks.
Use synthetic hosts and private temporary folders. No sockets, live shutdown,
rescue downloads or real wallet operations are needed for these command tests.

## Observed results

All 102 programs and 366 operations match Java in normal and ASAN/UBSAN builds.
The initial comparisons took 2.59s/3.10s. No production discrepancy was found.
Both full builds pass. Registry and shared Main regressions are recorded in the
project review. The two fixture groups now use separate Java output directories
so parallel CTest execution cannot read class files while the other javac rewrites
them. Each request resets the configured rescue node before applying its inputs.

The command replies retain their source meaning: quit may report completion with
no Main or after a caught partial-shutdown error; rescue reports a queued attempt.
Neither result establishes a complete working native node. All port statuses stay
implemented_partial with parity not_verified.

Final comparisons after fixture isolation pass in 3.18s/2.43s. All five selected
regressions pass in both builds: 12,289 command cases, 1,265 runner cases, 1,120
runner-error cases, 336 Main programs and native ownership. The normal batch took
9.69s; the sanitizer batch including the new commands took 24.00s. Both fixture
groups also pass when run concurrently in the normal build (4.17s).
