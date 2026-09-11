## Code Review

### Summary
Reviewed all changed native production files and test fixtures/drivers, plus the
CMake addition, against pinned source and local OpenJDK wait implementation.
Ownership is retained across waits; interrupt callbacks do not acquire the pool
lock and run outside the interrupt-state lock. Wait nodes are weakly referenced
and resolved once, avoiding a late callback dereferencing a retired wait.

### Findings
A correctness issue found during review was fixed before final verification:
the triggering interrupt must be consumed before pool-lock reacquisition. Clearing
it afterward loses a second interrupt delivered during reacquisition. Both actual
Java and native fixtures now hold the pool lock, wait for first consumption,
deliver a second interrupt, and require the thrown exception plus a pending flag.

No remaining blocking finding for this bounded change. Timer synchronized-list
reentrancy is a known existing parity gap; complete executor/thread lifecycle,
allocation failures and every concurrent schedule are not certified here.

### Verdict
Approve the scoped change. Both executable builds and all six focused CTests pass
normally and with address/undefined sanitizers. Full source parity remains open.
