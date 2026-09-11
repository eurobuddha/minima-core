# Latest checkpoint — transaction-object Streamable integration

Base HEAD 3505766ffe07baf9edc79c0768917d2f4046c3a1. Branch/project/executable
MinimaCoreC++. Goal ACTIVE, not complete. 411 counterparts = 364 production +
47 original tests; 46 absent = 5 production + 41 tests. All present remain
implemented_partial; all 457 parity not_verified. 261 default CTest entries;
full suite NOT run. Historical checkpoint notes follow below.

Six more explicit types implement Streamable: StateVariable, Address, ScriptProof,
Token, Coin and CoinProof. Twenty of 44 adopted; 24 remain. No new mapped-file count.
Existing scalar/MMR codecs reused. StateVariable reader assigns port/type before
value; unknown-type writer now emits only the two tag bytes like Java. Address
updates hash/script before Mx text. ScriptProof updates script/proof before address.
Token updates six fields before its ID. Failed prerequisite reads retain old derived
identities. Coin/CoinProof in-place reader bodies unchanged; writers now append
nested fields directly, preserving output before later hash/null-state failures.
Token calculation catches/logs mapped exceptions without resetting ID; JavaError
and std::bad_alloc escape. Token converter now logs IOException and returns null;
runtime exceptions escape. Source logger body runs in fixtures; only exception
classes are compared, timestamp/message/frame presentation excluded.

Both complete builds PASS. Both interface suites PASS 2,771 comparisons / 20,914
assertions; transcript 54dc918cdd7679d94c066bb26cd314b19a6c483dc53f7dcdc0789c142893c32c.
Twenty-type base calls, all small-fixture prefixes, offsets/trailing bytes, actual
MiniFile/MiniData/Crypto/NIO callers, derived ID timing, independent token/script-root
hashes, invalid state tags/nonzero booleans, nested bad writes and Token logging.
Native complete stderr empty. Initial fixture assumed true only for byte 1; source
and both implementations use every nonzero byte. Expectation corrected, no
production change for that assertion. Initial log and correction note saved.
All seven regressions PASS in both builds: malformed_streams, java_differential,
object_differential, kissvm_differential, utility_differential,
coin_streams_differential, token_validation_differential. Eight-suite totals normal
66.1s, sanitizer 191.66s. Heavy SPHINCS signing NOT rerun.
All handles terminal: 60239/45257 builds0, 45313 initial fixture8 (superseded),
27195/87105 final tests0. No active jobs or MySQL containers; old fixtures stale.
No native H2 provider changes.

Evidence: validation/streamable-transaction-objects-work-in-progress. Student lab:
student-guide/new-tests/streamable-transaction-objects.md. Six per-file lessons and
interface lesson/lab updated. Pinned/current object codecs, scalar/MMR dependencies,
coin-reader fixtures and object/token callers inspected. Sibling objects/StateVariable
uses wrong tags; Address omits script and uses direct hash; Coin uses wrong spent
encoding and skips Token; CoinProof depends on those incompatible codecs. Current
proven implementations reused and minimally adapted.

Next REQUIRED: remaining 24 Streamable implementations, then inherited subclasses
and complete null/default/reference APIs. Address default null fields and Token /
ScriptProof child identity remain gaps. Custom streams, allocation failure timing,
concurrency, all logger failures and subclass dispatch remain unverified. Continue
with dependent Transaction/Witness and database/tree/runtime containers using source
read/write timing, preserving partial nested output rather than whole-child buffers.
Five absent production files still include inactive P2PSwapLink and JNI/benchmark
wrappers; JNI implementation source remains unavailable, do not invent it. Another
41 original test counterparts remain absent. Continue full port after the local
commit and verified recovery bundle; mapped/adopted counts do not prove parity.

---

# Latest checkpoint — scalar Streamable integration and signed normalization

Base HEAD 0f9221cbd63673fea09d9bafe7d626aa4720c25e. Branch/project/executable
MinimaCoreC++. Goal ACTIVE, not complete. 411 counterparts = 364 production +
47 original tests; 46 absent = 5 production + 41 tests. All present remain
implemented_partial; all 457 parity not_verified. 261 default CTest entries;
full suite NOT run. Historical checkpoint notes follow below.

MiniData, MiniNumber and MiniString now implement Streamable: 14 of 44 explicit
source implementers adopted, 30 remain. Existing codecs/UTF8Spans/ByteIO reused.
MiniData validates length, replaces its array with zeroed bytes, copies available
payload, and preserves partial bytes on EOF. Expected-size and hash read variants
follow source rejection ordering. The old allocation stays alive while reading,
including when the supplied span refers to the object's own old bytes.
Its lazy numeric cache survives reads like Java. isLess/isMore/shifts use the cache.
Atomic shared-pointer load/store and explicit copy/move keep read-only cache access
safe; this does not certify concurrent mutation or Java mutable-array aliases.
MiniNumber and MiniString replace their value only after complete decoding.
MiniString writer delegates to MiniData. Numeric wire reads retain existing
64-digit rounding without constructor limits. No new library or H2 provider change.

New tests exposed redundant signed BigInteger bytes being retained by native
factories. Both BigInt::fromJavaBytes overloads now reuse existing normalise().
MiniNumber and MMREntryNumber regressions cover redundant/required positive and
negative sign bytes. Original failed comparison saved in normalization-discovery.json.
Pre-fix sanitizer run also reproduced the discrepancy; its log is preserved.

Both complete builds PASS. Both final interface suites PASS 2,095 comparisons /
12,220 assertions, identical transcript 4343ef558254443b55292ca15effa29ebb2a4ac322dfda99bd684a212953e63e.
Fourteen-type base references, every small-payload truncation, offsets/trailing
bytes, partial state/output, data cache/shifts/comparisons, expected-size/hash
rejection order, self-backed input, malformed UTF-8 and extreme signed numbers.
Exception classes compared; diagnostic prose excluded. Native complete stderr empty.
All nine affected regressions PASS both builds: MiniByteTests, malformed_streams,
java_differential, object_differential, kissvm_differential, utility_differential,
coin_streams_differential, MiniStringTests, ministring_original_tests.
Ten-suite totals normal 65.49s, sanitizer 176.9s. Heavy SPHINCS signing not rerun.
All handles terminal: 23100/32203 builds0; 13919/45559 pre-fix tests8 (superseded);
78808/76270 normalization builds/tests0; 74083/36442 final linear-normalization builds/tests0. No active jobs or MySQL containers. Old fixtures stale.

Evidence: validation/streamable-scalars-work-in-progress. New student lab:
student-guide/new-tests/streamable-scalars.md. Scalar/MMREntryNumber/interface
lessons updated. Sibling types/MiniString.hpp/.cpp and MiniData.hpp, data/number
codec sections inspected: unchecked pointer reads/malformed behavior differ.
Current source-pinned codecs were reused; existing normalise fixes both callers.

Next REQUIRED: remaining 30 Streamable implementations, then inherited subclasses.
While tests ran, pinned and current StateVariable were inspected in full, along
with current Address.hpp. StateVariable source writer emits only port/type for an
unknown type; current native writer throws. Preserve that behavior after partial
invalid reads when adding its in-place reader. No StateVariable edits yet; complete
its reuse/caller/test review before editing. Full scalar API gaps include mutable
MINIMA_CHARSET/limits, nullable arguments, Java array aliases and allocation timing.
Five absent production counterparts still include inactive P2PSwapLink and JNI
wrappers/benchmark; JNI implementation source remains missing, do not invent it.
Another 41 original test counterparts remain absent. Continue full port after local
commit and verified recovery bundle; do not equate adopted/mapped counts with parity.

Review changed normalise to one prefix scan/erase, avoiding quadratic copying.
Four 64 KiB signed coefficients pass in both interface suites.

---

# Latest checkpoint — MMR and signature Streamable integration

Base HEAD 1c3d2d6824c1682e8f0465a44c7ffa1bcdc98b9e. Branch/project/executable
MinimaCoreC++. Goal ACTIVE, not complete. 411 counterparts = 364 production +
47 original tests; 46 absent = 5 production + 41 tests. All present remain
implemented_partial; all 457 parity not_verified. 261 default CTest entries;
full suite NOT run. Historical checkpoint notes follow below.

