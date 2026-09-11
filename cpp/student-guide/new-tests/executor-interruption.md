# Waiting for workers without losing interruption

This lab follows NIOManager's use of a fixed thread pool. Its normal and hard
shutdown both call shutdown, awaitTermination, then shut down the server.
The Java pool's awaitTermination behavior is more specific than a generic sleep.

The installed OpenJDK source first converts milliseconds to nanoseconds with
saturation. Under its main lock it returns true if already terminated, or false
for a nonpositive timeout. Those paths do not consume pending interruption.
Only an active, positive wait reaches Condition.awaitNanos, which can consume the
pending flag and throw InterruptedException without a message. The pool continues
running/draining; interrupting a waiter does not cancel its tasks.

The native adaptation reuses NativeThread's pending bit and existing executor
queue, state lock, worker ownership and termination predicate. A scoped wakeup
registration lets interrupt signal the current wait. Delivery occurs outside the
interrupt-state lock; weak ownership prevents a late callback touching a destroyed
wait. A private wait record has one outcome: termination signal, interruption or
timeout. Its own mutex and condition variable avoid a lost notification without
polling. Termination signals all registered waiters, while an interrupt targets
only its thread. Large native delays are divided into bounded chunks.

After waking for interruption, the waiter consumes the triggering flag, then
reacquires the pool lock and throws. A second interrupt during reacquisition stays
pending, as the added two-interrupt case checks. Otherwise the pool state is checked again, since termination
may have happened while a timed-out waiter was reacquiring the lock. Interruption
arriving after a termination signal remains pending, matching the signal-winning
Condition path. This finite experiment is not a proof of every race schedule.

The new Java/C++ comparison uses actual fixed thread pools. It checks empty/open,
terminated and held-worker pools with negative, zero, small-positive and saturated
long timeouts; external interruption and completion; multiple waiters with only
one interrupted; and a thread reusing its registration after three timeouts.
A controlled pool-lock case delivers termination before interrupt while preventing
the waiter from reacquiring the lock. Java uses the actual reentrant mainLock and
shutdown; the native friend fixture performs the same existing empty-pool state
transition and signal while retaining its nonrecursive state lock.

Four integration cases execute the actual NIOManager hard/normal shutdown methods.
A real held executor task makes a pending interrupt throw before server shutdown.
An empty terminated pool preserves the pending bit and lets server/processor stop.
The server is unstarted; these are lifecycle ordering tests, not socket tests.
No user database, preview or mainnet node is opened.

## Timer follow-up

TimerProcessor also reuses NativeThread for its actual one-second sleep and keeps
a worker descriptor for stop(). A callback during polling can observe the pending
interrupt immediately after calling stop; the later sleep consumes it and exits.
An interrupt without an explicit stop similarly exits through the source catch.
The source's null-entry log-and-skip branch is now preserved in pollAt as well.
Three additions to the existing thread comparison use the actual timer worker and
logger boundary: stop during polling, self-interrupt during polling, and a null
queue entry installed under the real queue lock. They observe the flag before and
after the callback, its consumption, queue size and worker shutdown. No sleep is
injected or shortened. Complete timer reentrancy and other API gaps remain open.

## Verified results

Normal and address/undefined sanitizer executable builds pass. Each build passes
37 executor comparisons / 273 assertions and the expanded 12 thread comparisons /
194 assertions, including three new timer cases. All six focused CTests pass:
these two comparisons, runtime sleep (26 comparisons / 326 assertions), executor
uncaught exceptions, fixed executor and message workers. The complete suite was
not rerun. These counts describe the finite driver checks, not all possible inputs.

[Saved results and scope](../../validation/executor-interruption-work-in-progress/progress.json).
[Java experiment](../../support/ExecutorInterruptOracle.java),
[native experiment](../../support/executor_interrupt.cpp), and
[comparison driver](../../tools/executor_interrupt_differential.py).

Check your understanding: why does an already terminated pool preserve the pending
interrupt, while an active positive wait throws? Why must a second interrupt during
lock reacquisition still be visible after the first InterruptedException is caught?
