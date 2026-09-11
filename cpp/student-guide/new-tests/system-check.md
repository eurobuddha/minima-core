# Lab: diagnostics that also post work

Written before the new tests. Read pinned systemcheck.java in full. Its default
list action reports seven processor queues, timer count, database lock information
and shutdown state. It then posts work: a checker call to TxPoWProcessor, one to
Main, and a one-second Main timer with `timer:true`. A read-only replacement would
miss part of the command's behavior.

Reuse the existing Main runtime differential harness. Keep workers unstarted so
queue sizes and post order are deterministic. Seed queues and last-message state,
call the actual command, inspect all resulting queues, and bound timer deadlines
against the clock sampled around the command. Test repeated calls and failure
while posting the timer: the earlier checker messages must remain queued.

Details prints a selected processor's name and queued messages through MinimaLogger.
It does not return the queue. Preserve case-insensitive comparisons, the source
selector typo `senpollmanager`, the response key `TxPowProcesssor`, and the success
message even for an unknown selector. Unknown actions return an empty response.
Exercise missing/null/wrong-typed parameters, absent services, null last messages,
Unicode case variants, helpers, metadata/factories and the real command runner.

The existing MessageStack printing implementation writes directly to stderr;
the Java source calls MinimaLogger with notification disabled. Reuse that logger
so queue details receive the same timestamp/memory prefix as other messages without
notifying listeners. Validate displayed logs and notification-queue effects.

Controlled queues establish command behavior, not a globally atomic snapshot or
completion of asynchronous checker work. Add actual-node smoke checks in both
runtimes, and keep those timing-dependent assertions limited to valid shapes and
observed processing. Run relevant regressions and normal/sanitizer builds.

Review also identified callback reentry as a necessary part of the logger change.
The local JDK 26.0.1 LinkedList source checks `nextIndex < size` before checking
the iterator's modification count in `next()`. A logging callback may therefore
clear the queue without throwing, while appending requires a concurrent-modification
exception before another message is fetched. The native recursive lock, counter
and index loop preserve those rules. Test reads, append, clear, retain-all and
retain-first callbacks at queue sizes one through seven.

Two initial comparison reports are retained: fixture-only Main removal emitted an
extra native log; a runner error produced different Java/native stack-frame counts.
The first was corrected in the fixture. The second is an explicit platform boundary:
the observer classifies stack-frame notifications and excludes them while preserving
non-stack errors, log-prefix counts and command-generated queue effects.

Observed results: both full builds pass. Each configuration passes 262 comparison
programs containing 718 operations, including 35 reentry programs. Both actual-node
runs pass with two processes each and all three checker deliveries. Seven affected
regression groups pass in each build. The normal Java observer needed a variable
alias fix before its final two-group rerun; the sanitizer's final nine-group run
passes in full. See [saved evidence](../../validation/systemcheck-work-in-progress).
The full 236-test suite was not run, and H2 AOT is outside C++ sanitizer coverage.