Six additional explicit source implementers now use Streamable: MMRData,
MMREntryNumber, MMREntry, nested MMRProofChunk, SignatureProof and Signature.
Eleven of 44 are integrated; 33 remain. No new mapped-file count this checkpoint.
Existing codecs supply values; in-place readers preserve pinned assignment order.
MMREntry has an explicit empty flag, cleared before reading any bytes. Its null
writer appends row and entry first, then throws JavaNullPointerException.
MMREntryNumber assigns only after the complete decimal is decoded; empty unscaled
bytes now throw JavaNumberFormatException. Signature uses HORSTSignature's shared
Data/list pattern: old list handles survive replacement, containing aliases see
the replacement. TreeKey and envelope fixture retain/dereference shared list handles.
Nested writers append directly, preserving output prefixes on oversized hash errors.

Both complete builds PASS. Both interface suites PASS 1,299 comparisons / 6,290
assertions, normal 3.293s and sanitizer 3.250s. Matching transcript SHA256:
6e5a3aebfbb1ebadd2f687923034cc077084ece8acfe98e86a176d02430fb50b.
Every small-vector truncation, offsets/trailing bytes, in-place partial state,
explicit empty/unspendable flags, retained lists/object aliases, null and oversized
hash partial writes, actual generic MiniFile/MiniData/Crypto/NIO callers.
Exception classes compared; diagnostic prose excluded. Native complete stderr empty.
All six regressions PASS both builds: MiniByteTests, malformed_streams,
java_differential, object_differential, utility_differential,
sphincs_signature_differential. Normal six-suite 36.27s; sanitizer seven-suite
including interface 117.11s. Object corpus includes actual TreeKey signing and
verification. Heavy SPHINCS signing NOT rerun; container codec/state suite rerun.
No active test handles; 59949, 9804, 34954, 75828 and 6123 all terminal exit0.
No MySQL containers; old /tmp fixtures stale. No native H2 provider changes.

Evidence: validation/streamable-mmr-signatures-work-in-progress. New teaching lab:
student-guide/new-tests/streamable-mmr-signatures.md. Six source lessons plus
interface lesson/lab updated. All 457 lessons remain authored, teaching != parity.
Pinned source and current native codecs/callers/tests inspected. Sibling
/Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/mmr/MMRData.hpp/.cpp
and MMREntry.hpp/.cpp use incompatible framing/spent flag/fixed integers. Current
codecs reused; no algorithm replacement or new dependency.

Next REQUIRED: integrate remaining 33 source Streamable implementers with actual
source read/write timing and partial-state validation, then inherited subclasses.
Complete constructors/null/reference APIs remain open, including MMRProofChunk's
no-argument construction and nullable SignatureProof entries. Vector/span does not
certify custom streams, allocation failures, concurrency or full node/API parity.
Five production counterparts still absent: inactive P2PSwapLink, benchmark,
jnitest, jniminingtest, jnifunctions. JNI implementation source remains unavailable;
do not fabricate it. Another 41 original test counterparts also remain absent.
Continue full port work after this local commit and verified recovery bundle.

---

# Latest checkpoint — Streamable base and five concrete integrations

Base HEAD 6b01bd6e0f4f43211813776c90d8411772a17431. Branch/project/executable
MinimaCoreC++. Goal ACTIVE, not complete. 411 counterparts = 364 production +
47 original tests; 46 absent = 5 production + 41 tests. All present remain
implemented_partial; all 457 parity not_verified. 261 default CTest entries;
full suite NOT run.

New utils/Streamable.hpp/.cpp defines pure virtual writeDataStream(vector&) const
and readDataStream(span,offset&), virtual destructor, serialise() collection bridge.
Existing MiniFile/Crypto/MiniDataVersion/NIO templates can now accept base references.
MiniByte inherits and reuses wire::byte for in-place read; static deserialise uses
that reader and serialise uses the shared collection bridge. MMRProof,
HORSTSignature, FORSSignature and SPHINCSSignature inherit/override with their
existing method bodies unchanged. All dependent native objects rebuilt normally
and with sanitizers. No native H2 provider change.

Both interface suites PASS 909 comparisons / 3,850 assertions. Normal 2.934s,
SAN 3.253s. Same transcript 2f82001a4a4239116bfe8e7659ec48922a4787219f288e38ac9b8ae5a808e359.
Actual base references, prefix writes, all truncations of small proof/signature
vectors, read offsets/trailing bytes and partial reserialization, custom failing
writer, actual generic file write/append/in-place load, MiniData conversion,
single/pair SHA3 and NIO payload construction. Exception classes compared, prose
excluded. Native complete stderr empty. All six regressions PASS in both builds:
MiniByteTests, malformed_streams, java_differential, object_differential,
utility_differential, sphincs_signature_differential. Normal six-suite 44.42s;
SAN seven suites including interface 116.87s. Heavy SPHINCS signing not rerun;
existing container codec/partial-state suite rerun and passed.

Driver tools/streamable_differential.py; support/StreamableOracle.java and
streamable.cpp. Evidence validation/streamable-work-in-progress. The pinned source
has 44 explicit implementing classes; only five adopted. Saved
explicit-implementations.json records the remaining 39, including MMRProofChunk.
Next REQUIRED: continue real interface adoption with source write/read bodies and
partial-state validation for those 39; validate inherited subclasses as bases join.
Do not use assignment from deserialise where source exposes partial mutation.
Vector/span backend is not certification of arbitrary Java streams, blocking I/O,
allocation failures, custom subclasses, concurrency or complete caller error paths.

Read pinned Streamable and P2PSwapLink, current generic helpers/ByteIO, complete
MiniByte and existing proof/signature stream methods, relevant tests. Sibling
serialization/DataStream.hpp/.cpp has incompatible 2-byte string framing and
whole-read bounds behavior; reuse current proven codecs instead. Student interface
lab and all six affected per-file lessons updated. Teaching != parity.

All handles TERMINAL0/CLOSED: normal build81683, normal interface80764,
normal regressions14826, SAN build11509, SAN tests47729. No new live containers.
All previous fixture JSONs remain STALE, fixture CMake caches empty. No heavy
SPHINCS jobs live. Remaining absent production files: P2PSwapLink.java (every line
commented), base benchmark/jnitest/jniminingtest, utils/jni/jnifunctions. JNI native
sources still missing; retain prior source search/question and do not invent a
replacement. Also 41 original test mappings and complete node/API fidelity remain.
Use GRAPHIFY_SKIP_HOOK=1 for local commits. Commit/recovery details in Git log and
ignored local-backups manifests. No GitHub push requested.

---

# Latest checkpoint — help counterpart and curated catalogue

Base HEAD 995880769300727fa16fbf1100a2f70b7f39a610. Branch/project/executable
MinimaCoreC++. Goal ACTIVE, not complete. 410 counterparts = 363 production +
47 original tests; 47 absent = 6 production + 41 tests. All mapped implementations
remain implemented_partial and all 457 parity not_verified. 260 default CTest
entries; full suite NOT run.

New commands/help.hpp/.cpp translates the complete pinned help body and registers
it in the real runner. Explicit 94-constructor catalogue, metadata-only detailed
lookup, case-sensitive name selection after Command trim, inherited full help,
factory and UTF-16 padding/truncation reuse existing native helpers. No JNI
metadata placeholder. The three absent source registry commands are benchmark,
jnitest and jniminingtest; full help for them still unavailable.

Both complete builds PASS. Both help suites PASS 761 cases/2,543 assertions,
94 catalogue entries, exact insertion order and serialized JSON, every existing
native command's short/full help, null metadata, controlled registry probe that
must never run, fresh factories, reply parameter aliases, actual chained and
non-admin dispatch, UTF-16 random/unit-boundary/null/negative padding cases.
Normal 2.430s; sanitized result has its own duration in saved JSON. Common hash
44c138190f7cff00e779184893ec20c91647208d80eb1edf519a308c254eb00e.
NPE/classcast prose normalized; exception class and other messages compared.
Complete native stderr empty. All three affected runner/errors/text regressions
PASS in both builds; total four-suite times 11.61s normal / 16.51s SAN.
Driver tools/help_command_differential.py; fixtures support/HelpCommandOracle.java
and help_command.cpp. Evidence validation/help-command-work-in-progress.
Student lab and per-file lesson updated; previous incorrect HashMap explanation
fixed after inspecting pinned JSONObject extends LinkedHashMap. Teaching != parity.

All handles TERMINAL0/CLOSED: normal initial build 64678, initial comparison1461,
SAN initial build6156, final normal10917 and SAN77013. No live test/server/container
from this turn. All old MySQL fixture JSONs remain STALE and fixture CMake caches
empty. Existing native-h2-archive provider unchanged. No heavy SPHINCS jobs live.

