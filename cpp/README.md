# MinimaCoreC++ — incomplete

FORS validation is complete for the documented scope: both builds passed 1,246
comparisons and 757 independent assertions using the full million-leaf forest.
The explicit sanitizer demo audit passed with empty stderr and exact Java output.
[Saved evidence](validation/fors-work-in-progress/progress.json). Earlier process
status notes below are historical; full SPHINCS and node parity remain unfinished.

Pinned Java source: `../upstream-study`, commit
`5564b9d78978efcaf23679190d05253961fc6e69` (`1.1.2.8`).

The requested full 1:1 C++ node is **not finished**. [SOURCE_MAP.md](SOURCE_MAP.md)
assigns all **457 Java files** (369 production, 88 tests) unique, package-relative
`.cpp` paths. **364 production counterparts and 47 mapped test files exist;
46 mapped files remain unported.** These existing files are still classified
as partial until their complete API and behavior have been reviewed.

Distinct MiniData hash writers now preserve Java override dispatch and nested
failure prefixes. Both builds pass 1,848 comparisons and 12,752 independent
assertions, plus eight affected regressions across restricted and local-network
runs. See the [hash-writer lab](student-guide/new-tests/hash-writers.md) and
[checkpoint evidence](validation/hash-writers-work-in-progress/progress.json).

The native Streamable interface now connects 24 existing concrete classes. Both
builds pass 7,705 comparisons and 96,172 assertions. Magic additionally passes
326 focused programs and 2,412 independent checks in each build. All 17
selected normal regressions and four selected sanitizer regressions pass. The full
Magic demonstrations timed out at 1,200 seconds; longer saved-binary replays remain pending. Another 20 explicit source implementers need
integration. See the [Magic lab](student-guide/new-tests/streamable-magic.md) and
[current evidence](validation/magic-work-in-progress/progress.json).

The help command now preserves the source's 94-entry curated catalogue and detailed
lookup through the native registry. Both builds pass 761 comparisons and 2,543
assertions, plus the affected runner/text regressions. The three JNI-dependent
commands remain absent. See the [help lab](student-guide/new-tests/help-command.md).

The mysql archive command now implements all source actions and its static entry
points, and is registered in the native runner. Both recording-SQL suites
pass 2,800 comparisons and 2,061 independent checks. Both live suites also pass 3,202 assertions across 17 node processes each:
257-block MySQL persistence, cross-runtime raw/H2 imports with cascades,
and genesis chain/reset/seed resync followed by opposite-runtime reopening
and signing. See the [live archive lab](student-guide/new-tests/mysql-live-archive.md).
Long-chain resync and full node parity remain unverified.
The full SPHINCS command sanitizer run also completed: 131 comparisons and
34 independent checks pass, including all five full-size signature paths.

ArchiveServer now has a native counterpart and standalone `minima_archive_server`
executable. Both builds pass81handler comparisons, a4-case reentrancy audit and
19CLI cases plus real MySQL/TCP service tests. Pinned Java writes the archive data;
both servers return exact bytes and retain the ten-second close delay. See the
[archive-server lab](student-guide/new-tests/archive-server.md) and
[evidence](validation/archive-server-work-in-progress/progress.json).

The mysqlcoins command now compiles with all five actions. Both builds pass
771 command/SQL comparisons. The optional native dependency now includes the pinned
MySQL driver. Both builds pass 52 live driver programs and four actual Java/C++
node processes across restart. See the [native MySQL lab](student-guide/new-tests/native-mysql-storage.md).
The live archive lab now covers nonempty updates; complete database/runtime parity remains unverified.
See the [coin-index lab](student-guide/new-tests/mysql-coins-command.md) and
[evidence](validation/mysql-coins-work-in-progress/progress.json).

The original GET and SUBSET tests now have mapped C++ files. Both builds pass
31 and 33 assertions per runtime respectively. See the
[indexed-variable and slicing lab](student-guide/new-tests/get-and-subset-original-tests.md).

The original CONCAT test now has its mapped C++ file. Both builds pass all three
source methods and 21 assertions per runtime. See the
[concatenation lab](student-guide/new-tests/concatenation-original-tests.md).

Both distinct tutorial classes and the white-paper command now preserve their
pinned text and reply shapes. Both full builds and comparisons pass 71 cases and
428 independent assertions. See the [text lab](student-guide/new-tests/static-command-text.md)
and [evidence](validation/text-commands-work-in-progress/progress.json).

The `test` command now delegates to the native database refresh implementation,
and its separate demonstration executes100000random-address iterations. The
normal and sanitizer command comparisons each pass274programs; both real-H2
node restart checks pass four processes and12refresh commands. The unused private resource helper remains unrepresented.
See the [lab](student-guide/new-tests/database-test-command.md) and
[evidence](validation/base-test-command-work-in-progress/progress.json).

The `sphincs` command now has all five source actions and is registered in the
native runner. It reuses the actual full-size signer, transaction helpers and
file implementation. Its normal full suite passes131cases, including five full-size signatures; the
sanitizer suite remains active. See the
[command lab](student-guide/new-tests/sphincs-command.md) and
[current evidence](validation/sphincs-command-work-in-progress/progress.json).

TransactionTest now compiles with its pinned script and full-size signing logic.
Both builds pass 169 formatter/logging comparisons and 338 independent assertions;
the normal actual source mains match (221 operations, expected rejection). The
sanitized actual main also passed (1411.275seconds,342assertions). Shared helper extraction also passes
the 268-case transaction replay in both builds. See the
[demonstration lab](student-guide/new-tests/sphincs-transaction-demo.md) and
[current evidence](validation/transaction-test-work-in-progress/progress.json).

SPHINCSUtils now builds signing messages, installs all 66 signature state values,
and recomputes coin/transaction IDs. Both VMs accept the normal full-size signed
transaction in 937 operations. The corrected 268-case replay and affected
regressions pass in both builds. Full transaction signing has completed in both
builds; the resumed full signer sanitizer suite has also passed.
See the [transaction lab](student-guide/new-tests/sphincs-transactions.md) and
[evidence](validation/sphincs-utils-work-in-progress/progress.json).

SPHINCS now composes the actual TreeKey192/4 and full-size FORS implementations.
Both complete builds and object/key/envelope regressions pass.
The normal signer run completed its signatures and mains; its one null-log
fixture error is resolved by a separate audit. The original sanitizer run timed out on its third signature after158matching
cases and two matching signature pairs. The resumed suite now passes all 299 live
comparisons, 1,377 independent assertions and both actual source mains. It explicitly
rechecks those two saved records and generates the missing full-size signature. The
[signer lab](student-guide/new-tests/sphincs-signer.md) and
[current evidence](validation/sphincs-work-in-progress/progress.json) distinguish
compiled code from completed validation.

SPHINCSSignature now preserves its nullable component references, exact envelope
encoding, and partial field assignments. Signature/SignatureProof expose shared
incremental writers so later failures keep their output prefix. Both full builds
pass 1,449 envelope comparisons and 158 independent assertions, plus the 9,464-case
object regression. See the [student lab](student-guide/new-tests/sphincs-signature-envelope.md)
and [evidence](validation/sphincs-signature-work-in-progress). The SPHINCS signer now compiles;
its remaining API gaps and full node parity remain unfinished.

The two FORS counterparts compile with sixteen complete HORST trees (1,048,576
inner leaves), nested signatures, and the shared incremental MMR proof writer.
Both complete builds and shared object/HORST regressions pass. Normal FORS validation
passed 1,246 cases and both source demos; sanitized validation and the explicit
stderr audit also passed. See the
[student lab](student-guide/new-tests/fors-signatures.md) and
[current validation status](validation/fors-work-in-progress/progress.json).

The four legacy HORST classes now compile with real 65,536-leaf key trees and
source-compatible signatures, mutable lists, and wire error behavior. Both builds
pass 1,438 Java/native comparisons and 1,299 independent assertions, including
Python reconstruction of the complete trees and every generated authentication
path. The 9,464 object/MMR and 1,364 utility regression cases also pass in both
builds. See the [student lab](student-guide/new-tests/horst-signatures.md) and
[saved evidence](validation/horst-work-in-progress). FORS validation and SPHINCS integration remain open.

