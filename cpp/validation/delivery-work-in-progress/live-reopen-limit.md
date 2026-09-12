# Live reopen timing remains unresolved

The actual e82e91f runtime ZIP passed relocation hashes, launcher help using a
system-only PATH, fresh startup, SHA3 command, peer height/hash agreement at2309856,
and clean saved-state shutdown. Its immediate reopen remained behind the peer and
failed the unchanged150-second target. This failed result is retained.

A subsequent180-second diagnostic restart of the same saved node and unchanged
package loaded height2309860, caught up at2309864, and followed new blocks through
2309869 with matching peer hashes and clean exit. The block log records actual
successful block-check results and additions, not merely socket connectivity.
This does not convert the first reopen failure into a pass.

The first reopen log received733 TxPoW replies. Both inspected TxPoWProcessor
implementations defer blocks whose referenced transactions are not all available;
the existing controlled corpus covers out-of-order blocks and later transactions.
That mechanism is a possible contributor, not an established explanation of this
live timing result. A caught closed-H2 diagnostic also appeared during shutdown;
its scheduling cause is unconfirmed.

An unmodified pinned-Java control on separate port23001 did not complete its fresh
stage within150seconds: it sent a greeting but never reported a connected chain.
It exited cleanly. It therefore provides no successful Java reopen control and does
not establish that native delay is inherited. No production behavior or acceptance
timeout was changed to hide these results.

Next action: capture connection/processor queue timing and missing transaction IDs
on an uninterrupted fresh/reopen pair, obtain a successful comparable Java control,
and distinguish source deferral, provider lifecycle and native scheduling costs.
All fixture nodes exited; the user's existing preview and data remain untouched.

## Optimized package observations

The `-O2 -g` package at e540881 initially passed fresh startup at height 2309904.
Immediate reopen reached a matching saved height/hash before the persistent socket
had connected. The verifier incorrectly asserted connection readiness at that point;
its condition now waits for connection within the unchanged 150-second limit.
The original failure and exact state are retained in
[initial readiness failure](optimized-initial-readiness-failure.json) and
[its explanation](optimized-readiness-failure-explanation.json).

The corrected [fresh/reopen repeat](optimized-runtime-corrected.json) failed during
fresh startup: 30 status observations remained at height 0 with no connections,
then shutdown completed cleanly. The log did report a successful connection
attempt, but status never reported an established peer or a chain. No reopen stage ran. The repeat neither validates
the corrected reopen branch nor establishes reliable mainnet synchronization.
The observed network variability also affected the pinned Java control. Its cause
has not been established. Keep socket-fixture sandbox failures in the broad test
run separate from these escalated live workflow observations.
