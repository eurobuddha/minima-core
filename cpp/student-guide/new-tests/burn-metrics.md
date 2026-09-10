# Lab: historical burn metrics and stateful helpers

Written before the new comparison. This command measures burns; it does not
create a transaction. Scan at most 50 ancestors, counting the block's own
transaction and each available referenced transaction, including duplicates.
Keep separate windows for counters 0, below 10, and all accepted helper calls.

Reuse the real TxPoW burn calculation, MiniNumber arithmetic, tree and TxPoWDB.
Generate known input/output differences in Java, store referenced objects in RAM,
and also test missing records, recording-SQL lookups and failures. Assert exact
numeric JSON fields for count/min/max/median/average and window boundaries.

The median sorts the caller's list descending and returns element size/2; for
an even list this is the lower middle, not the mean of the two middle values.
Check stable ties, returned element identity, empty/null lists and small malformed
lists. A failing comparison exposed that std::stable_sort may lose a moved
shared pointer when its comparator throws. The adapted OpenJDK run discovery,
binary insertion and merge schedule preserve Java's visible list prefix.
Completed runs contain no nulls and can use std::inplace_merge. Test nulls at
the beginning, middle and end of ascending, descending and shuffled lists,
including lengths around the 32-element merge threshold. This narrow helper
is not a generic replacement for sorting with arbitrary throwing comparators.

Use one retained command instance for repeated runs. Its accumulators persist;
a fresh getFunction object starts from zero. An empty run changes the minimum
sentinels to zero, affecting later calls. Public checkBurn accepts negative and
large counters; only runCommand restricts traversal to 50 blocks. A non-list
action returns an empty response, while runner parameter validation rejects the
hidden action parameter. Add explicit tests for these differences.

The raw burn arithmetic does not filter by token ID. A fixture with token input
99 plus burn input 8 and burn output 1 contributes 106. An initial independent
expectation incorrectly used 7; retain that failure and explain the correction.

## Observed results

Both final builds pass 364 programs / 4,361 operations / 30,113 SQL observations.
Repeated empty-list medians also return the same shared zero object. Both complete
builds pass. The affected runner, history and scanchain comparisons pass in both
configurations; see [saved evidence](../../validation/burn-metrics-work-in-progress/progress.json).
These are finite tests with recording SQL, not full-node parity certification.