The mapped `ZipExtractor` now performs streaming extraction with canonical path
checks and source-compatible partial-file/error behavior. Both full builds and
708 Java/native extraction cases pass, plus shared file, SQL and backup regressions.
The tests exposed and fixed canonical mkdirs and symlink error-boundary differences.
See the [student lab](student-guide/new-tests/zip-extraction.md) and
[saved evidence](validation/zip-extractor-work-in-progress).

The mapped `AesUtil` now compiles with PBKDF2-HMAC-SHA1, nullable UTF-16 text,
Base64/hex helpers and the source's persistent cipher behavior. Its comparison
found and fixed empty-decryption and initial-provider-failure differences. Both
full builds pass 1,957 programs / 10,728 operations and four affected regression
groups. All 457 student lessons remain authored and audited. See the
[student lab](student-guide/new-tests/aes-text-interoperability.md) and
[saved evidence](validation/aes-util-work-in-progress). Non-UTF-8 default charsets
and other provider configurations remain unverified.

The mapped `systemcheck` command now reports all source processor and lock fields,
posts both checker calls and the one-second timer, and logs selected queue details.
Shared queue logging now uses MinimaLogger and preserves reentrant callbacks and
LinkedList-style modification detection. Both builds pass 262 comparison programs
and actual-node checker delivery, plus seven affected regression groups. See the
[student lab](student-guide/new-tests/system-check.md) and
[saved evidence](validation/systemcheck-work-in-progress).

The mapped `megammr` and `megammrsync` classes now compile and are registered.
Main supplies the MegaMMR serving callback by default. Both builds pass 579
command/helper comparisons and ten loopback cases, covering export files, mutating
integrity checks, wallet detail selection, coin search and peer updates. Both
builds also pass four live import/resync workflows across ten node processes each:
identical Java/C++ backups, real genesis coin proofs, exact loopback requests,
saved peers, opposite-runtime reopen and wallet signing. Pruned-coin recovery,
chain advancement and complete parity remain unverified. See the
[student lab](student-guide/new-tests/mega-mmr-commands.md) and
[command evidence](validation/mega-commands-work-in-progress), plus the
[live-node lab](student-guide/new-tests/mega-mmr-live-node.md) and
[live evidence](validation/mega-runtime-work-in-progress).

The mapped `restoresync` class now compiles, including its remote synchronization
method. It preserves the source's six-section restore, wallet close/reopen order,
two-day age gate and configurable key-use increment. Both builds pass 181 Java/native
comparisons and real nodes in both restore/reopen directions with restored-key
signing. A separate loopback test exercises old backups, a nonempty retained-genesis
reply, an empty archive, failed preflight and later failure, with exact request
frames and opposite-runtime key/chain/coin reopen. It preserves real retry sleeps
and checks different key-use effects despite identical restart replies. Both builds
pass all eight workflows using 19 real node processes each. Full chain
advancement and synchronization parity remain unverified. See the
[student lab](student-guide/new-tests/restore-sync.md),
[remote lab](student-guide/new-tests/restore-sync-remote.md),
[command evidence](validation/restoresync-work-in-progress), and
[remote evidence](validation/restoresync-remote-work-in-progress).

The mapped `archive` class includes every source action branch and both network
helpers. Both builds pass 509 storage/query cases, 37 loopback request/retry cases,
and six live workflows using 13 real node processes each. Both archive formats,
reset/restore, worker shutdown, opposite-runtime chain/coin/key reopen and signing
are checked. Live chain input is genesis only; full synchronization parity remains
unverified. See the [archive lab](student-guide/new-tests/archive.md),
[live-node lab](student-guide/new-tests/archive-live-node.md),
[network lab](student-guide/new-tests/archive-network.md), and
[saved runtime evidence](validation/archive-runtime-work-in-progress).

The mapped `reset` command now compiles and coordinates nested commands through
the real runner. Both builds pass 224 Java/native cases, including four real H2
restore/reopen cases using Java- and C++-generated backups. The archive import is
recorded in this fixture; full reset/archive synchronization remains unfinished.
See the [student lab](student-guide/new-tests/reset.md) and
[saved evidence](validation/reset-work-in-progress).

The mapped `restore` command now restores all six backup sections using real H2.
Both complete builds pass 128 Java/native cases, ten selected regression suites,
and the primitive suite. Six actual node processes per build verify both restore
directions, worker shutdown, preserved chain state, and restored-key signing after
reopen. See the [student lab](student-guide/new-tests/restore.md) and
[saved evidence](validation/restore-work-in-progress). Full parity remains unverified.

The mapped `backup` command now exports real H2 wallet SQL and five other node
sections into Java-compatible encrypted backups. Normal and sanitizer builds each
pass 96 command cases, 384 gzip stream cases and 132 cross-decryptions of 66 files.
Both complete builds and all seven selected regressions pass. See the
[student lab](student-guide/new-tests/backup.md) and
[saved evidence](validation/backup-work-in-progress). Full restore parity remains unverified.

The mapped `decryptbackup` command now compiles and is registered. Both normal
and sanitizer comparisons pass 422 cases with exact output-file snapshots,
including 113 damaged inputs that preserve Java's success-with-partial-output
behavior. It reuses the existing cipher, file helpers and gzip decoder. See the
[student lab](student-guide/new-tests/decrypt-backup.md) and
[saved evidence](validation/decrypt-backup-work-in-progress).

The `MinimaCoreC++` executable now starts a real solo node when built with the
optional native H2 provider. It creates genesis and databases, answers hash/status,
and saves on quit. Twelve Java/native startup processes cross-open complete node
directories in both directions, including encrypted wallets and compact shutdown.
Real H2 comparisons cover 122 programs / 1,974 operations plus four file cross-opens.
An eight-worker ownership/concurrency check inserts and verifies 512 rows.

H2 is the pinned dependency compiled to a native shared library; no Java Minima
classes or JVM process supply application behavior. See [build instructions](native/h2/README.md),
the [student lab](student-guide/new-tests/native-h2-storage.md), and
[saved evidence](validation/native-h2-work-in-progress). Without the optional
provider configuration, startup still reports the missing SQL provider.
Public-network sync, long-running mining, daemon/RPC integration, MySQL, all
remaining files and full behavior parity remain unfinished.

The existing launcher boundary checks cover 487 public-wrapper cases, five
configuration-exit comparisons, three starter-thread cases and 16 shutdown-hook
process comparisons. These are separate from the real solo startup tests.

ItemList now compiles with its source constructor and array-cast quirks intact.
Both builds pass 1,493 programs / 11,690 operations, comparing UTF-16 strings,
mutable list aliases, raw objects, all declared overloads and bounded observation
of the non-advancing empty-separator loop. See the
[student lab](student-guide/new-tests/item-list.md).

incentivecash now compiles and is registered. Both builds pass 106 programs /
207 operations and 79 real loopback PUT requests per runtime, including exact
preference files, fresh reloads, save-before-request ordering and failure behavior.
See the [student lab](student-guide/new-tests/incentive-cash.md) and
[saved evidence](validation/incentive-cash-work-in-progress).

nodecount now compiles as a directly callable class. It remains absent from the
command registry, matching the pinned source. Both builds pass 171 programs with
314 loopback pings per runtime, comparing traversal order, estimates, errors and
CSV output. Both full builds and four affected suites pass. See the
[student lab](student-guide/new-tests/node-count.md) and
[saved evidence](validation/node-count-work-in-progress/progress.json).

multisig and multisigread now compile and are registered. They share the exact
contract and real nested transaction workflows. The 340-program comparison checks
files, signatures, vault behavior and posting; source-specific address and error
handling remain intact. See the [student lab](student-guide/new-tests/multisig-workflow.md)
and [saved evidence](validation/multisig-work-in-progress).

tokencreate now compiles and is registered. Both builds pass 335 programs /
1,349 operations, including real token and transaction signature verification.
Both full builds and affected mining, custom token and registry checks pass.
See the [student lab](student-guide/new-tests/token-creation.md) and
[saved evidence](validation/token-creation-work-in-progress/progress.json).

burn now compiles and is registered, with retained metrics and Java-compatible
nullable median sorting. See the [student lab](student-guide/new-tests/burn-metrics.md)
and [validation evidence](validation/burn-metrics-work-in-progress).

txnauto now compiles and is registered. Its static helper shares the existing
burn builder while retaining the early validity check and default link hash.
Both normal and sanitizer comparisons pass 414 programs / 1,655 operations.
Affected proof/burn, funding and runner checks pass, as do both full builds. See the [student lab](student-guide/new-tests/automatic-transaction.md).
There are 217 registered CTest entries; full-node parity remains unfinished.

