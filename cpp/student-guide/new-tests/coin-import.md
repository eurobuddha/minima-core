# Lab: import a coin proof and preserve the historical MMR root

Written before the new test. Reuse CoinProof conversion, the existing MMR proof
checks/update, TxPoWSearcher and the wallet fixture's real MinimaDB/tree objects.
The command has two success paths whose differences must remain visible.

First it rejects a spent coin. It then constructs a proof from the current local
MMR for the imported coin's entry. If that proof validates and a tree node already
contains this coin ID, importing adds its entry to that node's relevant list and
recalculates relevant coins. An already relevant entry is an error. This path does
not read track or validate the supplied historical proof; malformed track and an
old supplied proof can therefore be ignored when the existing coin is found.

Otherwise validate the supplied proof against the current MMR history. Find the
tree node at the proof's block time, save its root, unfinalize its MMR, update the
entry using the supplied proof, and finalize again. Add the coin to the node before
parsing track. A bad track value therefore fails after some state changes. A valid
false value leaves the coin untracked; absent track defaults to true. Finally the
root must equal the saved root. Do not roll back mutations that Java retains.

Compare exact replies, errors, serialized MMR state, coin lists and relevant entry
lists after every operation. Use actual proof fixtures and include complete and
pruned local knowledge, existing coins, duplicate imports, spent and tampered
coins, missing history/tip, typed parameters, trailing/truncated wire data and
registered runner dispatch. Where SQL storage is selected, reuse the recording
SQL fixture and verify its calls; that does not establish durable storage.

The root invariant is the main independent assertion: adding knowledge of a coin
must preserve the cryptographic commitment. Tracking is local knowledge, not
permission to spend. The native command must not add wallet keys or signatures.

## First expectation corrected from Java

The first 37 programs agreed. The next Java-only assertion expected every repeat
import to reject an already tracked coin. That rule is conditional: rejection
occurs only when the current local proof validates and finds the existing node.
In the pruned fixture, a later spent sibling prevents reconstructing a valid
current proof even after restoring the older leaf. The historical proof remains
valid, so repeating the historical path appends another coin and relevant entry.
The corrected case asserts those duplicate records and the unchanged MMR root;
no production behavior was changed to enforce uniqueness. The initial failure
is retained in the validation directory.

A second expectation distinguished a full MMR from deliberately incomplete
fixture knowledge. Removing all non-peak entries also removes the newer spent
leaf. The pinned checkProofTimeValid accepts a historical valid proof when its
subsequent leaf lookup is empty; it cannot compare a newer spent record that
the fixture discarded. Java therefore accepts that old unspent proof in this
artificial pruned case, while the complete history rejects it. Preserve both
results. This fixture probes the method's missing-information rule; it is not
a claim that a live node may discard arbitrary spent records safely.

The first 117 comparisons then agreed, including historical restoration and
malformed input. The SQL insertion case differed only in its random tree-node
identifier. The existing node-state projector already validates and consistently
labels these IDs for SELECT statements. Its optional SQL-statement list now also
covers the coin INSERT for this test, using one shared identity map across writes
and reads. Other test groups keep the original SELECT-only default. All non-ID
SQL bindings, call ordering, exceptions, coin bytes and block heights remain exact.

SQL failure injection then exposed a fixture capture boundary at program 130.
The new all-coins observation performs SQL reads after the command. Native
recording services included those read diagnostics, while Java had already
closed its log capture. For this experiment only, Java now includes the node
observation in a finally-restored capture scope. The compared logs cover both
the command and its explicit state probes. Production logging is unchanged.

## Observed results

Both builds pass all 142 programs and 390 operations. CTest durations were
7.23 seconds normal and 17.15 seconds under ASAN/UBSAN. The
[saved evidence](../../validation/coin-import-work-in-progress/progress.json) includes
the initial expectation/fixture differences and corrected passing reports.
Source exception prose and random SQL tree IDs have the explicit normalization
described above; serialized coin/MMR data and non-ID SQL bindings remain exact.

The affected regression checks also pass: all 20 selected integration tests in
the rebuilt normal configuration, plus node-state and command-runner comparisons
in both configurations. The node-state comparison exercises 363 programs, 1,411
operations and 21,009 recorded SQL calls; command dispatch covers 1,265 cases.
For the concurrent sanitizer baseline, the node-state driver uses the separate
`coin_import_runtime` executable so the baseline executable stays intact. These
checks test the shared fixture and registry changes as well as the new command.