Next production mappings: Streamable.java, entirely commented P2PSwapLink.java,
base benchmark/jnitest/jniminingtest and utils/jni/jnifunctions. JNI native sources
still absent: retain prior search/question and do not invent a substitute.
Remaining 41 original tests and full API/runtime/concurrency/platform fidelity
also remain. Allocation failure timing and full registry fidelity not certified.
Use GRAPHIFY_SKIP_HOOK=1 for local commits. Current commit/recovery details are in
Git log and ignored local-backups manifests. No GitHub push requested.

---

# Latest checkpoint — live MySQL archive integration

Base HEAD 6026383791c31f69739b5f18e948307452b889fa; branch/project/executable
MinimaCoreC++. Goal ACTIVE, not complete. No production translation changed.
409 counterparts = 362 production + 47 original tests; 48 absent = 7 production
+ 41 tests. All present remain implemented_partial, all 457 parity not_verified.

New tools/native_mysql_archive_startup.py reuses native_restore_startup Node,
ready/key_state/chain, native_archive_startup coins, existing archive oracles,
ArchiveServerSeed, Runner and independent wire framing. Actual pinned Java and
native nodes use the existing real native-h2-archive provider and an owned
loopback/tmpfs MySQL fixture. Explicit connector 8.0.24 jar remains required for
Java; native uses the existing AOT H2/JDBC provider, not a Minima JVM fallback.

Normal live validation PASS: 17 completed node processes, 3,202 assertions,
160 command transcripts, 81.039s. Sanitizer PASS with identical counts, 343.945s,
no reported native sanitizer errors. First normal
attempt stopped on a test expectation (quoted parameter trim and uppercase BIP39
cleaning); preserve initial report. Production behavior required no change.

Checks cover 257-block update/repair, exclusive oldest-block skip, ignored endfix,
exact durable block/cascade bytes, full address histories, 514 indexed coins,
raw cap quirks, both directions of raw/H2 import/export with and without cascade,
saved login restart and actual genesis chain/reset/seed resync. Opposite runtime
reopens persisted data, checks chain/coins/key counters, signs, verifies and rejects
altered data. Synthetic linked/cascade blocks test storage only. Successful
consensus resync in this lab is genesis only. Long-chain/cascade resync, crashes,
reconnect/TLS, Error/interruption/concurrency and platform fidelity remain open.

Optional serial CTest native_mysql_archive_startup added (3600s timeout); it
compiles its own source oracle. Default suite stays 259; four optional live
MySQL entries require explicit MINIMA_MYSQL_FIXTURE_FILE. Full suite NOT run.
Evidence validation/mysql-live-archive-work-in-progress; student lab
student-guide/new-tests/mysql-live-archive.md. All 457 teaching pages exist;
teaching != parity. Sanitizers instrument C++, not the third-party AOT provider.

Both live sessions terminal0/CLOSED: normal 67275, sanitizer 5304. Do not poll.
Owned fixture d0ad4a49770179522a6a7051882e78f9f9f4e0145e4351e0d3b726244ca3ae39
removed after exact identity/tmpfs/loopback checks; all other IDs unchanged.
/tmp/minima-mysql-command-fixture.json is now STALE. Both fixture CMake caches
cleared; default test inventory 259 verified. Commit and recovery bundle details
are available from Git log and ignored local-backups manifests.

Next: remaining help/Streamable/P2PSwapLink counterparts and original tests;
JNI sources still missing (retain existing question/search, no invented substitute).
Do not confuse mapped files, compilation, scoped tests or authored lessons with
full parity. All heavy SPHINCS jobs are terminal and saved; do not poll them.

---

# Latest checkpoint — mysql command implemented; both command suites pass

Previous HEAD 5bece26 (completed full SPHINCS command sanitizer evidence).
Git log gives the current MySQL checkpoint. Branch/project/executable MinimaCoreC++.
Goal ACTIVE, not complete. 409 counterparts = 362 production + 47 original tests.
48 absent = 7 production + 41 tests. All present remain implemented_partial;
all 457 parity not_verified. 259 default CTest entries; full suite NOT run.

New mysql.hpp/.cpp translates every source action plus convertMySQLParams/main,
registered in CommandRunner. Reuses MySQLConnect, ArchiveManager, RawArchiveInput,
Main/vault/wallet/BIP39, CommandResult, streaming GzipOutput/BufferedFileOutput,
Java-width arithmetic/file/lowercase helpers. Static configuration reuses existing
MySQLConfiguration (only its comment changed). Preserve fresh login override only
when BOTH host/database exist, parameter/error order, height-difference totals,
OR/null edge failure, parent-before-height integrity, source always-true update
condition ONLY for empty MySQL, exclusive local ranges, explicit normal shutdown,
resync counters/poll limits/no active cascade installation/shutdown order,
addresscheck coins reply/blockid, both H2 paths, write-before-cap raw exports,
rawimport flag exception handling, fixmissing startfix/endfix quirks and reset
nested command strings. No production loop limits or mock SQL added.

Both complete builds PASS. Both command suites PASS 2,800 comparisons,
2,061 independent assertions and 73,670 SQL events. Normal 23.841s, SAN 67.013s;
same transcript 6413869fc68a49dcf8caf2855135f7f7584a54f8b1ec4e8f470a8899f11a3522.
Driver tools/mysql_command_differential.py. Test fixtures extend the existing
SqlDBOracle/SqlRecordingFixture with optional finite rowsBySQLBatches, leaving
production DB code real. Actual command bodies, linked/gap/bad/zero synthetic
blocks, coin/state codecs, raw gzip files, preferences and per-event SQL/runtime
failures are compared. Demo SQL failure logs separated from plain stdout;
stack frames/NPE/classcast prose excluded, timestamps checked then normalized.
No JVM is used by the native command. Recording fixture is NOT live storage.

Comparisons found real existing dependency defects: missing raw files lost Java
FileNotFoundException details; truncated deflate EOF lost its explicit message;
RawArchiveInput bypassed MinimaLogger. Fixed with MiniFile-style open checks,
opt-in EOF message preservation retaining inheritance, shared logger routing.
Standalone raw test harness routes logs to stderr. Initial failures retained,
including two fixture-only mismatches (H2 connect event and demo log capture).
Both affected 5-suite regressions PASS: raw archive 613, archive command 509,
mysqlcoins 771, SqlDB and ArchiveServer cleanup. Normal 107.61s, SAN 192.74s.
Guide audit 457 pages, 9,271 links, 2,632 anchors, zero errors. Teaching != parity.
Evidence validation/mysql-command-work-in-progress; lab and per-file lessons updated.

NO live MySQL fixture/container was created this turn. Old fixture JSON files in
/tmp remain STALE. Both builds still use native-h2-archive provider from previous
checkpoint, SHA 9edf7d22f394875edb0d5b15ead0ce1e192c7f7781b5a84f0fc2c807ad2aa4bf.
MINIMA_MYSQL_FIXTURE_FILE remains empty. Next REQUIRED: actual MySQL/H2 command
persistence, nonempty update, cascade-bearing raw/H2 import/export, successful
Main resync/seed/reset/shutdown flows. Reuse native_mysql_startup.py,
native_archive_startup.py, ArchiveCommandOracle/native archive fixture and
ArchiveServerSeed; read full relevant code first and report minimal adaptations.
Use new owned tmpfs/loopback-only MySQL container, no host volumes or user DBs.
Additional Error injection/resource/GC/interruption/platform fidelity remains open.

SPHINCS COMMAND sanitizer session 24343 is now TERMINAL0/CLOSED, no need to poll.
Final 131 cases/34 checks/all five full signatures PASS in 8,573.312s; transcript
1ffd436dddf5a578cdf8b3e4d8036455efdec772ef7d87f986a3ea143f1f53e4 matches normal.
Saved by 5bece26, including full result/progress and student explanations. The signer
41760 was already closed/PASS c6fdf53. NO heavy SPHINCS jobs remain live.
All this turn's handles CLOSED: 97884 initial private IBD alias compile failure;
75644 protected Main fixture compile failure; 43787 missing-file comparison;
54099 H2 fixture event; 60018 gzip EOF comparison; 24802 initial 2749 PASS;
90804 added demo failure mismatch; 82086 full SAN build PASS then old demo harness
failure; 86568 normal final PASS; 42971 SAN final PASS. Do not poll again.

Remaining production files: help.java, Streamable.java, P2PSwapLink.java (entirely
commented), base benchmark/jnitest/jniminingtest and utils/jni/jnifunctions.
Help's mysql dependency is now present. JNI implementation sources still missing;
existing search/question retained, do not invent a libnative substitute. Also 41
original tests and all full parity gaps. Base test private classloader helper still
unrepresented. Preserve scope: live MySQL/resync validation is not optional merely
because more mapped files can be added.