peers now compiles and is registered. Both builds pass 271 programs / 1,355
operations, including 48 loopback HTTP requests per runtime, real file writes
and checker queue effects. See the [student lab](student-guide/new-tests/peers-command.md).


tokenvalidate now compiles and is registered. Both builds pass 126 programs /
254 operations and 33 loopback HTTP requests per runtime. Signature conversion
now preserves Java's I/O catch/log boundary; null imported tokens raise the source
exception category. See the [student lab](student-guide/new-tests/token-validation.md)
and [saved evidence](validation/token-validation-work-in-progress/progress.json).
Its seven affected comparisons pass in both builds;
the command fixture required a logging fix and a fresh rebuild before its final pass.

scanchain now compiles and is registered. Both builds pass 320 programs and
2,027 operations covering inclusive offset/depth loops, numeric fields, extreme
dates, ordered transaction details and SQL failures. See the
[student lab](student-guide/new-tests/scan-chain.md) and
[saved evidence](validation/scan-chain-work-in-progress/progress.json).
There are now 208 registered CTest entries. Existing history (484 programs),
TxPoW search (680 programs) and runner (1,265 cases) comparisons pass both builds.

coinimport is compiled and registered. Both builds pass 142 Java/C++ programs
and 390 operations, including historical MMR restoration, root invariance,
conditional tracking shortcuts, retained failure effects and recorded SQL.
See the [student lab](student-guide/new-tests/coin-import.md) and
[evidence](validation/coin-import-work-in-progress/progress.json). Full parity
and durable SQL remain unfinished. The rebuilt normal binaries pass all 20
affected integration tests; node-state and command-runner comparisons also pass
in the sanitizer build. Their reports are saved with the coin-import evidence.

hashtest now compiles and is registered, reusing the actual miner hash-speed loop.
Its [student lab](student-guide/new-tests/hash-speed-command.md) explains real
hashing, bounded clocks, decimal rate checks and the initial JSON type mismatch.
Both normal and ASAN/UBSAN comparisons pass 59 programs / 61 operations,
including the real million-hash default. [Saved results](validation/hashtest-work-in-progress/progress.json)
record the initial type mismatch and sanitizer test-deadline adjustment. Both
relinked command-runner (1,265 cases) and network-utility (5,668 cases) regressions
also pass in both builds. The earlier normal full baseline has finished; the
sanitizer baseline is still running.

RPC is **work in progress**. Shared authentication flags and password now use
atomic booleans and the existing locked nullable-string snapshot pattern. Both
full builds and 22 affected tests per build pass; 266 RPC programs / 614 operations
match Java. All 38 comparison reports are saved. Shared user-array access and
command-driven live enable/authentication have further validation below. See the
[RPC lab](student-guide/new-tests/rpc-command.md) and
[configuration evidence](validation/rpc-configuration/progress.json). There are
207 CTest entries at that stage. The last fully saved numbered checkpoint remains 362.

Shared JSON storage is a further **work-in-progress** change: owned value reads,
retained root/child references and checked ordered iteration replace borrowed
vector references. Both full builds and the two focused tests per build pass
(67 programs / 606 operations plus native concurrency). The normal 204-test
baseline finished with 203 passing and a stale MiniByte
exception catch. That test is corrected and its CTest rerun passes in both builds.
The sanitizer baseline is still running. Its NIO comparison timed out initially;
an unchanged direct rerun passes 1,918 programs and 4,677 packets within the
original subprocess deadline. See the
[shared JSON lab](student-guide/new-tests/shared-json-rpc.md).

Live RPC integration now passes in both builds: two LF/CRLF lifecycles, each
with 41 Java/C++ observations, 25 raw HTTP requests and two actual RPCClient calls
per runtime. It exposed and fixed Authorizer's missing default MinimaDB lookup.
Database replacement, password rotation, user changes, same-handler listener
restart/disable and real one-second pauses are covered. See the
[live lab](student-guide/new-tests/rpc-live-command.md) and
[saved evidence](validation/rpc-live-work-in-progress/progress.json). The full
204-test runs launched before this addition cover an older baseline. Normal
targets have now been relinked and all 20 selected integration checks pass,
including the live RPC comparison. The sanitizer baseline remains in progress.

status now compiles and is registered. Its [student lab](student-guide/new-tests/node-status-command.md)
explains full report assembly, device estimates, median speeds and the explicit
native memory boundary. Both full builds pass, and 187 programs / 634 operations
match Java in both builds. All 25 affected regressions pass in both builds
(301.18s normal, 952.17s ASAN/UBSAN). There are 200 CTest entries.
[Checkpoint evidence](validation/checkpoint-362/checkpoint.json) contains all
44 comparison reports and both CTest summaries.

logs now compiles and is registered. Its [student lab](student-guide/new-tests/logging-flags-command.md)
explains selective updates, partial failures and the native atomic storage used by
seven shared logging flags. Both full builds pass; 327 programs / 982 operations
match Java in both builds. All 25 affected consumer regressions pass (194.27s
normal, 457.64s ASAN/UBSAN). There are 199 CTest entries. [Saved checkpoint evidence](validation/checkpoint-361/checkpoint.json)
includes all 42 result reports.

healthcheck now compiles and is registered. Its [student lab](student-guide/new-tests/healthcheck-command.md)
explains 176 programs and 635 operations matching Java in both builds, including
real tree/cascade state, exact height differences, response types and runner gates.
Both full builds and all fourteen affected regressions pass (115.26s normal,
247.61s ASAN/UBSAN). There are 198 CTest entries. [Saved checkpoint evidence](validation/checkpoint-360/checkpoint.json)
contains all twenty-six comparison reports.

Local Git project/branch: **MinimaCoreC++**. Checkpoint 359 adds compiled,
registered trace and timemilli commands with 209 Java/C++ programs covering 652
operations per build. Both full builds and all eleven affected regression tests
pass (36.66s normal, 61.15s ASAN/UBSAN). There are 197 CTest entries. See the
[Git workflow](GIT_WORKFLOW.md), [trace and clock lab](student-guide/new-tests/trace-and-clock-commands.md)
and [saved validation](validation/checkpoint-359/checkpoint.json).

network now compiles and is registered. Its [status-command lab](student-guide/new-tests/network-status-command.md)
explains 124 programs and 562 operations matching Java in both builds, including
UID filtering, traffic reset, shared tracing and queued restart behavior. Both
full builds and ten affected regression groups pass (19.15s/27.27s). There are
196 CTest entries at that checkpoint.

ping now compiles and is registered. Its [nullable-reply lab](student-guide/new-tests/nullable-command-replies.md)
explains the existing JSONValue return representation, the runner's status check
outside its catch block, and nullable Greeting extra data. The new normal comparison
passes 95 command programs and nine isolated loopback cases; the expanded runner
error comparison passes 1,680 requests. All 35 selected regressions pass in both builds (181.54s normal, 762.10s sanitizer).
That checkpoint had 195 CTest entries.

disconnect, message and p2pstate now compile and are registered. Their
[peer-command lab](student-guide/new-tests/network-peer-commands.md) explains
222 programs and 794 operations matching Java in both builds (3.55s/5.68s),
including real queues, packet encoding and detailed status. All 12 affected
regressions pass across the validation phases. That checkpoint had 194 CTest entries.

quit and rescue now compile and are registered. Their
[lifecycle command lab](student-guide/new-tests/main-lifecycle-commands.md) explains
102 programs and 366 operations matching Java in both builds, with actual Main
calls and queued rescue messages. That checkpoint had 193 CTest entries.

Main now compiles and binds the existing native runtime components. Its
[runtime lab](student-guide/new-tests/main-runtime.md) explains lifecycle ordering,
notifications, backup SQL and failed-startup ownership. All 336 programs and 897
operations match Java in both builds (8.45s/5.11s); native ownership checks pass
(0.69s/1.70s). Full startup, durable SQL/MySQL drivers, complete command registration
and the Minima entry point remain unfinished. That checkpoint had 192 CTest entries.

Pulse.createPulse and instance decoding now connect to the actual database/tree
components. Retained pulse lists survive tree recalculation and partial reads.
The [runtime lab](student-guide/new-tests/main-runtime.md) explains the 152 programs
and 946 operations matching Java in both builds (10.18s/25.01s). That prerequisite checkpoint
had 190 CTest entries.

