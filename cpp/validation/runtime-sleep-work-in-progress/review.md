## Code Review

### Summary

Reviewed the six production integrations and Main header, changed fixtures,
new Java/native oracle and comparison driver, CMake registration and documentation.
The change reuses NativeThread and restores exact local InterruptedException
handling at Main's three waits. Tests compare observable state and real network
replacement, not compilation alone. No production blocker was found in this scope.

### Findings

#### Resolved — Test setup lacked required SQL services

The initial native fixture could not construct MinimaDB. It now reuses the existing
SQL recording connector. No wallet database is opened. Initial failure is preserved.

#### Resolved — Completion depended on scheduling delay

A 25 ms fixture sleep could permit old-network completion before the branch under
test. The helper now waits until the caller's pending bit is consumed, forcing the
wait-loop branch. Both configurations pass after this change.

#### Resolved — Network failure diagnostics were suppressed

The sandbox run terminated before its JSON record during real network startup.
Tests pass with permission to bind temporary sockets. The oracle now sends runtime
diagnostics to stderr so later startup failures remain observable. No source
constructor or sleep was replaced to bypass the failure.

### Remaining scope limits

P2P's existing std::exception catch is broader than Java Exception for native
JavaError subclasses. This separate gap is recorded, not declared solved. New
direct interruption comparisons do not cover the NIO shutdown-loop pause, Minima's
full daemon lifecycle or ArchiveServer's post-response delay. These integrations
retain source catch structure and pass relevant scoped regressions; full API and
behavior parity remain not_verified. No tests/corpora were weakened or removed.

The test controls only the old network and records SQL calls. A real replacement
network binds port 0, disables P2P/P2P2 and uses an empty CONNECT_LIST. It never
opens the user's preview or mainnet data. Production ownership/join logic is unchanged.

### Verdict

✅ Approve for this scoped local checkpoint. Both actual executable builds pass;
19 comparisons / 167 assertions and seven focused CTests pass per configuration.
The full suite was not rerun. Remaining parity work is explicit.