Last pre-turn recovery: local-backups/MinimaCoreC++-20260911T105205Z.bundle,
SHA 4df084567adc6469333008c6690d69216672c2a161d483dac06dcd260b6a91a0,
50 paths byte-verified. Create new bundle after final commit via already-read
/tmp/minima-hashtest-backup.py; await terminal, independent clone/fsck/pin check,
then byte-verify ALL paths changed since 25386a3, covering both new commits.
No GitHub push destination requested; local bundle is not off-machine backup.

Earlier notes below historical where contradicted by this checkpoint.

---

# Latest checkpoint — ArchiveServer compiled and validated through real MySQL/TCP

Previous HEAD d6624fd7940c2cc6745afd95d98dd07b2bae60f3; gitlog gives current.
Branch/project/executable MinimaCoreC++; goalACTIVE, notcomplete.
408counterparts=361production+47tests;49absent=8production+41tests.
All present implemented_partial; all457 parity not_verified.258defaultCTestentries,
3optionalMySQL tests iffixture configured. Full suite NOT run.

New ArchiveServer.hpp/.cpp and minima_archive_server standalone entry translate
allsourceflows using existing HTTPServer shared-owner factory, NIOConnections,
MiniData::readFromStream, IBD/MySQLConnect, ConsoleInput/JavaInteger/trim/logger.
Keep ignoredtype/trailingbytes, -1probeatZERO+emptyIBD, cachedcascadeonlyatZERO,
range order/nullentries, catchExceptionnotError, one-secondretrywithoutreinit,
ten-secondsleepafterwrite, distinctearlyreturnclose/clean behavior, EOFloop,
v1.3startup/v0.8help, argument/environmentorder and sourcequit/shutdownlogs.
SystemClean usesrecursive staticmonitor, Java-width subtraction andloggerclock/GC.
Handler mMySQL returns currentvector API; arbitrary subclass/alias/fieldrace and
GC/interruption/platform/stackframe fidelity stillopen. Sourceconstructor becomes
existing create factory so shared ownershipexistsbefore start.

Native JDBC bridge addsopcode62 Class.forName, NativeH2Runtime::loadMySQLDriver.
Freshcompiledprovider cpp/build/native-h2-archive, bothbuildsconfiguredtoit.
LibrarySHA9edf7d22f394875edb0d5b15ead0ce1e192c7f7781b5a84f0fc2c807ad2aa4bf.
NoMinimaJavaclasses/noJVMfallback. ExistingreviewedMySQLmetadataunchanged.
Both complete buildsPASS. Handlerdifferential81cases/53checksPASS both:
normal94.016s,san94.269s;same36220f1e...transcript. Real10secondwaitsretained.
Originalnormalcase81found IBD::serialise nullblockthrowswrongexception; fixedonly
thatpath toJavaNullPointerException. Initialreportretained. BothIBD1057regressionsPASS.
Reviewcaught nonrecursiveSystemClean lock; changedtorecursive andactualMainnotify
reentryaudit4cases/11checksPASSboth,70b3fba6...hash. Separatecleanreportscopes.
RealstandaloneMain suites19CLIcases+2serverprocesses+110checksPASSboth,
normal23.441s,san24.47s. JavaSeed writes3realTxBlocks+3nodeCascade torealMySQL;
4TCPrequests perruntime verifyprobe/cachedcascade/ranges/empty,10secclose,console
unknown/trim/quit. This isactualserver, notstubSQL. Native shutdownstderrEMPTY.
Selected4regressions bothpass: IBD,HTTPServer,H2diff,H2concurrent40.10s/102.10s.

Fixtureminima-cpp-mysql-archive-b7050737d3,IDf0c1f91958e7917a67588ee1ca22e36a98071799165811966ac5e9c6f0c719b2
wasREMOVEDandverified; othercontainersuntouched. /tmp/minima-archive-fixture.json
isnowSTALE; create a NEW isolatedtmpfs/localhostfixture to rerunliveSQLtests.
CMake MINIMA_MYSQL_FIXTURE_FILEclearedinbothbuilds. Optionalarchive_mainDEPENDS
archive_server_differential tocompilepinnedoraclefirst. Standalonecanbeconfigured
withsourceMySQLenvironmentvariables/-port flags. Readnative/h2/README.md.

AllArchiveServerjobhandlesTERMINAL/CLOSED:31316providerPASS,65562initialcompileFAIL
(missingSocketExceptioninclude),17133initialfullFAIL(fixtureboolean/referenceAPI),
28361fixedfullPASS,83091initialcomparisonFAIL(nullIBDblockcategory),40493SANbuildPASS,
22916normalfull+comparison+regsPASS,66775SANfull+comparison+regsPASS,
76052fixturecreatePASS,2339/59528normal/SANmainPASS,66077/94013finalbuildsPASS,
99112/51638reviewbuild+cleanauditPASS,8050/95371finalscopedcleanauditPASS.
functions cell5201cleanupDONE;doNOTpolltheseagain.

LIVE24343 fullsanitizedSPHINCScommands, /tmp/minima-sphincs-command-sanitized-full.log.
Latest130cases/0mismatches: inline/file/test/1inputfullsignaturesmatched; final8input
transactionfullsignature remains. Revalidatehandle; neverrestartmerelyforquiet.
Reportbuild-sanitized/sphincs-command-full-results.json onlyafterterminal.
Resumedsigner41760wasPASSc6fdf53;alreadyclosed299comparisons/1377checks+mains.

Remainingproduction:mysql.java(1446lines),help.java,Streamable.java,P2PSwapLink.java
(entirelycommented),benchmark,jnitest,jniminingtest,jnifunctions. Plus41originaltests
andallfullparitygaps. JNIimplementation sourcequestionpending;donotinventlibrary.
Base test unusedprivateclassloaderhelper stillunrepresented. MySQL largecommand
andhelp nowhave actualnativeSQLprovider; inspectpinnedsource/fullcurrentcandidates/
callers/tests/siblingthenreportreusebeforeadapting. Keepstudentguidepagescurrent.

Previous d6624fd recoverybundleverified all49changedpaths acrossbothcommits:
local-backups/MinimaCoreC++-20260911T102016Z.bundle SHA256
 a799ea7568266da1608b79f5f556edae6cfb095fdda91686abdb1bd005cbb4cd.
Createcurrentcheckpointbundleaftercommitusingalreadyread/tmp/minima-hashtest-backup.py,
awaitterminal, clone/fsck/sourcepincheck then byteverifyALLHEADchangedpaths.
NoGitHubpushdestination; localbundles areNOT off-machinebackup.

Earliernotesbelowhistoricalwherecontradictedbythischeckpoint.

---

# Latest checkpoint — native MySQL provider and real node tests pass

Branch/project/executable MinimaCoreC++. Goal ACTIVE; not complete.
Previous commit c6fdf534dd67a1d33b94a04ea15da80681c87d8e records completed
resumed SPHINCS validation; git log gives current MySQL checkpoint.
407 counterparts=360production+47originaltests;50absent=9production+41tests.
All present remain implemented_partial; all457 parity not_verified. Full suite NOT run.

The optional native H2 library now includes pinned Connector/J8.0.24 with --mysql.
Reuse NativeH2Bridge C ABI and NativeH2Connection ownership/metadata/error transport;
nullableConnector preserves optional credentials. Launcher shares one isolate through
MainServices.sql and MainServices.mysql. No Java Minima classes/JVM fallback.
Reviewed tracing-agent metadata in native/h2/mysql-metadata. Initial filter, compiler
class-version and auth-message assertion failures retained. Profile52programsPASS.
GraalVM path /private/tmp/minima-native-h2-toolchain/graalvm-community-25.3.4.1+1.1/Contents/Home.
Fresh provider cpp/build/native-h2-mysql, both builds now configured to it.
Library SHA256 554d480a40932a6fad765ef62179975884c91c154349b77f60fb79ddec7f8c28.
Both full C++ builds PASS. Both realdriver suites52programs/444ops/169checksPASS,
normal38.561s,sanitizer36.356s, same8c0a1971...transcript and empty native stderr.
Existing H2 differential+concurrent2/2PASS both:4.65s/5.08s.
Both actual-node suitesPASS4processes/build,17mysqlcoins commands/runtime:
normal22.666s,sanitizer134.169s. All8keys retained acrossrestart and clean shutdown.
Checks info/wipe/emptyarchiveupdate/querywrite/search/hiding/doubles/null/errors.
Nonempty livearchiveupdate/autologin/autobackup/reconnect/TLS/platform/fullparity
remain open. Evidence validation/native-mysql-work-in-progress; lab+lessons updated.
Optional CTest fixture setting registers2tests, verified then cleared; default256tests.
Own MySQL container minima-cpp-mysql-1b8c8a131d was removed and removalverified.
Its old /tmp/minima-native-mysql-fixture.json is now stale; create a NEW isolated
fixture to rerun. No user container/host volume touched. See fixture-cleanup.json.
Tool sessions97650profile,85895provider,78307normalbuild/test,17026sanbuild,
6376san-driver+bothnode suites,45305/67396H2regs,12552configure,56757cleanup are
TERMINAL0/CLOSED. functions cell5161 also CLOSED. Do not poll these again.