MySQLConnect now compiles. Its [connection lab](student-guide/new-tests/mysql-connection.md)
explains SQL ordering, partial failures, real wire codecs and locale-sensitive
numbers. All 1,598 programs, 5,399 operations and 60,287 SQL observations match
Java in normal and sanitizer builds (40.98s/110.76s). A native MySQL driver and
live persistence remain open. That checkpoint had 189 CTest entries.

seedrandom now compiles and is registered. Its [seed derivation lab](student-guide/new-tests/seed-random-command.md)
explains exact framed hashing, Unicode and lock-check order. All 309 programs,
782 operations and 14,616 SQL observations match Java in both builds
(12.30s/5.50s), including independent SHA3-256 expectations. All four selected
regressions pass (23.31s/75.25s).

Four [original casting test counterparts](student-guide/new-tests/cast-original-tests.md)
now compile: BOOLTests, HEXTests, NUMBERTests and SCRIPTTests. All 87 original
assertions per runtime pass; the existing cast implementations were reused.
Both full builds and all ten final casting/harness checks pass (11.35s/11.12s).
That checkpoint had 188 CTest entries. All 457 student lessons are authored; full-file
behavioral parity and the complete native node remain unfinished.

mempool, cointrack and magic now compile. The [node-state lab](student-guide/new-tests/node-state-commands.md)
explains their behavior and four comparison findings. mempool and cointrack are
registered; magic stays unregistered like the pinned source. All 363 programs,
1,411 operations and 21,009 SQL observations match Java in normal and ASAN/UBSAN
builds (20.23s/35.38s). Both full builds and all 13 selected regressions pass
(64.09s/145.86s). That checkpoint had 179 CTest entries. All 457 student lessons remain
authored; teaching coverage does not certify completed port behavior.

txpow now compiles and is registered. Its [search lab](student-guide/new-tests/txpow-search.md)
explains query precedence, RAM/SQL/archive fallbacks, on-chain confirmations,
stable deduplication and MySQL delegation. All 680 programs, 2,099 operations and
41,510 SQL observations match Java in both builds (10.28s/26.97s). Both full builds
and all 12 selected checks pass (58.91s normal, 156.18s sanitizer). That checkpoint had 178
CTest entries. Native MySQL, durable SQL and the complete node remain unfinished.

history now compiles and is registered. Its [transaction-history lab](student-guide/new-tests/transaction-history.md)
explains SQL action selection, wallet-relative token totals, Java collection order,
chain counts and real-clock validation. All 484 programs, 1,489 operations and
52,194 SQL observations match Java in normal and ASAN/UBSAN builds (5.46s/10.04s).
Both full builds pass without warnings. All ten selected checks pass across the
validation phases; the sanitized SQL group required a separate run after the
concurrent batch hit its 120-second limit. That checkpoint had 177 CTest entries; the full node
and durable native SQL remain unfinished.

coins now compiles and is registered. Its [coin-search lab](student-guide/new-tests/coin-search-command.md)
explains relevance by parameter presence, age/depth, state matching, miner and
mempool filtering, and reused amount selection. All 287 programs, 1,144 operations
and 25,168 SQL observations match Java in both builds (20.89s/41.27s). That checkpoint contained
176 CTest entries. Both full builds and all nine selected checks pass
(61.55s normal, 193.68s sanitizer); the complete node remains unfinished.

balance now compiles and is registered. Its [balance lab](student-guide/new-tests/wallet-balance.md)
explains confirmation depth, sendable classification, exact token filters, MegaMMR,
raw token names and simple-view parsing. All 275 programs, 776 operations and
21,515 SQL observations match Java in normal and ASAN/UBSAN builds. The tests also
corrected the shared arithmetic constructor's overflow-message representation;
24 direct probes distinguish it from the String constructor. Both full builds
and all 13 distinct selected checks pass across the validation phases. The
regression batches took 60.64s normal and 193.38s sanitizer; balance and primitive
checks also ran separately. That checkpoint contained 175 CTest entries.

consolidate now compiles and is registered. Its [wallet-consolidation lab](student-guide/new-tests/wallet-consolidation.md)
explains address grouping, age and signature limits, forwarded send parameters,
password redaction and child-reply selection. All 247 programs, 742 operations
and 23,185 SQL observations match Java in both builds. A shared runner fix now
preserves Java's missing/wrong-type status exceptions. Both full builds and all
eight selected checks pass (58.53s normal, 191.87s ASAN/UBSAN); that checkpoint contained 174 CTest entries.

consolidatefrom now compiles and is registered, including its separate public
createConsolidate helper. The [consolidation lab](student-guide/new-tests/consolidate-custom-address.md)
follows real coin search, maturity and mempool filtering, stable selection order,
burn limits, one-versus-ten outputs, nullable helper inputs and cleanup. All 331
programs and 3,815 observed child calls match Java in normal and ASAN/UBSAN builds.
Both full builds and all eight selected checks pass (22.25s normal, 65.92s
ASAN/UBSAN). That checkpoint contained 173 CTest entries.

sendfrom and createtokenfrom now compile and are registered. Their
[custom-address lab](student-guide/new-tests/custom-address-send-commands.md)
compares funding/posting error order, token output replacement, preserved change,
raw decimal parsing and fixed-precision supply. All 492 programs and 2,052 child
calls match Java in normal and ASAN/UBSAN builds. MiniNumber now exposes MINI_UNIT
and gives Java's exact arithmetic exception for out-of-range powers. Both full
builds and all ten selected checks pass (40.23s/124.63s); these include 99 original
number assertions plus eleven supplemental error observations per runtime.
That checkpoint had 172 CTest entries.

rawfrom and rawtxnfrom now compile and are registered. Their [raw-transaction lab](student-guide/new-tests/raw-transaction-commands.md)
compares distinct input/script orders, typed JSON fields, state conversion and
real exported row bytes. The final 656-program comparison passes in both builds,
with 3,991 child calls each. StateVariable now handles Java’s Unicode Boolean
case comparison; the final object regression passes 9,464 cases per build.
Both full builds pass. Eight distinct selected checks pass across the extraction
and focused state-fix phases. That checkpoint had 171 CTest entries.

Four wallet wrappers now compile and are registered: createfrom, constructfrom,
postfrom and signfrom. Their [orchestration lab](student-guide/new-tests/wallet-command-orchestration.md)
explains exact child-command order, cleanup, ignored failures, nullable errors,
exported transaction bytes and decoded signature verification. The expanded
comparison passes 659 programs and 2,060 observed child calls in normal and
ASAN/UBSAN builds. Both full builds and all five selected checks per build pass.
That checkpoint had 170 CTest entries.

The sendpoll command now compiles and is registered. Its [polling lab](student-guide/new-tests/send-polling-command.md)
compares command rewriting, queue snapshots/removal and actual worker processing
through CommandRunner. Both builds pass 367 programs and 3,216 operations. Both
full builds and all six selected checks per build pass. That checkpoint had 169 CTest
entries. The fixture records timers; full native Main ownership remains absent.

The sendnosign command now compiles and is registered. Its [unsigned-file lab](student-guide/new-tests/unsigned-transaction-files.md)
follows confirmed coin selection, fractional splits, proofs, unsigned burns and
actual create–view–sign–post sequences. The final 309 programs pass in normal and ASAN/UBSAN builds, comparing 1,248
operations and 20,081 recorded SQL calls each. All seven selected checks pass per
build after correcting one independent zero-total test expectation.
That checkpoint had 168 CTest entries.

The sendview, sendsign and sendpost commands compile and are registered. Their
[offline-file lab](student-guide/new-tests/offline-send-files.md) compares complete
TxPoW bytes, signature verification, SQL effects, reply snapshots, queued mining
and exact filesystem errors. MiniFile now preserves the tested Java file-open and
file-create exception categories and order. The sendsign demonstration main also
prints the source's 512 values. Normal and ASAN/UBSAN comparisons pass 201 programs each, with 558 operations
and 9,773 recorded SQL calls. All twelve selected checks pass in both builds. That checkpoint had 167 CTest entries.

The send, txnaddamount and txncoinlock commands now compile and are registered.
Their [coin-funding lab](student-guide/new-tests/coin-selection-and-funding.md)
compares selector order, locks, partial failures, real proofs and signatures,
custom-token burns, dry runs and miner submission. Both builds pass 736 programs, 3,418 operations and 39,016 recording SQL calls.
All twelve selected checks pass in both builds, including shared-component
regressions and the original tests below. That checkpoint had 166 CTest entries.

