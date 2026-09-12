# Cascade cleanup crash validation

This record covers the September 11 cascade destruction stack overflow. Full
Minima behavioral parity remains unverified: 456/457 counterparts, external JNI
missing. This change adds no newly converted Java counterpart.

## Reproduction and correction

[Technical crash extract](crash-evidence.json): UUID-matched old `6c28109` preview,
SIGBUS/Stack Guard, 14,309 recorded stack frames, temporary IBD cleanup inside
receiveGreeting. The unfixed native 30,000-node regression returned SIGBUS (`-10`)
on a 512 KiB worker. CascadeNode now queues parent reference releases iteratively
and explicitly preserves its existing copy/move operations. No live shared node
is detached. The production diff is confined to CascadeNode.hpp/.cpp.

## Build and test scope

The normal Debug, optimized Release (`-O2 -g`, assertions retained) and Debug
ASan/UBSan builds each rebuild the native node plus the affected test harnesses.
Their `results.json`, `ctest.xml`, build/test logs and differential result files
record the actual commands, exit status and observations. Sanitizer runs use
halt-on-error. The existing native H2 dependency is reused; it is not rebuilt with
C++ sanitizers.

The selected CTests are cascade_release, cascade_differential, ibd_differential,
nio_message_differential, runtime_mains_differential, fixed_task_executor,
source_mapping, source_mapping_snapshot and native_node_startup. The last test
creates isolated temporary plaintext/encrypted nodes and checks Java/native
startup, hash/status/quit and database reopen. It does not open the user's wallet.

The lifetime regression has ten cases, each exercised through a 512 KiB pthread
and FixedTaskExecutor, with 30,000-node long chains. Weak observers check every
node for eventual release, retained owners check surviving ancestry, and the
complete-IBD case uses actual wire reconstruction. Thread-local teardown and
custom-deleter reentrancy are included. The [student lab](../../student-guide/new-tests/cascade-release.md)
explains each case and the difference between native lifetime and Java parity tests.

## Result interpretation

All three configurations passed all nine selected suites (27 suite runs total).
The new regression passed ten cases / twenty worker runs in each configuration
(60 worker runs total), at the unchanged 30,000-node depth and 512 KiB small-stack
setting. No ASan/UBSan diagnostic was reported.

| Configuration | Selected suites | Long-cascade regression | Test time |
|---|---:|---:|---:|
| [Debug](normal/results.json) | 9/9 | 20/20 worker runs | 214.49 s |
| [Optimized](optimized/results.json) | 9/9 | 20/20 worker runs | 76.60 s |
| [ASan/UBSan](sanitized/results.json) | 9/9 | 20/20 worker runs | 691.66 s |

Each configuration also passed 2,892 cascade comparisons, 1,057 IBD cases,
1,918 NIO programs / 4,677 packets, 18 runtime-main comparisons / 121 assertions,
and 12 real Java/native node lifecycle processes. The [build identities](build-identities.json)
record source hashes and binary UUIDs. All 457 student lessons pass structural
audit (9,663 links, 2,628 line anchors).
Passing these selected checks is not a claim that the entire historical suite was
rerun, that every concurrency schedule was tested, or that overnight mainnet
stability is established. ASan/UBSan are not ThreadSanitizer. Shared ownership
cycles and concurrent mutation of the same node remain outside this correction.

The original preview and wallet were copied locally and all 29 files byte-verified
before replacement, following the owner's later instruction to overwrite the
preview and start fresh. Wallet files and the raw OS report are not published.