SPHINCS resumed sanitizer session41760 is TERMINAL0/CLOSED. Final299comparisons,
1377independent checks,3309.726s; actualsource mainsmatch, emptyprocessstderr.
Two saved signature records explicitlyrechecked plusone newlygenerated fullsignature.
Evidence copied to validation/sphincs-work-in-progress/sanitized/sphincs-resumed-*.
Original timeout stillretained. Commitc6fdf53 saves this result and studentexplanations.
Do not poll41760 again.

LIVE24343 full sanitized SPHINCS command, /tmp/minima-sphincs-command-sanitized-full.log.
Latest129cases/0mismatches/30checks: inline/file/test fullsignatures match, two
transaction signatures remain. Revalidate before inferring status; neverrestart
merely for quiet output. Final build-sanitized/sphincs-command-full-results.json.
Normal131/34fullcommand suite alreadyPASS. No full SANcommandpass claim yet.

Guide audit457pages/9263links/2632lineanchors/0errors. Teaching coverage != parity.
Remaining production mysql.java,help.java,ArchiveServer.java,Streamable.java,
P2PSwapLink.java(all commented),benchmark/jnitest/jniminingtest/jnifunctions;
plus41originaltests and all full-fidelity gaps. JNI librarysource questionpending;
noimplementationfound; do notinvent a loaded library or count failurestubs asport.
ArchiveServer source fullyread lastturn; reread and inspectHTTPServer/Socket/NIO
candidates/dependencies/tests and sibling beforeadapting. The realnativeMySQL
provider now supplies its previouslymissing runtime dependency. mysql.java remains
large1446lines; help depends on its metadata. Base test privateclassloaderhelper
stillunrepresented. Continue reuse-first, tests, student explanations and commits.

Last pre-MySQL bundle d4517e3: local-backups/MinimaCoreC++-20260911T094827Z.bundle
SHA256 8b8567fe48566630a4296db7900957c2b02d988ef4b76cbff7587a5b7fd784f7,
all31changedpaths byteverified. Create this checkpoint's recovery bundle after
commit, independentlyclone/fsck/source-pincheck, byteverify ALL changedpaths
since d4517e3 to cover both signer and MySQL commits. Use alreadyread
/tmp/minima-hashtest-backup.py; awaitterminal before reading manifest.
No GitHubpush destination chosen; local bundles are NOT off-machine backups.

Earlier notes below are historical where superseded by this checkpoint.

---

# Latest checkpoint — mysqlcoins command validated through recording SQL

Previous HEAD71d34a6; git log gives current. Local branch MinimaCoreC++. GoalACTIVE.
407counterparts=360production+47tests;50absent=9production+41tests.
Allpresentimplemented_partial/allparitynot_verified.256CTest entries/fullsuiteNOT run.
Both complete builds and mysqlcoins suites PASS771cases/1007checks/24372SQL events.
Normal14.523s,sanitizer25.142s; same59a346cf...transcript; native stderr empty.
Source allfiveactions translated, actualMySQLConnect/UserDB/codecs/dateformatter.
Retain respnse spelling, autologin precedence, whole-block strictcoinlimit,
blocklimitbeforeprocessing, Java-widthwrap, parameter/error order and explicit
shutdownonlyon normalpath. Optional logger memorycollection hint for System.gc.
Fixture uses unchangedSqlDBOracle/SqlRecordingFixture+Archive/Coin helpers.
NO live native MySQL provider is configured; runtime gap remains, not fakeengine.
No durablepaging/concurrency/>5sprogresslog test. Repeated ranges bounded maxblocks.
Initialnormal4thcase fixture CommandException mislabel fixedusingexistingSPHINCS
adapter; originalreport retained. Terminal70825exit1(initialfixture),49371normalPASS,
47075fullSAN+suitePASS areclosed. DoNOTpollagain. Earlier56907recoveryterminalPASS.

LIVE41760 resumedSANsigner now299comparisons/0mismatches; lastcomparison1375checks.
Two saved signatures explicitlyrechecked plusmissingempty-message fullsignature
newlygenerated. Comparisonprocessesexit0/stderrEMPTY. Actualsource mains running.
Copied comparisonprogress/process-results/3vectors tosanitized/resumed-comparison-*.
No finalfullsuite reportyet. Log/tmp/minima-sphincs-sanitized-resumed.log.
LIVE24343 fullSANcommand latest127/0, firstinlinefullsignaturematched,restremain.
Log/tmp/minima-sphincs-command-sanitized-full.log. Revalidatebeforestatusinference;
neverrestartmerelyforquietoutput. Original31941timedout;89965normalcommandPASS.

Previous71d34a6bundle verified/all27changedpathsbytechecked:
local-backups/MinimaCoreC++-20260911T093445Z.bundle SHA256
 ec981148243c529eb939752359c1faa3f939c7e966d3c64e5ab60fdd7f193eae.
Clone.../minimacorecpp-recovery-nkfblls_/MinimaCoreC++.
Create owncheckpointbundle aftercommit andbyteverify allchangedpaths.

Remainingproduction:mysql.java,help.java,ArchiveServer.java,Streamable.java,
P2PSwapLink.java(entirelycommented),benchmark/jnitest/jniminingtest/jnifunctions.
Plus41originaltests andallfidelitygaps. Help dependsnowonlyonmysql commandmetadata.
NativeMySQLdriverabsence explicit inMainServices/MySQLConnect::runtimeServices;
complete working backend required forfullgoal. JNI librarysource questionpending.
Continue reuse-first read/current/sibling/fullcandidate/report beforeedits.
Keepverifiedlocalcommits; no GitHubpushdestination specified.

Earlier notes are historical where contradicted by this checkpoint.

---

# Latest checkpoint — original GET and SUBSET tests pass

PreviousHEADb601c5e; gitlog givescurrent. LocalbranchMinimaCoreC++. GoalACTIVE.
406counterparts=359production+47tests;51absent=10production+41tests.
Allpresentimplemented_partial/allparitynot_verified.255CTestentries/fullsuiteNOT run.
Bothcompletebuilds andGETTests/SUBSETTests/get_original_tests/subset_original_tests
PASS:normal7.93s,san7.86s.3unchangedJavamethods/file;31GET+33SUBSETassertions/runtime.
Nativeempty stderr,UBSANhalt. Sourcefixturesandoperationorder preserved; production
functions unchanged. Evidencevalidation/get-subset-original-tests;labandlessons.
Terminal90513normal and43780san arePASS/closed;doNOTpollagain.

LIVE24343SANfullSPHINCScommand and41760resumedSANsigner confirmedlive thisturn.
Lastobserved127/0 and156/0 respectively. Revalidate;doNOTrestartforquietoutput.
Original31941signer timedout,89965normalcommandpassed;seeearliercheckpoint.
Previousb601c5erecoverybundle verified+all22changedpaths bytechecked:
local-backups/MinimaCoreC++-20260911T092830Z.bundle SHA256
89ce49ff0aef9d5dd51abc147d35ccd5a3a27f15f74dee3dbc86b9812fe6d2ed.
Clone.../minimacorecpp-recovery-y53lxze8/MinimaCoreC++.

Nextproductionfocus mysqlcoins.java: entire367lines fullyread, plusnative
MySQLConnect.hpp/.cpp,MySQLConnectOracle.java,support/mysql_connect.cpp,
SqlRecordingFixture.hpp,MinimaLogger.hpp/dateformat,UserDBMySQLaccessors.
No mysqlcoins implementation edits yet. ExistingSQLfixture andruntimeServices
canvalidatecommand withoutnewproductiontesthooks. Trace actualruntime driver
configuration too: nativeMySQLConnect::init rejectswhenconnectserviceabsent.
NeedreadSqlDBOracle/dependenciesandmysql_connect_differential fully,searchsibling,
reportreusebeforeimplementation. Preservewipe misspelling'respnse',argumentorder,
autologinprecedence,updatecoins/maxlimits/date/logs,inputspent/outputblockmutations,
where/address/queryprecedenceandshutdownonlyon normalreturn. No dummybackendclaim.
JNI source questionpending; plentyindependentworkremains. No GitHubpushdestination.