The original [REV/LEN tests](student-guide/new-tests/reverse-and-length.md) now
have compiled counterparts and pass in both builds, twelve assertions each per
runtime. Their production functions are unchanged.

vault and txnsign now compile and are registered. The [vault/signing lab](student-guide/new-tests/vault-and-signing.md)
explains encrypted seed transitions, real signatures and side effects before
failures. Normal and ASAN/UBSAN comparisons both pass 247 programs.
The original [BITGET/BITSET tests](student-guide/new-tests/bit-get-set.md) now pass
in both builds with 272/271 assertions per runtime. At that earlier checkpoint the
CTest inventory contained 161 entries; not all entries were rerun here.

The keys command also compiles and is registered. Its [wallet-key lab](student-guide/new-tests/wallet-keys.md)
distinguishes seed derivation checks from public/private matching and follows
filter precedence and background key-creation requests. Its corrected comparisons pass 132 programs in both builds.
Those prior 24 selected checks passed in normal and sanitizer builds.

The [student guide](student-guide/how-to-study-this-port.md) explains how to read
the source, translation and test evidence. Per-file lessons for all 457 sources
now have an authored first teaching pass, organized by package. The
[checker rejection lab](student-guide/new-tests/checker-rejections.md) records
the initial mismatches, corrections and verification limits.

Three chain-inspection commands now compile and are registered: block, printmmr
and printtree. Their [student lab](student-guide/new-tests/chain-commands.md)
records 324 matching Java/C++ programs and 4,096 operations across four time zones
in both builds. Tests found and removed an older TxHeader date formatter by reusing
the existing Java-compatible helper. The renderer also preserves signed integer
wraparound in depth subtraction; extreme populated-tree traversals remain untested.
Both full builds and all ten selected checks pass in normal and sanitizer builds.

Basic/simple/script/MMR/signature rejection paths now use the actual logger,
including failures during logging. Transaction validation preserves token table
traversal; empty bodies, tokens and signature lists produce source exception
categories on the tested paths. The expanded comparison passes 1,343 requests
from 100 Java fixtures, and the timed failure suite passes 105 cases. Fourteen
affected suites pass in both normal and ASAN/UBSAN builds. Full file parity and
full node integration remain incomplete.

CommandRunner now uses the actual logger for generic command failures and lets
Java-style Error categories escape. Its new [error-path lab](student-guide/new-tests/command-runner-errors.md)
records 450 initial mismatches, corrected to zero; the expanded 1,120 cases now pass in both builds.
All nine affected selected suites pass; the HTTP handler required a rerun with
local socket access after the sandbox rejected binding.

Four wallet commands now compile and are registered: scripts, newscript,
removescript and sign. Their [student lab](student-guide/new-tests/wallet-commands.md)
records 675 passing Java/C++ programs with 1,907 operations and 50,275 SQL calls in
both builds. Testing corrected SQL exception labels, cleaning logs and shared
parameter type errors. The SQL receiver records requests; native H2 persistence
and full node startup remain unfinished.

Four more commands now compile and are registered: newaddress, getaddress,
coinnotify and slavenode. Their [student lab](student-guide/new-tests/address-commands.md)
records 134 passing Java/C++ programs, 276 operations and 3,693 SQL calls in both
builds. The fixture uses real default TreeKey creation and temporary preference
files; all six selected regressions pass. SQL durability, distinct-row random
selection and a complete node restart remain unverified.

txnmmr and txnscript now compile and are registered. Their [witness-command lab](student-guide/new-tests/witness-commands.md)
records 418 passing Java/C++ programs, 1,128 operations and 8,637 SQL calls in both
builds. Tests compare proof/script insertion, partial failure state and full custom
transaction bytes. A shared JSONObject parameter cast now preserves ClassCastException.
All nine selected suites pass. A subsequent [coin ownership lab](student-guide/new-tests/coin-ownership.md)
corrects shared Coin mutations and adds 220 programs; both object suites now pass
9,464 cases. The later reader work below adds outer MMRProof ownership and an
owned state-list bridge; nested identity and full node ownership remain incomplete.

Coin now uses shared field storage for ordinary handle copies. The existing codecs,
JSON, hash calculation and explicit deepCopy algorithm are retained. All 220 new
ownership cases failed before the correction and now pass in both builds. The
21 selected normal regressions pass; the sanitizer run passes 20 initially and
its timed-out block-ownership check passes an isolated rerun (141.61s). See the
[ownership lab](student-guide/new-tests/coin-ownership.md) for the measured limits.

Coin, CoinProof and MMRProof now have in-place readers preserving partial changes.
Their [reader lab](student-guide/new-tests/coin-streams.md) records 3,045 passing
Java/C++ programs per build, including retained objects, consumed bytes, actual
logging and null copy/search results. Both final full builds and all 29 selected
checks pass in each build, after correcting one normal search-harness logging
protocol failure. Nested object identity remains incomplete.

The registered automine command now preserves its pinned active behavior: read
a parameter and return the standard reply, with all mining branches left inactive.
Its 32 added direct/runner programs pass in both builds (1,265 runner cases total).

MiniStringTests now has its mapped counterpart. Its lesson separates the two
annotated source methods (five successful-path non-null assertions) from the
unannotated helper (one supplemental assertion). Production code is reused unchanged.

The missing ScriptValueTests and HexValueTests counterparts now compile. Their
unchanged Java methods and native assertions pass in both builds: 10 script-value
and 35 hex-value assertions. HexValue numeric constructors now preserve the source
NumberFormatException category. All eight affected value/VM/object suites pass
normal and ASAN/UBSAN; their student lessons explain the tests and limitations.

NumberValueTests now preserves 99 active source assertions, including exact
ArithmeticException checks. Zero divided by zero now reports Division undefined;
nonzero division by zero reports Division by zero. Seven additional error
observations match Java exactly. All 16 affected normal and ASAN/UBSAN suites pass.
DUMMYFUNCTION, MinimaFunctionTests and ABSTests now compile. The original Java
function tests and their native equivalents pass: 11 MinimaFunction assertions
and 23 ABS assertions. Nine selected suites pass in both builds. DUMMYFUNCTION
is a helper linked into the test, not a registered production function.
FLOORTests and CEILTests preserve 30 and 26 original assertions respectively;
all ten selected original-test/mapping suites pass in both builds. Production
rounding code was reused unchanged. MINTests and MAXTests now each retain 120,023
assertions and all 60,000 boundary trials. Their unchanged Java methods and native
tests pass in both builds, along with all twelve selected suites. Native random
samples are reproducible and distinct from Java’s unseeded samples; production
MIN/MAX code was reused unchanged.

MinimaUncaughtException now compiles with the source duplicate suppression,
shared captured default handler, logging order and immediate fatal shutdown.
Native message and timer threads dispatch escaping failures through a default
handler registry. Message processing preserves Java catch/finally order, including
cleanup after a logger failure. The native MVStore exception type is only an
exception carrier; it does not implement H2. Other worker launch sites and full
Main startup still need integration. 121 Java/C++ handler programs, six fatal
process checks and 13 worker scenarios pass. All 28 affected regression suites pass
in normal and ASAN/UBSAN builds; validation limits are recorded in REVIEW.md.

HTTP listener/request threads and the fixed task executor now use the same
native uncaught-handler boundary. HTTPServer preserves the source distinction between socket and other I/O
errors, and executor failures start replacement workers before handler
dispatch. Thirty HTTP failure scenarios and ten executor scenarios pass against
Java. Thirteen affected normal suites and sixteen sanitizer suites pass. NIO
server/connect threads and the timed checker now use this integration as described
below.

NIOManager server/connect workers now dispatch escaping failures through
NativeThread. NIOServer preserves Exception-versus-Error catch boundaries and
uses the actual logger, including trace notification flags; NIOClient cancellation
also preserves Error propagation. Sixty-three server/client failure comparisons,
twelve actual connector comparisons and four native server factory checks pass.
Ten affected normal suites and thirteen ASAN/UBSAN suites pass. This does
not establish full node startup, complete socket diagnostics or all NIO APIs.

