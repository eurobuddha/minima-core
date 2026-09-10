# TimedChecker — logic, translation and tests

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/brains/TimedChecker.java](../../../../../../../../upstream-study/src/org/minima/system/brains/TimedChecker.java) | [src/org/minima/system/brains/TimedChecker.cpp](../../../../../../../src/org/minima/system/brains/TimedChecker.cpp) |

## Teacher's worked explanation

TimedChecker runs block validation on another thread and limits how long its
caller waits. Its two flags answer different questions: `finished` says whether
the worker reached its final assignment, and `valid` stores the validation result.
A worker can stop exceptionally without setting either flag.

### Follow both threads separately

The constructor initializes both flags to false. A checking call records the
time, starts a worker, then sleeps in 50-millisecond intervals. After each sleep
it examines `finished`; otherwise it updates elapsed time. The limit is 120,000
milliseconds. Java interrupts the worker after this wait and finally returns the
stored validity flag. An optional final block log happens outside the main catch.

The worker calls `TxPoWChecker.checkTxPoWBlock`. On an ordinary Java Exception it
logs the exception and sets validity false. It then sets finished true. A Java
Error does not enter that Exception catch. Also, if printing the exception throws,
the assignments after the print are skipped. Thus “worker stopped” does not always
imply “finished is true.”

Neither flag is reset at the start of another checking call. This is source
behavior, even if a different API design would be easier to explain. A previous
valid result can still be returned while a new worker is running. The tests must
preserve this behavior rather than improving it as part of translation.

### What C++ has to own

The caller may return before the worker completes. Capturing references to local
variables would leave the worker reading destroyed objects. The native worker
therefore captures shared ownership of its flag state, parent, block and
transaction list. It also retains ancestor nodes because native tree-parent links
are weak references. The adapter for a vector of transaction values creates
aliases into that original vector and retains the vector even when it is empty.

The flags use atomics for native communication. Escaping Error and allocation
failures pass through the existing NativeThread uncaught-handler boundary.
Ordinary failures retain the source logging and assignment sequence. Keeping an
object alive does not make simultaneous mutation safe: callers must still avoid
changing validation inputs while the worker uses them.

### How the tests support the translation

`timed_checker_checks` covers ordinary validation and ownership scenarios.
`timed_checker_errors_differential.py` compares actual Java and C++ checker calls
using Java-generated serialized parent/block/transaction fixtures. It changes
chain ID, height, parents, time, difficulty, magic, transaction count, duplicates,
MMR root and null arguments. Logger failures are injected at nested catch levels;
the report compares ordered events, returned value, finished/valid flags and
uncaught-handler dispatch.

The current regular suite passes 105 comparisons in the normal build. Fourteen
of those extend the earlier 91-case corpus with empty-MMR-root behavior and log
failures during its handling. Generated thread counters, elapsed log milliseconds,
automatic stack frames and Java NPE expression diagnostics are excluded; these
exclusions are explicit limits on the comparison.

One important test first primes a valid result, starts another worker, and holds
that worker at the logger until the checking call returns. Only then does the
fixture release it. The observation is a returned true value followed by a final
false validity flag. A release signal makes this reproducible; guessing that Java
and C++ workers will finish within the same 50ms interval would be a flaky test.

Separate deadline tests use the real two-minute limit. They verify ownership
after a timed return and the flags after an uncaught worker Error. Earlier normal
and sanitizer runs passed these deadlines; they were not rerun merely because
this latest change added empty-root fixtures. Do not describe the deadline as
tested with an artificially shortened timeout.

### What remains unfinished

Native Java-style interruption, the stored Java Thread object API, thread groups,
and arbitrary thread-creation/resource failures are not implemented or fully
verified. The native worker detaches with owned captures. It does not provide
general cancellation of arbitrary blocking code. Therefore this is a partial
translation with tested ownership and failure paths, not complete thread parity.

**Exercise:** draw the two flags after (a) a normal false result, (b) an Error
before the worker assigns validity, and (c) an Exception whose logging throws.
Repeat the exercise with a checker previously primed to true.


## Open the test evidence

- [timed.cpp](../../../../../../../support/timed.cpp)
- [timed_checker_errors_differential.py](../../../../../../../tools/timed_checker_errors_differential.py)
- [TimedCheckerErrorsOracle.java](../../../../../../../support/TimedCheckerErrorsOracle.java)
- [timed_checker_errors.cpp](../../../../../../../support/timed_checker_errors.cpp)

## Dependencies and reading map

Direct Minima imports: [TxPoWTreeNode](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [GeneralParams](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [TimedChecker](../../../../../../../../upstream-study/src/org/minima/system/brains/TimedChecker.java#L19), [checkTxPoWBlock](../../../../../../../../upstream-study/src/org/minima/system/brains/TimedChecker.java#L24), [run](../../../../../../../../upstream-study/src/org/minima/system/brains/TimedChecker.java#L35).


Pinned Java SHA-256: `4f531dffac2a87a54335adac5a82ad0e967bcba2c4c5fd027b4beb7517ed72f3`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