Earliernotes arehistoricalwherecontradictedbythischeckpoint.

---

# Latest checkpoint — original CONCAT tests pass in both builds

Local branch/project MinimaCoreC++; previous HEAD a3b0140, git log gives current.
Goal ACTIVE.404counterparts=359production+45tests;53absent=10production+43tests.
Allpresentimplemented_partial;allparitynot_verified.251CTestentries/fullsuiteNOT run.
Bothcompletebuilds andCONCATTests/concat_original_tests PASS (normal5.05s,san3.87s).
Nativefixture followsLENTests adapters; Javawrappercalls3unchangedsource methods;
21assertions perruntime. ProductionCONCAT unchanged. Native stderr empty,UBSANhalt.
Sourceemptyhex case succeeds despiteinvalid-section heading; preserved exactly.
Evidencevalidation/concat-original-tests; lessonandlabupdatedbefore/aftertests.
Terminal59912normal and38512san PASS/closed; doNOTpollagain.

LIVE24343fullSANSPHINCScommand and41760resumedSANsigner remainactive; latest127/0
and156/0respectively. Signer nowgeneratingmissing empty-messagefullsignature after
explicitlycheckingbothsavedmatchedrecords. No reductioninforests or fullpassclaim.
See previouscheckpoint below for logs,resumecommand,oldtimeout andterminalhandles.

Previousa3b0140recoverybundle verified; all42changedpaths bytechecked:
local-backups/MinimaCoreC++-20260911T092301Z.bundle SHA256
 d16b4f6ef2f8d1f3db73b29d9930372688e340e113589f85d9ff309a86dc8993.
Recoveryclone.../minimacorecpp-recovery-3dqt7h4x/MinimaCoreC++.
Create thischeckpoint's bundleaftercommit andbyteverifyallchangedpaths.
Nextindependentscope: remainingGET/SUBSEToriginaltests (sourcewaspartiallyread,
repeatcomplete targetedreads before adapting), or unresolvedproductionfiles.
Streamable andfullycommentedP2PSwapLink source fullyread butnoimplementationedits.
No pending GitHub push; remote destination unspecified. JNI source questionpending.

Earliernotes arehistoricalwherecontradictedbythischeckpoint.

---

# Latest checkpoint — exact text commands; normal SPHINCS command complete

Local branch/project remains MinimaCoreC++. Previous HEAD aa5edad; git log gives
this checkpoint. Goal ACTIVE: 403 counterparts (359 production, 44 original tests),
54 absent (10 production, 44 tests). All present remain implemented_partial;
all parity not_verified. Both full builds pass. 249 CTest entries; full suite NOT run.

New root tutorial, base tutorial, and whitepaper classes retain exact pinned text,
metadata, reply shapes and fresh factories. The tutorial classes remain distinct;
only the root variant is registered. Public whitepaper::WP is immutable.
Both differential suites pass 71 cases/428 independent assertions (2.618s normal,
4.137s sanitizer), identical d0d478ed... transcript. Payloads 6825/6733/43554 bytes.
Evidence: validation/text-commands-work-in-progress. All 457 lessons audited.

Normal full SPHINCS command session89965 is TERMINAL0/closed: 131 cases/34 checks,
1529.766s; all five actual full-size signatures matched. No live network funding
or transaction-admission claim. Do NOT poll89965 again.
Original sanitizer signer31941 is TERMINAL1/closed: third signature response
exceeded1800s; cleanup120s timed out and killed native child. First158 comparisons
and two signature pairs matched. No final report, mains or retained native stderr.
Original evidence preserved under sphincs-work-in-progress/sanitized/initial-*.

LIVE — revalidate, do not restart merely for quiet output:
24343 full sanitized SPHINCS command, /tmp/minima-sphincs-command-sanitized-full.log.
Latest127 cases/0 mismatches; inline full signature matched, four signatures remain.
41760 resumed sanitizer signer, /tmp/minima-sphincs-sanitized-resumed.log.
Latest156 live comparisons/0 mismatches,1065 independent checks. Two saved signature
records explicitly rechecked, missing empty-message signature and mains remain.
Driver uses --resume-vectors validation/sphincs-work-in-progress/sanitized/
initial-two-signatures.json --response-timeout3600; separate sphincs-resumed-* files.
It does not count saved signatures as new live comparisons. Full CTest timeout28800.
No complete sanitizer suite claim until actual terminal result.

Text terminal sessions47675,83619,47594 all PASS/closed; do not poll again.
Previous aa5edad bundle verified and all38 changed paths byte checked:
local-backups/MinimaCoreC++-20260911T085951Z.bundle, SHA256
6400b5b78456a71a02749731decfe039c570d43e98d01255f607f5da395a13ac.
Create and byte-verify this checkpoint's own recovery bundle after committing.

Remaining production: mysql/mysqlcoins, benchmark/jnitest/jniminingtest, help,
P2PSwapLink (entirely commented Java), Streamable, jnifunctions, ArchiveServer.
Help source fully inspected but unported: its fixed default list depends on both
MySQL command constructors. Do not omit them or introduce fake backend stubs.
JNI source query remains pending; no library bodies found in pinned/sibling code
or exact public GitHub symbol search. Continue independent work without inventing
those implementations. Base test private resource helper remains unrepresented.
Keep local commits and verified recovery bundles; GitHub destination unspecified.

Earlier notes below are historical where contradicted by this checkpoint.

---

# Latest checkpoint — test command validated; TransactionTest sanitizer complete

PreviousHEAD83afe92; gitlog identifies currentcommit. Full457-file goalACTIVE.
400counterparts=356production+44originaltests;57absent=13production+44tests.
Allpresentimplemented_partial/allparitynot_verified. BothcompletebuildsPASS.
248CTestentries/fullsuiteNOT run.457lessons/9246links/noauditerrors.

Newcommands/base/test.cpp/hpp preservespublicconstructor,params,reply,log/refresh/
log order,getFunction andfull100000-random-address main. Registeredactualrunner.
UsesactualMinimaDB refresh (archive thenTxPoWSqlDB),Logger,MiniData/Address.
IMPORTANT: unusedprivateJava getFileFromResourceAsStream isUNREPRESENTED; no native
classpathresource serviceexists. Explicitfidelitygap,notfullclassparity.
Normal+SANbase_test_command_differential.py reuseexistingMinimaDBOracle/minima_db
andSQLfixture/projectnormalization unchanged;274programs/616ops/114checksPASS,
30.943snormal/62.695ssan,same572f1a50...transcript. Actualbothmains100000iterations
finishwithonlyStart/Finishmarkers. Sourceenvelope39bytes starts0x01=>305bits=>61
base32digits+Mx=63. No reductioniniterationcount.

native_base_test_startup.py reusesNode/ready/key_state fromrestorestartuphelper.
Usescompiledbase-test-command-oracle (runcommanddifferentialfirst;CTestDEPENDS).
Fresh/restartedactualJava/C++solo/noconnect/H2nodes:fourprocesses,12testcommands,
8keys retained,SQLfilespresent,statusvalid,gracefulshutdown. NormalPASS29.629s;
SANPASS149.797s. InitialnormalnodeFAIL onlyover-strict emptystderr fixture; native
clog containsordinarylifecyclelogs. Originalreportretainednormal/initial-node-
results.json. FixeddriverdupsstderrdescriptorbeforeNode.finishcloseshandle,
retainsalllogs+requireszeroexit/UBSANhalt+rejectssanitizerdiagnostics. No logging
productionchange. Evidenceinvalidation/base-test-command-work-in-progress.

23405oldTransactionTestsanactualmain isTERMINAL0/closed. Final169cases/342checks
PASS1411.275s,exactJava/native stdout1931chars,expectedfalse221ops,stderrEMPTY,
bothmains exit0. Same11fb7a34...transcriptasnormal. Finalreport/demooutputcopied
into transaction-test-work-in-progress/sanitized;progress/provenance/guidesclosed.
Do NOT poll23405again.