TimedChecker now dispatches escaping worker failures through NativeThread while
preserving the Java finished/valid flags on reuse. Block validation uses the source
catch-log order and visits every transaction before checking duplicate coin IDs.
Ninety-one targeted comparisons and the actual two-minute worker-error deadline
pass; eleven affected normal suites and thirteen sanitizer suites pass. The
final release-signal fixture also passes in both builds. Java interruption
semantics, remaining checker APIs and complete node integration remain incomplete.

NotifyManager and the `webhooks` command now compile. Their 527 Java/C++ programs
cover 2,667 operations in normal and ASAN/UBSAN builds: original JSON identity,
listener mutation, hook persistence/filtering, command dispatch and error order.
A native worker test covers 400 concurrent events, hook updates and shutdown
ownership. HTTP/listener/logger receivers are controlled in these comparisons;
The production defaults now call the native RPC client and logger; Main integration remains incomplete.

RPCClient now compiles with GET/PUT/POST, Basic authentication and HTTPS.
477 Java/C++ loopback cases compare 597 requests per runtime, including redirects,
status errors, UTF-8, connection failures and selected TLS trust behavior. The
notification worker delivers seven real HTTP requests from two queued events and
logs both expected failures. Ten relevant suites pass in normal and ASAN/UBSAN
builds. This remains a partial client: complete JDK URL, TLS/provider, mutable
charset, binary-header and process-global network behavior is not established.

MinimaLogger now compiles and passes 2,016 Java/C++ cases across four time zones,
including signed-long timestamp extremes, UTF-16 text, notification order, callback
failures and explicit exception-frame sequences. Native worker checks cover 400
concurrent print/notification pairs, runtime receiver retirement and queued
listener-error logging. Its default memory prefix measures the native allocator;
full mutable date-formatter APIs remain incomplete. Native construction-time
frame capture now covers the shared typed exceptions, I/O errors and JSON parser. Seven related suites pass in normal and ASAN/UBSAN builds.

A native RPC command-line executable now builds as `cpp/build/minima-rpc`.
It translates the pinned MinimaRPCClient argument handling, interactive input,
UTF-8 form encoding, Basic authentication, JSON display and error paths. 89 Java/C++
cases compare 70 HTTP requests per runtime; ten executable checks include pipe
input before EOF and terminal Ctrl-D recovery. All pass normal and ASAN/UBSAN. Java is
used only by test oracles; the executable links the native implementation.
For an existing local node, run:

```sh
cpp/build/minima-rpc -host http://127.0.0.1:9005 -command status
```

This executable is an RPC client, not the still-unfinished native node. Its
`-sslpubkey` setting follows the pinned source, whose custom SSL setup is commented
out. Construction-time native exception stacks retain their original frames
through copying and rethrows; their symbols and frame counts differ from the JVM.
Standard exceptions outside the captured typed hierarchy still need integration.

Startup configuration now has native `ParamConfigurer` and `TestParams`
counterparts, including all 67 active parameter handlers. Arguments, environment
entries and UTF-8 configuration files feed the actual global settings, P2P log
level, directory creation and native peer-list HTTP requests. 719 Java/C++ programs
and eighteen affected regression suites pass in normal and ASAN/UBSAN builds.
The five original parameter tests have a mapped C++ test. Shared fixes add Unicode decimal digits,
64-bit argument bounds, bulk-map sizing, canonical directory creation and strict
buffered UTF-8 decoding. Full Main startup and persistence are still incomplete.

The compiled subset includes base values, JSON serialization/parsing, address/base
conversion, hashing, MMR data/entry/proof objects, state variables, tokens,
coins, script/coin proofs, transactions, signatures, Winternitz and witnesses.
Also implemented are MMR trees, hierarchical TreeKey signatures, global/general
parameters, Magic adjustment, TxBody, TxHeader, TxPoW, TxBlock construction and
MegaMMR block application/pruning/wire snapshots. File persistence is now connected for MegaMMR, Cascade and TxPowTree;
node database integration remains outstanding. Genesis coin/MMR/TxPoW construction
and greeting/pulse wire objects are included. All 84 KISS production classes now have compiled counterparts: values, tokens,
exceptions, expressions, parsers, statements, Contract, and all 53 built-ins.
Their execution is under differential review; full API and edge-case parity
is not established.
Cascade pruning and chain-tree GHOST selection also compile, with 2,892 cascade
and 1,027 tree Java comparisons passing. Wallet/SQL/message service connections
remain unwired in the standalone library. MiniFile, MiniFormat and five encryption
classes pass 1,361 interoperability comparisons in normal and sanitized builds.
Consensus validation and generation pass 2,890 Java comparisons in normal and sanitized builds.
RAM database behavior passes 315 comparisons; preferences pass 1,130. Six message/timer
classes pass 905 message comparisons, 135 HashMap programs, and concurrent worker
checks, also under sanitizers. Custom transaction storage passes 2,503 comparisons in normal and sanitized builds.
Archive cache/list behavior passes 734 and IBD selection/weight/range behavior passes
1,057 comparisons, also under sanitizers. MegaMMR backup/sync wrappers pass 3600
comparisons in normal and sanitized builds. Send-poll concurrency, retry, removal and
worker/timer checks also pass both builds. RawArchiveInput streams gzip into 256-block
batches and passes 613 comparisons in both builds. Wallet row constructors and JSON pass
1,710 comparisons in both builds. Pinned BIP39 word/seed behavior passes 8,736,
FastByteArrayStream passes 190 operation programs, and RPC authorization/Base64/traffic
accounting passes 5,636 comparisons, all under sanitizers as well. The command base, conversion/hash/random commands, MMR commands, maths/runscript and
signature verification, debugflag and txnlock pass 12,289 comparisons in normal and sanitized builds.
The dispatcher passes 1,233 comparisons. Nine transaction commands pass 1,129
sequences covering 6,904 operations, also under sanitizers. TxPoWSearcher and token/coin-proof commands pass 189
programs containing 6,468 queries in both builds. Maths, Stack and TimedRequest pass 3,809
programs, including raw floating-point results and stateful request-list checks.
Peer address/swap/walk behavior passes 1,447 programs; peer state, greetings and
P2PDB pass 798 programs containing 26,118 operations, in both builds. The three
mapped peer tests include exact greeting JSON and a 1,000-peer persistence round trip.
NIOClient/NIOClientInfo pass 714 comparisons covering 13,972 operations plus
UTC/London/New York/Tokyo date cases. Real loopback checks cover a 1 MiB payload,
backpressure, selector interests, traffic and EOF in normal and sanitized builds.
NIOServer now has a native dual-stack event loop, with Java/C++ lifecycle and startup
failure checks. P2P2DB passes 211 programs covering 5,145 operations, including
shared-list ownership, exact persistence bytes and partial failures; both new suites
pass normal and ASAN/UBSAN builds. NIOManager now has a compiled counterpart;
Main and full MinimaDB runtime integration
remain outstanding. Greeting creation and the connect command now pass 532 Java/C++
programs (including 1,764 greeting calls) in both builds, covering lock/failure order,
repeated chain append, peer fields, parsing and command dispatch.
NIOManager transport passes 640 Java/C++ serialization/send cases and 17 real
loopback ping cases in both builds. A native integration fixture exercises the
connect command, outgoing greeting, 32 incoming framed dispatches through four
workers, disconnect cleanup, socket shutdown and manager lifetime release.
Executor checks cover queued FIFO, failure replacement and shutdown draining.
Manager state transitions now pass 1,449 Java/C++ programs containing 3,879
operations in both builds, covering reconnect modes, disconnect policy, timers,
sync requests, stale reads, queries and receiver failures. Full runtime and API
parity remain outstanding.
NIOMessage now has a compiled dispatcher covering greeting negotiation, transaction
validation and relay, pulse synchronization, peer messages, archive requests and
block-node messages. Its offline oracle executes the actual pinned Java dispatcher;
1,918 programs / 4,677 packets pass normal and ASAN/UBSAN builds. A real TCP fixture also routes transaction-ID and single-ping packets
through NIOMessage in normal and sanitized builds. Full Main/P2P/database/processor/
miner bindings, MegaMMR sync service binding and log/notification parity remain pending.
Headers supplement the 1:1 `.cpp` destinations. Extra test/tool sources under
P2PFunctions and UtilFuncs now compile and pass 672 Java/C++ programs covering
3,670 operations in normal and sanitized builds. Tests include connection policy,
peer lookup, message queues, local-interface/cache behavior and seeded selection.
Another 39 local HTTP cases compare results and request paths, including dropped
responses and proxy redirects. Default HTTPS/TLS settings and complete logger/node
runtime parity remain under review.

