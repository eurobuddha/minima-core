# Lab: old backups and a loopback archive

Written before the remote integration test. Reuse native_restore_startup.Node,
ready/key_state/chain, native_archive_startup.coins, and the peer checker's real
loopback Responder. All node commands use the production registry and processors.

Start a real solo Java node, create its eight keys at uses seven, and save it.
To make an old-backup fixture without waiting two days, load that saved one-block
tree through the existing archive fixture. Require genesis at height one and no
cascade. Change only its header timestamp to zero, recompute the TxPoW ID, and
recalculate the tree. Assert that transaction-body and MMR encodings are unchanged
and the block still meets its difficulty. Perform the same operation on copied
directories in both runtimes and require identical resulting tree/block bytes.
This is a deliberately retimed genesis fixture, not an untouched historical node.

Open each retimed directory as a real node and make a backup through the actual
backup command. The target node starts with key uses 100, then restores a backup
written by the other runtime. The restored tip is old enough to enter performResync.
Serve real MiniData/IBD encodings, comparing both encoders first. A successful
sequence answers the -1 preflight, returns the retained genesis on request one,
and ends with an empty IBD on request two. This tests nonempty network decoding and
duplicate-block processing, not advancement to a new chain height.

Also test an immediately empty archive, three failed preflight attempts, and a
connection failure after one batch. Preserve the real ten-second retry sleeps.
Record every framed request and independently assert its requested start height.
The source returns a restart message even when the preflight fails. That branch
returns before the key-use increment, so reopened uses stay seven. Success and
later connection failure occur after the increment and reopen at 263.

For every workflow require clean process exit, removed restore folder, exact
retimed chain/coin state and complete key map after opposite-runtime reopen.
Sign and verify a message, reject altered data, and verify exactly one use increment.
Do not claim cascades, chain advancement, multiple batches or memory-threshold
resets from these one-block tests. Keep those requirements open.

Observed result: all eight workflows and 19 node processes pass in each of the
normal and ASAN/UBSAN builds. Both fixture builds and the existing 509 storage plus
37 network comparisons pass. No production fix was needed. The native H2 dependency
is not covered by C++ sanitizer instrumentation. See the [saved results](../../validation/restoresync-remote-work-in-progress).
