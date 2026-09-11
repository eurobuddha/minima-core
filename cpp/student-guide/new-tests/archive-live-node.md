# Lab: archive import through a running node

Written before the integration test. The archive command comparison deliberately
records recursive resync calls. This lab removes that probe by launching the real
pinned Java Minima and native MinimaCoreC++ executables in isolated solo mode.

Reuse Node, ready, key_state and chain from native_restore_startup.py. Create a
real genesis chain and all eight default solo keys, set their uses to seven, make
a password-protected backup, and shut down. The archive fixture loads this saved
chain and saves its actual TxBlocks into ArchiveManager. It does not manufacture
coin proofs or bypass the production processor during the subsequent imports.

Have both fixtures export the same saved chain as H2 gzip SQL and raw gzip blocks.
Normalize only archive insertion timestamps before export, as in the existing
archive fixture. Require identical bytes for each format across both runtimes.
Then copy the original node directory for each workflow. Set destination key uses
to 100, import an opposite-runtime archive through the real command runner, and
require the nested resync result as well as the outer command to succeed.

After the import shuts down, reopen with the other runtime. Compare the chain hash,
height, length, branches and weight, the status coin count and total Minima, and the
complete public-key/use map. Plain chain import must preserve uses 100. A reset
restore workflow must restore the backup uses seven plus the source safety increment
256, giving 263. Sign with a restored key, verify it, reject a changed message, and
check that only the signing key's use count increments.

Run both archive formats in both runtimes and reset restore in both runtimes. Keep
all inputs in temporary test directories and use no public peers. Assert graceful
process exit and removal of temporary import/restore folders. Capture exceptions
and process logs on failure so a success string cannot hide missing chain state.

This genesis integration proves a real local import path, worker shutdown and
cross-runtime persistence for its input. It does not prove cascaded or multi-batch
archives, wallet replacement from a phrase, remote retry behavior, or complete
node parity. Those remain separate required tests.

Both normal and sanitizer runs pass six workflows each (13 node processes). The
source fixture and native fixture export identical saved-chain bytes. No production
change was needed. The evidence records each runtime/format direction and all
persistence/signing assertions; see [runtime evidence](../../validation/archive-runtime-work-in-progress).