SwapLinksFunctions also compiles. Its 651 Java/C++ programs cover 1,959
operations in normal and ASAN/UBSAN builds: greetings, duplicate incoming connections, IP discovery, shared JSON,
link removal and load-balancing walks. Complete Main/runtime bindings remain
outstanding.

WalkLinksFuncs now passes 883 programs covering 2,153 operations in normal and
ASAN/UBSAN builds. The suite checks routing cutoffs, returning paths, duplicate/null
addresses, actual connection requests, swap selection and signed client-slot counts.
Complete peer-runtime integration remains outstanding.

P2PPeersChecker now passes 339 Java/C++ programs with 758 operations and 329 real
loopback pings in normal and ASAN/UBSAN builds. These compare request bytes, traffic,
chain checks, peer sets, parent messages and timer requests. Separate native worker
startup/shutdown tests pass; full parent/runtime and timer dispatch remain pending.

P2PTestParams now copies test settings into P2PParams with shared peer-list identity,
fixed-list behavior and nullable log levels. The expanded peer-state comparison
passes 874 programs / 26,348 operations in normal and ASAN/UBSAN builds.

P2PManager now compiles with all pinned method bodies translated. Native state,
persistence, routing, test-mode initialization, actual timer dispatch and manager/
checker shutdown tests pass normally and under ASAN/UBSAN. Manager Java comparisons
pass 405 programs / 2,915 operations, including 46 emitted packets. Discovery adds
31 programs / 84 operations / 39 real TCP pings in both builds, including actual
five-second retries. State references/scalars are synchronized; concurrent status
reads pass normal, ASAN/UBSAN and ThreadSanitizer tests. Complete node bindings,
arbitrary global configuration mutation and full lifecycle integration remain open.

P2P2Functions and P2P2Manager now compile and pass 837 Java/C++ programs with
3,374 operations in both builds. Actual worker initialization, timer dispatch and
shutdown ownership pass separately. The pinned P2P2 manager itself contains inactive
connection-selection/slow-loop branches; these remain inactive in the port.

RPC Server/HTTPServer now compile. Java/C++ TCP comparisons pass automatic/manual
startup, concurrent binary handlers, bind collision and shutdown in both builds.
The selector wakeup helper is shared with NIOServer; existing NIO listener and
manager loopback regressions pass. CMDHandler also compiles and passes 2,106 actual TCP requests across six RPC/auth
configurations in both builds. Tests compare response bytes, URL decoding, POST
character counts, malformed/truncated input, header rules, authorization and command
dispatch; only the wall-clock Date value is normalized. Full Main/database integration,
quit-command shutdown and complete logger/exception parity remain outstanding.

NetworkManager now compiles and owns the native NIO/P2P/P2P2/RPC lifecycle. Eight
actual worker combinations pass RPC commands/restart, NIO ping dispatch, status,
traffic, normal/hard shutdown and owner release in both builds. Host selection and
network status pass 540 Java/C++ programs in both builds, including signed traffic
rates and database receiver failures. Full Main/database binding, all lifecycle
error paths and arbitrary concurrent configuration changes remain outstanding.

SqlDB now compiles with its pinned lifecycle, encrypted-reopen URL and credentials,
backup/restore statement ordering, generic result conversion and JSON/data helpers.
464 Java/C++ programs cover 1,260 operations and 8,352 recording-driver calls in
normal and ASAN/UBSAN builds. A separate native test verifies 1,024 serialized
concurrent calls and two reentrant schema calls. The native H2-compatible connection
provider is still required and is **not implemented**; these tests do not execute
SQL storage, H2 encryption or backup scripts. Other SQL database subclasses and
full Main database integration remain outstanding.

TxPoWOnChainDB now compiles with the pinned schema, prepared statements, block
membership queries, first/last records, count and cleanup. 855 Java/C++ programs
cover 3,639 operations and 29,167 recording-driver calls in normal and ASAN/UBSAN
builds. These checks also found and fixed MiniNumber's range-exception type/message,
reusing the existing BigDecimal formatter. Eight selected primitive, VM, cascade,
command and SQL suites pass both builds. Native H2 persistence remains absent.

TxPoWSqlDB now compiles with the pinned transaction/block schema, queries, insert,
cleanup and reconnection behavior. 2,592 Java/C++ programs cover 8,729 operations
and 86,333 recording-driver calls in normal and ASAN/UBSAN builds, including actual
serialized TxPoWs, malformed data and failure-side parameter binding. The checks
also correct TxPoW conversion's I/O-only catch and primitive exception details.
Twelve selected object, archive, network and SQL regressions pass both builds.
These comparisons do not supply native H2 persistence or complete node integration.

TxPoWDB now composes the actual RAM and SQL counterparts, preserving lookup order,
duplicate handling, relevance checks and queued message references. 1,355 Java/C++
programs cover 7,407 operations and 80,960 effects in both builds. Ten selected
regression suites pass normally and under ASAN/UBSAN. Main and wallet callbacks
remain required, and native H2 storage and full node integration are unfinished.

CoinDB now compiles with the pinned tree-node cache schema, constructor file
deletion, static replacement, queries and compact reopen. 1,350 Java/C++ programs
cover 4,318 operations and 28,803 driver effects in both builds. Eight selected
object, mapping and SQL suites pass normally and under ASAN/UBSAN. Coin decoding
now preserves Java’s I/O-only catch. Native SQL storage and node coin-cache binding
remain unfinished.

Wallet now compiles with seed management, script caches, deterministic TreeKey
creation, signing and SQL key-use update ordering. 1,433 Java/C++ programs cover
6,521 operations and 85,452 driver effects, including real signature bytes and
failed key inserts. Eight selected suites pass normally and under ASAN/UBSAN.
Native durable wallet storage, runtime ownership and complete concurrency/API
parity are still unfinished.

ArchiveManager now compiles with the pinned SQL statements, block-save retry,
range sorting and shutdown checks, cascade validation/replacement and cleanup.
Its 3,333 Java/native programs compare 10,831 operations and 125,655 recorded SQL
effects. Nine selected regressions pass normally and under ASAN/UBSAN. Native H2 storage and
full runtime integration remain unfinished.

MinimaDB now compiles and coordinates the existing databases, state files,
recovery/restart and shutdown. Its Java comparison covers 2,367 programs,
7,519 operations and 334,711 recorded SQL calls. Seven selected suites pass normally and under ASAN/UBSAN,
including concurrent read/write lock checks and transaction/search regressions.
The command, MegaMMR search and global MMR fallback bindings retain shared ownership
during use. Restoration checks include nonempty chain/cascade objects, truncated
state files, file sizes and held MMR objects across reset/load. A separate ownership
test exercises 7,000 lookups during 1,000 database component replacements.
Native H2 storage, remaining runtime bindings, full API/edge-case parity and the
working node are still unfinished.

TxPoWMiner now compiles with native SHA3 mining, source nonce-byte patching,
coin reservations, Main notifications, automine timers and hash-rate routines.
357 Java/C++ programs compare 1,352 operations in normal and ASAN/UBSAN builds.
Native worker checks pass both builds and exercise
actual mining, cancellation, retained ownership and callback-failure recovery.
Full Main integration, concurrent access to shared mutable TxPoWs and remaining
API/error cases still need completion.

TxBlock and the synchronous/timed checker now retain shared TxPoW objects.
In-place TxBlock restoration, aliases, repeated transaction references and validation
cache mutations pass 1,558 Java/C++ programs (6,219 operations). Ten selected suites,
including two real 120-second deadline/ownership checks, pass normal (264.38s) and
ASAN/UBSAN (589.95s). Collection aliases and complete concurrency semantics remain
unfinished. TxPoWProcessor now compiles and passes 384 Java/C++ programs (1,434 operations)
in both builds. Its worker tests cover failure recovery, original block ownership,
shutdown during active IBD and 24 strict clock boundaries. Database indexes,
archive writes and Main/network receivers are controlled in these fixtures;
complete native H2 and full node integration remain outstanding.

`support/` and `tests/` are infrastructure, not claimed Java counterparts.

## Build and test this port

From the Java repository root:

```sh
cmake -S cpp -B cpp/build -DCMAKE_BUILD_TYPE=Debug -DICU_ROOT=/opt/homebrew/opt/icu4c
cmake --build cpp/build -j 4
ctest --test-dir cpp/build --output-on-failure
```

