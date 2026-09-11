# Lab: restoring one backup section at a time

Written before the new tests. Follow the pinned restore.java. After the key gate,
parameters, file and gzip header are accepted, Main.restoreReady stops normal
workers. The command reads wallet SQL into a temporary folder, disables state
saving, writes cascade/tree/user/P2P files and loads them. It replaces relevant
history, closes its streams, allows saving again, imports wallet SQL and increments
all key-use counts by 256. It deletes archive storage and temporary restore files,
saves databases, then optionally marks/stops Main and notifies its listener.

There is no rollback catch or finally. A failure after the third section has
different effects from a failure in the first header. Reading the whole gzip
payload before applying sections would move failures earlier and change behavior.
Reuse CipherInput/GzipInput, MiniFile and the real H2 stores. Add a callback-based
MiniData reader sharing the existing length validation, reading exactly four
length bytes and then exactly that payload. Match DataInputStream.readFully.

The fixture will restore identical files into copied real H2 baselines and compare
responses, event ordering, state files, preferences, history, SQL wallet rows,
key-use increments and cleanup. Use an unstarted Main with a controlled
restoreReady override for many malformed-file cases; production Main keeps its
real method, and actual-node process tests exercise real worker shutdown/reopen.
Compare cached wallet seed separately from persisted SQL: importing a script does
not necessarily refresh a pre-existing in-memory SeedRow.

Generate valid fixtures from the existing Java/native backup commands; require
successful restores and independently check restored state. Mutate frame lengths,
truncate at section boundaries, break gzip/cipher tails, and use invalid wallet
SQL or history bytes. Do not label matching errors as a successful restore.
Readiness/parameter tests must run before any state mutation. Test both shutdown
values and command registry dispatch. Record each failure and any narrowly defined
normalization. Full API/resource-exhaustion/concurrent-state parity remains open.

Before the real-node process test: use the existing native_node_startup launcher
arguments and reply parser. Ask the actual node to create all default keys, wait
until their requested use count is visible, and make a backup. Restore it using
the other runtime after changing destination key uses, then reopen with the
original runtime. Repeat in the opposite direction. Check every public key's
increment, the genesis chain and the successful shutdown/reopen. This exercises
real Main.restoreReady and workers, independently of the controlled fixture.


The first real-node observer waited for 64 keys. Inspection of Wallet's constructor
showed that TEST_PARAMS (enabled by solo mode) sets the count to eight. The node
was already ready. We stopped that specific isolated node gracefully, saved the
failed observer report, and changed the test to require eight completed keys plus
a successful backup auto:false readiness gate. Production key creation is unchanged.

Before the final signing check: reuse the inspected sign/verify commands in both
actual nodes after reopening. Sign fixed fixture data with one restored key,
require verification to succeed for that data and fail for different data, then
check that this key's use count changes from 263 to 264. This tests that restored
private-key material remains usable, beyond merely checking SQL row counts.

## Results and what they establish

Normal and ASAN/UBSAN builds each pass all 128 comparisons: 16 successful restores
and 49 failures that leave state saving disabled, matching the source. The ten
selected regression suites and the separate 3,802-case primitive suite also pass.
Both complete builds succeed. This is a selected regression run, not all 225 CTests.

The actual-node test passes in both builds with six processes each. It covers
Java backup → C++ restore → Java reopen and the reverse direction. All eight solo
keys move from seven uses in the backup to 263 after restore, replacing the target's
100 uses. Chain state is preserved. One restored key signs the fixture data;
verification accepts the original data and rejects changed data. Signing advances
only that key to 264 uses. Thus the restored private material works after reopening,
which a comparison of public-key rows alone could not establish.

The first controlled-fixture flag lookup and cipher exception diagnostics failed;
those reports are retained with the corrected final results. Tests use plaintext
SQL storage with password-encrypted backups. Encrypted SQL wallet restore, damaged
running-node restores and public-network/concurrent behavior remain unverified.
See [saved reports](../../validation/restore-work-in-progress/progress.json).