LIVE — revalidate before restart/terminal inference:
31941 originalsanSPHINCSsigner /tmp/minima-sphincs-sanitized-test.log
89965 normalfullsphincscommand /tmp/minima-sphincs-command-full.log
24343 sanitizedfullsphincscommand /tmp/minima-sphincs-command-sanitized-full.log
Latestobservedcommandnormal129cases0failures:126preliminary+inline/file/testfull
signatures allmatched;one/eightinputtransactionfullsignaturesremain. SAN126/0
preliminarycomplete,fullsigningactive. Oldsignerprogress158/0,thirdfullsignnext.
Oldsignerloadedoldnull-logcapturefixture,expectedonecase299errorresolvedseparately.
No fullsan signerpassclaim. No sourceforests reduced/restarted merely forquiet.

TERMINALnew:24120normalfixturebuildPASS;22873normalcommandcomparisonPASS;
84364fullsanbuild+comparisonPASS;75824fullnormalbuildPASStheninitialnodefixture
FAIL(stderr);27942fixednormalnodePASS;55110sanH2nodePASS;38115publicrepoqueryPASS.
Do NOT polltheseagain. Prior1733bundlejob83afe92terminal and26pathsbyteverified.

JNIgapinvestigation: readpinnedbase/benchmark.java,jnitest.java,utils/jni/
jnifunctions.java. Native methodbodies absent; separateSystem.loadLibrary(native).
No matchincurrent/siblingCpp. PublicGitHubownerrepos+minima-global reposqueried;
exactghsearchcodehashHeaderWithDiffreturnsONLYfourpinnedJavafiles. No nativebody
found. Asyncquestionpendingaskinguserforlibnative source/buildlocation; noapproval
needed,nooverallblockedstatus becauseplentyofotherworkremains. Do notinvent JNI
algorithm or claimunloadednative-errorstubs fulfillallloadedbehavior.

NEXT: pollthreeliveSPHINCSruns;saveactualterminalreports/resolvefailuresifany.
Remainingproduction: mysql/mysqlcoins,benchmark/jnitest/jniminingtest,base/tutorial,
commands/help/tutorial/whitepaper,P2PSwapLink(commentedJava),Streamable,
jnifunctions,utils/mysql/ArchiveServer. Plus44originaltests andallparitygaps.
Goodnextindependentscope: help/tutorial/whitepaper source/readactualcandidates/tests
first, orremainingoriginalobject/VMtests. Do notloseprivateclassloaderhelpergap.
Keepcommitting/recoverybundles; do notpush unspecifiedGitHubremote.

Earliernotesbelowarehistoricalwherecontradictedbythischeckpoint.

---

Normal expanded preliminary cases now126/126 match, including the NUL-path regression; full signing remains live. Sanitized preliminary progress91cases/0failures at this snapshot.

# Latest checkpoint — sphincs command compiled; full suites LIVE

PreviousHEADf62253e; gitlog identifies latestcheckpoint. Full457-filegoalACTIVE.
399counterparts=355production+44originaltests;58absent=14production+44tests.
Allpresentimplemented_partial/allparitynot_verified. BothcompletebuildsPASS.
246CTestentries/fullsuiteNOT run.457guides/9243links/noauditerrors.

Newcommands/base/sphincs.cpp/hpp preservesallfiveactions andactualrunnerregistry.
ReuseSPHINCS/SPHINCSUtils/MiniFile/CommandResultfirst,status,reject;sharedprivatecoin
formatterandJavaUnixFileabsolute/length. JavaStringgetString iscast,so bothsource
errorbranchesuseexistingreject. Preservefileprecedence,MAX8inputs,outputsfalse,
sourcechildorderandexplicitcleanuponly. GuardNULbeforeexists/delete(JavaFile).

FixturesSPHINCSCommandOracle.java/support/sphincs_command.cpp executeactualclass
andrunner. Actualtxncreate/deletewithrealTxnDB; otherchildprototypesrecordrows
andfundsyntheticcoinswithinfixtureonly. No productionhooks/networksubmissions.
DriverreuseForestRunner/walletwrappernormalization(UUIDs,NPE/ClassCastprose).
Quickinitial125cases/24checksPASS113.224s (beforeNULguard/regression). Preserved
normal/initial-quick-results.json. Firstsetupfailurebeforeanycase wasfixtureTxnDB
notinitialized; fixedusingexistingWalletCommandsFixture/reflectionapproach.
Newfullsuites126quickcases+5fullsignatures: inline,file,test,1inputsuccess,8input
postfailure. Eachactualsignforestfull16x65536,TreeKey192/4. CTesttimeout14400;
Runnerresponse3600,teardown120;nativeemptystderrrequired. No fullpassclaimyet.

LIVE — revalidatehandles,neverrestartsolelybecausequiet:
89965 normalcompletebuildPASS then fullcommanddriver /tmp/minima-sphincs-command-full.log
24343 sanitizedfullcommanddriver /tmp/minima-sphincs-command-sanitized-full.log
31941 earlieroriginalfullsanitizedSPHINCSsigner /tmp/minima-sphincs-sanitized-test.log
23405 earlierTransactionTestsanitizedactualmain /tmp/minima-transaction-test-sanitized.log
Finalcommandreports:build{,-sanitized}/sphincs-command-full-results.json.
Incrementalcommandreports:sphincs-command-full-progress.json. Quickreportseparate.
Oldsigner31941loadedoldnull-logfixture;oneexpectedcase299errorresolvedbylogaudit.
MostrecentownedCPUcheck56123signer84%CPU/58min;77776Transactiondemo89.5%/18min,
bothactualnativecompute. Oldsignerprogress158cases0failures(twofullsignaturesdone).

TERMINALnew:62772initialfixturebuildPASS;69210initialfixturetestFAILbeforecases;
67212fixedinitialquickPASS;68746fullsanitizedbuildPASS. Don'tpolltheseagain.
4989 wasfunctions-execwaitcell,closed andreturned67212.
Normalfullbuildlog/tmp/minima-sphincs-command-full-build.log reaches100%;driver
launchedvia&&,so normalcompletebuildPASS. No reboot/restartneeded.

NEXT: inspectliveprogress,finishactualterminalreports/evidence/reviewwhenready.
Ifcommandtests fail, inspectactualfailurebeforeediting. Driverpostedassertions
expecttransactionJSONstatearray66 andalloutputstorestatefalse. Controlledpost
rejectionmustcleanup; source'signoredintermediatefailurestatusesneedbroadercases.
Do notrerunfullforestsforfixture-onlychangeswhenexistingvalidvectorscanreplay.
Keepcode+evidence+guides/provenance/source-mapreviewconsistentandcommit/bundle.
Fullnodeparityandremaining58filesstillrequired; do notdeclare100percent.

Earliernotesbelowhistoricalwherecontradictedbythischeckpoint.

---

# Latest checkpoint — TransactionTest translated; two sanitizer runs LIVE

PreviousHEAD2087648; gitlogidentifiesnewercommit. Map398=354production+44tests;
59absent=15production+44tests. FullgoalACTIVE,allparitynot_verified.
BothcompletebuildsPASS.245CTestentries/fullsuiteNOT run.457guidesaudited9240links.

LIVE, revalidatehandlesbeforeterminal/restartinference:
31941 original fullsanitizedSPHINCSsigner /tmp/minima-sphincs-sanitized-test.log.
23405 TransactionTestsanitizedactualmain /tmp/minima-transaction-test-sanitized.log.
Old11888 originalsanitizedUtils isnowTERMINAL1/closed; donotpollagain. Fullactual
signaturesmatchnormal,937opVMacceptance,1281.096s,269cases/267matched,1679assertions.
ONLYthetwo oldCHECKSIGtextdifferences255/264,alreadyfixedby268-casereplays.
OriginalfullsanUtilsreports/vectorscopiedunderutilsvalidation/sanitized.

NewTERMINAL:85223fixturebuildPASS;21698normalfullbuild/replayPASS;
6000sanfullbuildPASS;71927sanreplayPASS;82779normalTransactionTestdriverPASS.
Normaldemo169cases/342assertions,286.371s;exactJava/C++stdoutinclmessage/vars,
validfalse,221operations. Actualmainsbothzeroexit,nativestderrempty.
San169publiccases/338assertionsPASS,actualmainstillrunning. SharedUtilsreplay
268/1677passednormal16.026s/san29.676s,unchanged04c3b45c...transcripthash.