Outputs:

- `cpp/build/libminima_file_port.a`: native port library.
- `cpp/build/minima_commands`: native command console with 24 command counterparts; connect requires the node runtime binding.
  Custom transactions live in memory for the console session; `txnexport` saves
  files or HEX for a later `txnimport`. `checkaddress` still requires live wallet
  callbacks; this console does not open a wallet or start a node.
- `cpp/build/minima_primitives`: offline operation/serialization probe.
- `cpp/build/minima_objects`: offline object/crypto probe.
- `cpp/build/minima_json`: offline JSON lexer/DOM/event-parser probe.
- `cpp/build/minima_kiss_values`: offline KISS value/token probe.
- `cpp/build/minima_kissvm`: offline KISS expression/contract execution probe.
- `cpp/build/*differential-results.json`: Java/C++ fixture results.

Building requires a C++20 compiler, CMake, Python 3, OpenSSL 3.2+, libcurl 7.85+, zlib and ICU 78+ (uc/i18n). Set ICU_ROOT to your installation. Differential tests
also require JDK 21+ (tested with OpenJDK 26.0.1) and the pinned upstream checkout/JAR. Consensus, RAM, preferences, message and custom-transaction tests freshly compile all 369 upstream production Java files.
Other tests freshly compile the classes under comparison; the committed
JAR supplies remaining Java dependencies (including Bouncy Castle). Native binaries do not use Java.

```sh
printf 'number 255\ndiv 100 4\nless 0x0001 0x02\n' | cpp/build/minima_primitives
cpp/build/minima_commands 'maths calculate:"1+2 * (3/4)"'
cpp/build/minima_commands 'txncreate id:example;txnstate id:example port:7 value:[offline];txnexport id:example'
python3 cpp/tools/source_map.py --check
cmake --build cpp/build --target check_full_port
```

`check_full_port` deliberately fails while implementations/parity are missing.
Current status records no files as fully parity-verified. Do not change that
based solely on compilation or a filename match. Regenerate mapping metadata
after reviewed source changes with `python3 cpp/tools/source_map.py`.

## Existing C++ node baseline

The pre-existing implementation remains at
`/Users/eurobuddha/Projects/minima/tools/minima-core-cpp`. Its local edits were
preserved. It was compiled separately, using its own CMake project:

```sh
cmake -S /Users/eurobuddha/Projects/minima/tools/minima-core-cpp \
  -B cpp-baseline-build -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug \
  -DMINIMA_BUILD_TESTS=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build cpp-baseline-build -j 6
ctest --test-dir cpp-baseline-build --output-on-failure --timeout 60
cpp-baseline-build/minima_node --help
```

`cpp-baseline-build/minima_node` is a compiled arm64 macOS node executable,
but it is **not the completed 1:1 port of this upstream**. All 25 baseline
CTest suites passed after rerunning the three loopback networking suites
outside the socket-restricted sandbox. Passing these tests does not resolve
the consensus discrepancies recorded in [REVIEW.md](REVIEW.md).

## Reuse and remaining work

[provenance.json](provenance.json) records exact copied source paths and
input hashes. MiniByte is adapted from the existing header implementation.
The original C++ license is preserved in [REUSED_CODE_LICENSE](REUSED_CODE_LICENSE).
The nullable-number sort adapts OpenJDK TimSort run scheduling and insertion;
its original notices remain in the header and [license](OPENJDK_TIMSORT_LICENSE).
The streaming ZIP and Unix canonicalization adapters also retain OpenJDK notices
and use this GPLv2-with-Classpath-exception license. Their source hashes and exact
upstream paths are recorded in provenance.json.
Other candidate paths in the map are name/role search matches, not claims
that the files have been reviewed or proven compatible.

The current primitive suite compares 3,802 cases, including malformed UTF-8,
decimal wire scales, powers, square roots, and signed integer conversions.
The object suite adds Java-produced fixtures, binary round trips, truncations,
checksums, transaction checks, MMR branches, and complete Winternitz keys and
signatures. Exact current totals and failures are written in the build reports.
Mapped native tests include MiniByteTests, TransactionTests, TxHeaderTests,
ValueTests, BooleanValueTests, LexicalTokenizerTests, four expression test classes,
ContractTests, StatementBlockTests, ASSERTstatementTests, EXECstatementTests,
InetSocketAddressIOTest, GreetingTest and P2PDBTest. JSON comparisons
cover 5,545 cases, including UTF-16 escapes, error positions and stoppable
SAX-style parsing. KISS value/token comparisons cover 1,451 cases. The executing VM suite currently compares 4,471 cases across all 53 built-ins.
It checks outcomes, KISS errors,
instruction counts, stack depth, variables, monotonicity and execution traces.
Generic Java/native runtime diagnostics are normalized in this suite and
remain an explicit parity gap; they are not silently classified as equivalent.

Bouncy Castle's GMSS source was inspected because the sibling C++ WOTS
implementation uses incompatible parameters and seed derivation. Its relevant
algorithm is adapted in Winternitz.cpp, with source hashes and license recorded
in [provenance.json](provenance.json) and [BOUNCY_CASTLE_LICENSE](BOUNCY_CASTLE_LICENSE).

Remaining work includes complete primitive/API equivalence, all protocol
objects and cryptography, KISS API/edge-case parity, MMR/tree/consensus,
persistence, networking/synchronization, lifecycle, every terminal command,
and the remaining Java tests. Work should continue against the pinned Java
sources, reusing candidates only after checking their implementations and
running cross-language fixtures. No production node or wallet was started. Socket tests use local loopback only.

The transaction-input work adds native txninput, txnbasics and all txnutils method
bodies, including linked burn construction. Existing search, MMR, wallet, signing
and ID code is reused. The [student lab](student-guide/new-tests/transaction-inputs.md)
explains state changes before errors and the shared MiniNumber syntax fix. The
Main miner lookup remains an explicit integration boundary; native startup and
durable H2 storage remain unfinished. The new suite passes 671 programs, 2,734
operations and 41,653 recorded SQL calls in both builds; all 13 selected
regressions also pass in both builds.

The transaction-check work adds the actual txncheck command and preserves shared
transaction/witness owners through TxBody, TxPoW and the generator. The
[student lab](student-guide/new-tests/transaction-check.md) explains independent
validity fields, cached flags, retained owners and partial in-place body reads.
The new suite passes 396 programs and 1,049 operations in both normal and
AddressSanitizer/UndefinedBehaviorSanitizer builds. SQL is a recording boundary;
these fixtures do not establish a complete working node. All 21 selected affected
regressions also pass in both builds.

Mapped INCTests and DECTests preserve the unchanged original Java tests' 30 active
assertions per class in each runtime. Their
[decimal arithmetic lab](student-guide/new-tests/increment-decrement.md) distinguishes
executed assertions from comments and failure-branch call sites. The inventory now
contains 311 native counterparts and 146 absent destinations; every counterpart
still has partial status and unverified full parity.


The mining/posting step adds txnpost, txnmine and txnminepost using the actual
native generator, shared transaction/witness owners, signed burn construction and
miner. The normal and sanitizer Java/C++ comparisons each pass 207 programs and 699 operations,
including actual synchronous mining and queued mining. Main messages are locally
recorded in the fixture; native Main startup and H2/MVStore remain unfinished.
All sixteen selected affected regressions have passing final results in each build,
including the targeted rerun after the older transaction harness was corrected. See the
[student mining lab](student-guide/new-tests/transaction-mining.md).

Mapped POWTests and SIGDIGTests run the unchanged Java test methods and the native
assertions in both normal and sanitizer builds: 25 and 89 assertions per runtime.
The [worked lesson](student-guide/new-tests/power-significant-digits.md) explains
parameter order and why these samples do not establish complete arithmetic parity.
The file map has 311 counterparts (275 production and 36 tests), with 146 absent
(94 production and 52 tests). All counterparts remain partial/parity not_verified.


BITCOUNTTests now has its mapped native file. BITCOUNT also exposes and reads the
source's mutable lookup table; direct popcount did not preserve that API behavior.
The original 34 assertion calls, 256 table predicates and three supplemental
mutation observations pass in both builds, as does the 4,471-program KISS VM suite.
See the [bit-counting lesson](student-guide/new-tests/bit-counting.md).
