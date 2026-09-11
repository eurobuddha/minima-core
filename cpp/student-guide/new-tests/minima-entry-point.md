# Lab: the Minima application boundary

Written before testing the next mapped file. Minima resets defaults, derives the
versioned home directory, applies config/environment/arguments, creates the data
folder, derives ports with Java integer arithmetic, logs its banner, installs the
uncaught handler, constructs Main and registers the shutdown hook when enabled.
Daemon mode polls shutdown completion. CLI mode reads UTF-8 lines, trims nonempty
input, checks the current singleton, runs commands, formats results and recognizes
a quit reply. Exactly "quit" suppresses output; combined commands do not.

runMinimaCMD validates a trimmed copy but dispatches the original string. Pretty
mode uses MiniFormat; compact mode unwraps a single result. mainStarter invokes
main on a new named native thread with the existing uncaught-error boundary.

Reuse the console reader from MinimaRPCClient, actual configuration/runner/format
logic, Main and NativeThread. Compare direct wrappers and configuration failures
with pinned Java. For CLI control-flow tests, clearly label the Main-construction
and shutdown-hook platform boundaries. Test real process signal/exit handling and
the actual Main startup failure boundary separately. No recording SQL fixture is
a durable database; successful full startup is not proved until the native SQL
provider and real runtime integration are present.

## What the first experiment found

Both builds pass 487 wrapper comparisons, five configuration-exit processes,
three starter failure/exit processes and sixteen shutdown-hook process pairs.
The command table is real. The singleton fixture bypasses Main construction only
for identity tests. No source rewriting or replacement Main class is used.

Whitespace-only batches revealed a generic vector error in the runner. Source
split[0] instead throws ArrayIndexOutOfBoundsException; the native runner now uses
that category and message. Null caller IDs must be checked after invalid parameter
names, and an empty batch does not inspect the caller at all. Empty pretty output
is an empty string; empty compact output is `[]`.

The first hook comparison lost already-buffered stdout after readline when Python
communicate read the underlying pipe. Direct fixture runs showed the hooks worked.
Using an unbuffered pipe made the readiness line and remaining output observable.
The failed report is retained to distinguish this harness correction from a
production fix. Four hooks rendezvous at a latch before printing, so a sequential
hook implementation would time out. Hook failures reach the existing native
uncaught boundary; normal process exit and signals run hooks, while halt skips them.

Native input owners use Main's mutex/snapshot pattern because this installed libc++
does not support atomic<shared_ptr>. Process-owned hook/worker registries retain
objects through exit. Hook setup rolls back ordinary failed setup operations, and
jthread owners join already-created hook workers during unwinding. Resource failure
and concurrent signal installation are not validated by these finite tests.

The default executable still fails at the missing native SQL/Main service boundary.
The driver asserts that failure and timely exit separately, without awarding it a
Java parity pass. Successful normal CLI/daemon operation, shared starter arguments,
full native host lifetime, shutdown receiver integration and isolated-surrogate
command strings remain unfinished. The map marks this file implemented_partial.