Newutils/sphincs/TransactionTest.cpp/hpp preservesentirepinnedmain,including
fixed30DE...6822EscriptkeyvsderivedDBCD...050BF,LT1proofloop,AMOUNT/TOKENID
separators,7input/2outputcoins,paddyToken scale1/amount1,tokenonlyonoutputs,
all66signaturestates andnoexplicitcoinIDrecompute. Actualsigner/FORSnotreduced.
SPHINCSHelpers.hpp extractsUtilsformatter(separatorparams)andstateinstaller
unchanged;Utilsstillperformspost-stateprecomputeCoinID+calculateTransactionID.
MainusesactualContractwithwitnesscoinproofsandTOTIN/OUT. No shortcutskipsforest.

TransactionTestOracleextendsHORSTOracleandusesObjectOracle.coin; nativefixture
usesCoinFixture.hpp. Driverindependentlyassemblesformatterstrings/token/message,
logsnull/empty/Unicode/linebreak/NUL;actualmainchecked_demo(timeout3600each),
CTest7500. FullSANmainpending.NoJvmbackendinC++.
Evidence/studentlab/provenanceupdatedundertransaction-test-work-in-progress.

NEXT: poll31941+23405. FinishdemoSANevidence/reviewwhenactualterminal. No redundant
restart. Thencommands/base/sphincs.java: source311linesfullyreadthisturn,still
unported. NativeCommandRunnerregistry inCommandRunner.cpp:126; readcurrent
Command/Runner/MiniFile/TxnDB/CommandResult helpers+tests/siblingbeforewriting.
Sourceactions generate/sign/verify/transaction/test; transactionnestedcommands
create/addamount/delete/txnscript/txnmmr/txnpost;MAX8inputs;disableoutputkeepstate.
Do not silently replace privateoutCoin helper's sourceformat;sharedformatterfits.

Earliernotesbelowarehistoricalwherecontradictedbythislatestsection.

---

# Current work — SPHINCS utilities validated; two full sanitizer runs LIVE

Full457-file goal ACTIVE; do not mark complete. Branch/project/executable MinimaCoreC++.
HEAD before this checkpoint05d0171; gitlog identifies newer localcheckpoint.
Map397=353production+44originaltests;60absent=16production+44tests.
All counterparts implemented_partial, allparitynot_verified. BothcompletebuildsPASS.
244CTestentries;fullsuiteNOT run. All457lessonsauthored/audited,9237links/noerrors.
NoGitHubpush. Bundles localonly; originisupstream. Never claim off-machinebackup.

## Revalidate LIVE handles before restart/terminal inference

31941 original sanitized SPHINCS driver: /tmp/minima-sphincs-sanitized-test.log.
11888 original sanitized SPHINCSUtils driver: /tmp/minima-sphincs-utils-sanitized-test.log.

They loaded older fixture/error code and remain doing actual full-size signing.
Signer runs3full signatures+actualJava/native mains; utility runs1fullsign+scripts.
Each forest16x65536leaves; TreeKey192/4. Response/demo1800s; signerCTest7200s,
utilityCTest3600s;ForestRunnerteardown120s. Do NOT shrink parameters/restartsolely
because stdoutquiet. LastownedCPUobservation: signerPID56123,99.6%CPU,22MBRSS;
utilityPID64722,98.8%CPU,404MBRSS. NoOOMevidence. Fullsizedworkmustfinish.
Incrementalreportsinbuild-sanitized; finalreports onlyaftercompletion.
One-second sample94021 is TERMINAL0: owned56123insideFORS::FORS->HORST
public-key/MMRconstruction. Excerptsavedunderutilsvalidation/sanitized.
No productionprocessinterruption; actual full-size computation confirmed.

## Completed processes — do NOT poll again

3008 normal signer ORIGINAL driver terminal1:301cases/300matched,1373independent
assertions,1138.085s; ONLYfailurecase299null-log wasJavafixture captureerror.
Actualsource mainsmatched,20667byte signature,publicDBCD39B760A213147B1CB3829B65E98D57EFCE11B4C72E0A431FE4013FB050BF,total1358954496,verifytrue.
Newnull-safeSPHINCSOraclecaptureString.valueOf;explicitlogaudit3cases/6assertions
passedbothbuilds. Originalreportretained;auditsupersedesONLYcapturedefect.
Normalactualsignatures/messagesFFEEDD(repeatedstatic/instance)andemptyallmatched.

86908 normalUtils ORIGINAL driver terminal1:269cases/267matched,1679assertions,
311.165s. All250helpersmatched. Actualtransaction2inputs/2outputs signedfullsize;
bothKISSVMsvalidin937operations. ONLYcases255/264errorstringmismatchonstate100
malformedsignature. FixedproductionTreeKey nullableSignature overload/CHECKSIG
pointerdelegation/ContractJavaNPEformatting. Originalreportretainedunder
validation/sphincs-utils-work-in-progress/normal/initial-...results.json.

72585 normalfixedreplayPASS268cases/1677assertions8.382s.
57201 sanitizedfixedreplayPASS268/1677,28.486s.
Bothtranscript04c3b45c0b9ca9e4430e5a516ca8599d2540af246d9f98437a9e0f43a08c8618.
Replayusesactualsavednormaltransactionvectors;NOnewforestgeneration.
53459normalfullbuild+4regressionsPASS53.54s.
69483sanitizedfullbuild+4regressionsPASS135.50s.
Regsobject9464/KISSVM4471/envelope1449/log3. BothfullbuildsPASS.
Initialotherhandles3208compilefailure(privateJSONArrayvalues,fixturefixed),
73001fixturecompilePASS,14758sanfullPASS,99044normalfull+regsPASS,
32789bothlogauditsPASS,9931initialsanregsPASS. Allterminalclosed.
PriorFORS25692/89215terminal0,evidence/versionedreviewclosedlastcheckpoint.

## Implementation and evidence

New SPHINCSUtils.cpp/hpp exactsourceconstant;rawASCIItransactionmessage includes
scaledtokenamounts/keepstate/order/counts;noinputsIndexOutOfBoundsException.
Setupsequential66states;partialfailureleavespriorwritesandcachedID;gapsretained.
ReusesTransaction/Coin/Token/StateVariable/MiniString/TxPoWGenerator.
MiniDataVersion.hpp extractedunchangedfromFORSSignatureIO-onlylogger/optional.
CurrentTreeKeynullableSignatureoverloadthrowsobservedJavaNPEdiagnostic;
CHECKSIGdelegatesnullconvertedSignature;ContractrecognizesJavaNPEtype.
Otherunrepresentednullfields/Javaarrays/subclasses/concurrencyremainopen.

OracleusesObjectOracle.coin/FORSOracleandactualContract;nativeusesCoinFixture.hpp.
Utilsdriver--replay PATH runs250helper+18scriptcheckswithoutresigning;defaultstill
runsgenuinefullsign. Sourcefixtures/versionedvectorsunder
validation/sphincs-utils-work-in-progress. VectorreplaySHA256
cf04c5628efbe3eb071826153b44bc9eed7133a48b3fe9e8f3fc4d0ed6b7a7b4.
NormalSignerfinalevidenceunder validation/sphincs-work-in-progress/normal.

## Progress file isolation

Replay nowwrites sphincs-utils-replay-progress.json; fullrunuses
sphincs-utils-progress.json. Initialreplayssharedthefullrunprogressfilename,so
older268case/cross-writer-script18snapshotcanbereplay,NOToriginalfullrun.
Do not infer fullruncompletionfromit; revalidate31941/11888andfinalreports.
26053progressisolationauditterminal0; reran268/1677normalreplayPASS.

## Next actions

Revalidate31941/11888;preserveoriginalfullsanreports,oldfixture/fidelityfailures,
actualsignatures/mainsandcomparewithfixedreplay/logaudits. Investigateunexpected
mismatches/timeouts from actualevidence,doNOTrelaunchliveprocesses.
Thenfinishcheckpointstatus/evidence/reviewandlocalcommit+recoverybundle.
Nextproduction: pinnedSPHINCS/TransactionTest.java thencommands/base/sphincs.java.
Finishreusegatebeforeeachscope:readsource/currentdeps/tests/relevantsiblingand
reportexactreusepaths. Do not use siblingbinaryTreeKey/RSA CHECKSIG.
Userrequestedteachingbackfillalreadycomplete;nonewagentsneeded.

Recovery: reusable /tmp/minima-hashtest-backup.py requirescleanworktree,creates
bundle--all,verify,independentclone/pin/connectivity. ThenbytecompareeveryHEAD
changedpathviarecoverygitcat-file;saveSHAmapinbundlesiblingmanifest.
Gitmutations/builds/testsrequire_escalatedauto-review;GRAPHIFY_SKIP_HOOK=1commits.
Do not rerun old /tmp docgenerators; they overwrite later corrections/status.
