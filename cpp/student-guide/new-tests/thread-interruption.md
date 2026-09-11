# Interrupting a sleeping worker

This lab covers a real shutdown difference in `MessageProcessor`, `txnlock` and
`SendPollManager`. It does not certify complete Java thread parity.

## The observable problem

The pinned Java processor waits in 250 ms steps. Once its accumulated wait is
strictly greater than the timeout, it marks shutdown complete, clears running,
and interrupts its worker. The old C++ implementation changed the flags but did
not deliver the interrupt. A handler sleeping between commands could keep going.

A particularly useful example is a delayed unlock. Java sleeps **before** clearing
the transaction lock. If that sleep is interrupted, control exits by exception and
the lock remains held. Returning success or releasing it would change the source
behavior, even if doing so seemed like convenient cleanup.

## Reusing the existing runtime

[NativeThread](../../src/support/NativeThread.hpp) already supplies worker identity
and uncaught-exception delivery. The change adds an interrupt state to that same
runtime. The existing timer worker supplies the local mutex/condition-variable
pattern. The existing transaction lock supplies the bounded conversion of very
large millisecond delays. No new dependency or thread library is introduced.

The state contains a mutex, condition variable and pending bit. An interrupt sets
the bit under the mutex and notifies the sleeper. Sleep checks the bit while
holding the same mutex, including before waiting. This prevents an interrupt
between the initial check and the wait from being lost. Repeated interrupts
coalesce into a bit; they are not counted as queued exceptions.

A negative delay throws before reading that bit. A valid delay, including zero,
consumes a pending interrupt and throws `java.lang.InterruptedException`. A
non-clearing status query differs from `interrupted()`, which reads and clears.
Large delays are divided into at most one-day chunks so conversion to the native
clock cannot overflow for Java's signed-long maximum.

The processor owns its descriptor before starting the thread and assigns the
actual worker ID inside the thread. Its existing shared ownership keeps it alive
until the handler returns. `stopMessageProcessor()` still only requests ordinary
shutdown. The timeout path adds interruption; `joinMessageProcessor()` still waits
for actual exit rather than trusting the early completion flag.

## What the tests execute

[ThreadInterruptOracle.java](../../support/ThreadInterruptOracle.java) runs the
unchanged pinned Java processor, transaction lock, send-poll manager and command
runner. It reuses the existing reflection helper to install a database cleanup
recorder, without opening a wallet or database. Logger output is suppressed.
[thread_interrupt.cpp](../../support/thread_interrupt.cpp) runs the native classes.
Neither fixture substitutes the sleep or injects an interruption exception.

[The comparison driver](../../tools/thread_interrupt_differential.py) compares nine
complete scenario records, then checks expected outcomes separately for both
runtimes: 158 scalar outcome checks per build configuration.

| Scenario | Required observation |
|---|---|
| Pending flags | Negative delays retain interruption; valid delays consume it; repeated clearing returns false |
| External interruption | A long sleeper remains blocked before the request, then throws with the bit cleared |
| Immediate lock/unlock | No sleep occurs, so the pending interrupt remains pending |
| Hard sleep shutdown | The handler throws and database cleanup executes exactly once |
| Blocked lock acquisition | The interrupt exits indefinite retry without clearing the existing lock |
| Delayed unlock | The interrupt exits before unlocking, including a signed-long-maximum delay |
| Ordinary shutdown | The short sleeping handler finishes normally without an interruption |
| Interrupted shutdown waiter | The caller catches its own interrupt, keeps the source counter behavior, then interrupts the worker |
| Actual send-poll shutdown | The hash command succeeds and is removed; interruption prevents a second retry timer |

The send-poll test executes `hash data:interrupt-fixture` through the actual command
runner in each runtime. It waits for the successful command to leave the queue,
then invokes the real timeout shutdown. One initial timer remains. Two timers
would mean execution incorrectly continued past the interrupted pause.

The unchanged worker tests retain their 2,000-message FIFO/cleanup run, concurrent
lock acquisition, timer deadlines, lifetime checks, queue mutation cases and
uncaught-error ordering. Both the ordinary and sanitizer configurations pass the
six focused CTests; both build the actual node executable. Results are archived
under [thread interruption evidence](../../validation/thread-interrupt-work-in-progress/progress.json).

## Boundaries and next work

This is cooperative interruption, not forced termination. Raw C++ sleeps in other
mapped files still need migration. Socket I/O, monitor waits, arbitrary external
handlers, pre-start/terminated-thread interrupt semantics and all possible
interleavings are not certified. The fixtures record database cleanup calls rather
than exercise durable database locks. Exception categories are compared; exact JVM
stack frames, diagnostic prose and elapsed scheduling times are not compared.

## Check your understanding

Why should a zero-delay unlock preserve a pending interrupt while `Thread.sleep(0)`
consumes it? The answer is in the source branch: zero-delay unlock never calls sleep.
