# Current work — multisig workflows validated; preserve this checkpoint

Project/branch MinimaCoreC++. Added send/multisig.cpp/hpp and multisigread.cpp/hpp,
common support/MultisigCommands.cpp, registration and unchanged shared contract.
Actual create/list/getkey/spend/sign/view/post use native child commands. Preserves
source raw read addresses, normalized full addresses, duplicate state matches,
first spend match, scaled change, raw command text, repeated signing and late errors.

Final normal and ASAN/UBSAN comparisons pass 340 programs / 1,089 operations /
22,269 SQL observations. Real exported signatures and signed posting/mining checked.
Both full builds pass. Four affected suites pass both: registry 1,265 cases,
transaction commands 1,129 cases / 6,904 ops; witnesses 418 programs / 1,128 ops /
8,637 SQL; offline send 201 programs / 558 ops / 9,773 SQL.

Malformed import exposed existing txnimport generic-runtime error; corrected to
JavaNullPointerException with replacement-ID parsing before null dereference.
Fixture exception recording now preserves embedded NUL via getMessage(). Optional
actual-child tracing, configured proof fixtures and verifyTxnFile added. Initial
failures saved. Final subdirectory case observes absolute temp-root export path;
source MiniFile treats slash-containing paths directly. Generated root nested file
was removed after identifying the test artifact. All current handles terminal.

Mapping 373 counterparts (329 production + 44 mapped tests), 84 absent (40 production
+ 44 tests), 214 CTest entries. All 457 student guides authored and structurally
checked. All counterparts still implemented_partial/not_verified. Evidence in
validation/multisig-work-in-progress; scope excludes durable SQL, live-node/full
API/reference/concurrency parity. No full current 214-suite pass asserted.

Ready for local commit and verified bundle recovery; inspect HEAD/latest ignored
local-backups manifest for preservation state. Prior HEAD 28af8f8 and bundle
MinimaCoreC++-20260910T224625Z.bundle, SHA256
2a35f1d7e0c52eecd39f4592a878f177109df88796fec8e9664a8707981fa5e3.
Reuse /tmp/minima-hashtest-backup.py and additionally verify both multisig sources,
lab and sanitizer report. No GitHub push. Full goal remains active.

Read-ahead only: network/nodecount.java and base/incentivecash.java fully read;
no native changes for them. nodecount depends on HashSet socket-address ordering,
real NIO ping, float arithmetic and CSV; incentivecash saves UserDB before raw URL
PUT/JSON parsing. Next work still requires inspecting local/sibling implementations
and reporting reuse. Prior reads P2PSwapLink.java (entirely commented/inactive) and
Streamable.java (active stream interface) remain unported. Historical notes follow.

# Current work — tokencreate validated; preserve this checkpoint

Project/branch MinimaCoreC++. This turn is PROGRESS. Added base/tokencreate.cpp/hpp
and registration. Reuses actual numeric/object/wallet/MMR/signature/generator/miner
components; preserves source-specific direct selection, raw parameter casts,
optional limits, negative burn, JSON-name mutation and late failures.

Both final comparisons pass 335 programs / 1,349 operations / 26,719 SQL observations.
Independent supply/color/change/conservation, transaction signatures and token
metadata signatures checked. New fixture reserveMining uses the initialized miner
queue; the old selection-only miner opcode allocated a queue-less Java instance.
The initial fixture mismatch and compile correction are retained.
Both full builds and affected registry/mining/custom-token tests pass. Reports in
validation/token-creation-work-in-progress. Scoped review, provenance and lab saved.
All 457 student guides authored and structurally audited. Mapping 371 counterparts
(327 production + 44 tests), 86 absent (42 production + 44 tests), 213 CTest entries.
All counterparts remain partial; no full current 213-suite/node parity claim.

All current work handles terminal after finalization; no live old baseline.
Ready for commit and verified bundle recovery. Inspect HEAD/latest local-backups
manifest to establish preservation state. Prior HEAD 5137cd1, prior bundle
MinimaCoreC++-20260910T223019Z.bundle, SHA256
06dfcee39acc4176067fd485f2b1b60b4234cecb90eef8fd6a94c43194e0680b.
Use /tmp/minima-hashtest-backup.py; additionally verify tokencreate source, student
lab and sanitizer report in recovery clone. No GitHub push. Full native-node and
457-file goal remains active. Older notes follow for historical context.

# Current work — burn metrics validated; preserve this checkpoint

Project/branch MinimaCoreC++. This is PROGRESS, not full-goal completion.
Added burn.cpp/hpp with real TxPoW/tree/TxPoWDB/MiniNumber composition. Preserves
1/10/50 windows, repeated-instance accumulation, duplicate references, lower-middle
median, hidden direct action and runner allow-list behavior. OpenJDK-derived
JavaNullableNumberSort.hpp preserves list state on null comparison failures;
license/notices/hash retained. Empty medians return a shared helper zero.

Mapping: 370 counterparts (326 production + 44 tests), 87 absent (43 production + 44 tests),
212 CTest entries. All 457 student guides authored and structurally audited.
Both final burn comparisons pass 364 programs / 4,361 operations / 30,113 SQL; both full builds pass.
Affected runner 1,265 cases, history 484 programs / 1,489 operations / 52,194 SQL,
scanchain 320 programs / 2,027 operations
pass both. Regressions preceded final helper guards/identity correction; final
burn corpus/full rebuilds cover final source. No full current 212-test suite claim.
Evidence:validation/burn-metrics-work-in-progress. Initial build, incorrect test
expectations and real null-sort mutation failure retained. Scoped review and lab
saved. Full native-node/inherited/reference/durable SQL work remains unfinished.

All work handles terminal when this note was finalized; no active old baseline.
Original 204-test baseline terminal/composite reruns documented in older notes.
Ready for local commit and verified bundle recovery. Inspect HEAD and newest
local-backups manifest to confirm preservation finished. Prior HEAD 3bb9976;
prior bundle MinimaCoreC++-20260910T220811Z.bundle. Reuse
/tmp/minima-hashtest-backup.py; additionally verify burn source, sort adapter,
license, student lab and sanitizer evidence in the recovery clone. No GitHub push.

Older notes follow; current counts/status above supersede them.

# Current work — txnauto validated and older baseline finished

Project/branch MinimaCoreC++. This turn is PROGRESS. Added txnauto.cpp/hpp,
registered command and a nullable static helper. Shared txnutils burn algorithm
through src/support/AutomaticTransaction.hpp: txnauto enables early validity,
no link assignment, always signs, ignores address as source does. Existing burn
path preserves late validation, exclusions, link and optional signing.

Mapping369 counterparts (325production+44tests),88absent(44production+44tests),
211CTest. All457 student guides authored/audited. All counterparts still partial;
full457-file/native-node goal active/incomplete. Lab automatic-transaction.md
written before tests. Normal and sanitizer414programs1655ops29925SQL pass
34.16s/177.74s. Initial one-byte/default-link expectation failure retained.
All affected checks pass BOTH current builds: inputs671programs2734ops41653SQL;
funding736programs3418ops39016SQL; runner1265cases. Full normal+sanitizer builds
and relinks pass. Evidence:validation/automatic-transaction-work-in-progress.

NO LIVE baseline now. Original sanitizer handle42976 TERMINAL exit8 after
5556.22s:202/204 initially, only MiniByteTests and nio_message_differential fail.
Previously saved reruns both pass; NIO unchanged1918programs4677packets same100s
child deadline. Whole terminal log+summary now retained in
validation/shared-json-work-in-progress/sanitized/full-ctest-initial.txt and
full-summary.json. Progress/provenance updated. This is composite older baseline
evidence, not fresh full-current211suite. Old normalbaseline203/204+MiniBytererun.
All sanitizer consumers now rebuilt; no baseline executable protection needed.
All current jobs terminal; do not restart old handles. Last numbered checkpoint362.

Ready to commit/bundle this scoped change; inspect Git HEAD/status and newest
local-backups manifest to see whether preservation finished. Prior saved HEAD
af59f634323187cdab2c5a6ce9bd6cf8576f843a, verified/restored bundle
local-backups/MinimaCoreC++-20260910T215307Z.bundle, SHA256
f23af117cbd6c733eb2c28971e173fd961aa6d67cd8a8c87ceee95448a69e34b.
Reuse /tmp/minima-hashtest-backup.py, additionally verify txnauto source, lab and
sanitizer report in recovery clone. No GitHub push. Continue missing production,
tests, full inherited APIs, native entry point and durable SQL integration.

Older notes follow; current counts and terminal statuses above supersede them.

# Current work — peers port and integration checks pass

Project/branch MinimaCoreC++. This goal turn is PROGRESS. Added peers.cpp/hpp,
registered command, real Main/P2P/checker/MiniFile/RPCClient composition.
Mapping now 368 (324 production + 44 tests), 89 absent (45 production +44tests),
210 CTest. Full 457-file goal remains active/incomplete; all counterparts partial.
All457 student guides authored. Lab peers-command.md was written before tests.

New peers_command_runtime reuses main_runtime + MainNetworkFixture in a distinct
executable. New driver reuses existing Main projection, Runner and loopback Server.
271programs1355operations48HTTPrequests/runtime pass both (3.79s/5.27s).
Initial temporary-copy lifetime and missing DB service fixture failures saved.
All six affected checks pass current binaries: network222programs794ops,
status124programs562ops, runner1265cases each build. Normal full build passed.
Saved evidence under validation/peers-command-work-in-progress. Scoped review
and lessons updated. Regeneration/audit must pass before committing.

LIVE original sanitizer baseline42976: /tmp/minima-shared-json-all-tests-san.log,
last seen177 coin_search_command_differential. Poll same handle. Do not overwrite active baseline
executables; peers checks used their separate target. Baseline uses original204
entries, not a current210-suite run. Initial MiniByte and NIO failures have
passing reruns saved in shared-json-work-in-progress. Last numbered checkpoint362.

This peers checkpoint is ready to commit and bundle. Inspect Git HEAD/status and
latest local-backups manifest to determine whether preservation finished.
Prior saved HEAD cb8cf01d8264ae54de5188e74673b2906689736b; restored/verified bundle
local-backups/MinimaCoreC++-20260910T214013Z.bundle, SHA256
188d0e80b09b6c7b6c8251d653bc5e9e2d46593cdf74be13e766637b57c30821.
Reuse /tmp/minima-hashtest-backup.py then check peers source/lab/sanitizer evidence
in recovery clone. No GitHub push. Continue remaining native APIs, files, tests,
entry point and durable storage; no full parity/completion claim.

Older notes follow; above supersedes older counts/job states.

# Current work — tokenvalidate validated; local preservation checkpoint

Project/branch MinimaCoreC++. Full 457-file native-node goal remains active and
incomplete. Mapping: 367 counterparts (323 production + 44 tests), 90 absent
(46 production + 44 tests), all existing counterparts still partial. 209 CTest.
All 457 student guides authored; teaching coverage does not certify parity.

New tokenvalidate.cpp/hpp matches pinned signature/web validation and reuses
searcher, Signature/TreeKey, JSON parser, RPCClient and Java string trim.
Signature converter now catches/logs IOException only; non-I/O exceptions escape.
Null imported tokens now raise JavaNullPointerException in getAllTokens.
TokenValidationOracle and token_validation_runtime reuse wallet fixtures;
TxPoWSearcherFixture reset extracted unchanged from two existing fixtures.
Student lab written before testing: student-guide/new-tests/token-validation.md.

126 programs / 254 operations / 33 HTTP requests per runtime pass normal and
ASAN/UBSAN (3.78s / 10.26s). All seven affected comparisons pass both builds:
9464 object, 4471 KISS, 12289 command, 1265 runner, 1129 transaction programs /
6904 operations, 189 searcher cases, 477 RPC cases. All fourteen reports retained
under validation/token-validation-work-in-progress/{normal,sanitized}.
Initial command regression failed because restored production logs polluted the
fixture stdout protocol. command.cpp now copies transaction_commands.cpp stderr
logger routing, matching CommandOracle. Fresh minima_command rebuild/reruns pass
13.72s normal / 24.57s sanitized. Initial sanitizer seven-test batch had rebuilt
minima_commands (different target); its command pass was stale, superseded by
that singular-target rebuild. Initial failure and both reruns retained.

Full normal build/relink passed before the final fixture-only change; the changed
fixture was rebuilt afterwards. Affected sanitizer binaries rebuilt. Original
full sanitizer baseline remains LIVE handle 42976, redirected authoritative log
/tmp/minima-shared-json-all-tests-san.log, latest test 162 wallet key commands. Keep
polling same handle. Do not restart or overwrite active baseline executables.
MiniByte and NIO initial baseline failures have passing explicit reruns saved.
No full-current-209-suite or full-node parity claim. Last numbered checkpoint 362.

This checkpoint is ready for a local commit and verified recovery bundle; inspect
Git HEAD/status and latest local-backups manifest to learn whether those steps
finished. Prior HEAD 7bd0b9d2b8195a25c573201a74bf4246ce74d1dd, verified bundle
local-backups/MinimaCoreC++-20260910T212502Z.bundle, SHA256
0ec3461eabd1aae4f09915dd3e4d27a971e35ec79a286bfc522988a396f9e8b6.
No GitHub push. Reuse /tmp/minima-hashtest-backup.py for bundle/clone checks,
then additionally verify tokenvalidate code, lesson and sanitizer report in clone.
Next: finish pending audit, continue missing production/test counterparts and
inherited API/runtime integration. Native entry point and durable SQL unfinished.

Older notes follow; their counts and job states are superseded above.

# Current work — scanchain passes both builds and affected regressions

This goal turn is PROGRESS: translated and registered scanchain with real
Command/MinimaDB/tree/TxPoWDB/date/transaction reuse. Mapped count is now 366
(322 production + 44 mapped tests), 91 absent; 208 CTest. All 457 guides authored.
Full 457-file working-native-node goal remains active/incomplete.

New source: cpp/src/org/minima/system/commands/base/scanchain.{cpp,hpp}.
New test: cpp/tools/scan_chain_differential.py, scan_chain_runtime reuses wallet
fixture in a separate executable. Optional historyFixtures time field and
scanDetails operation added; old defaults unchanged. Lab written before tests.
320 programs / 2027 operations pass both: 23.57s normal, 67.47s ASAN/UBSAN.
Two fixture setup failures retained (string state brackets, MiniNumber range).
Reply numbers, inclusive/wrapped bounds, extreme dates, duplicates/missing/empty
references, ordinary/burn details, null helper inputs and SQL failures checked.

All affected checks are TERMINAL pass in both builds: registry 45018/33880,
1265 cases; history + TxPoW 6345/65867, respectively 484 programs / 1489 ops /
52194 SQL calls and 680 programs / 2099 ops / 41510 SQL calls. Saved all six
reports and logs under validation/scan-chain-work-in-progress. Newtest runs
70869/46999 terminal pass. Earlier fixture-failure handles terminal; ignore stale
reports, final reports are saved explicitly.

Normal full relink 11412 TERMINAL exit 0; saved normal/full-build.txt, no observed
compiler warnings. All normal executables current. Library, scan_chain_runtime
and runner also current in sanitizer build. Do not relink sanitizer baseline executables
while they are active. LIVE original sanitizer full baseline: 42976,
/tmp/minima-shared-json-all-tests-san.log, last started 137. Keep polling same
handle; original normal baseline terminal and saved. MiniByte and NIO initial
sanitizer failures have passing reruns; no later failure seen yet.

Before ending: commit
scanchain implementation, lab, source-map and evidence locally with
GRAPHIFY_SKIP_HOOK=1, then create/restore-verify a new bundle. Last saved HEAD
051c4dd3106f9b2bf8689f38c26b27478845f63b; verified bundle
local-backups/MinimaCoreC++-20260910T211432Z.bundle, SHA256
9ffa09908df58f81375949b7664a982a4b19f049e8a9d6c5eac329bb94844dc8.
No GitHub push. Last numbered checkpoint 362; no full 208-test pass claim.

Older notes follow; their job/count states are superseded above.

# Latest saved evidence — affected integration checks pass

Project/branch MinimaCoreC++. Coinimport implementation saved at
66f6b91ee64438ba592a6d86747ac401f0b458f9. Verified local bundle
local-backups/MinimaCoreC++-20260910T211111Z.bundle, SHA256
1ee57d56ee154eb7288b0092abdc97b075d895c3e29e04c772cd095253d0cbc6.
Recovery clone checked HEAD, connectivity, source pin/gitlink, coinimport code,
lab and sanitized comparison report. Local only; no GitHub push.

Normal affected batch 1937 TERMINAL exit 0: all 20 integration tests pass,
131.05 seconds. Current normal binaries rebuilt after coinimport/hashtest and
Authorizer. Sanitized node-state direct driver 18296 TERMINAL pass: 363 programs,
1411 operations, 21009 SQL calls, using separate coin_import_runtime target.
Registry build/test 82445 TERMINAL pass, 1265 cases. All four comparison reports,
normal 20-test summary and three logs saved in coin-import-work-in-progress.

Only full baseline still live: sanitizer 42976, original 204 entries, log
/tmp/minima-shared-json-all-tests-san.log (last started 100). Normal baseline is
terminal 203/204 with corrected MiniByte now explicitly passing CTest both builds.
NIO sanitizer unchanged driver replay 11958 TERMINAL exit 0, log
/tmp/minima-nio-message-san-rerun.log. All 1918 programs / 4677 packets pass with
the unchanged 100-second child limit and native executable. Report, initial
timeout excerpt, rerun log and summary saved in shared-json-work-in-progress/
sanitized. Cause of initial timeout is not proven; builds were finished before
rerun, but full sanitizer baseline remains active. Preserve both outcomes.
Do not restart the full sanitizer baseline or overwrite its active binaries.

Mapping remains 365 counterparts (321 production + 44 tests), 92 absent;
207 registered CTest entries. All 457 teaching pages authored. Full goal active,
incomplete; last numbered checkpoint 362. Next save these evidence/docs changes
and verify a new bundle, then continue missing native functionality.

Older notes follow; current states above supersede their live-job notes.

# Latest active jobs — normal full build complete; affected checks started

Normalfullbuild40482 TERMINALexit0,noobservedwarnings. Allnormalbinariesnow
linkedcurrentcoinimport/hashtest/Authorizer. Newnormal20-test affectedCTestbatch
started: /tmp/minima-post-rpc-normal-regressions.log (inspectlivehandleinthread).
Sanitizernode-state directdriver18296 uses --native-target coin_import_runtime;
registrybuild+CTest82445. Theirlogs /tmp/minima-coin-import-node-state-san.log
and /tmp/minima-coin-import-registry-{build,test}-san.log. Pollandinspectresults.
Originalsanfull42976 remainsLIVE(last93), normalfull29381 terminal.
MiniByteexplicitCTest48309/2233PASSboth. Source-map365/92,audit9082/2634/0.

# Current work — coinimport comparison passes, full normal baseline finished

This goalturn is PROGRESS: added compiled/registered coinimport translation and
142program390op Java comparisons, matching both builds. Mapping365 counterparts
321prod+44tests;92absent;207CTest. Full457goalACTIVE/incomplete.

coinimport reuses CoinProof/MMR/MinimaDB/TxPoWSearcher/tree; early tracking path
ignores track/suppliedproof, historical path preserves lateparsemutation order
androotcheck. Newtest reuses walletfixture as distinctcoin_import_runtime so
livefullsuite binaries remainunchanged. NewCoinImportOracle subclassesexisting
WalletCommandsOracle, sourcegenerated full/prunedMMR/proofs. Extraallcoins/root/
finalized observations gatedbycoinImport. Java logcapturecoversnewSQLprobes with
finallyrestore. node_state_corpus.project optionaltree_sql defaultsunchanged;
newtest includesINSERT+SELECT randomIDs withconsistentcheckedmapping. Optional
--native-target addedtowalletdriver foraffectedtestisolation,defaultwallet_commands.

Coinimport passesboth142/390:normal7.23s,san17.15s. Fourinitialreports saved:
repeat-importexpectation, pruned-spentexpectation, INSERT randomIDdifference,
post-commandobserverlogcapture. No productionchange wasmade tofakeexpectedpolicy.
NewlabwrittenBEFOREtests, guide/source-map/auditupdated(9082links2634anchors0errors).
Affectednode_state/command_runner regressions stillpending; nextlaunchthem.

Normalfullbaseline29381 TERMINALexit8:203/204pass,onlyMiniByteTestsstale catch.
ExplicitMiniByteCTestnormal/sanPASSED0.39/0.31s (48309/2233 shouldterminal).
Savednormalfull204log+summary andbothreruns under shared-json-work-in-progress.
Thisvalidatespriorbaseline5fbb638,NOTallcurrent207tests.

Only originalfullsuite stillLIVE42976 sanitizer /tmp/minima-shared-json-all-tests-san.log
(last89). Twoobservedinitialfailures:fixedMiniByte andnio_message_differential69
(timeout100s in nativeprocess, not yetreplayed). Inspect/replayNIOafterfullsuite
andlowload; don'trestartwholefullsuitefromtimeout. Keepfullredirectedlogauthoritative
becauseconcurrentfocusedCTestruns shareTestingdirectory.

NEXT: savecoinimportWIPcommit+verifiedbundle; runnormalfullbuild nowthatnormal
fullsuitefinished, affectedMain/DB/network/auth/registry/node-state checks. Use
coin_import_runtime forsanitizernode-state viadriver --native-target whilefull
sanitizerusesoriginalwallet_commands. Do not replaceactivefullsuitebinaries.
Lastnumberedcheckpoint362. Latestearliersavedcommit2542fe0; inspectactualHEAD
fornewsave. NoGitHubpush, GRAPHIFY_SKIP_HOOK=1.

Older notes follow.

# Latest saved validation — hashtest and two affected regressions PASS

Hashtest implementation committed00c4867d52ec26eb840f86052955e6a12b68a031.
Verifiedbundle MinimaCoreC++-20260910T205244Z.bundle SHA256
a85b67f26c06f501d048b7435352d1adda32c6a03622b28e6ad2d8feb892ca8f.
Recoverycloneverified HEAD/connectivity/pin/source/evidence;localonly.

Affectedbuild/testhandles9583/37480 now TERMINALexit0. Both complete library
rebuilds and relinked minima_command_runner/minima_network_utilities pass,
noobservednativewarnings. Both2-testCTestbatchespass;1265runner+5668networkutility
casesperbuild. All4reports and2CTestlogs read/savedhashtest-work-in-progress.
Newhashcommand59programs61opspassesboth; source-map/audit364counterparts93absent,
457guides,9079links2634anchors0errors. Current206CTest; fullgoalstillincomplete.

ONLY LIVE testhandles now29381normal and42976sanitizer, original204fullsuites.
Lastnormal144,san59; use /tmp/minima-shared-json-all-tests{,-san}.log forcomplete
output. NewfocusedCTestinvocations also useTestingdirectory, so do not relyon
sharedLastTest.log as sole evidence or --rerun-failed auto selection. Readfull
redirectedlogs, inspectallfailures, explicitlyrerunfailednames. KnownMiniByte
failurealreadyfixed/directpassboth; requiredCTeststillpendingafterfullsummary.

Afterfullsuitesfinish, relink remainingaffectedMain/DB/network/HTTP/auth targets
andcheckRPC/sharedJSONregressions withcurrentbindings/registry. Do not replace
executablesstillusedbyrunningfullsuites. RPC-liveoldtargetalreadytesteddefault
Authorizer binding, but now needsrelink forhashtestregistry ifbroaderintegration.
Nooverall206currenttreepassclaim and no numberedcheckpointyet. Continueporting
unportedfiles once thisvalidation step is settled.

Older progress notes follow; theirLIVE statuses are superseded above.

# Current work — hashtest passes both builds; regression builds/tests LIVE

Full457workingC++goal ACTIVE/incomplete. Mapping364=320production+44tests;93absent.
206CTestregistered. Last numberedcheckpoint362; no new completeness claim.

New hashtest.cpp/hpp registered inCommandRunner, actual TxPoWMiner speed routine
unchanged, real clock/log and 4-significant-digit MH/s. Initial JSON hashes string
mismatch corrected using existing JSONValue::raw(number.toString()). Failure saved.
New lab BEFOREtests. Existing Main fixture compiled into minima_hashtest_runtime
so running204fullsuites retain existing binaries. main_runtime_differential adds
--group hashtest and --native-target(defaultunchanged). 59programs61ops PASSboth:
normal15.65s, ASAN/UBSAN84.39s. Initialsanitizer generic30s response deadline timed
out onactualmillionhashes; onlythisgroup uses120sresponse/240sCTest, rerunpasses.

LIVE affected regression build+tests: normal9583 and sanitizer37480. Buildtargets
minima_command_runner/minima_network_utilities; then2matchingCTestentries. Logs
/tmp/minima-hashtest-regression-build{,-san}.log and
/tmp/minima-hashtest-regression{,-san}.log. Builds unexpectedlyrecompiledlibrary
afterCMake reconfigure; normal/sanitizer flags confirmed Debug and ASAN/UBSAN.
Both full suites had already passed these entries before targets replaced.
Poll completion, inspect reports, save results underhashtest-work-in-progress.

LIVE fullbaseline suites: normal29381 /tmp/minima-shared-json-all-tests.log
and sanitizer42976 /tmp/minima-shared-json-all-tests-san.log. Lastnormal139,san57.
Only observedfailure MiniByteTests stale catch, alreadycorrected/directpassboth.
Do not restartslowtests. Fullsuites captured204beforeRPC-live/hashtest additions;
mostexecutables baseline5fbb638. Neverclaimcurrent206allpass. Relink remaining
affected Main/DB/network/auth targets aftertheirfullruns finish and rerun.
ExplicitMiniByteCTestneededafterfullsummary. Source-mapupdated364/auditpasses.

Saved RPC livecommitc002323bb16ee6af884438aa04d303e4ab30ba5f. Verifiedlocalbundle
MinimaCoreC++-20260910T204256Z.bundle SHA256
8d29fa02509fbc95c73f3d270bc8c5a08edad6d925ee873ae7c01a3bdd7b6500.
Hashtestwork beingcommitted/bundled; inspectactualHEAD/latestmanifest. Localonly,
noGitHubpush. GRAPHIFY_SKIP_HOOK=1. All457studentguides authored, fullparitynotverified.

Inspectedadditionalabsents fornextwork: P2PSwapLink.java is ENTIRELY COMMENTEDOUT
(noJava class); ItemList.java has two-stringctor sp=s quirk,getArray alwaysCCE,
exactemptyseparatornonterminates,multitokenStringTokenizer behavior. Noedits tothese.

Older notes follow.

# Current work — live RPC passes; broader shared JSON suites still running

Full 457-file goal ACTIVE/incomplete. Branch/project MinimaCoreC++. Mapping still
363 counterparts / 94 absent; 205 CTest registered, previous full runs captured204.
Live driver passes normal and ASAN/UBSAN:2LF/CRLF lifecycles,41observations each,
25rawHTTPrequests+2RPCClientcalls+8realpauses per runtime/lifecycle. Includes replacing
MinimaDB with listener active, rejecting old user and adding user to new DB.

Production Authorizer now defaults to current MinimaDB->getUserDB lookup. First
live run exposed empty callback; failure saved. Existing direct/override API kept.
MainFixture extracted from main_runtime, adds database replacement via existing
Main databaseServices. Live fixture uses actual command/network/HTTP/authorizer,
stable logger sink, unstartedMainworkers, SQLconstructorfixture only.

Full tests STILL LIVE: normal29381 /tmp/minima-shared-json-all-tests.log;
san42976 /tmp/minima-shared-json-all-tests-san.log. Last observed normal92, san36.
Only observedfailure MiniByteTests stale catch; corrected/directed reruns pass.
Poll, do not restart merely slow. These runs cover shared JSON baseline5fbb638;
most executables predate new Authorizer default. Relink existing targets after
full suites finish, then affected Main/DB/network/auth and MiniByte CTest reruns.
No claim full205 currenttree tests passed. Live results saved under
validation/rpc-live-work-in-progress. Driver report-only empty-list guard added
after successful runs; successful path unchanged.

Latest verified baseline commit5fbb63871844c282664a781dac18d4b3d8a917a4, bundle
MinimaCoreC++-20260910T202838Z.bundle SHA256
a6c4356ae52149914449288d766da4743f2c44ebc427fe9fbfd6860302f29f01.
Current live work is being committed/bundled: inspect actualHEAD and manifest.
No GitHub push. GRAPHIFY_SKIP_HOOK=1 for localgit writes. Last numberedcheckpoint362.

Older notes follow.

# Current work — shared JSON WIP; full tests LIVE

This goal turn is PROGRESS: implemented shared JSON storage/root ownership,
new Java comparisons/native concurrency test, student lab, and a stale test fix.
Full457workingC++goal ACTIVE/incomplete.363counterparts94absent;204CTest.

LIVE full-suite handles (must poll, DO NOT restart from stale notes):
- normal29381: /tmp/minima-shared-json-all-tests.log, last observed consensus32.
- sanitizer42976: /tmp/minima-shared-json-all-tests-san.log, last observed peers23.
Both launched full ctest --output-on-failure with required loopback escalation.
Both recorded MiniByteTests1 abort: stale std::out_of_range catch vs existing
wire::EOFException. Read actual pinned MiniByte.java/test, native implementation,
ByteIO.hpp and malformed_streams test; fixed only mappedtest catch/include. Both
target builds + direct executable reruns84227/92373 exit0. Full suite will still
report its earlier failure; collect all outcomes then rerun failing CTest entries.
No production source edits after full-suite start; only MiniByte test changed.

New shared JSON implementation: support/JSONSequence.hpp owns mutex+orderedvector
in shared State. get/find/snapshot copy under lock, iterator holds owner/modcount,
checks structural mutation on next, Array.hasNext current-size semantics and
Map.hasNext initial-end semantics. JSONObject overwrite existing slot doesn't
increment modification. JSONArray.get now Java IndexOutOfBoundsException (first
67program test exposed3mismatches). elements/entries return retained ranges;
JSONArray.snapshot explicitly copies for wallet fixture's probe append. ibd fixture
uses owned range. JsonDB params() returns locked root shared_ptr snapshot, setParams
locks replacement; copies retain same root. getJSONArray allows a concurrently
read null reference after creation check, like Java cast. P2PDB adapts params()->.
Nested JSONValue pointers preserve child identity; serializers don't hold parent
lock while traversing child. No JSONSequence general inheritedJavaAPI certification.

Full builds60565/52182 PASSnoobservedwarnings. Initialbuild1035 alsoPASS;
first67programtest97813 failedonly3indexcategories (saved initial-index-failure.json).
Corrected focusedtests16098/1600PASSboth:67programs606ops, 1600concurrentiterations,
1594validserializations/6permittedCME each (observed, nondeterministic counts).
Focused evidence saved validation/shared-json-work-in-progress/{normal,sanitized}.
Lab written BEFORE tests. Student guides for JSONArray/JSONObject/JsonDB/P2PDB and
MiniByteTests updated. Source map still363; full guide audit pendingfinaledits.
Implementation/evidence WIP commit and localbundle follow: inspect actualHEAD and
latest ignoredmanifest for authoritative saved IDs. Last numberedcheckpoint362.

NEXT: finish both fullregressions, inspectreports/failures, fix within sourcefidelity,
rerun affectedtests. Then actual rpc-command enable:true listener+auth integration:
reuse MainNetworkFixture/Oracle + actual NetworkManager/HTTPServer/CMDHandler and
RPCClient; don't naively enable livebackgroundlogging during main_runtime's per-op
std::clog.rdbuf swap. SharedJSONbasicaccess now implemented, subjecttoregression.
Previous authentication flag/password followup is committed233ffcf5b520904c0b8266b3d68c34445207bfd3.
Its verifiedbundle MinimaCoreC++-20260910T201520Z.bundle SHA256
12fa9c2bb61bd007826f1d6f7c3529ef704054a92352e0c2e5f9d652a2fb7ab8.
Branch/project MinimaCoreC++; keepJavaPin5564b9d. NoGitHubpush. Git writes use
GRAPHIFY_SKIP_HOOK=1. Do not tagfullRPCor markgoalcomplete from focusedtests.

Older notes follow.

# Current work — RPC configuration storage fixed and validated

Full 457-file native working-node goal ACTIVE/incomplete. The prior naming-only
turn was no progress toward the port; current turn makes concrete progress.
363 counterparts (319 production + 44 tests), 94 absent, 202 CTest entries.
All 457 lessons authored; audit 9061 links / 2634 anchors / zero errors.
Last numbered checkpoint remains 362. Do not tag RPC complete yet.

NEW: support/JavaStringReference.hpp extracts RPCClient::UserAgent's existing
mutex/optional-string storage. RPCClient keeps its UserAgent alias. RPC_PASSWORD
uses the extracted adapter; RPC_ENABLED and RPC_AUTHENTICATE use the existing
atomic<bool> logging-flag pattern. Explicit load snapshots in Authorizer,
NetworkManager/rpc replies and ParamSnapshots/main_runtime fixtures. Null password
is preserved and does not compare equal to literal "null" or empty text.
Command stop/sleep/update/start order unchanged; no multi-field transaction.

Both FULL builds PASS with zero observed compiler warnings. 22 selected tests PASS
per build: batch21 normal81.37s/sanitizer126.11s, plus CMDHandler4.60s/5.30s.
266 RPC programs/614ops, 5668 network-utility cases, new native8000-loop concurrent
snapshot test, existing477RPCclient cases/597requests, 2106CMDHandler requests over
6configurations, native8networklifecycles and affected command/config regressions.
All38JSONreports read and saved with CTest summaries in
validation/rpc-configuration/{normal,sanitized}; progress.json/provenance/README,
review and affected student lessons updated. Lab written BEFORE tests. No TSAN.
All handles terminal: builds52472/36379, tests54888/56134, handler57471/90986.
Logs /tmp/minima-rpc-config-{build,build-san,tests,tests-san,handler,handler-san}.log.
Current implementation/evidence commit and verified local bundle follow; inspect
actual git HEAD/latest local-backups manifest for their authoritative identifiers.

NEXT: finish shared RPC-user JSON state and successful command-driven live enable.
Do not move to an easier command or call the full goal complete.
Read FULL this turn: pinned JSONArray.java (ArrayList inheritance, iterator-based
serialization and primitive/object overloads), JSONObject.java (LinkedHashMap,
iterator serialization), JsonDB.java and corresponding native headers/bodies;
JavaList.hpp and JavaConcurrentMap.hpp. Current JSON get returns borrowed const
JSONValue&, elements/entries borrowed const vector&, no synchronization. JsonDB
mParams is a shared_ptr accessed/replaced without synchronization. UserDB.get/set
RPCUsers alias the stored array. rpc appends in place/removes via replacement;
Authorizer and CMDHandler read it; returned list replies retain it for serialization.
A lock only during a getter cannot protect a borrowed reference after unlock.
JavaList also lacks synchronization. JavaConcurrentMap has tested locked snapshot
operations, but hash traversal and weak enumeration cannot replace ordered JSON
containers unchanged. Native ParamConfigurer uses a modification count for a
specific fail-fast check (configure lines165ff), not generic list iteration.
These are reuse candidates, NOT an implemented JSON-container design. Preserve
array/object identity and source iterator failure behavior; do not silently replace
shared RPC records with a copied backend. Review callers/lifetimes if returning
JSONValue by value; JsonDB getBoolean/getJSONArray take addresses of get().value.
The generic full inherited Java collection API also remains incomplete.

Live-test reuse already inspected fully: support/network_manager_native.cpp;
actual NetworkManager startRPC/stopRPC/getBoundRPCPort; HTTPServer/CMDHandler,
Authorizer and RPCClient. MainNetworkFixture/Oracle provide actual unstarted
NetworkManager. Need rpc command enable:true to create real listener, then
credentials/request/stop/restart observations. Avoid main_runtime's per-operation
std::clog rdbuf swap while background server logs run. Existing handler/listener
regressions do NOT prove rpc-command enable integration. Source one-second pause
remains real; interruption fixture models service exception boundary only.

Local project and branch MinimaCoreC++; cwd unchanged. No GitHub push authorized
to a chosen remote; existing destination question remains unanswered. Local
commits/bundles are authorized. Use GRAPHIFY_SKIP_HOOK=1 for Git mutations.

Older notes below are historical.

# Current work — RPC WIP, passing sequential comparisons, save locally

Full457-file native working-node goal ACTIVE/incomplete. This turn is PROGRESS.
Prior verified checkpoint362:83abf274cbc2c19cb91ba1a4735ad2017fa105f5, tag
minimacore-cpp/checkpoint-362. Its verified bundle is
local-backups/MinimaCoreC++-20260910T194812Z.bundle, SHA256
488a0a7ad6f2e4451fd4443ca76795f153c349858c183d0b2b5979ef1a2ea8f4.

NEW RPC cpp/hpp, registration and Main sleep binding implemented. JsonDB.getJSONArray
fixed wrong category: std::bad_variant_access now explicit JavaClassCastException;
null/missing array creation and shared ownership retained. Native RPC body follows
source enable precedence, stop/sleep/update/start-or-stop order, unused ssl,
duplicate password in valid list, aliasing add/replacement remove, full listusers
records and lowercase mode. Short help constructs current port; fullhelp reads it.

Both FULL builds PASS, no observed warnings. 258 programs590operations PASS both;
all11selectedregressions PASS85.74snormal/127.35ssanitizer.22JSONreports read,saved
validation/rpc-work-in-progress/{normal,sanitized};progress.json explicitly WIP.
363counterparts=319prod44tests;94absent=50prod44tests.201CTest.457authoredguides,
9058links2634anchors; final audit has zero errors.

ALL HANDLES TERMINAL:29719,6406,19289,4911 builds;4230initialtest blankexpectation;
74680test exposedJsonDBwrongcategory;74280/22705finalbuild+11test runs exit0.
Logs /tmp/minima-rpc-command-{build,build-san,test,regressions,regressions-san}.log.
Do not restart completed tests without new changes. WIPcommit/backup follows;
inspect git log/newestignoredmanifest for actual saved hash. No checkpoint363tagyet.

NEXT WORK: finish RPC, do not move to an easier command or label this complete.
1. Address concrete review concerns in REVIEW.md: RPC_ENABLED/AUTHENTICATE plain
bool and RPC_PASSWORD std::string are written by command and read by live request
threads. Reuse prior GeneralParams atomic migration; RPCClient::UserAgent is an
existing locked optional<string> snapshot candidate for shared string storage.
Read full relevant candidates/callers/tests and report reuse gate before adapting.
No atomic/shared-string edits made yet. ParamSnapshots explicit snapshots will need
adaptation if fields change. Also review shared RPC-user JSONArray/JsonDB state:
Authorizer and serialization readers can overlap in-place append/replacement.
Preserve tested array aliasing; do not substitute a copied users backend silently.
2. Implement successful live enable/authentication/loopback command validation.
Current258suite does NOT successfully startRPC: only stopped-managerdisable,
invalidenable/password and actualJavaThread.interrupt/nativepauseexception.
Source default1sec pause reused via MainServices.sleep. Native wholeThread.interrupt
mechanism remains absent; fixture has bool boundarysignal only, documented.
3. Existing network_manager_native.cpp exercises actual HTTPServer/CMDHandler RPC
start/stop/restart across8networklifecycles. Reuse that proven integration.
MainNetworkOracle/Fixture can provide actual NetworkManager on unstartedMain;
HTTPServer getBoundRPCPort native, Java mRPCServer.mServerSocket reflection allows
port0 observer. RPCClient.sendGETBasicAuth exists in org/minima/utils/RPCClient.
Beware async server startup logs and fixture std::clog streambuf swaps; do not
introduce concurrent streambuf mutation or normalize unvalidated outcome data.
4. Expand relevant network/HTTP/authorization/configurer and state regressions,
update lessons/provenance/map/review, then validated checkpoint and verifiedbundle.

The RPC lab was written BEFORE tests; all touched Main/JsonDB/RPC lessons updated.
First source test expected blank action to fall through, corrected to actual
getParam blank rejection; source port unchanged. Native JsonDB mismatch was real.
No user question required; already authorized localcommits/backup. NoGitHubpush:
remote fork/ghfork destination unanswered. Worktree path/branch MinimaCoreC++.

Older notes below are historical.

# Current checkpoint 362 — status validated, ready for final local commit

Full 457-file working native port goal ACTIVE/incomplete. This turn made progress:
status header/body, registration, runtime collection hint, Main fixture and oracle,
187-program / 634-operation corpus, student lab/lessons, mapping and evidence.
Implementation commit e826360 is saved on MinimaCoreC++. Final evidence/docs commit
follows; use git log and checkpoint-362 tag for its hash.

Both FULL BUILDS PASS without observed warnings. All 25 affected regressions PASS
301.18s normal / 952.17s ASAN/UBSAN. All 44 JSON comparison reports
read without failures; both CTest summaries saved in validation/checkpoint-362/
normal and sanitized. 362 counterparts=318prod+44tests; 95absent=51prod+44tests.
All map entries partial/not_verified. 200 CTest. All457studentlessons remain authored.
Final guide audit:9052links,2634anchors,zeroerrors.

ALL HANDLES TERMINAL. Final regressions7645/40204 exit0. Latest build66252/25262
exit0; latest isolated comparisons49199/42930 exit0. Earlier builds33622/62409
and first successful comparisons55373/89648 terminal. Initialbuild54612 API adapter
compile fix; initialtests60565/53407 expected-value corrections. Logs all under
/tmp/minima-node-status-{build,build-san,test,test-san,regressions,regressions-san}.log.
No background work remains. Do not restart completed checks without new changes.

Finish final local commit/tag and run /tmp/minima-status-backup.py to create,
verify and recovery-clone a bundle. Its ignored local-backups JSON manifest records
actual HEAD/SHA/recovery details; inspect the newest manifest if already done.
No GitHub push: remote destination still unanswered. Full node goal unchanged.

status follows pinned source ordering, including clean presence before parameters,
early complete tip dereference, DECIMAL32/clamped hash rate/device ceiling, file
sizes and sparse no-root fields, median speed, numeric/string distinctions,
source debug order and optional network. Static main also ported and compared.
MinimaLogger Runtime adds collectMemory(default no-op); Main binds existing service.
Native allocator statistics differ from JVM heap; tests validate them independently
before normalization. UserDB, SQL and actual chain objects reused. No full startup,
durable SQL, JVM collection guarantee or exhaustive scheduling certification.

Possible next command: RPC. FULL pinned network/rpc.java and its existing lesson
read during the wait; native NetworkManager.cpp read fully and UserDB get/setRPCUsers
examined. No RPC edits/lab/test written. Native startRPC/stopRPC run the actual
HTTPServer/CMDHandler. Need complete reuse gate, dependencies/tests and source
NetworkManager start/stop review before implementation. Source enable wins over
action, stops RPC, sleeps1000ms, then mutates flags/password, then starts or stops;
ssl is advertised but unused; valid list repeats password; addusers preserves
array aliasing, remove replaces array, list exposes saved extra-user records.
No alternate authentication backend or placeholder server should be invented.

Older notes below are historical and may be obsolete.

# Current checkpoint 361 — logs validated and ready for local commit

Full-port goal ACTIVE/incomplete. This turn made progress: logs command, atomic
loggingfields,fixtures,tests,lessons and evidence. BranchMinimaCoreC++; priorHEAD
1177d18healthcheck. New361validatedcommitfollows; noGitHubpushpendingdestination.
Map361=317prod+44tests;96absent=52prod+44tests. Allpartial/not_verified.199CTest.
All457lessons, finalauditfollowsthisnote. StudentlabBEFOREtests.

BothFULLBUILDS PASSwithoutobservedwarnings. Newlogs327programs982operationsPASS
normal4.59s/san7.91s. All25affectedregressionsPASS194.27s/457.64s. All42reportsread
no failures,savedvalidation/checkpoint-361/{normal,sanitized}. ALLHANDLES TERMINAL:
build22819/45999,newtest98688/19773,regressions40392/30621. Logs
/tmp/minima-logs-{build,build-san,test,test-san,regressions,regressions-san}.log.

Newlogs.hpp/.cpp registered+CMake. Seven GeneralParams flags nowstd::atomic<bool>
followingTxPoWSqlDB. Preserveseparatefixed-orderwrites,omissions,partialfailures,
TXPoWblock+transactionflags andsource replyreads. Atomicsperfieldnottransaction.
ParamSnapshots nowload()boolsnapshots/captures; processorfixture two directflag
assignmentsreplacebool-pointerentries. Reverseatomicchangesexactlyreproduceprior
GeneralParams.hpp/.cpp+ParamSnapshotshashes; evidenceinprovenance. No source-body
correctionsneededaftertests. Allmodifiedcode/fixtures/corpusreviewedafterapply.

327programs cover18typedvalues*8keys*2priorstates, omittedoptions andsingleupdates,
hiddenSQLtransactionflag,partialchangedprefix/unchangedsuffix,metadata/freshfactory,
runnergates,actualMain.setSyncIBDlogconsumer. NativeMAXIMA_LOGS flag hasnosource
consumeroutside declaration/reset/logs inthispin. Do notclaimfullMaximalogging.

25selectedtests:source,Command/runner/errors,params+original,TxPoWSqlDB,miner+
worker,processor+worker,checkerlogs,timederrors,peerschecker,NIOmessage/state/
transport,Main,lifecycle,networkpeers/status,trace,health,ping,logs. Loopbackonly,
escalationsaccepted. 21JSONreportsperbuild; worker/original/sourcegatesinCTest.
NoTSAN/fullnode/exhaustiveJava-racy-scheduleclaim. No newagents.

Lessons logs+GeneralParams,lab,README/REVIEW/provenance/map/index updated. Finalaudit,
gitdiffcheck,commitwithGRAPHIFY_SKIP_HOOK=1,tagunique361,bundle/recoverycloneverify.
Evidencefoldersnormal/sanitizedavoidrootbuildignore; originalhistory/pinretained.

Nextcandidate status.java nowreadFULL343lines. Existingreuseidentifiedbutnotyet
fullyreviewed forimplementation: MinimaLogger.hppFULL/cpp1..78 runtime public
getRuntime().totalMemory/freeMemory already nativeallocator onApple/glibc;
MiniFile getTotalFileSize/getTotalFileSizeWithNames alreadyexist. TxPoW.getWeight
usesMMREntryNumber::dividePositive(Crypto::MAX_VAL,difficulty,7),exactDECIMAL32
buildingblock candidate (needreadhelperfully). TxPoWGenerator median/speed funcs
exist. Main.collectMemory currentlyprivate optionalhint; source status callsGC on
clean PRESENCE beforedebug/complete parsingandbeforeMain/DB lookups. Do not replace
that with anearlyrequiredMainlookup. No statuscode/lab/tests written. Sourceincludes
staticmain hardcodedfileprint helper, preservemappedAPIexpectations. Mostdetails
useactualDB/wallet/sql/chain/cascade/network; RAM/uptime requirevalidatedsampling
orcontrollednativeboundary,notblanketnormalization. Fullnodegoal unchanged.

Older notes below may be obsolete.

# Current checkpoint 360 — healthcheck validated, ready for local commit

Full-port goal ACTIVE/incomplete. This turn made progress: native healthcheck
header/body, registration, fixture/corpus, student lesson and saved validation.
Local branch MinimaCoreC++, prior HEAD d9ea4f5; validated360 commit follows.
Map360=316production+44tests;97absent=53production+44tests. Allpartial/not_verified.
198CTest. All457guides;9041links2634anchors;0errors after final audit.

Both FULLBUILDS PASS, no observed compilerwarnings. All176newprograms635operations
matchJava in bothbuilds. FOURTEEN affected regressions PASS115.26s/247.61s. All26
reports read,no failures; saved validation/checkpoint-360/{normal,sanitized}.
ALLHANDLES TERMINAL: build59127/45449; newtests23046/89281; regressions20031/77928.
Logs /tmp/minima-healthcheck-{build,build-san,test,test-san,regressions,regressions-san}.log.

Initial testfailure:1e40height outsideMiniNumberrange causedsetupfailure/noTIP.
Retained rejection case, usedallowedlargeheights elsewhere andassertedsetup success.
No productioncode correction. Preserve healthcheck rawlogic: repeateddb/tree/cascade
lookups,missingTIPCommandException,tip-root(no+1),nocacade,root-1adjacency,length,
noMaxima. NativeactualCommand/MinimaDB/TxPowTree/Cascade/MiniNumber reused.
Fixtures actualnodes,recalculateTree,addToTip,Main unstarted. GHOSTbranch,gapped/
decreasing/fractional/largeheights,nullcascade,statereplacement,noSQL/timer/listener/
queueeffects anddirectvsrunner gates checked. No fullstartup/concurrencycertificate.

Studentlab BEFOREtests. Lesson,README/REVIEW/provenance/map/index/audit finalized.
Savecoherentcheckpoint using GRAPHIFY_SKIP_HOOK=1 forgitcommit; Gitmutations need
escalation,allaccepted. Tagunique360,refreshbundleandcloneverify. Newevidence uses
normal/sanitized folders toavoidgeneric build ignore. No GitHubpush: user's target
forkdestinationanswerpending. Existing source pin/worktree/originalhistory retained.

NEXT logs source read FULL again. Also read GeneralParams.hpp/.cpp FULL and
TxPoWSqlDB.hpp; searched ALL seven log flag references in native src/support.
Current flags plainbool, asynchronously read by Main/TimedChecker/TxPoWChecker/
TxPoWMiner/TxPoWProcessor/NIO/P2PPeersChecker. Use existing atomicbool pattern from
TxPoWSqlDB LOG_ADD_BLOCKS/LOG_ADD_TRANSACTION (alreadyatomic). Seven GeneralParams
flags likely need atomics before command concurrent writes. Two fixture adaptations
identified: ParamSnapshots.hpp JSON puts + auto saved captures need load();
txpow_processor.cpp line21 bool-pointer initializer list includes BLOCK_LOGS and
IBDSYNC_LOGS; split those into direct assignments if madeatomic. No code for logs
or atomic migration written. Read full affected fixture/callers/tests before edits,
report reusegate and write studentlab before new tests. Preserve source fixed
mutation order, omitted flags unchanged, partial changes on malformed later field.

Older notes follow and may be obsolete.

# Current checkpoint 359 — trace and timemilli validated and saved locally

Full 457-file native-port goal remains ACTIVE and incomplete. Local Git branch and
project description are MinimaCoreC++. Existing history and source pin retained.
359 mapped counterparts = 315 production + 44 tests; 98 absent = 54 production +
44 tests. All mapped entries remain partial / parity not_verified. 197 CTest tests.

Both full builds PASS. Trace/clock differential: 209 programs / 652 operations
PASS in both builds. All eleven affected regressions PASS, 36.66s normal / 61.15s
ASAN/UBSAN; all twenty reports read, no failures. Sessions 99435 and 11104 finished.
Logs /tmp/minima-trace-clock-regressions{,-san}.log. Reports saved under
validation/checkpoint-359. Student lab preceded tests; both lessons updated.

Reuse actual Command, MessageProcessor shared trace state, atomic NIO flags,
TimerMessage clock, MiniNumber narrowing and JavaDateString. Preserve trace's raw
enable read and earlier trace change when network parsing fails. Preserve Java
32-bit multiplication wrap before long clock subtraction. Independent bounds and
UTC date assertions precede clock normalization. No full-node scheduling or
exhaustive locale parity claimed. Fixtures use real unstarted Main and diagnostics.

Git checkpoints already exist: e2c0d06 verified357; 9b06c3b WIPtrace/clock; c19fc27
study graphs. New validated359 commit follows this note. Use GRAPHIFY_SKIP_HOOK=1
for commits to prevent incidental rebuilds. Git mutations require auto escalation;
no rejection occurred. Timestamped bundles under ignored local-backups include all
refs and pinned upstream tag. Previous c19fc27 bundle was recovery-clone verified.
No GitHub push: user's destination answer remains pending; do not guess which fork.

Next source candidates already read: logs.java and healthcheck.java. logs needs
review of GeneralParams plain bool fields before introducing concurrent mutation.
healthcheck can reuse actual MinimaDB/TxPoWTree/Cascade and Main chain fixture, but
needs a cascade fixture and differential corpus. Follow reuse gate and write the
student lab before tests. Full startup/durable SQL/remaining registry and absent
Minima entry point still prevent claiming a working complete node.

Historical notes below may describe obsolete WIP or live handles.

# Current work — local Git checkpoints and trace/timemilli in progress

USER NOW EXPLICITLY AUTHORIZES/REQUESTS REGULAR LOCAL GIT COMMITS/BACKUPS.
Localproject/branchname MinimaCoreC++ (userrequested), workspacepathunchanged.
Verified357checkpoint committed e2c0d06. Pinupstream-study recordedasgitlink+
.gitmodules: existinglocalworktreeunchanged, freshclonecaninitializesubmodule.
Graphifyhooksonbranchswitchlaunchedrebuildandbackedcuratedgraph2026-09-10;
futurecommitcommands use documented GRAPHIFY_SKIP_HOOK=1 optout toavoidbackground
rebuildschangingfilesduringcheckpoint. Hooksnotremoved. Root.gitdescription MinimaCoreC++.
No GitHubpush yet: asyncdestinationquestionpending choicesminima-core-fork/minima-core/
localonly. Do not pushoriginupstreamorchoosewrongfork. Gitmutationrequiresautoescalation;
allreviewrequestsaccepted, noapprovalrejection. LocalGitignore excludesDSStore,
local-backups,graphcache/machinepathfiles; oldtrackedJavaresourceDSStoreunchanged.

FullportgoalACTIVE/incomplete. Newtrace.cpp/.hpp+timemilli.cpp/.hpp written+registered
andCMakeadded. Source-map359=315prod+44tests,98absent=54prod+44tests;196CTest.
These2commands are WIP; lastverifiedcheckpointremains357. NOnewfixture/testsyet.
LabBEFOREtests student-guide/new-tests/trace-and-clock-commands.md. Guidesportstatus
updatedWIP butearlierfuturewordingneedsfullrewriteaftervalidated. CurrentREADME
stillverified357; makefuturecounts/distinctionexplicit. BothFULLBUILDS nowPASS withtrace/timemilli; handles27672,40491terminal.
Logs /tmp/minima-trace-clock-build{,-san}.log; no observedcompilerwarnings.
Command-specifictestsSTILLPENDING. Guideaudit359counterparts9035links2634anchors0errors.

Sourcebase trace.java, timemilli.java, logs.java,healthcheck.java allreadFULL.
Onlytrace/timemilli implemented. GeneralParams.hpp/.cppFULL, TimerMessage.hpp/.cppFULL,
TxPoWSqlDB.hppFULL, MessageProcessor.cppfirst50lines andheaderFULL; relevantNIOtrace
field declarations/use sites; MiniNumber.cppgetAsBigInteger/getAsLong/getAsInt600..625;
JavaDateString.hppFULLread. Siblingsearch foundno commandwrappers. Reusegateannounced
Command,MessageProcessor/NIOflags,TimerMessageclock,MiniNumberintegerconversion,
JavaDateString andexistingunsigned/bitcastoverflowpattern.

trace rawenable null=>JavaCheckedException Must specify enable;wrongtypeCCE;
exacttrueonly(no trim). filter=getParam(defaultempty), updateMessageProcessor global
trace/filter BEFOREgetBooleanParam(network,false); malformednetworkleavesfirsteffect.
ThenNIOClient/NIOServer atomicmTraceON updates; responseenabled/filter/shownetwork.
MatcheractualMessageProcessor lowercasesboth despitehelpcase-sensitiveclaim.
timemilli clockcapturedfirst;minutesbackpriorityhoursback;MiniNumber.getAsInt;
32bit factor60000/3600000 multiplicationWRAPS beforelongassignment;64bitsubtraction
unsigned/bitcast preservesJava; responsemillis+javaDateString. HelpfirstcharacterTAB.

NEXT implementtrace/clock diagnostics fixture+corpus usingMainRuntimeunstartedMain
asglobaltraceobserver withnewoptionaldiagnosticsreset/snapshotops. Observeactual
NIOatomicflags. Crossrawenable,filter,network typedparams andpartialchanges; use
actualMain.isTrace/getTraceFilter/checkTraceFilter. Compareexchangeswithnetwork
loggingon/off. Clockindependent offsetreconstructioninsidebefore/after andexactUTC
renderingfromreturnedmillis beforeclocknormalization. Sharedclock/MiniNumber/parser
helpersalreadytested; testsmustassert32bitmultiplyoverflow andminutespriority.
logs remainsunported: uses7GeneralParamsboolflags+2SQLatomicflags, sourceorderpartial
mutations. Potentialnativeconcurrentboolaccessrequiresreviewbeforeport; no dependency
changesmade. healthcheckremainssource-onlyread; needsrealcascade/treefixture.

Prior verifiedcheckpoint357follows. Itremainsauthoritativeforpassedtests.

# Verified checkpoint357 — network status/control command

GoalACTIVE/incomplete. CurrentturnPROGRESS. 357counterparts=313production+44tests,
100absent=56production+44tests. 196CTest. All457guidesauthored;9031links2634anchors
0errors. Allmappedpartial/paritynot_verified. ALLHANDLES TERMINAL. No newagents.

NEW network.hpp/.cpp nowcompiled/registered/CMake. Bothfullbuilds PASS noobserved
warnings. Normalinitial119programs544opsPASS handle79040. Expanded124programs562ops
PASS bothbuilds4.53s/3.80s. All10selectedregressionsPASS19.15s/27.27s,handles47993,
32086terminal. All18JSONreportsreadnofailures. source-map357checkpassedinbothbatches.
Logs /tmp/minima-network-status-{build,build-san,test,regressions,regressions-san}.log.

ExistingCommand/Main/NetworkManager/NIOClientInfo/NIOTraffic/MessageProcessorreused.
Sourcevalidparamsonlyactionbutlistbodyreadsuid =>directfilterandrunnerrejection
preserved. listconnectingbeforeconnected; nonemptyuidfirstmatchstops; secondMain
networklookupforgetStatus(true). resetrealtraffic;recalculateactualconfigured-host/
readonlylocalinterfacealgorithm;restartMainqueue;loggingNIOthenpeerupdatesGLOBAL
statictrace. Existingteacherpageper-subsystemwordingcorrected; partialeffecttested.
Nativecommandheader/bodyandbothmodifiedfixtures/fullnewdriversectionsreviewed.

Opt-instatusFixture inMainNetworkFixture/Oracle seeds initialtrafficstart0 and
syntheticclienttime0(nativeNIOclockalready0),totals12345/23456,breakdowns1024/2048,
tracefalse/filterempty,hostconfig andports9001/9005. getUserDB callbackboundactual
MinimaDB asMain.newNetwork. Additionalsnapshottrace/filter/host/hostset/totals/start
onlystatusFixture. Actualresetclockboundedbefore/after; resetdateprosenormalized.
Mainruntime --group status usesUTCJava/native; othergroupsretained. Newcorpusmodule
network_status_commands_corpus.py, resultnetwork-status-command-results.json.
Independentemptylist, duplicateUIDfirstconnectingmatch, loggingpartialon/off,
exactrestartqueue, UIDrunnergate andresetstatsassertions. Sharedoldgroups pass.

LabBEFOREtests network-status-command.md, newauthor_network_status.py+rewritten
networklesson,README/REVIEW/provenance/index/auditupdated. Fullnode/storage/etcstill
open. No nextsource chosen afterthischeckpoint. Staged/tmpfilesnowAPPLIED; do not
rerunstagingapplyscripts (wouldduplicatechanges). Nextworkshouldpickremaining
source-map productionpaths andfollowreusegate/studentlab-before-tests.

Previousnotesbelowincludeoldin-progress/livehandles; thisheaderoverridesthem.

# In progress after checkpoint356 — network command applied and compiling/testing

All staged files APPLIED, notjust/tmp. NEW mapped network.hpp/.cpp+registry+CMake,
map357=313prod+44tests,100absent=56prod+44tests. 196CTest. Normal/sanFULLBUILDS
PASS handles4049,56047terminal, /tmp/minima-network-status-build{,-san}.log.
NEWnetworkstatus119programtestrunning: get live handle from currenttoolresult,
log /tmp/minima-network-status-test.log. No networktestresultsaccepted yet.
MainNetworkFixture nowgetUserDB boundrealMinimaDB; opt-instatusFixture fixesinitial
trafficstart0, totals12345/23456 breakdown1024/2048,tracefalse/filterempty,hostconfig,
ports9001/9005RPCfalse; snapshotextra trace/filter/host/hostset/totals/start.
JavaMainNetworkOracle same, alsofixesclientconnectiontime0 (nativeNIOfixtureclock0).
main_runtime_differential now --group status, uses importedprojector/validate,
java-Duser.timezoneUTC,nativeenvTZUTC onlystatusgroup. Reportnetwork-status-command.
Studentnetwork-statuslabwrittenbeforetests. Networkcommandlesson/README/REVIEW/
provenance/mapauditNOTYETupdatedfor357; completeaftertestpass.
MiniFormat::formatSize0 verifiedexact0 bytes. Newcorpuscurrently119; addpartial
loggingoff andduplicateUIDfirstmatch/emptylist independentcasesafterfirstbaseline.
Nativebodyandfixture/drivernewsectionsread/reviewedafterapply. Javafixturebody
stillneedsrereadafterapply. Bothbuildsnoobservedwarningscheckpending.
Previouscheckpoint356summarybelowauthoritativeforsharednullableAPI35regressions.

# Verified checkpoint356 — nullable command API and ping

Both full builds PASS. Both selected CTest batches35/35PASS, normal181.54s,
sanitizer762.10s. All66freshJSONreportsread/no failures. Finalwalletaddress134programs
passed147.30ssanitizer. ALL HANDLES TERMINAL, including67880. Ping95+9loopbackcases,
runnererrors1680 eachbuildPASS. README/REVIEW/provenance/labupdatedfinalresults.
Source-mapcheck356 andstudentguideaudit9027links2634anchors0errorsPASS.
GoalACTIVE/incomplete; nextnetworkcommand staged/readybutnotcompiled. Priorin-progress
notes below have obsolete live/pending statements; this paragraph overrides them.

# In progress after checkpoint355 — ping and nullable Command API

Goal ACTIVE/incomplete. This turn PROGRESS. Map now356=312prod+44tests,
101absent=57prod+44tests. 195CTest. All457guides authored; audit9027links,
2634anchors0errors. All mappedpartial/paritynot_verified. No newagents.

LIVE handle67880: sanitizer CTest35selected, log /tmp/minima-ping-regressions-san.log.
Normal handle35202 TERMINAL PASS35/35 181.54s. Both fullbuildhandles96901,40397
TERMINAL PASS; logs /tmp/minima-ping-build{,-san}.log. Currentnewping normal95programs
+9loopback PASS, runnererrors1680PASS. Readall33freshnormalJSONreports nofailures.
San reports onlyfunding andkeysfresh sofar; continuewait and inspecteveryreport.
README/REVIEW/provenance currently say sanitizer validation running; updatefinal.
All tool requests for loopback autoaccepted, no rejection/no need askuser.

Currentimplementation: noargCommand.runCommand nowJSONValue, 166declarations/
definitions in165files, string-takinghelperoverloads remainJSONObject. Persisted
support/nullable-command-migration.json records before/afterhashes; reversesubstitution
verifies163puremechanicalfiles. OnlyCommand.hppcomment and command_runner_errors.cpp
newprobes differaftermanifest. CommandRunner nowretainsJSONValue, buildsordinary
errorrepliesincatches, appendsnullablevalue thenCommandResult.status outsidecatch.
NewCommandResult.status(JSONValue) checksnull/object/cast thenexistingBooleanhelper.

NEWping.hpp/.cpp registered andCMakeadded. Sourcefirstcolon/getParam/trim/Javaint,
nullno-colonbeforereceiver; evaluateMain.getNIO expressionevenstaticmethod then
actualNIOsend(ip,port,false); validfalseorversion+nullableextradata. Greeting.hpp
newgetExtraDataOptional exposesalreadyexistingoptionalstorage. NIOtransportbody
unchanged, loggingdiagnosticprose/failurebehaviorlimitations recordedinREVIEW.

TestslabBEFOREnewtests student-guide/new-tests/nullable-command-replies.md.
ExistingRunnerErrorsdriverexpanded10to15kinds=1680cases withnull,missingstatus,
nullstatus,wrongstatus,false. NormalizeONLYescapedNPE/CCEdiagnostics. Independent
entryorder/no-followup/logassertions. Initialfixture.removecompilefailurecorrected
byexistingentries/putfilter. Initialtest staleoldbinary failedassertion; notaccepted.
Finalnewfixturebuildand1680comparisonPASS. Normal/sanfullbuilds bothPASSafterfix.

NEW tools/ping_command_differential.py reusesMainRuntime/MainNetworkfixtures;
95programs+9loopbackcases normalPASS. tools/nio_manager_transport_differential.py
pinghelper optionalrequestfactory retainsolddefault, exactrequestbyteassertion.
Loopbacknormal,fragmented,nullextra,nestedextra,ignoredtype,trailing,emptyframe,
typeonly,truncated. Existingtransportregression640+17normalPASS. No publicpeers.
New CTestping_command_differential195. All35affectednormal/san regex:
command|wallet_wrappers|txn_lock_workers|source_mapping|nio_manager_transport|network_startup

Studentpinglessonrewritten,Command/Runner/Greetingappendnullsections;
student-guide/tools/author_ping.py reproduceslessonupdates (appendsections notidempotent,
do notrerununnecessarily). All457auditPASS. READMEcounts356. source_map.py regenerated.
provenance newping_and_nullable_commands, REVIEW newsectionpendingfinalsan.

NEXT sourcealreadyreadFULL: upstream network/network.java132lines. Native
NetworkManager.cpp/.hpp FULL, NIOManager.hpp FULL, relevantgetAllConnectionInfo,
NIOClientInfo.cpp/.hpp andJava FULL, NIOTraffic.cpp/.hpp andJava FULL,
NetworkManager.java calculateHostIP/getStatus andMessageProcessor.java30..100,
Main.newNetwork servicesbinding68..75. Existingnetwork_manager.cpp/Oracle/driverFULL
read. MainNetworkFixture.cpp andMainNetworkOracle.java FULLread; reusedexisting
PeerRuntimeFixture NIOclientkeys/maps. Siblingmain hasno compatiblecommandwrapper.
NEWlab BEFOREnexttests student-guide/new-tests/network-status-command.md.
NO mappednetwork.hpp/.cpp YET. Prepared UNCOMPILED candidates in
/tmp/minima-network-command-stage/network.hpp,network.cpp,network_status_commands_corpus.py.
No mappedfile/registry/fixturechangesapplied yet. Reviewthenapplyaftercurrentvalidation.
Also staged apply_fixture.py and apply_driver.py toextendexistingMainfixtures/group.
Reviewtheirstringreplacementsbeforerun, andrunpy_compileonmodifieddriver. Staged
network_status_commands_corpus.py hasproject/validate forresettime/UID assertions;
checkactualMiniFormat zero spelling beforeaccepting hardcoded0bytesassertion.
Currentstagedcorpus count119? computeafterimport; no testsrun yet.
NeedaddCMake network.cpp andnewstatusCTest,registerCommandRunner,andcopystagedfiles.
Thenbuildnormal/san,teststatus+affectedMain/peer/ping/manager/messageprocessor.
Do notrerunall35commandregressions fornewregistryonlyif current35alreadyallpass.
Reusegateannouncedexplicitly; labwrittenbeforestagedcorpus.
author_ping.py nowguardsappendedsectionsagainstrerun, priornonidempotentnoteobsolete.

network.java details: validparamsONLYaction thoughlistbodyreadsuid => directfilter
worksbutrunneruidrejected. listdefaultgetParam(action,list);uiddefaultempty;
Main.network.NIO.getAllConnectionInfo iteratesconnectingthenconnected, filterexactUID
breakfirstmatch, buildconnections, getMain.networkAGAIN thenifnonnullgetStatus(true).
reset Main.NIO.traffic.reset +fixedreply;recalculateip actualcalculateHostIP andglobal
host; restartMain.PostMessage(MAIN_NETRESTART). loggingon/off invokeNIO thenpeer
setFullLogging(bool,empty), globalMessageProcessor statictrace; partialfirsteffect
persistsifsecondreceiverfails. UnknownactionCommandException Invalid action.

Fixtureprerequisitefornextnetworklist: MainNetworkFixture'sNetworkManager constructed
with{} meansgetUserDBcallbackmissing. BindrealMinimaDB.getDB.getUserDB likeMain.newNetwork.
NativeNIOfixtureclock=0, JavaMainNetworkclients mTimeConnected currentlywallclock;
setJavafixturetime0forlistcomparison. Trafficstartwallclockneedsinitialfixed0or
normalizeonlyvalidateddates. Observetrace/filter, resetstartwithinbefore/after,
totals+breakdown separately. Keepadditionalfixtureobservationsopt-in toavoidchanging
existingpingand222networkpacket snapshots. GeneralParamsresetnativeallvsJavafewfields
requiresresetnewhost/trace fieldsforrepeatability. Do notstartactualrestartworker.

Previous stable checkpoint follows.

# Verified checkpoint355 — direct-peer commands and IPv6 interface fix

Goal ACTIVE/incomplete. Current and previous turns PROGRESS. 355counterparts
=311production+44tests,102absent=58production+44tests. 194CTest. All457lessons
authored; allmappedpartial/paritynot_verified. NO LIVE HANDLES. No newagents.
No realnode/userwallet/seeds/credentials/remotepeer/distribution/commits/push.
FullnativeSQL/MySQLdrivers,Minimaentrypoint,remainingregistry/MegaMMRstillopen.

NEW mappednetwork commands disconnect.hpp/.cpp,message.hpp/.cpp,p2pstate.hpp/.cpp
registered inCommandRunner.ALL_COMMANDS. PinnedJava bodiesreadFULL previous/current
turn. CurrentCommand/Runner/Main/NativeNIOheaders and relevantmethods inspected.
SourceNIOManager.sendNetworkMessage951+,P2PFunctions.getAllConnections203+, P2P
getStatus550+ read. Siblingmain.cpp no compatiblecommandwrapper. Gate reported
reuseofCommand/Main,NIOManager/Server/Client,MiniString,P2PManager; no alternative
protocol. All3nativeheader/body files reviewed, sourcehelp literal extraction.

Disconnect rawuid cast/no trim; null=>No uid specified; exactall=>queueDISCONNECTALL,
elseactualNIO.disconnect(uid); status/message afterqueue. message rawdata/uid casts,
nullUID=>emptybroadcast+"all"reply vs explicitemptyUID=>samebroadcastbutdifferent
reply. MiniStringdata framing; Main.getNIOManager receiver expression evaluated
beforestaticNIOsend (nullNIOreturnalloweduntilstaticbodyrequiresit). p2pstate actual
Main/networkpeer cast; nonnullwrongtype=>CCE,null=>NPE ongetStatus, realgetStatus(true)
under"p2p-state" top-levelkey. Metadata/help/freshgetFunction tested.

SHARED NIOManager::mainManager andP2PFunctions::manager nowthrowJavaNPE forabsent
runtimeLookup ornullNIO result, replacingbad_function_call/genericRuntimeException.
Newcommandcomparisoncase95 foundmissingNIO message dataempty sourceNPE vsnative
RuntimeException: Null Main NIOManager. No semanticfallback/no fakeowner introduced.

NEW support/MainNetworkFixture.hpp/.cpp testadapter usesexistingPeerRuntimeFixture
andP2PManagerFixture headers INSEPARATETRANSLATIONUNIT toavoidglobalPeerHarness
object/array/stringhelpers conflictingwithSqlRecordingFixture helpersinMainharness.
NetworkManagerFixture friend subclass setsactualNIO/P2Powners unstarted. Peerschecker
IdleChecker; peer.initialize onlyrecordsits3timers, no worker. Clients actualNIOClient
constructor withdummyselectionkeys, no sockets, assignedsyntheticUID/validflags.
MainNetworkFixture.create(config,publish) publishesactualnetwork BEFORE constructing
clients; constructorsgetMainnetworktraffic. THEN injectnullNetwork/nullNIO/wrongpeer.
FinalnullNIO mutatesretainedNetworkManagerinplace toavoidfixture-onlyretirementstop.
Snapshot drainsrealNIOqueue, observesrealclientMiniDatapackets andtrafficbreakdown.
WrongpeerusesactualNIOManager asMessageProcessor; nofakep2pstatusresponse.

NEW support/MainNetworkOracle.java usesNIOClientOracleUnsafe/reflection/channel/key,
actualNIOClientconstructor, NIOManager/NetworkManager/P2PManager allocatedunstarted,
realP2PState andpeerscheckerverified/unverifiedsets. Samepublicationorder andfields.
MainRuntimeOracle/main_runtime addnetwork/networkSnapshot,retainedadapter perrequest;
MainFixtureaddsnetworksetter throughactualMain.replace. No productionfriendadded.
Driver main_runtime_differential.py --groupnetwork sharesactualharness, ownclasspath/
source-list/report, compilesnewJavahelper forall3groups. Scopefieldcorrectfornetwork.

NEW tools/network_peer_commands_corpus.py222programs794ops, CTest194
network_peer_commands_differential. Rawtypes/blank/whitespace/all/ALL/Unicode/NUL,
8193chars, nullMain/network/NIO/P2P,wrongP2Ptype,explicit/absentUID,valid/invalid/
pendingclients,8stateflagcombinations,metadata,permissions/invalidparams/repeats.
Independenttype5+4byteUTF8length+databyteassertions,recipientselection(singlecan
sendtoinvalidconnectedclient,broadcastonlyvalidconnected),disconnectqueueorder,
successfulfixturesetup/healthyP2Pstatus/freshmetadata. No publichost contact.
Firsttestbothfixturesfailedsetup becauseclientctor neededMainnetwork; independent
packetassertcaughtit. Correctedorder beforeacceptingrun. Initialcomparisonafter
NPEfix222/794PASS10.28normal/7.06san; finalafterfixturetightening3.55normal/5.68san.

SHARED P2PEnvironment.cpp numeric interfacefallback IPv6scopeFIX: oldcondition
OSsin6_scope_idnonzero droppedzoneonunique-localutunaddress. ExistingP2PFunctions
real-localscase641 foundJavazone%interface vsnativeunscoped. ReadFULLP2PEnvironment
.cpp/.hpp,InetSocketAddressresolve/validScope,getHostAddress; nativefunctioncallers/
tests+sourceP2PFunctions.getAllNetworkInterfaceAddresses273+. Siblingrggetifaddrs
no candidate. Reviewedcached /private/tmp/minima-jdk26-NetworkInterface.c exact
BSD enumIPv6Interfaces1750..1769 setsaddr.sin6_scope_id=getIndex(sock,ifa_name) for
ALLIPv6; createJavaobject792..796 setsid+name. Changednumeric conditionto
v6&&if_nametoindex(name), retainingexistingformatter/PTRforwardvalidation/cache.
Cachedsourcehash inprovenance. No networkinterfaceschanged. ActualfullJava/native
hostname setnowmatches. WebsearchreturnedolderprimaryJDKsources; twoattemptedraw
JDK26URLs cachemiss; usedlocallycachednativefile+actualJDK26behavior as evidence.
Do notclaimdownloadednewsourceoruseolderwebcode as26reference.

Finalfullbuild5normal/sanPASSnoobservedwarnings. All12distinctregressionspass
acrossphases. Initial11batches8pass/3fail37.16normal55.10san: transport/errors
loopbacksandboxblocked; P2Pfunctionsreal-localsIPv6difference. Reranloopbackswith
require_escalated (authorized,auto-reviewaccepted), normal2PASS26.60. NativeIPv6
fixthenfinalbatches withsocketaccessPASS55.91normal/56.89san: P2Pfunctions,
networkmanager,newcommands; sanalso2loopbacks. No approvals/userquestionspending.
All24JSONreportsinspected:Command12289,Runner1265,RunnerErrors1120,Main336/897,
Lifecyclecmd102/366,NIOstate1449/3879,NIOtransport640+17ping,connectorErrors12+4
nativefactory,P2Pfunctions672/3670+39HTTP,P2Pmanager405/2915,Networkmanager540,new
commands222/794. Mainownershipnativealsopassedinitialbatches. Finalmappingcheckpass.
Olderfailurelogs andp2p-functions-failure.json retainedasdiagnostics; currentreports
PASStruth. No testlimits relaxed orblockedtests removed. Source-mapnow355.

Docs3commandlessons,NIOManager/P2PFunctions+IPv6note,labnetwork-peer-commands BEFORE
tests,README/REVIEW/provenance/map/index/auditupdated. Audit457authored9019links/
2634anchors/0errors. Allcounterpartsremainpartial, nofullnodeclaim.
Logs /tmp/minima-peer-commands-build{,2,3,4,5}{,-san}.log (onlysomegenerationssan),
minima-peer-commands-test{,2,3,4}{,-san}.log,regressions{,-san}.log,
minima-peer-loopback-regressions.log,final-tests{,-san}.log. Latestbuild5; finalnew
command report inbuild{,-sanitized}/network-peer-commands-results.json. Allhandles
terminal (61678,29142,99186,18936,26736,15600,44724,58418,86813,34521,76457,66113,64438).

NEXT: Continuefullworkingnodegoal. ping.javaFULLread (79lines) butNOCODEyet. Returns
JavaNULLwhenhosthasnocolon; currentnativeCommand.runCommand purevirtualJSONObject
byvalue/Runner.runSingleCommand JSONObjectcannotrepresentnull faithfully. Need
inspectfullcallers/candidate nullablepatternandadaptsharedAPI, notturnnullintoempty
object/error orclaimpartialpingcomplete. parsefirstcolon,trimip/port,javaParseInt,
invalidportCommandException, evaluatesMain.getNIOManagerbeforestaticping, then
responsevalid/version/extradata. ExistingNIOManager.sendPingMessage real10sloopback
transport alreadytested17casesanddriverreusable. CommandnullableAPI adaptation
notdesigned/writtenyet. Source-maplists58prod44testabsent. Restoffullruntimegaps
fromcheckpoint352stillapply, includingMainMegaMMRcallback andnativeSQLdrivers.

# Verified checkpoint352 — Main, quit and rescue

Goal ACTIVE/incomplete; current and previous turns PROGRESS. 352 counterparts
=308production+44tests,105absent=61production+44tests. 193CTest. All457lessons
authored. All mapped rows implemented_partial/paritynot_verified. NO LIVE HANDLES.
No new agent needed (student backfill done earlier). No realnode/wallet/seeds/
server/credentials, commits/push/deployment. FullnativeSQL/MySQLdrivers, Minima
entrypoint, fullcommandregistry/MegaMMRsync remain unfinished. Do notclaim100%.

NEW Main.hpp/.cpp source startup/lifecycle/messages +realcomponentbindings.
PinnedMain1379lines FULLread; hash143e67f90b0941fa9c7bd8626e6092396d64c0cb85fd5e9a5346132daf18ffa1.
Useupstream-study ONLY: workingrootMainlaterMDS1424lines. Factoriesreuseactual
MinimaDB,NotifyManager,TxPoWProcessor/Miner,NetworkManager,SendPollManager. Runtime
bindsMySQLcallbacks,NIOManager/NIOClient/NIOMessage,Greeting/IBD,commandreceivers,
vaultMainReceiver,loggerMainReceiver. MainServices requiresSQL/MySQLconnect;
optionalnativeLoader,gcHint,clock,sleep,halt/exit,currentThreadName. Defaultreal
halt/exit; ONLYfixtures throw. getCurrentMegaMMRIBD remainsunbound dependency.
Source startupoptionalJNI,timer,privateCLEAN/genesis,DBloads,MySQLhelper,persisted
slaveprefs,hashwarming/rate,keys,workers,genesis,network,timers/flags translated.
No successfulfullstartupclaim. Managedfactorypublishowner,bindRuntime,startworker,
initialize;catchcleanupcreatedowners,clearsingleton/rethrow. RetainsmTimerOwner
sofailureBEFOREtimerdoesn'trequireglobaltimer. Stops/joinsMain/children/timer/net.
Singleton/listener/childowners swapundermutex butretireAFTERunlock toavoidreentry
orjoin deadlocks. Direct publicstartupflagsatomics/errorsoptional+mutex; notall
concurrencycertified. NativeRAII replacesJVMgc; noactualJNIloader/backendprovided.
SourceThread.sleepInterruptedExceptionhandlingnotnativeinterruptible yet; injected
sleep exceptions can differ at swallowed-InterruptedException boundaries.

NEW support/MySQLConfiguration.hpp inlineconfigure optionalstring+sleep: translated
pinnedmysql.convertMySQLParams ONLY(fullmysqlcommandstillabsent). First@ thenfirst
usercolon/DBslash, exactUTF16lengthJDK26substringfailuretext, maskedlog,orderedUserDB
flags/credentials,optionalcoins/delay,actualMySQLConnectreadonlyinit/shutdown,
Exceptioncatch/errorrethrow. Nativehelperacceptsnull; GeneralParams stringnonnullable.
MySQLsingleTxPoWbackup nowexplicitcredentialevaluationlefttoright.

SHARED fixes: NativeThread threadlocalpointer/currentNativeThreadName tracksnested
worker+uncaughthandler,restoresonreturn; fallbackoutsidewrappedthreads "main".
Message boxedint/float null=>JavaNPE,wrongbox=>JavaCCE; boolnullfalse/stringnull
optionalpreserved. JsonDB Boolean/string getters explicitCCE/nullfallback.
getData/getJSON/getJSONArray casts notfullyreviewed. FoundBooleanpoisonedprefs
RuntimeExceptionbad_variant_access vssourceCCE atMaincase199, stringatcase256.

NEW Mainfixture support/main_runtime.cpp/MainRuntimeOracle.java: actualMainbodies,
unstartedworkers,realMinimaDBchildren,sharedSQLrecordingfixture+ArchiveFixture.
NativeManualMain protectedconstructor/friendMainFixture; JavaUnsafealloc no ctor,
sourcefields/timersinitialized. Sourceclassloadedfrompinnedsrc. Opsflags/listener/
retainedJSON/uptime/timers/command/partialshutdown/restore/chain/MySQL/message etc.
Logscompareafterruntimeprefix/framesremoved; SQLnormalizerkeepsclassesbutnotCCE/NPE
prose. Before/afterrealclockvalidatesuptimeoverflow+timerdeadlinebeforelabeling.
Main SQLsyntheticdriver JavaONEinstallDriver("jdbc:"): sequentialinstaller used
previously removedMySQLdriver, fixedfixture-only mismatch. SharedC++harnesscatch
CommandException nowcorrectclass for newdirectcommands.
Maincorpus336programs897ops PASSnormal8.45/san5.11; finalaftercommandfixtureextensions
normal4.15/san3.67. Coverslistenernullable/throw/Error/alias,shutdowngates/timerfirst,
wrongMessageboxes,checker,MySQLconfiguration+enabledsingleTxPoWbackup/SQLfaults.
No actualenabledbackupcommand,fullstartup,networkworkerdispatch orliveSQLtested.
Native main_ownership tests nestedOUTER/INNER names+uncaughthandler, reentrantlistener
destructor, loaderfailureBEFOREtimer andfirstSQLfailure(realMainfactory). Synthetic
haltError, weakMainexpired,singletoncleared,timerjoined,privatemkdtemp only. Bothpass.

All14distinctMainregressionspass acrossphases. Normal13batch68.13s plusfinalstring
getter prefs/utility/MinimaDB40.70s. Sanitizer13/14pass120.12s,NIOprocess100stimeout;
isolatedNIOretryPASS83.64s1918programs withoutcode/timechange. Reportsinspected:
Message905,Prefs1130,Utility1361,Logger2016,Uncaught121,Executor10,MinimaDB2367/7519,
NetworkManager540,NIOmessage1918;worker/nativechecksallpass. Bothfullbuild8logs
noobservedwarnings. Mainreportsinbuild{,-sanitized}/main-runtime-results.json.

NEW mappedcommands base/quit.hpp/.cpp andbackup/rescue.hpp/.cpp registered in
CommandRunner.ALL_COMMANDS,actualMain calls(noextra mocks/runtimecallbacks).
Pinnedbodies FULLread; reusableCommand.cpp/.hpp,Runner.cpp/.hpp,Mainfixture,
MessageStack source/native inspected. Siblingmain.cppsignalshutdownnotcompatible.
quitparsecompactbeforetwoMainlookups,missingMainstill"Shutdown complete";
rescueexplicittrimmedhost wins/configfallbackraw,blankexception,actualPostMessage
MAIN_DO_RESCUE host before response. Fullrescuehandler NOTdispatchedintests.

Mainfixture extended meta/direct/run(caller)/rescueNode/drain; drainsactualqueue
withoutprocessing. Newmain_lifecycle_commands_corpus.py102programs366ops (meta,
null/wrong/blank/Unicode/embeddedzero,missingMain,repeat,configuredhost,adminchecks).
Independentrescuereply=>exactqueuedtype/host/order assertion. CTest193
main_lifecycle_commands_differential via main_runtime_differential.py --groupcommands.
Drivergroupsseparateoracleclasspath/source-listfilenames toavoidsame-buildparallel
javacoverwritingclasses. Javaresetrescueconfigperequest matchesnative resetDefaults.
InitialcommandcomparisonsPASS2.59/3.10, final3.18/2.43. Fullbuildlogsno warnings.
All5regressionsPASS9.69normal/24.00san(batchincludesnewcommands):Command12289,
Runner1265,RunnerErrors1120,Main336/897,Mainownership. Bothfixturegroupsconcurrent
normal4.17pass. All10reportsinspected. No productioncommandmismatchfound.

Docs Main/Message/JsonDB/quit/rescue,labsmain-runtime/main-lifecycle-commands,README,
REVIEW,provenance,mapupdated; guideaudit457lessons9008links2634anchors0errors.
Latestlogs /tmp/minima-main-{commands-build,commands-build-san,commands-test,
commands-test-san,commands-regressions,commands-regressions-san,command-final-tests}.log.
FullMaincheckpointlogs /tmp/minima-main-build8{,-san}.log,main-test6{,-san}.log,
main-regressions.log,main-final-regressions.log,main-regressions-san.log,
main-nio-san-retry.log. All handlesfinished; don'trerunoldgenerations.

NEXT stillfullworkingnodegoal. Four smallnetworkcommands alreadyreadFULLthisturn:
upstream-study/.../network/disconnect.java71,p2pstate.java42,ping.java79,message.java74.
NativeNIOManager.hpp andnetwork/connect.cpp/.hpp readFULL. ExistingCommand/runner
andMainfixture reusable. Need inspectNIOManagerOracle/manualNIO/P2P/Networkfixtures
beforeactualtest/runtimebinding. NOCODEforfouryet. Keyparitydetails: disconnect/raw
uid andmessage/rawdata casts doNOTtrim unlikegetParam; nullchecks precedeMain;
NIOsendNetworkMessage/sendPingMessage staticstill evaluateMain/getNIO Javaexpression.
p2pstate dynamiccastmustdistinguishwrongP2Pclassfromnull. pingno-colonreturnsJavaNULL,
butcurrentnativeCommand.runCommand returnsJSONObject byvalue => APIneedsnullreturn
support beforeporting faithfully. Don'tsilentlyturnnullintoemptyobject/error.
OtherMainMegaMMRhelper inspection isrecordedbelow; stillunimplemented. Source-map
lists61production+44testremaining. No barrier/permissionneeded forauthorizedwork.

# Verified checkpoint349b — Pulse runtime dependency (no new mapped file)

Goal ACTIVE/incomplete; current and previous turns PROGRESS. 349counterparts
305prod+44tests,108absent64prod+44tests. 190CTest. All457lessons authored. All
counterparts partial/not_verified. NO LIVE HANDLES. Main/Minima/durableSQL/full
mysqlcommand andnativeMySQLdriver stillabsent. No realnode/wallet/seeds/server.

Mainintegration inspection found Pulse.createPulse API absent and treepulse list
clearedinplace unlikeJava replacement. ImplementedactualPulse::createPulse using
globalMinimaDBreadLock, getTree/getSharedPulseList, sourceExceptioncatch thenunlock.
Acquisitionoutsidecatch; JavaError/badalloc rethrow withoutunlock. No finallyadded.
Pulse sharedvector list, getSharedBlockList/setSharedBlockList preservealiases;
existingreferencegetters preserved. Addedinstance readDataStream: replaceslist
BEFOREreadversion, unknownversion actualMinimaLogger.log thenreturn, partial
prefixandreaderpositionretained onfailure; staticdeserialise delegatesinstance.
Tree nowsharedvector/getSharedPulseList; calculatePulseList buildsnewvector then
publishesAFTERtraversal. ExistingPulse keepsoldlist. Source 512IDlimit unchanged.

PinnedPulse.java readFULL, siblingPulse.hpp readFULL(1bytecount/60IDcap=>incompatible).
NativePulse/Tree relevantmethods, JavaTree186..204, existingobject/tree/MinimaDB
fixtures+drivers/dependencies inspected. Labnew-tests/main-runtime.md BEFOREtests.
Nativeobjects.cpp Pulse-read branch temporarily suppresses logger justasJava
ObjectOracle readbranch; restorespreviouslogger viaRAII. Alloldtests stillpass.

Extended existingMinimaDB fixturewithpulsecreate/new/hold/set/mutate/recalc/wire/
read/version/snapshot/dataRead, actualTreechains+global/retainedDB replacement.
PulseFaultDB virtualreadLock/getTree recordsandforwards realDB; sourceUnsafealloc
reflectionmatchesnativefriendPulseRuntimeFixture replacingonlytestglobal. Tests
Exception/Error/nulltree/clearglobal duringgetTree. Javafixture catchesThrowable
forobservations; nativeNativeThrowable classification. Fixture safe releaseslocks
AFTERobserving. ProductionMinimaDBheaderonlyfriendaddition. No nativeengineused.
Newpulse_runtime_corpus.py throughminima_db_differential --grouppulse; CMake190th.
Independentaliasassertions ensure sourcecurrenttree/pulse same, divergeafterrecalc,
oldheldmutations stayoldlist. 0/1/2/8/511/512/513nodes, partialeveryprefixsamplewire,
mutableversion, negative/wrappedcounts, null-list, explicitremainingbytes.

First85programcorpus failedcase84 oversizeMiniDataheader0x20000001 afterPulse
version/count: source java.io.IOException: Read Error : MiniData Length larger
than maximum allowed (256 MB) 512.0 MB vsnative MiniData length above maximum.
Fixed MiniData.cpp maxmessage viaexistingMiniFormat.formatSize, expectedsize
message exactsource too. Limitis512MB, sourceerror256textisstale andpreserved.
35directreader cases verifymaxbeforeexpectedsize,negative/zero/truncated data,
expectedsize -2/-1/0/1/2. MiniFormatonlyEnglishdefaultlocale tested; itsglobal
localeparityremainsopen. NoMaxconstant changes orpayloadsizeallocationadded.

BothfullbuildsPASS. First117programs911opsPASS11.40s/25.81s; final152programs946ops
PASS10.18s/25.01s. Handles17917/51731terminal0. All8regressions15201/8360terminal0
PASS63.66s/184.70s. Both7JSONreportsinspected: pulse152,objects9464,tree1027,
utility1361,NIOclient714/13972ops,MinimaDB2367/7519ops/334711drivercalls,
chaincommands324/4096ops; lock/ownershipnativepass. Nativeproductionunchanged
sincebefore8regressions; finaladditivefixture dataRead changedonlynewcommand.
Finalfullbuildlogs /tmp/minima-pulse-runtime-{build3,san-build3}.log no warnings.
Finaltests /tmp/minima-pulse-runtime-{test3,san-test3}.log; regressionlogs
/tmp/minima-pulse-runtime-{regressions,san-regressions}.log. Earlierfailedlogs
build/test/ sanbuild/test retained, no needrerunoldgenerations.
LessonsPulse/Tree/MiniData/Main+lab/README/REVIEW/provenance/index/audit updated.
Latestguideaudit9003links2634anchors457hashes0errors. Mapremains349. UnusedPulse
mPulsePoW notrepresented; nullMiniDataelements/bytealiases/mutableglobalcaps/
arbitraryconcurrentmutation open. No fullfileparity claim.

NEXT: implementMainbody/header andrealservicebindings, notyetwritten. PinnedMain
readFULLpriorcheckpoint; all headerslistedbelowinspected. IBD::services and
Greeting::services NOT currentlybound inMinimaDB::bindRuntime; Mainmustbindthose,
inadditionNIOServices/commands/vaultMainReceiver/loggerMainReceiver. Pulsefactory
nowexists. Source helper megammrsync.getCurrentMegaMMRIBD379..540 readthisturn:
getAllCoinProofs(searchMegaCoins) -> realcoinexportcommand->CoinProofdecode, fresh
IBD.createCompleteIBD, peers.getPeersList(20). searchMegaCoins walksfromtipthrough
allcoins, matchesaddressORstate orpubkeystate, spent-IDdedup,deepCopy, optionally
locks globalMegaMMR coins afterreachingend; nofinally unlock, noMega passifinitial
tipnull. Fullmegammrsync commandnotread/ported; helperstillmissingnativebinding.
mysql.convertMySQLParams helperstillmissing, priorcheckpointhasexactbehavior.
Mainstartup optionalJNI/gc nativeplatformadaptation remains toresolve; other
sourcecallbacks requireactualMain-ownedcomponents, notrecording/fakesinruntime.
No Mainmappedfile counted or Mainstartupclaimmade. Mainlabcurrentlyplansits tests
andrecordsverifiedPulse dependency only. Continue fullworkingnodeobjective.

# Verified checkpoint349 — MySQLConnect prerequisite for Main

Goal ACTIVE/incomplete. Current and previous goal turns PROGRESS. 349 counterparts
=305production+44tests;108absent=64production+44tests. All457studentlessons authored;
allcounterparts implemented_partial/paritynot_verified. Main/Minima/durable native
SQL/MySQL driver/fullmysqlcommand remain open. No live handles, no agents needed,
no realwallet/seed/node/DBserver/credentials, no commits/push/deployment.

NEW src/org/minima/utils/mysql/MySQLConnect.hpp/.cpp fulldatabase-method bodies,
CMake librarysource+minima_mysql_connect harness+mysql_connect_differential(600sec),
189CTest total. Optional driver callback REQUIRED; defaultinit throws SQLexception.
No fake successful storage. JDBCdriverdiscovery replacedcallback; demo main body
translated but noClass.forName/driver-class path and nottested. Requires native
MySQL driver/liveisolatedSQL tests beforefullintegrationclaim.

Exact4DDL+14preparedSQL, nullablecredentials, readonlyskipsDDLonly, partialinit
assignments retained, noautomaticreopen, firstDESC/lastASC, range>=start ASC250,
SQLcatchscopepermethod, partialrange/nullconvertedblocks, 14coinbindings with
setDouble(4,14), MMRdecimal-long truncation/wrap usingunsigneddigits, Clobexact
metadata/null/hidecase, sourceerrorJSON. Helper fixedDouble reusesJavaFloating,
JDK26 FormattedFPDecimal(decimal Double.toString thenhalf-up), ICUdigits/decimal,
negativezero/nonfinite/extremes. JDKsource /tmp/minima-FormattedFPDecimal.java.

SharedSqlConnection.hpp adds SqlDouble(doubleValue), defaultvirtualsetDouble
THROWS unsupporteddriverSqlException. SqlRecordingFixture.hpp addsrealDouble
object andbitpatterncell, setDoubleeventstringJavaFloating; SqlDBOracle.java
actualDoublecells/setDoubleevent, installDriver(prefix) overload defaultjdbc:h2:.
New mysql_connect.cpp/MySQLConnectOracle.java useactual archive/coinfixture codecs,
exacteffects/logs, realclockbefore/after. mysql_connect_differential.py normalizes
SQLdiagnostics with existingnormalize; blockSave timestampwindowchecked first.
1598programs5399ops60287SQL, 316doublevaluesx7locales, allpublicmethods,SQL/runtime
failuresatdifferentcalls, nullobjects/statements/rows/meta, truncation/wrap/Clob,
readOnly/nullableunicodecredentials. Independentquery/bindassertions.

Finalfullbuild26277/7954PASS andcomparison40.98s/110.76sPASS, bothreportsread.
Earlierbuildfailures fixedSQLDDLextractrawtabs viajsonstrictFalse, missingcomplete
JSONArrayinclude. No productioncomparisonfailure. All5regressions96566/29082PASS
42.11s/118.87s, all10reportsinspected:SqlDB464,TxPoWSQL2592,CoinDB1350,Archive3333,
Wallet675. Logs /tmp/minima-mysql-connect-{build3,san-build2,test,san-test,
regressions,san-regressions}.log. No furthercodechangesafterpassedtests.

Lesson MySQLConnect updated, labnew-tests/mysql-connection.md BEFOREtests,
README/REVIEW/provenance/map/index/audit349,9000links2634anchors457hashes0errors.

NEXT MAIN: pinned upstream-study/src/org/minima/system/Main.java1379lines READ
COMPLETE. Workingroot src/Main haslater1424lines MDS changes; usePINNEDONLY.
PinnedMainhash143e67f90b0941fa9c7bd8626e6092396d64c0cb85fd5e9a5346132daf18ffa1.
No Main nativebody/header ornewMainlabyet. Main/service/header gate inspection
performed: fullMinimaDB.hpp,NetworkManager.hpp/.cpp,MessageProcessor.hpp,
TxPoWProcessor/Miner/Generator,NotifyManager,SendPollManager,NIOManager/NIOMessage,
NIOManagerServices/NIOMessageServices,P2PManager/P2P2Manager/P2PFunctions headers,
SqlDB/MiniFile/TimerProcessor/Message/MessageListener,UserDB/GeneralParams,
GenesisMMR/TxPoW,TreeNode,RawTxnCommands,CommandRunner/Command headers.
Sibling minima-core-cpp/src/main.cpp readwhole overchunks; separatelegacychain,
miner/genesis/options/storage incompatiblewhole. SQLiteDatabase.hpp mostlyread
plusfirst85persistencetestlines; cannotreplaceMySQLdialect/metadata. Native
existingcomponents andfixtures remainreusable. Announcedexactreuse to user.

Mainstartup requires mysql.convertMySQLParams; sourceONLYthatmethod1356..1446
read(notwhole1446linecommand). Nativewholemysqlcommand absent. Helperparse
first@,userfirstcolon,DBfirstslash;substringerrorscaught;maskedlog;setGeneral
MYSQL_STORE_ALLTXPOW true thenUserDBhost/db/user/password/autologin/autobackup,
optionalcoinsbackup; optionaldelay(nonzero,negativeexception); newMySQLConnect
readonlyinit/shutdown; catchExceptionlog Failed to connect... formaterror.
Need reuse actualhelper translation without claimingfullmysqlcommandexists.
NIOMessage.getCurrentMegaMMRIBD callback delegatesunported megammrsync helper;
Main runtime can'tclaimcomplete untilactualbindingimplemented. Main bind
NIOManager.runtimeLookup/NIOClient.runtimeLookup/NIOMessageservices; MinimaDB
bindRuntime already bindschain/consensus/greeting/IBD/searcher. Command bindings
needed: txnutils.getMiner,txnminepost.postMessage,search.keys.postMessage,
network.connect.postMessage,send.sendpoll.getManager. Searchrestforotherstatics.
System.gc onlyhint needsnativeownershipadaptation; noexistingallocatorrelease
helper found. JNIload optionalnative-library path unported. No Maincodewritten.

Mainmethods source summarized in previous turn compaction; readagainfrompin for
translation. Notable: listeneroneargthrows, twoargignoresException; clearsingleton
beforeSHUTDOWNlistener once; shutdownFinalProcs notify onceconditionallegacyMDS
flag+processorstopwait; restoreReadyForSync recreatesprocessor beforeDBreload;
restartNIO DBreadlock finallyunlock, networkstop+250mswait hardafter>10s,5sdelay;
MAIN_NETRESTART10sdelaythenIBDreset+clearpeer/IBD+restart. Timers rescheduled
BEFOREwork exceptMySQLperiodic aftercatch. Use existing recursive/ownerlocks and
factories aftersharedownership; testwithactualcomponents/recordingboundaries,
no realnode/network/userdata. Mainhasnotbeen counted yet.

# Verified checkpoint348 — seedrandom and four original casting tests

348 counterparts (304production+44tests),109absent(65production+44tests).
All457studentlessons authored; allcounterparts implemented_partial/paritynot_verified.
Full native node/Main/Minima/durableSQL/mysql stillincomplete. GoalACTIVE and
currentturn PROGRESS. No live handles. No realwallet/seed/node used. No commits.

Since343: base/seedrandom.cpp/.hpp source79lines, registered likeJava.
Reuseswalletlockedcheck BEFORE modifier parsing, getParamUTF16 reply, existing
UnicodeencodeUTF8 Java '?' surrogate replacement forMiniString, MiniData wire,
Crypto.hashObjects twice with marker0xDEADDEAD, cachedSeedRow.getSeed.
Sourcehashbytes B(B(UTF8(modifier)))||B(marker), then B(hash)||B(seed).
wallet_commands_differential --groupseedrandom, newseed_random_corpus.py, no
new fixtureoperations (reuseload/vaultSeed/locked/meta/direct/run).
309programs782ops14616SQLnormal12.30s/san5.50sPASS. Independenthashlibexpected,
synthetic seeds only; isolatedsurrogate response transport becomes '?' onJava
stdout. No productiondiscrepancy found. All4regressions(wallet675,command12289,
runner1265,node_state363)PASS23.31s/75.25s. Both4reportsinspected.
Fullbuildsessions39103/19369terminalPASS. Regression8986/52021terminalPASS.
Logs /tmp/minima-seed-random-{build,san-build,test,san-test,regressions,
san-regressions}.log.

4newmappedtest/cast/{BOOLTests,HEXTests,NUMBERTests,SCRIPTTests}.cpp reuseLENTests
nativeContract/constant-expression harness. Sourcefullread4tests157/177/149/172
lines andnative+JavaBOOL/HEX/NUMBER/STRING, values/expression/functionchecks,
registry; siblingFunctions.cpp relevantcasts andscript-leveltest examples read.
Allsourceactiveassertions preserved:21/24/19/23 (87total),3methods each.
SCRIPTTests tests STRING. Historicalcomments"fails"stale; activeexpectationspass.
No productioncastchanges. Java launchers support/{BOOL,HEX,NUMBER,SCRIPT}CastOriginalTests.java,
script_value_original_tests.py extendedchoices/package/metadata. CMakeadds8CTest,
188total. Nativecastresultclass nowdynamic_cast asJavacast (reviewrefinement).
BothfullbuildsPASS, initial8checksPASS7.70s/7.68s; finaltargetrebuild+10checks
includingLEN+MinimaFunction originaltestsPASS11.35s/11.12s. Both6finalJSONreports
read, no compilerwarnings. Sessions62688/50653/1045/91697terminalPASS.
Logs /tmp/minima-cast-tests-{build,san-build,build2,san-build2}.log and
/tmp/minima-cast-tests{,2,-san,-san2}.log.

LabsBEFOREtests seed-random-command.md/cast-original-tests.md; source/testlessons
updatedincluding4productioncastlessons. Tools author_seed_random.py added.
README/REVIEW/provenance/map current348. Latestguideaudit8997links2634anchors
457hashes0errors; all457authored. No newagentneeded/backfillalreadycomplete.
RootJava/upstreamstudypinunchanged. Don'trerunold/tmpgeneration scripts.

Next unresolved work: 65production+44tests. timemilli.java73lines completelyread
thisturn, notimplemented: System.currentTimeMillis then minutesback takespriority
elsehoursback; numericgetAsInt then1000*60[*60]*int ALL32bitoverflow BEFOREwidening
tolong; subtractwrappedresult thenDate.toString. Help begins literal tab due
source\timemilli escape. Needreuseexistingdateformatter/clock+driver boundedtime
comparison afterdependency/siblinggate. Couldpairtimewithanothercommand but
no furtherimplementationplanned. Casttests remainingbase/sha/state/etc listed
source-map. Main/nativeSQLstillmajorunfinished work; no fullparity claim.

# Verified checkpoint343 — node-state commands

343 counterparts (303 production+40tests),114absent (66production+48tests).
All457 lessons authored; allcounterparts partial/not_verified; fullnode incomplete.
No live sessions. Normal/sanitized full builds PASS, 363programs1411ops21009SQL
node_state comparison PASS20.23s/35.38s. All13regressions PASS64.09s/145.86s.
Both11JSONregressionreports inspected. Logs /tmp/minima-node-state-{build5,
san-build5,test7,san-test7,regressions,san-regressions}.log. No warnings observed.
179CTest; source-map andguideaudit PASS8982links2634anchors457hashes0errors.
New base/mempool+cointrack+magic; onlyfirsttwo registered, magic source class
NOTregistered. Fourbaselinefindings fixed: magicregistration, JsonDBgetNumber
Stringcast, MinimaDBnullableSQLCoinbridge usingfriendCoin(nullptr), TreeNode
emptyinnerloop nullread. Retainedlistaliases stillopen. Random SQLtreeID shape
checked32bytes+stablelabels, otherbindings/effects exact. Labwrittenbeforetests,
lessons3commands+Coin/MinimaDB/JsonDB/TreeNode updated, rootREADME/REVIEW/provenance.
Nextseedrandom source fullyread; gateinspection ofWallet/SeedRow/Crypto/
MiniString/MiniData/Command/Unicode/SQLfixture andcurrent+siblings inprogress.
UsegetParamUTF16 for replymodifier (retainisolatedsurrogates), encodeUTF8
Java replacementforMiniString; Crypto.hashObjects givesobjectlengthboundaries.
No seedrandomcodewritten atcheckpoint. No newagentneeded:457backfilldone.

# Latest VERIFIED checkpoint — txpow search; map340

Goal ACTIVE/incomplete. This goalturn PROGRESS; previous goalturn PROGRESS.
340 counterparts =300 production+40 tests;117 absent =69 production+48 tests.
178 CTest entries. All457 student lessons authored. Allcounterparts partial,
paritynot_verified. Main/Minima/durableSQL/native mysql absent. No blockers/
approvals/agents/commits/pushes/livewallet/node. Do NOT mark goal complete.
NO LIVE TOOL/BUILD/TEST SESSIONS. All handles polled terminal.

Both full builds93598/38830 PASSwithoutwarnings; alsoinitial86769/5362PASS.
Initial677programs2090ops41337SQL PASS10.93s/25.58s inbuild/test4.
Final680programs2099ops41510SQL PASS10.28s/26.97s. All12selectedchecks39590/55967
PASSnormal58.91s/sanitizer156.18s. All22finalreportsread. Not all178CTest.
Othergroups wallet675/1907ops50275SQL13.00s/72.14s;
history484/1489ops52194SQL5.71s/9.20s;Archive3333/10831ops125655SQL32.12s/85.19s;
TxPoWDB1355/7407ops80960effects23.38s/60.94s;OnChain855/3639ops29167SQL4.90s/6.10s;
SqlDB464/1260ops8352SQL2.61s/2.87s;searcher189/6468queries10.31s/22.35s;
Command12289(8.16s/18.79s),runner1265(2.84s/3.13s),errors1120(3.85s/3.85s),map340.
No native/compiled fixture edits afterfinalfullbuilds; three latercorpusreviewcases
ran infinalbatch. Do not repeat passedchecks withoutnewchanges/concerns.

New search/txpow.hpp/cpp registered+CMake. Pinnedfull294lines + relevantnative
DB/OnChain/Archive/Searcher/TxBlock/UserDB,CommandResult,RawTxnCommands.object,
StateText,existingfixtures/tests read. No siblingtxpowcommand/currentqueryrouting.
Priority action>txpowid>relevant>inblock>onchain>block>address bypresence.
info SQLsize,OnChain.size/first/last,retentiondays; txpowid getAddressParam -> real
RAM/SQL -> archiveTxBlock.sharedTxPoW -> optionalautoMySQLprefs, realrunner
mysql action:findtxpow txpowid:<id>,firstreplystrictJSONObject/Booleanfound;
ignoresnestedstatus,returnsrawtxpowinclnull/nonobjectwhenfoundtrue,exactnotfound.
relevantpresenceIGNORESvalue,maxSQLmutabledefaultintwrap,SQLgetAllRelevant then
IDdedupencounterorder. Membershiponlyunordered_set: neveriterateit. inblock
retrievesOnChainDBfirst, lower0x ID/getDataParam elsegetNumber.getAsLong, txnsarray.
onchain actualsearcherthenOnChainDB, getblockobjectTextnonnull+MiniNumber,
Stringblock/blockid/tip/confirmations=tip-height(no+1);falseonlyfound:false.
block actualchainthenarchiveheight,errorifmissing; address actualsearcherincludes
mainANDburninputsoutputs thenIDdedupstable. Reuse sourcehelpandvalidparams.
No productionbehavior mismatchfound. RetainedJSON/referencefullparitystillopen.

Sharedrecordingfixtures SqlRecordingFixture.hpp/SqlDBOracle.java extendedoptional
rowsBySQL forpreparedqueries: PreparedstoresSQLtext,Rows picksconfig.rowsBySQL[key]
ifpresentelseoriginalconfig.rows. Existingmetadata/events/defaultdynamicrowsretained.
Permits trueSQLmiss+archivehit inonecall. No engine/queryfilterimplementation.
Walletfixtureadds txpowStores(actualSQL+OnChain+Archiveload),txpowPrefs(login/days),
txpowRAM(actualRamData insertion via publicpool), staticretentionreset3perprogram.
Java txpowSearchFixtures convertswireusingactualTxPoW+TxBlock andemitsID/wire/block/
serializedJSON. Reuses balanceFixtures/nativecoins,historyFixtures fordeterministic
TxPoWs,existing3nodeMMRfixtureswithreplacedTxPoWs andsourcecoins.
RecordingMysql inheritsCommand withfixturevalidparamsaction/txpowid, recordsactual
text/params,returnscontrolledJSONObjectorthrows. NativeRAIIsavesrestoresregistryand
appendsprototypeonlyinsidefixture; Javaexistingfinalarrayelementreplace/restore
likeSendRecordingScope. Realrunnerused. inputtxpowSearchaddsforwardedoutputperop.
MySQLproductioncounterpartstillabsent, doNOTclaimrealMySQLintegration.
Driver --group txpow uses txpow_search_corpus.py fixtures/corpus/project/validate,
preservetextusingexistingbalancehelper. No newnormalization. Explicitstoredtimes.
680cases coverprecedence,IDnormalization,relevantpresence,int/longwrap,SQL/OnChain/
archivehits/misses/null/partialload/failures,sourcewiredecoding,ram/chainnoSQL,
addressdedupinclburn,confirmationsnegativeandtipzero,mysqltypednestedresults,
failednestedstatusfoundtrue,othermodesnoMySQL,publicregistry/permissions/errors.
InitialJavafixture attemptedfinalarrayassignment correctedbeforecomparisons;
independentaddress0x01expectedomittedleaf2burninput corrected; forwardingfieldmissing
fixed inbothfixtures. No source/nativeproductionfixnecessary. Allthreeexplainedlab.

LabBEFOREtests student-guide/new-tests/txpow-search.md inclreviewextensions;
author_txpow_search.py reuseshistoryauthorheading/sourcehashprep. txpowlesson,
courseREADME/INDEX/audit/rootREADME/REVIEW/provenance current. Audit8976links2634
anchors457hashes0errors. Scoped12sourcefileswhitespace/conflictsPASS;gitdiffcheckPASS
(untrackedcpp limitation). Reviewscopedapprovewithsuggestions. All457backfilldone;
no newagentneeded. No newdependencies.
Logs /tmp/minima-txpow-search-{build-fixed,san-build-fixed,test4,san-test4,
regressions,san-regressions}.log; earlierbuild/testfailurelogsremain.
Reports eachbuild: txpow-search-command-results.json,wallet-commands-results.json,
transaction-history-command-results.json,archive-manager-results.json,
txpow-db-results.json,onchain-db-results.json,sql-db-results.json,
searcher-differential-results.json,command-differential-results.json,
command-runner-differential-results.json,command-runner-errors-results.json.
AvoidglobdumpingolderhugefailureJSON. Nolivehandles.

NEXT PREREAD: FULLsource base/mempool.java82lines,cointrack.java93lines,magic.java67
READthisturn. No otherreusegate/lab/codeyet. Canbatchthesethreecommandportsnext
withsharedwalletfixture ifappropriate, butfinishreusegatefirst.
mempool no validparams/help override. DB.getCompleteMemPool elements enumeration
ConcurrentHashMap<RamData>. Fornoncascade, getTxPoW,size++,appendid/transaction/block,
then callisTransaction/isBlock againincrementcounts;cascadeonlycount,noTxPoWaccess.
resp txpowarray,totalpoolsize,onchain(non-cascadecountNOTactualchainflag),cascade,
transactions,blocks,sqlcoindb=CoinDB.getTxPoWTreeCoinDB().getSize;Javaintwrap.
cointrack rawcoinidrequired->MiniData parseBEFOREenableBooleanrequired; findnode
usingnewMiniDataagain; noNodeexact Coin not found coinid : <raw>; searchCoin(id,false)
whichcanreturnnullforspent evenwhengetTreeNodeForCoinfoundspentnode. Enabletests
node.isRelevantEntry(coin.MMRentry); alreadytrackedstring elseaddentrytoactual
relevantentries,calculateRelevantCoins,addedstring. Disable removeRelevantCoin,
calculateRelevantCoins,removedstring. Needobserveentrylist/relevantcoins/MMReffects
andtypedfailureordering, actualCoinDBinteraction. Sourcefullhelpavailable.
magic UserDBlookupfirst; ifpresentsetdesiredKISSVM thenmaxTxPoWSize thenmaxTxns via
getNumberParam, preservingearlierwritesiflatergetterfails. Onlyafterwrites gettip;
resp.lastblock=tip.txpow.magic.toJSON then desired MiniNumberOBJECTS(rawnumericJSON)
forKISSVM/size/txns. MissingtipNPEafterprefsmodified. No helpoverride. Needexisting
UserDBset/get/storage tests +Magic/TXPoW generation integration; no saveDBsourcecall.
No dependencies/candidates/tests for these three fullyinspectedyet beyondcurrent
TxPoWDB/RamData/searcher/membership reuseprereads. Neednextgate+announcepaths+pretestlab.

---

# Latest VERIFIED checkpoint — history; map339

Goal ACTIVE/incomplete. This goalturn PROGRESS; previous goalturn PROGRESS.
339 counterparts =299 production+40 tests;118 absent =70 production+48 tests.
177 CTest entries. All457 student lessons authored. Allcounterparts partial,
paritynot_verified. Main/Minima/durableSQL absent. No blockers/approvals/agents/
commits/pushes/livewallet/node. Do NOT mark goal complete.
NO LIVE TOOL/BUILD/TEST SESSIONS. All handles polled terminal.

Final full builds99172/3622PASSwithoutwarnings + history484 programs1489ops52194SQL
PASS5.46s/10.04s. No production edits after those builds. Initial full88826/40121
and reviewed98206/20682alsoPASS. Prior483suite48976/49578PASS5.59s/10.10s.
All10 distinct selected checks pass across phases, notall177CTest.
Normal9-regression14492PASS158.81s; sanitizer55233eightPASS, SQLtimeout120.01s,
batch160.85s. SeparatefullSQLdriver91536PASS2592programs8729ops86333SQL. Original
sanitizerCTestbatch remainsfailed; doNOTcallitpassed. All18finalreportsread.
Othergroups wallet675/1907ops50275SQL52.79s/136.01s;
balance275/776ops21515SQL42.69s/95.98s;coins287/1144ops25168SQL44.57s/99.90s;
command12289(26.56s/27.25s),runner1265(10.28s/8.25s),runnererrors1120(12.37s/8.38s),
HashMap135(158.80s/160.84s),SQLnormal46.97s,map339. No need repeat passed checks
withoutnewchanges/concerns. OtherregressionsranbeforefinalmutablehelperAPIedit;
that change only affects history/helper fixture, rerunfinalhistory afterbothbuilds.

New search/history.hpp/cpp registered+CMake. Fullpinnedsource/relevantdeps read;
no sibling history/getTxnDetails. Reuse actualSQL,Wallet.isAddressRelevant,Coin/
Token/MiniNumber,TimerMessage.currentTimeMillis,JavaHashtable,JavaHashMap forHashSet.
Actions size/relevant defaulttrue; false uses realnow-86400000, MiniNumber.getAsLong;
customsize rawwherepassesactualSQLfilter; transactions tipblockreadBEFOREloop
(depth0stillnotipNPE), counts eachisTransaction plusgetTransactions.size inclduplicates,
Javaintwrap,startblockrawJSONnumber; listmaxmutable100default+offsetintwrap,
SQLrelevant/latest, txpow.toJSON BEFOREvirtualgetTxnDetails, paralleldetails.
Helper maininputs thenoutputs, onlywallettrackedaddress, notsimple/state/burn.
Minima rawamount; nonMinimaCoin.getTokenAmount FALLBACKrawifmissingmetadata.
Hashtableamounts thenkeys enumeration ->JSON andHashSetunion, output-minus-input.
Reviewchangedconst TxPoW* to mutableTxPoW* topermitJavaoverride mutation.
sharedptrJSONObject return permits override null. RetainedJSONaliasfullparityopen.

Sharedwalletfixtures add historyLoad,historyLimit,historyDetails(nullTxPoW/body/
transaction),historyOverride; staticSQLlimitreset100 perprogram. Onlyhistoryflag
adds actualbefore/after clock andhistoryCalls. RecordingHistory returnsnull/object,
throwsor sets detailBlockusingactualsetBlockNumber, loggingID+blockobject oncepercall.
Java historyFixtures builds actualTxPoW wirefromexistingbalance Coin/Tokenfixtures;
optional wire/amountobject constructsactualCoinwithnewamount/copiesToken. Fixedtime0,
nonceindex,PRNG32x12,block,states,main/burncoinlists,duplicateblockIDs. Optionalarrays
skipped. CorrectsourceAPIaddBlockTxPOW (not addBlockTxPOWID).
Driver --group history usestransaction_history_corpus.py andexistingbalanceFixtures.
484casesinclmetadata,parameters/order,SQL/runtimefailures,nullable/partialload/rows,
malformedwire,maxoffsetintwrap/staticdefault,longwrap,realclock,helpernulls/overrides,
trackednonsimpleaddresses,tokenfallback/scales,signedzero/repeated/one-sided/manytoken
amounts,chaincounts/noTip/publicdispatch. replyTextpreservedusingbalancehelper.
Defaulttimestampvalidatedactualbefore/after/dayoffset + SQL/responseagreement before
replacingonlynumericstartmillifield. Explicit times exact. ExistingNPE/CCEproseonly.
Independentheightmutationtest: originalserializedheader20, helperargumentmutated99.
InitialfixturearrayNPEcorrected before comparisons; no productionbodymismatch.

LabBEFOREtests student-guide/new-tests/transaction-history.md (also beforemutable
reviewtest);author_transaction_history.py,historylesson/courseREADME/INDEX/audit
current. Authorconvertedalternatebackfillheadingsandreorderedlimits/sourcehashbefore
usinglesson_writer; duplicatesremoved. Audit8972links2634anchors457hashes0errors.
README/REVIEW/provenance current. Scoped10sourcefileswhitespace/conflictsPASS;gitdiff
checkPASS(untrackedcpp limitation). Reviewscopedapprovewithsuggestions. All457backfill
completedearlier; no newagentneeded. No newdeps or SQL implementation.
Logs /tmp/minima-history-{build-final,san-build-final,test-final,san-test-final,
regressions,san-regressions,san-sql-separate}.log. Initial/reviewlogsremain.
Reports eachbuild: transaction-history-command-results.json,wallet-balance-command-
results.json,coin-search-command-results.json,wallet-commands-results.json,
command-differential-results.json,command-runner-differential-results.json,
command-runner-errors-results.json,hashmap-differential-results.json,
txpow-sql-db-results.json. AvoidglobdumpingolderhugefailureJSON.

NEXT PREREAD: search/txpow.java FULL294lines READ thisturn. No othergate/lab/codeyet.
Priority action > txpowid > relevant > inblock > onchain > block > address.
actiononlyinfo: actualTxPoWDB.getSqlSize,OnChainDB.size/first/last,storedaysglobal.
txpowidgetAddressParam -> getTxPoW -> Archive.loadBlock -> optionalautoMySQL login:
realCommandRunner mysql action:findtxpow txpowid:<id>, castfirstresponse, Booleanfound,
returnresponse.txpow unchanged elseexactnotfound. mysql counterpartstillabsent.
relevantbyPRESENCEignorevalue, maxmutableSQLdefaultintwrap,SQL.getAllRelevant,
dedupIDwhilepreservingencounterorder, toJSONonlyfirst. inblock getsOnChainDBfirst,
rawString startsWithlower0x->getDataParam normalizehex elsegetNumber.getAsLong,
actualoverloadedgetInBlockTxPoW, response.txns.
onchain getDataParam,Searcher.searchChainForTxPoW -> fallbackOnChainDB.getOnChainTxPoW;
foundDB rowBooleancast,getblock.toString->MiniNumber, tip; Stringblock/blockid/tip/
confirmations=tip-block(no+1),notfoundonlyfalse. Foundchainusesactualblock.
blockgetNumber,Searcher.getTxPoWBlock thenArchive.loadBlockFromNumber, exactheight
errorifnull. addressgetAddressParam->Searcher.searchTxPoWviaAddress MiniData,
dedupIDencounterorder. No params error Must Specify search params. Metadatafullhelp
andvalidparams inclaction/inblock. Need inspectfullrelevantSQLOnChain/Archive/search/
runnerMySQL candidates/tests+siblingbeforeimplementing; preserveactualrunnerboundary.

---

# Latest VERIFIED checkpoint — coins; map338

Goal ACTIVE/incomplete. This goalturn PROGRESS; previous goalturn PROGRESS.
338 counterparts =298 production+40 tests;119 absent =71 production+48 tests.
176 CTest entries. All457student lessons authored. Allcounterparts partial,
paritynot_verified. Main/Minima/durableSQL absent. No blockers/approvals/agents/
commits/pushes/live wallet/node. Do NOT mark goal complete.
NO LIVE TOOL/BUILD/TEST SESSIONS. All sessions polled terminal.

Both full builds88906/43551 PASS without warnings. No productionchangesafterbuilds.
Final9checks64358/77145 PASSnormal61.55s/san193.68s; all16reportsread.
Coins287programs1144ops25168SQL20.89s/41.27s;
balance275/776ops21515SQL19.62s/38.31s;wallet675/1907ops50275SQL21.26s/84.45s;
funding736/3418ops39016SQL61.54s/193.67s;searcher189programs6468queries12.30s/26.22s;
command12289;runner1265;runnererrors1120;mapping338. Notall176CTest.
Initial285/1136ops24992SQL17579/25628PASS11.30s/27.70s afterfixturefix.
First53006/78837failedindependentblank-stateexpectation9.73s/22.35s, bothJava/native
correctlythrewBLANK param not allowed : state. No productiondiscrepancyfound.
No need to repeat passed checks absent new changes/concerns.

Newsearch/coins.hpp/cpp registered+CMake. Sourcefullfile/readrelevantdeps/tests;
no siblingcoins implementation. ReusesactualTxPoWSearcher.searchCoins,
txnutils.getMiner service for Main,TxPoWMiner.checkForMiningCoin,
TxPoWDB.checkMempoolCoins(actualRAMwitnessmembership),send.selectCoins,Token/Coin.
Parameterrelevance=existsParam(relevant) OR noneofrelevant/coinid/address/state/tokenid;
valueIGNORED evennull/false. Amountalone stillrelevant. Readsimplestate/sendable,
rawcoinid,amount,addressgetter,rawtokenid,state,coinage0,depthBILLION.getAsInt;
no-tip=>emptyarray BEFOREmegammr/checkmempool/order/totalamount. Megaiffglobal.
Searchwithstatewildtrue;agecreated<=tip.block-coinage;checkmempooltruegetsDBthen
Mainminer evenempty; percoinminingfirstthenRAM. NoMainthrowsatlookup; presentMain
nullminer onlydereferencedifcoinvisited. StableascendingcreatedonlyorderEXACTasc;
otherstringsretainorder. Totalamountifpresent: nonMinimatokenid+nonemptylist
requiresfirstcoinToken.scaleMinimaAmount, thenactualsend.selectCoins.
Coin.toJSON(simplestate)+ageString. IMPORTANT CORRECTION topriorpreread:
MiniData.ZERO_TXPOWID is0x00, SAMEasTOKENID_MINIMA; defaultdoesnotforceconversion.
Fullsourcehelp/metadata retained. No newsearch/miner/poolimplementation.

SharedwalletfixturePRODUCTIONTESTSUPPORTUNCHANGED. Drivernew--group coins imports
coin_search_corpus.py, existingwallet_balance_corpus.fixture_request restrictedto
native+JSONnamedRETURNTRUE+RETURNFALSEtoken groups; JavaalreadyexistingbalanceFixtures
andwitnessFixtures producecoins/state/proofs. Existingminerop/poolop used.
Corpus287: presencevsBoolean,depthwrap/age,simpleaddress,states/rawparams,
nullMain/nullminer,spent/dedup/Mega,reservations/actualRAMcascade/onChain/burn,
selector grouping/reordering/scaling/fallback,publicdispatch. Exact IDs/ages/
stableorder; reviewaddedindependentstateIDs(non-vacuous) and2RETURNFALSEtoken cases.
No newnormalization. CoinstoJSONplainnamesquotealready; rawmalformedbalanceunaffected.

LabBEFOREtests student-guide/new-tests/coin-search-command.md;author_coin_search.py,
coinslesson/courseREADME/INDEX/audit current. README/REVIEW/provenancecurrent.
Final guide audit8968links2634anchors457hashes0errors.
Scoped7changedsourcefileswhitespace/conflictsPASS;gitdiffcheckPASS(untrackedcpp limits).
No newagentneeded: all457backfilllessonsprealreadyauthored. Reviewscopedapprovewithsuggestions.
Logs /tmp/minima-coins-{build,san-build,test-reviewed,san-test-reviewed,regressions,san-regressions}.log.
Reports eachbuild: coin-search-command-results.json,wallet-balance-command-results.json,
wallet-commands-results.json,coin-funding-commands-results.json,command-differential-results.json,
command-runner-differential-results.json,command-runner-errors-results.json,searcher-differential-results.json.

NEXT PREREAD: search/history.java FULLYREAD thisturn. No body/lab/gatecompleteyet.
Actionsize relevanttrue=>SQLgetRelevantSize; false=>defaultcurrentTimeMillis-86400000
thenstartmilligetNumber(default).getAsLong;SQLgetLatestTxPoWSize;response startmilli,size.
customsize requireswhere rawString;SQLcustomSizeQuery. transactions depthdefault1720
getAsInt;tip.getBlock BEFOREwhile evenifdepth<=0; counts txpow.isTransaction plus
getTransactions.size pernode withJavaintwrap, movingparent;respstartblock MiniNumber
RAWobject,depthcount,transactions. list maxdefaultTxPoWSqlDB.MAX_RELEVANT_TXPOW100,
offset0,intwrap;getAllRelevant orgetLatestTxPoW;for each txpow appendtoJSON then
publicgetTxnDetails;resp relevant,txpows,details,size. Unknownaction throws.
PublicgetTxnDetails usesMAINtransactiononly, relevantwalletADDRESSonly;ins/outs
Hashtable<String,MiniNumber>; nativeamount orgetTokenAmount forothers. Enumeration
Hashtable.keys order yieldsins/outsJSON+addskeys toHashSet;iterateHashSet forout-in
JSON. No burn/state relevance included. NullTxPoW/transaction/coin/token typedfailures.
Need maintain publicoverridablehelper (Java defaultvirtual), sourcefreshfunction.
NativeSQL methods+mutableMAX_RELEVANT_TXPOW exist (searchedsignatures, NOTfullread).
FullJavaHashtable.hpp read; nativeJavaHashMap<T> inJavaConcurrentMap.hpp available
viaSocketAddressSet.hpp (wrapperread, underlyingNOTreadyet) forHashSetordering.
TimerMessage.hpp readcurrentTimeMillis staticnoninjected; clock strategyNOTchosen.
Need complete nextreusegate incl SQL/getTxnDetails candidates/callers/siblings/tests,
announce selectedpaths, thenwritepre-testlab. No newhistoryimplementationstarted.

---

# Latest VERIFIED checkpoint — balance + arithmetic constructor; map337

Goal ACTIVE/incomplete. This goalturn PROGRESS; previous goalturn PROGRESS.
337 counterparts =297 production+40 tests;120 absent =72 production+48 tests.
175 CTest entries. All457 student lessons authored; all counterparts partial,
parity not_verified. Main/Minima/durableSQL absent. No blockers/approvals/agents/
commits/pushes/live wallet/node. Do NOT mark goal complete.
NO LIVE TOOL/BUILD/TEST SESSIONS. All sessions polled terminal.

Both final full builds65913/95723 PASS without warnings; initial48626/74311 PASS.
All13 distinct selected checks PASS in each build across phases, not all175.
Balance98349 normal275/776ops/21515SQL11.57s; sanitizer99852 balance37.07s.
Normal11-regression batch57193 PASS60.64s; sanitizer12batch99852 PASS193.38s.
Separate primitives89834/42063 PASS3802cases1.48s/2.54s.
Other groups: consolidation247/742ops/23185SQL17.53s/36.46s;
wallet675/1907ops/50275SQL15.90s/82.87s; funding736/3418ops/39016SQL60.63s/193.37s;
command12289; runner1265; runnererrors1120; objects9464(29.31s/96.25s);
NumberValue original99assertions+11supplementalerrors5.27s/2.60s;
searcher189programs6468queries11.43s/26.89s; KISSVM4471(6.85s/11.24s); mapping337.
All24 comparison reports read. No production edits after final full builds.
No need to repeat passed checks without new changes/concerns.

Newbase/balance.hpp/cpp registered+CMake. Pinned383line source fullyread.
Reuse TxPoWSearcher, Wallet.isAddressSimple, Token/MiniNumber, JSONValue.raw,
JSONParser, RawTxnCommands.object strict helper. Sibling no balance command;
vendor SQLite balance routines unrelated. Full source help/validparams retained.
Get address/confirmations/debug/simple/coinlist/tokendetails/rawtoken in order;
no-tip CommandException before megammr parse. Noaddress uses relevant search,
ignores Mega; explicitaddress fullsearch optional Mega iff globally enabled.
Postsearch exactString filters; Minima seeded; first new token requiresnonnull
metadata immediately. Depth<confirmations unconfirmed elseconfirmed; counts/list;
sendable iffconfirmed+simple andcustomtoken scriptEXACT RETURN TRUE.
Debug SENDABLE checks onlysimpleaddress. EmitMinimafirst(unlessfiltered)then
encounterorder; scale usingactualToken; raw MiniString tokenname unconditional
sourceoverwrite canproduceinvalidJSON. Simple parses name actualJSONParser,
strictJSONObjectcast/getname. CoinlistnativeJSON values, no fullalias/lazytimingclaim.

Sharedwallet fixture: replyText serializes actualresult insideStringenvelope,
chain optionalmega/megaCoins fillsactualMegaMMR; Java balanceFixtures produces
wire for29sourceCoin/Token groups(native+names/scripts/scales) with6coins each,
amounts1..6,created8/9/11/12/0/10,addresses1/2/3. Missingmetadata/spent variants.
24directnumberRange ops useactualMiniNumber(BigDecimal coefficient/scale) or
String path. Driver --group balance imports wallet_balance_corpus.py. Preserve
raw replyText and coin debug logs aftergenericSQLnormalizer toavoid erasing
exceptionlike token names. Existing exception-prose normalization otherwise.
275cases:metadata,depthboundaries,views,details,names/scripts/scales,load/cache,
address/rawtoken,wrongtypes,missingmetadata,dedup/spent/relevant,Mega,public,
partialload,24constructorprobes. Independent expectedtotals/order/counts.

FIRST BASELINE program96:Java NumberFormatException plain60000000000000000000,
native scientific6.000000000000000000E+19. Preserved build/wallet-balance-range-baseline.json.
MiniNumber(BigInt,scale) checkedlimits BEFOREsourceBigDecimal.toPlainString.
Nownegative-scale constructor appends onlyzero digits survivingMAX_DIGITS64,
thenapplyMathContext/checkLimits. UsesexistingBigInt/pow10/context/JavaBigDecimalText;
Stringctorunchanged. Zero->scale0. Hugeexponentresourcebehavior stillunverified.
Initialboth96/251 fail6.58s/13.51s. Afterfix bothhit independenttestexpectation
bug: suppliedaddressblank rejected instead ofdefaultsearch. Correctedfixture
omitsaddresskey; keepsblankrejectiontest. No additional productionchanges.

LabBEFOREtests student-guide/new-tests/wallet-balance.md; author_wallet_balance.py,
balancelesson/MiniNumberlesson updated. RootREADME/REVIEW/provenance current.
Final guide audit8964links2634anchors457hashes0errors.
Scoped10sourcefiles whitespace/conflictsPASS;gitdiffcheckPASS(untrackedcpp limits).
Review scopedapprovewithsuggestions; no fullreleaseapproval. All457 backfill
lessonsalreadycompletedearlier; no newagentneeded/authorized.
Logs /tmp/minima-balance-{build-fixed,san-build-fixed,test-fixed2,regressions,
san-regressions,primitives,san-primitives}.log.
Reports perbuild: wallet-balance-command-results.json,wallet-consolidation-command-results.json,
wallet-commands-results.json,coin-funding-commands-results.json,command-differential-results.json,
command-runner-differential-results.json,command-runner-errors-results.json,
object-differential-results.json,number-value-original-results.json,
searcher-differential-results.json,kissvm-differential-results.json,differential-results.json.

NEXT PREREAD: search/coins.java fullyread thisturn; no body/lab/gatecompleteyet.
Metadatafullhelp; validparams include simplestate,totalamount,depth,state,megammr.
hardsetrel if NONE of relevant/coinid/address/state/tokenid exists; relevant=
existsParam(relevant) ORhardsetrel, IGNORINGparamvalue! amount alone still relevant.
Parse simplestate/sendable,rawcoinid MiniData,amount,addressgetAddressParam,
rawtokenMiniData,stateparam,coinage0,depthBILLION.getAsInt. Tipnull=>emptyarray
BEFOREmegammr/checkmempool/order/totalamount parsing. ElseMega iffglobal;
actualsearchCoins(tip,relevant,allfilters,statewildtrue,simple,depth,Mega).
Agefiltercreated<=tip.block-coinage. Thencheckmempoolfalse; iftrue retrieve
TxPoWDB THEN Main.miner EVENifnocoins; percoin minercheckfirst thenRAMmempool.
Elsekeep. ordergetParamdesc; onlyEXACTasc triggersstableascendingcreationblock;
otherordertext no sort. totalamount ifexists:parse, iftokenid
!=TOKENID_MINIMA andanycoin useFIRSTcoin.Token.scaledMinimaAmount (nulltokenfails),
thenactualsend.selectCoins. EmitCoin.toJSON(simplestate)+age tip.txpow.block-created.
CORRECTION from next source check: ZERO_TXPOWID is0x00, same asTOKENID_MINIMA;
the omitted token ID does not force token metadata conversion.
Reusabletxnutils::getMiner Main service alreadyinspected, sharedwalletfixture
minerop setsactualDormantMiner withqueuedTxPoWinputcoins; poolopactualRAMwitness.
Do not implement substitute miner/mempool/search. Needcomplete nextreusegate,
announceactualsourcepaths andwritepre-testlab before newtests.

---

# Latest VERIFIED checkpoint — base/consolidate; map336

Goal ACTIVE/incomplete. This goalturn PROGRESS; previous goalturn PROGRESS.
336counterparts=296production+40tests;121absent=73production+48tests.174CTest.
All457lessons authored; allcounterparts partial/paritynot_verified.
Main/Minima/durableSQL absent. No blockers/approvals/agents/commits/pushes/livewallet.
Do NOT mark goal complete. NO LIVE TOOL/BUILD/TEST SESSIONS.
Bothfinalfullbuilds90631/77313PASSwithoutwarnings.
Final8checks61919/48867PASSnormal58.53s/san191.87s. All14reportsread:
consolidation247programs742ops23185SQL17.06s/34.49s;
wallet675/1907ops50275SQL15.24s/72.88s;funding736/3418ops39016SQL58.52s/191.86s;
wrappers659/2060calls14.14s/64.38s;command12289;runner1265;runnererrors1120;mapping336.
Notall174CTest. No productionchangesafterfinalbuilds.

Newbase/consolidate.hpp/cpp registered+CMake; reusesactualsend.orderCoins,
TxPoWSearcher.getRelevantUnspentCoins,Wallet.getDefaultAddress,CommandResult.
Source236linesfullyread; no siblingconsolidate. Rawtoken,parseorder,agefilter,
DESCENDINGamountthenaddressgrouping(actualsourcehelpdiffers),min3beforelimits,
tokenamountbeforemaxsigs,post-addmaxcoins,defaultaddresslookup,exactforwardedsend
coinage/split2/dryrun/debug/burn/amount/address/tokenid/password. Passwordredaction
BEFOREgetParam. Childstatus->responseorstatusfalse+message/error/wholechild.
Javawrappingintincrement. No childvalidationenhancements.

Sharedwalletfixture scopedRecordingSend prototype restoredafterop;actualrunner
text/params recorded;controlledreplies/throws;3publicdispatchcases;4directstatusprobes.
NewJava consolidationFixtures usesexistingproofFixtures plusgrouped/tied/token/
missingToken/young/many25coinvariants. Real-sendcasesuseactualprooffixture.
wallet_commands_differential.py --group consolidation importsnewcorpus;actual
SQL/log/response/bytescompared. ScopedrealconsolidatedryrunPRNGnormalization checks
success+real+dryrun+32byteshape; nootherfields/controlreplynormalized.

BASELINE program167missingstatus:JavaNPE/nativeRuntimeExceptionbad_variant_access.
Savedbuild/wallet-consolidation-status-baseline.json. CommandRunner nowreuses
CommandResult.status forpost-executioncheckOUTSIDEoriginalcatchregion. Initial
244afterfixpassed;final247addspublicsuccess/permission/nestedmissingstatus.
Nootherproductiondiscrepancy. FirstcompilemissingMinimaLoggerinclude fixed.

LabBEFOREtests student-guide/new-tests/wallet-consolidation.md.
Newauthor_wallet_consolidation.py+consolidatelesson+CommandRunnerlessoncurrent.
README/REVIEW/provenance/CONTINUATIONupdated. Guideaudit8959links2634anchors457hashes0errors.
No newagentneeded: earlierstudentbackfillcompletedall457pages.
Logs /tmp/minima-wallet-consolidate-{build-reviewed,san-build-reviewed,regressions,san-regressions}.log.
Reports wallet-consolidation-command-results.json,wallet-commands-results.json,
coin-funding-commands-results.json,wallet-wrappers-differential-results.json,
command-differential-results.json,command-runner-differential-results.json,
command-runner-errors-results.json.

NEXT PREREAD: base/balance.java383linesfullyread. No implementation/lab/gateyet.
Address optionalnormalized;confirmationsdefaultdepth;debuggetterbutNOTvalidparams;
simple/coinlist/tokendetails;rawonlytoken;tipnullrejectBEFOREmegammrparse.
Defaultaddressempty usesgetAllRelevantUnspentCoins andignoresmegammr;explicitaddress
searchCoins withflagANDIS_MEGAMMR. ExactStringfilteraftersearch. SeedMinimaentry,
firstnewtoken Hashtable.put requiresnonnullcoin.Token. Classifydepth<confirmations;
count/list;sendableonlyconfirmed+simple+tokscriptEXACTRETURNTRUE(nonMinima).
Emitencounterorder,Minimaalwaysunlessonlytokenexcludes;tokenamountsscaled;name
unconditionallyMiniStringobject (needcheckJSONValuefallbackserialization). Simple
viewnonMinima JSONParser.parseMiniString.toStringthenstrictJSONObjectgetname.
Full metadata/remainingdetails in source. Nextreusegate pending.

---

# Latest VERIFIED checkpoint — consolidatefrom; map335

Goal ACTIVE/incomplete. This goalturn PROGRESS; previous goalturn PROGRESS.
335counterparts=295production+40tests;122absent=74production+48tests.173CTest.
All457lessons authored; allcounterparts partial/paritynot_verified.
Main/Minima/durableSQL absent. No blockers/approvals/agents/commits/pushes/livewallet.
Do NOT mark goal complete. NO LIVE TOOL/BUILD/TEST SESSIONS.

Bothfinalfullbuilds96651/25846PASSwithoutwarnings.
Final8checks45932/43123PASSnormal22.25s/san65.92s. All14reportsinspected:
consolidate331programs3815calls12.81s/22.38s;
custom492/2052calls15.75s/29.65s;wallet659/2060calls19.55s/65.92s;
raw656/3991calls9.38s/3.54s;command12289;runner1265;runnererrors1120;mapping335.
Notall173CTest. No productionchangesafterfinalbuilds. Don'trepeatpassingchecks
withoutnewchanges/concerns. Initial327/3603PASS6.57s/13.21s before4reviewcases.
Initialbuild39958/88846failedmissingGeneralParamsincludeinfixture;fixedinclude,
then98419/6455PASS;finalreviewmadecreateConsolidatevirtual+added4boundarycases.

Newconsolidatefrom.cpp/hpp registered+CMake. ReuseactualTxPoWSearcher(searchCoins
sameas txnaddamount),TxPoWDB/RamDB membership,sendstablesortcomparison,
CommandResult.first,actualCoin/tokenarithmetic. Noexistingconsolidatefrom insibling
src/tests. Source251linesFULLYREAD, no otherproductioncreateConsolidatecaller.
Mainrun: parsefrom/token/maxcoins50.getAsInt/burn (positiveburnnonnative rejects),
script/privatekey/keyuses;randomID;txncreate BEFOREmineparse;DBtip/currentblock;
actualsearchCoins(tip,false,false,ZERO,false,ZERO,true,from,true,token,
false,"",true,false,INTMAX,IS_MEGAMMR). StablesortMinimaamountdescending;filterage>=3
THENcheckmempoolonlyifoldenough. Zero valid throwsretainingrow. coincount1, forcoin
addtokenamount,++count(explicitJavawrap),txninput,ifcount>maxcoinsbreak;max<=0 addsone.
Burnifnative+positive muststrictlylesstotal elsedelete+throw. TENequaloutputs(total/10),
script/mmr/customsign/postmine/delete,returnpost.response withNOstatuscheck.
PublicvirtualcreateConsolidate nullablevector<shared_ptr<Coin>>*,borrowednumber
pointers,optionalStrings: providedinputorder/no search/filter/limit; positiveburn
regardless token;ONEoutput;postmine:true;delete;STRICTnullableJSONObjectresponsecast.
Nullcoins/list,burn,keyusesfailatoriginaloperation; nullableStringconcatsliteralnull.
Helpmentionsaddress/amountalthoughnotvalidparams; preservedmetadata.

Sharedwrappersfixture now9classes. fixtureChain resetsUSE_SQL_COINDBfalse/MEGAflag,
usesactualsource-supportedtestTxPoWTreeNode(height20default),coins andMegaMMR;
actualRAMpoolRamData wrappingrealTxPoW withinput+witnessCoinProof testsmembership
andcascade/onchainflags. NoSQLengine orMain loaded. Optionalexecute list runsactual
selectedtxninput/txnoutput/txnscript;existingmaterialize runsactualcreate/delete.
Publichelperfixture invokesactualmethod withnullableinputs; nevercopiesalgorithm.
--group consolidate importsconsolidate_address_corpus.py; prefixconsolidate-address-commands.
Independenttests ageboundaries,stableties,tokenamounts,spent/duplicateIDs,mega,
mempoolflags,limits(wrappingtoo),allchildfailurepositions,nullablehelper,cleanup,
ignoredpoststatus,wrongresponsecastAFTERdelete. Reviewadds55coins(default50,
explicit51,helperall55) and1E-44/10 =>tenzeros. ExistingnormalizationUNCHANGED.
No source/nativebodydiscrepancy found inthisbatch.

LabBEFOREtests student-guide/new-tests/consolidate-custom-address.md.
Newauthor_consolidate_address.py andfilelessoncurrent. README/REVIEW/provenance,
source-map,courseREADME/INDEX/audit updated. Guideaudit8956links2634anchors457hashes
0errors. Scoped9sourcefileswhitespace/conflictPASS;gitdiffcheckPASS(limiteduntrackedcpp).
Reviewapprovewithsuggestions scopedbatch, notfullrelease. Userinstructionteacher
backfillalreadycompletedall457pagesearlier; no newagentneeded/authorized.
Logs /tmp/minima-consolidate-{build-reviewed,san-build-reviewed,regressions,san-regressions}.log.
Reports consolidate-address-commands-differential-results.json,custom-address-send-
commands-differential-results.json,wallet-wrappers-differential-results.json,
raw-transaction-commands-differential-results.json,command-differential-results.json,
command-runner-differential-results.json,command-runner-errors-results.json.

NEXT PREREAD: pinnedbase/consolidate.java236lines FULLYREAD thisturn;unported.
ReusegateNOTcompleted/announced; no implementation/lab/test started.
Metadatahelpstatesminimum3,smallestfirstbutactualorderuses send.orderCoins DESCENDING
andaddressgrouping. SourcegetParam tokenid(noaddressnormalization),burn0,debugfalse,
dryrunfalseforcesdebug;tiplookup;coinagedefaultGlobalParams.MINIMA_CONFIRM_DEPTH;
rejectcoinagebelowdepthBEFOREsearch. getRelevantUnspentCoins(tip,tokenid,true),then
minblocktip.block-coinage;keepcreated<=min;send.orderCoins; <3 throwsbeforelimitparse.
MAX_SIGS5/MAX_COINS20 getAsInt(noenforcedmin/max). Loopcomputestokenamountbefore
checkingnewaddresssignaturelimit;newaddresscount+1>maxsigslogsifdebugandbreak;
addtotal/inccoin/log; iftotalcoins>=MAX_COINS log/break(afteratleastonecoin).
FetchWallet.getDefaultAddress THENconstructsendcommand:
send coinage:N split:2 dryrun:B debug:B burn:N amount:N address:HEX tokenid:RAW.
Ifpasswordexists debuglogscommand+password:#### BEFOREgetParam(password) andthen
appendquotedliteralpassword; otherwisedebuglogsplaincommand. DirectCommandRunner
runMultiCommand.get0JSONObject. Booleanstatus true=>ret.response;false=>ret.statusfalse,
message=result.messageifnonnull elseerrorifnonnull elseresultwholeobject.
Fullhelp+validparams+freshfunction. Needreuseactualsend.orderCoins,walletgetDefaultAddress,
existingwallet_commandsfixtureSQL andCommandRunner; no publicchildvirtualboundary
inthissource, so useexistingrecordingrunnerprototype orrealfundingsend fixture.

---

# Latest VERIFIED checkpoint — custom-address wrappers; map334

Goal ACTIVE/incomplete. This goal turn PROGRESS; previous goal turn PROGRESS.
334 counterparts =294production+40tests;123absent=75production+48tests.172CTest.
All457lessons authored. All counterparts partial/paritynot_verified.
Main/Minima/durableSQL absent. No blockers/approvals/agents/commits/pushes/livewallet.
Do NOT mark goal complete. NO LIVE TOOL/BUILD/TEST SESSIONS.

Both final full builds98803/44490PASS without warnings. Final10checks67119/33399
PASS normal40.23s/san124.63s. All18comparisonreports inspected after completion:
custom492programs2052calls13.69s/25.24s;
wallet659/2060calls16.88s/63.08s;raw656/3991calls8.30s/3.33s;
command12289,runner1265,runnererrors1120,objects9464(27.06s/124.61s),
primitives3802,originalNumberValue99assertions+11exacterrors(6.57s/3.32s),mapping334.
Notall172CTest. No production edits after final builds. No need to repeat passed
checks absent new changes/concerns. Both initialfullbuilds67099/44804PASS after
fixing new MiniData constructor call to existing fromHex API.

Newsendfrom.cpp/hpp andcreatetokenfrom.cpp/hpp registered+CMake.
Reusecreatefrom flow,CommandResult status/reject/first,RawTxnCommands.state,
JavaInteger,Coin/Token/Transaction/TxnDB. SourceMiniNumber.MINI_UNIT added native.
Source literals fromaddress: SPACE preserved. sendfrom swallows wrong stateobject
exception beforecreation; stateentry errors happenafterfunding/script. Typedstatus
checks funding beforecontinue and postingAFTERdelete. Nullerrors/NUL preserved.
createtoken raw decimals String cast/Integer.parseInt (no trim), default8,max16,
no minimum. floor supply1..trillion. MINI_UNIT *TEN.pow(decimals)*floor supply
using existingprecision; negative decimals canyieldzero. Token coin0xFF replaces
actualstoredoutputzero after Tokenconstruction andDBlookup; remainingoutputskeep.
Emptyoutputs typedIndexOutOfBounds. Script/MMR/sign returnedstatusesignored.

Sharedfixtures WalletWrappersOracle.java/wallet_wrappers.cpp now8subclasses;
optional materialize runsactualtxncreate/delete and explicit fundedOutputs through
actualtxnoutput. Row snapshots addactualTransactionJSON onlyinmaterializemode.
Controlled replies/throws happenbeforematerialization. --group custom imports
custom_address_send_corpus.py; uniqueoracle/reportprefix custom-address-send-commands.
Independentsequence/cleanup/tokenmetadata/supply/change/emptyrow assertions.
Realchildren test preservedemptyrow andofflinechainfailures; realsendcasecontrols
onlysigningreply. No fundedtransfer/networkacceptanceasserted.

Initial492programs489PASS bothbuilds. TWO failures exposedexistingMiniNumber.pow
wronggenericout_of_range; Java ArithmeticException: Invalid operation. Reused
JavaArithmeticException forguard outside+/-999999999. Fourdirectprobes (+/-1e9,
INTMIN/MAX) appended toNumberValueOriginalTests.java andmappedNumberValueTests.cpp,
99originalassertionsunchanged; supplemental count7->11insharedoriginaldriver.
Third discrepancy was NPEruntimeprose nestedinsideCommandException fromrealoffline
post; normalizer onlyexceptionfield matchingexactwrapper+NPE/CCEclass prefix keeps
classes and dropsprose. Othererrors remain compared, includingnull/NUL.
Preservedbaseline build/custom-send-error-baseline.json.

LabwrittenBEFOREnewtests student-guide/new-tests/custom-address-send-commands.md.
Newauthor_custom_address_send.py updates2lessons;MiniNumberandNumberValueTestslessons
also updated. RemovedstaleclaimMINI_UNITabsentinMiniNumberpage+author_base_objects.
README,REVIEW,provenance,source-map,courseREADME/INDEX/audit current.
Final guide audit8950links2634anchors457hashes0errors. Scoped15sourcefiles
whitespace/conflictPASS;gitdiffcheckPASS(limitedbyuntrackedcpp). REVIEW approvewithsuggestions scopedbatch, notrelease.
Logs /tmp/minima-custom-send-{build-final,san-build-final,regressions,san-regressions}.log.
Report filenames: custom-address-send-commands-differential-results.json,
wallet-wrappers-differential-results.json,raw-transaction-commands-differential-results.json,
command-differential-results.json,command-runner-differential-results.json,
command-runner-errors-results.json,object-differential-results.json,
differential-results.json,number-value-original-results.json.

NEXT PREREAD: pinned consolidatefrom.java251lines FULLYREAD thisturn.
Mainrun: parsefrom/token/maxcoins50.getAsInt/burn (positiveburnnonnative rejects),
script/privatekey/keyuses;randomID;txncreate BEFOREmineparse;DBtip/currentblock;
actualTxPoWSearcher.searchCoins(tip,false,false,ZERO,false,ZERO,true,from,true,token,
false,"",true,false,INTMAX,IS_MEGAMMR). Stablesortamountdescending;filterage>=3 THEN
checkmempoolonlyifoldenough. Ifzero valid throw retainingrow. coincount1, forcoin
addtokenamount,++count,txninput,ifcount>maxcoinsbreak (evenmax<=0 addsone).
Burnifnative+positive muststrictlylesstotal elsedelete+throw. TENequaloutputs
(total/10),script/mmr/customsign/postmine/delete,returnpost.response withNOstatuscheck.
Public createConsolidate(allcoins,burn,from,token,script,privatekey,keyuses): randomID,
create,allinputsinprovidedorder,positiveburnregardless tokenstrictlesstotal,
ONEoutput,script/mmr/sign/postmine:true/delete,returnSTRICTJSONObjectcast(response)
nullable; no statuschecks. Helpmentionsaddress/amount althoughnotvalidparams.
No nativebody/lab/tests yet; nextreusegateNOTcompleted/announced.
Search no sibling consolidatefrom match. Native reusable leads:
send.cpp93stable_sortdescending,txnaddamount.cpp55-56 exactsearch call,
TxPoWDB.cpp72 checkMempoolCoins delegatesRAM;TxPoWSearcher.cpp18fullsearch;
support/wallet_commands.cpp118chainfixture,146singleMMRnodefixture. Needreadcomplete
relevant candidates/dependencies/tests andchoosefixture beforeimplementation.

---

# Latest VERIFIED checkpoint — raw wrappers + Unicode state; map332

Goal ACTIVE/incomplete. Thisgoalturn PROGRESS; previousgoalturn PROGRESS.
332counterparts292production40tests;125absent77production48tests.171CTest.
All457lessonsauthored; rawfrom/rawtxnfrom/StateVariable/sendlessonsupdated.
Allcounterparts partial/paritynot_verified. Main/Minima/durableSQL absent.
No blockers/approvals/agents/commits/pushes/livewallet/node/deployment.
Do NOT mark goal complete.

NO LIVE TOOL/BUILD/TEST SESSIONS. OlderLIVE/PENDING belowSUPERSEDED.
Finalfullbuilds30033/82747PASSwithoutwarnings. Final2checks25318/33118PASS:
raw656programs3991childcalls4.06s/4.55s; object9464cases25.10s/91.34s.
Batches25.10snormal/91.36ssan. Bothreportsread forbothgroups.
Initial7checks39098/84645PASS88.75snormal/271.44ssan:
source_mapping332,command12289,runner1265,runnererrors1120,
walletwrappers659/2060childcalls19.57s/93.17s,
funding736/3418ops/39016SQL88.74s/271.43s,
raw632/3835calls5.76s/7.59s.
All14distinctcomparisonreportsread. Eightdistinctchecks acrossphases,notall171.
Initial7regressions NOTallrerunafterfocusedStateVariableUnicodefix; finalraw/object
validatefix. No productionchangesafterfinalbuild. Do notretestwithoutnewconcerns.

Newmappedrawfrom.cpp/.hpp,rawtxnfrom.cpp/.hpp incommands/send/wallet;registered.
NewRawTxnCommands.hpp sharesidenticaloutput/state loops+strictstring/objectcasts.
StateText.hpp extractsstateTextUNCHANGEDfromsend.cpp120-124;sendnowuseshelper.
CommandResult.booleanValue extractedUNCHANGEDfromstatus;statusdelegates;raw
storestate usesbooleanValue. Raw2wrappers sharefirstchildresultselection.
SourceJSONObject extendsLinkedHashMap (fullyread), iterationinsertionorderpreserved.
Rawfrom alternatesinput/script;rawtxnfrom allinputs/alloutputs/allscripts.
Nullarrayallowedgetter,loopfailsAFTERtxncreate;wrongtypefailsBEFOREcreate.
NullableStringfieldconcatsliteralnull;wrongtypesCCE;BooleanstorestateNULLNPE/
StringtrueCCE. Stateportparse BEFOREvalueconversion; StateVariablenormalizesdata.
Nochildstatuschecks/proofbuilder/signing/cleanupfinallyadded. Exportshowtxntrue,
deleteandreturnexport.response. Stalehelprawtxnfromrawfromexamplepreserved.

SharedfixtureWalletWrappersOracle.java/wallet_wrappers.cpp extended2Recording
subclasses andhelperdispatch. Pythonexistingdriver --group raw usesnew
raw_transaction_corpus.py; uniqueoracle/reportprefix raw-transaction-commands.
Originalwrappers group659unchangedPASS. Realexports fullyroundtrip+consumption;
independentassertsstorestate,normalizedstate,script/inputorder,retainedrows,
ignoredinputfailures. ExistingUIDnormalization verifiedleadingIDbytes/textonly.
No sourcealgorithmimitatedinfixtures, no actualfundednetworkacceptanceclaimed.

REVIEWFOUNDANDFIXED EXISTINGStateVariableboolbug:
raw-state-unicode-baseline.json (build) source accepts falſe asFALSE,nativeNumberFormat.
ReadcompleteJavaStringIgnoreCase.hpp anditsvault/InetSocketAddressusage; reusehelper
inStateVariable.cpp fortrue/falsecasecomparison, replacingASCIIboolbranches.
24newrawcases (direct+real) compareASCII/mixedcase/longS normalizedchildtext and
exportedtype8Booleanstate; raw656+object9464PASSbothbuilds. Prefixhex/Mxlogicunchanged.
Locale/fullnullable/reference/staticconstantmutationAPIsremainunverified.

Buildtiming: initialsan54210 stillfinishing when CTest started. Verifiedcurrent
wallet_wrappers/wallet_commands binarymtimes PRECEDED theirfreshJavaoracleclass
compilationtimestamps; actualnativeprocesseslaunchedlaterbydrivers loadednewbinaries.
54210terminalPASS. No restartedprocesses. FinaltestsstartedafterbothfinalbuildsPASS.
Initialfundingranolderpre-Unicodebuild; itvalidatesUNCHANGEDstateText extraction.
Logs/tmp/minima-raw-{build-final,san-build-final,tests,san-tests,state-final,
san-state-final}.log. README/provenance/REVIEW/courseindexupdated; pre-testlab
raw-transaction-commands.md +author_raw_transactions.py and4affectedlessonscurrent.
Reviewapprovewithsuggestionsscopedbatch, fullreleaseunapproved.
Guideaudit8939links2634anchors457hashes0errors. Source_map332checkPASS.
Scoped13sourcewhitespace/conflictPASS;gitdiffchecklimitedbyuntrackedcpp.
No pendingbatchwork.

NEXTPREREADS: sendfrom.java145 andcreatetokenfrom.java179 FULLYREAD (pinned).
No implementation/lab/tests yet. Need siblingsearch+relevantdeps/testsreview and
reuseannouncement beforecode. CurrentRawTxnCommands.state +CommandResult helpers
andwrapperfixture areclose candidates. StateText alreadysharedforObject+string.
sendfrom: parsefrom/to/amount/token/burn rejectpositivetokenburnbefore script,
privatekey,keyuses; generatesrandomID BEFOREmineBoolean(defaulttrue)/splitNumber1/
optionalstate. getJSONObjectParam(state) exceptionsswallowed=>null. txncreate;
txnaddamount literal ' fromaddress: '+addr plus split,burn; false=>delete+throw.
script;stateexactrawloop;mmr;customsign;txnpost mine;delete; THENcheckpoststatusfalse
throwchildnullableerror. Otherstatusesignored. Publicvirtualhelperfirstreply.
createtokenfrom: parsefrom,amount,script,key,uses,mine(defaulttrue); name object
mustcontainname(anyvalue) elserequiredstringwrappedinJSONObject. decimalsdefault8,
rawStringcast+Integer.parseInt WITHOUTgetParamtrim; >16 throwsgenericException
MAX16decimalplaces; no lowerbound. flooramount; >trillionor<=0 CommandException.
10.pow(decimals),MINI_UNIT*totaldecs*totaltokens;scale=MAX_DECIMAL_PLACES-decimals
(Javaintoverflowifreachable). randomID;txncreate;txnaddamountliteralspacefromaddress
fundsfrom/to sameaddr/minimaamount; false deletes+throws. CreateCoinOUTPUT,fromaddr,
minimaamount,TOKENID_CREATE,true; Token(CoinIDOUTPUT,scale,minimaamount,jsonname,
RETURNTRUE); attachToken. FetchMainDB customrowAFTERcoin/tokencreation, replace
outputs.set(0,tokenoutput), then script/mmr/customsign/post/delete/statuscheck.
NeedsactualDBrow+outputeveninrecordingfixture; mustreuseactualtxncreate/txnoutput
orfixturematerialization ratherthanbypasswrapperbody. Sourcefromaddressspace can
leaveemptyrow =>replacefirstfails; preserve. No APIparityclaimfromsmallfixture.

--- Older checkpoints (superseded where contradictory) ---

# Latest VERIFIED checkpoint — four wallet wrappers; map330

Goal ACTIVE/incomplete. Thisgoalturn PROGRESS, previousgoalturn PROGRESS.
330counterparts=290production+40tests;127absent=79production+48tests.170CTest.
All457studentlessonsauthored, affectedfour+CommandException+CommandRunnerupdated.
Allcounterparts partial/paritynot_verified. Native Main/Minima/durableSQL absent.
No blocker/approval/agents/commits/pushes/livewallet/node/deployment.
Do NOT mark goal complete.

NO LIVE TOOL/BUILD/TEST SESSIONS. All olderLIVE/PENDING belowSUPERSEDED.
Latestfullbuilds82893/39331PASSwithoutwarnings. Final5CTestbatches45576/36586
PASSnormal14.29s/san58.32s. Wrapper659programs2060childcallsPASS14.29s/58.31s.
Other4PASSperbuild: source_mapping330,command12289,runner1265,runnererrors1120.
All8finalcomparisonreportsread. Notall170checks. No laterproductionchanges.
Do not rerun passing checks withoutchanges/concreteconcerns.
Logs/tmp/minima-wrappers-full-build.log,-full-san-build.log,-checks-final.log,
-san-checks-final.log. Scoped14sourcewhitespace/conflictPASS. gitdiffcheckPASS
limitedbyuntrackedcpp. Finalguideaudit8926links2634anchors457hashes0errors.
README/provenance/REVIEW/source-map/courseindexupdated. No pending currentbatchwork.

New four src/org/minima/system/commands/send/wallet cpp/hpp:
createfrom,constructfrom,postfrom,signfrom. RegisteredCMake+CommandRunner.
Newsrc/support/CommandResult.hpp sharesfirstresult selection, statusBoolean
cast/unbox,nullable childrejection. PublicwrapperrunCommand(string) staysvirtual.
CommandException.hpp/.cpp now stores complete optionalstring separatelyfromwhat(),
nullptrconstructorandgetMessageconstreference. RunnerusesgetMessage=>error:null
andembeddedNULpreservedwithoutgenericlogging. BroadThrowableAPIstillpartial.
postfrom/signfrom ID argument evaluatedbeforecheckingnullrowreceiver.

New support/WalletWrappersOracle.java,wallet_wrappers.cpp;
tools/wallet_wrappers_differential.py +wallet_wrappers_differentialCTest.
FixtureactualMinimaDB+TxnDBwithoutSQLload, source reflectedDB/nativeexistingfriend
initpattern. Wrapper subclassesrecordchildcalls+rowwirebeforeeach, injectresult/
throwAt or callactualCommandRunner. Real64^3TreeKeycustomsign and actualdecoded
signatureverifyPASS. Exports roundtripfullTxnRowbytes+consumedlength. Public
recordRunner temporarilychangesprototype, finally/RAIIrestores. Logscomparedexcept
NPE/CCEprose, stackframes/loggerprefix. Sourcealgorithmsnotimitatedinfixtures.
UIDshapechecked32byteuppercasefromtxncreate; normalizeknownIDtext andonlyverified
wireleading4-byte66length+exactUIDbytes; remainingwireunchanged. FixedimportedID
untouched. No validfundednetworktransactionclaimedforofflineexports.

Important pinnedbehavior:
createfromliteral ' fromaddress: '+addr producesparserreplywhose status bypasses
wrapperfundingfailure; realfixtureexportsemptytxn+scriptanddeletes. MMRfailurealso
ignored. Sourcebehaviorpreserved, notfixed/silentlyescaped.
Commandrejectsemptycoinlist; ',,,'passesandtokenizeszero. Boolean 'bad' stringfalse,
noterror; JSONtruewrongStringtypecausesCCE. Late mine/key/use/posttypefailretains
importedrow, mmrtypefailbeforeinsert. signfromexportresponsecastafterdeletion;
posttxpowcopy smallcatch logsandswallows. Childstatusesmostlyignored asJava.

Reviewfixed/provenbaselines:
- Initial566had13typeerrors+2badtestexpectations: emptychildresultvector=>Java
IndexOutOfBounds; statusmissing=>NPE/wrongtype=>CCE. Bademptycoinlist/booleantest
expectations correctedagainstsource. Firstfixtureexportobservermisreadhash.data;
gatedrealwrapperoutputs, then639PASSbothbuilds.
- build/wallet-wrapper-id-order-baseline.json shows decodednull+idinteger sourceCCE
vsnativeNPE. ParseIDlocalfirst fixes;14crosscases=>653PASSbothbuilds.
- build/wallet-wrapper-null-character-baseline.json shows source before\0after vs
nativebefore. Storecompleteoptionalmessage separatelyfromwhat;6cases=>final659PASS.
All initialfailed/build handles terminal. Missingstringheader/TreeKeyfixtureinclude
fixed; finalbuildsclean. Initial5batchesfailedonlyfixtureobserver, all4regPASSbut
final5batchesrerunafterlatestCommandExceptionchangeandALLPASS.

Teaching: pre-testlabwallet-command-orchestration.md; newauthor_wallet_wrappers.py
updates4lessons andfinalvalidation. CommandException/CommandRunner lessons nullable/
NULbehavior. Reviewapprovewithsuggestionsscopedbatch; releaseNOTapproved. Native
nullableString/JSONObjectreturn/referenceAPIandfullruntimeintegrationunverified.

NEXT PREREADS (no code/lab/testwrittenyet):
Completepinnedrawfrom.java149 andrawtxnfrom.java157 FULLYREAD. Siblingsrc/tests
searchnomatch. CurrentCommandResult+wrapperharnessreusable. stateText helper in
send.cpp120-124 FULLYREAD: string=>asString,double=>javaFloating,elseJSONtext.
Candidateextractunchangedforrawstateconcatenation. StateVariable.cpp outputwas
truncatednearend; doNOTclaimcompletecurrentread. Needreadremainingdeps/callers/
tests andannounceexactreuseBEFOREnewimplementation.
rawfrom: getinputsJSONArray,outputsJSONArray,optionalstateJSONObject; thenrandomID,
txncreate; foreachinputcastJSONObject,getStringcoinid THENscript, txninputthen
immediatetxnscript; outputsgetStringaddr,amount,optionaltokeniddefault0x00,
storestateBoolean defaulttrue; childtxnoutput; statekeyInteger.parseInt,
valueObject+emptyString, StateVariable normalizedport/data thenquotedtxnstate;
exportshowtxntrue,delete,returnresponse. No childstatuschecks/MMR/signing.
rawtxnfrom: requiredinputs,outputs,scripts arrays BEFOREoptionalstate andtxncreate;
allinputStringcoinids first, alloutputs second, allscriptStrings third, thenstate,
export/delete. Sameoutput/statebodiesasrawfrom; getFullHelp stillrawfromexample.
NullStringfieldsconcat 'null'; wrongtypesCCE; nullarrayfailswheniteratedAFTERcreate.
State order followsactualJSONObjectkeySet (needreadmapimplementation: do notassume).

--- Older checkpoints (superseded where contradictory) ---

# IN-PROGRESS checkpoint — four wallet wrappers; map330

Goal ACTIVE/incomplete; currentturn PROGRESS previousturn PROGRESS. No blockers.
330counterparts290production40tests;127absent79production48tests.170CTest.
LIVE CTest wrappers-only normal74697 andsan26547, logs
/tmp/minima-wrappers-tests-final.log and -san-tests-final.log. Poll SAME handles.
No other live sessions. Initial5batches16725/68250terminalexit8 ONLYfixturedecoder
bug (hash.data misread as TxnRow); other4regressionsPASS each. Fixture nowlimits
decode to realwrapper cases, targetedbuild41613/57929PASS, rerunsLIVEabove.
Priorfullbuilds94360/26302PASS; latestfixtureonlychange compiledtargetsPASS.
Need finaltestreportread, finishdocsreview/guides/provenance thennextbatch.

Added fourwalletwrapperscreatefrom,constructfrom,postfrom,signfrom cpp/hpp,
CommandResult.hpp sharedfirst-index/statusunboxing/nullableerror helpers;
CommandException.hpp explicitnullptrmessage+optionalgetMessage;
CommandRunnercatch preserveserror:null, andregisteredfourprototypes+CMake.
Added WalletWrappersOracle.java/wallet_wrappers.cpp, differential.py+CTest.
Allsources/reusegate announced; sibling nofourwrapper candidates. Lab written
BEFOREtests wallet-command-orchestration.md. ExistingTxnDB/row/runner/commands
reused;4 publicrunCommand(string) methods remainvirtual. Sourceflow belowprior
checkpoint+newcode exact. TypedemptyfirstIndexOutOfBounds andstatusNPE/CCE fixed
from initial566comparison(551pass15fail:13typeerrors+2badtestexpectations).
Thenexpanded639programs withnullerror/wrongerror tests and publicrecordRunner
nullableerror test, independent exportedTxnRowroundtrip/consumed/signatureverify.
Bothfullbuildinitialmissingstringheaderfixed; latermissingTreeKeyfixtureinclude
fixed. Currentproductioncompletecompiles. No commits/push/livewallet/Mainnode.

Fixture creates actualMinimaDB+TxnDB WITHOUTloadSQL. Java reflects mTxnDB;
native reusesfriendWalletCommandsFixture initpattern, connects throwifused.
Subclassfourwrapperpublicchildoverloads recordcalls+rowwirebeforecall, inject
result or throwAt, or realCommandRunner. recordRunner replacesprototype within
try/finally/RAII andgetFunctionreturnsnewRecording. Actualcustomsign TreeKey64^3
anddecodedverify included; no fakecryptography. ChildresponseJO copied/native
sharednestedresponse preserves tested signfrom postmutation.

Important SOURCE findings provenJava+native:
emptycoinlist rejectedbyCommand; ',,,'passes andtokenizeszero.
Nonempty 'bad' Boolean text =>false, notexception; testTrue(JSONbool)forCCE.
createfromliteral ' fromaddress: '+address yieldsparserreplywithstatus thatdoes
nottriggerfundingfailure. ActualofflinecreatefromexportsEMPTYtxn+script anddeletes,
despiteignoredproofbuilderNPE. Assertthissourcebehavior, NOTvalidfundedtransfer.
Incorrectexpectationsfixed. Latebadtype mine/privatekey/keyuses/postretainrow;
mmrbadtype rejectsBEFOREinsert. Defaultpost false; allpostfromchildstatusesignored.
Initial566rawobservationsbuild/wallet-wrappers-observations.json maystaleuntilLIVE
rerunfinishes. Reportscurrentlyold566untilnew639finishes; donotclaimpassyet.
UIDnormalization checks32byteupperhexgeneratedfromtxncreatetrace; verifiedwire
prefix4-byte66length+exactUIDbytes replacedonlyfield; allremainingbytescompared.
FixedimportedID untouched. NPE/CCEproseexcluded asotherharness; expliciterrors/logs
compared. Exportdecode gatedrealcasesafterhashfixturemistake. NullCommandException
specificfixturetoStringfixed and publicrunnererror:null/no logging asserted.

StudentdocsUPDATEDBUTPENDING: new author_wallet_wrappers.py writes4lessons with
CURRENT_VALIDATIONplaceholder; statuslinesmappedpartial updated andlinks. CommandException
lesson updatednullmessage butparagraphsaysvalidationpending. Newlabexpanded source
quirks butEvidencepending. CourseREADME/indexgenerator lablinksadded.
Needrunfinalguideaudit, updateCommandRunnerlessonnullableerror, provenance/README/
REVIEW, finalcheckpoint. Review actualallchangedproduction+harnesses. Needreports
new2+reg6=8read. Fullall170notrequired; sharedexceptionchangesalready4regPASSperbuild.
Maybe run transaction-integration regression ifconcernsharedexceptions onlyunchanged
non-null constructor, oldcommand12289/runner1265/errors1120 passed.

--- Previous verified checkpoint ---

# Latest VERIFIED checkpoint — sendpoll command; map326

Goal ACTIVE/incomplete. Thisgoalturn PROGRESS; previousgoalturn PROGRESS.
No blockers/approvalneeded. No new agents/commits/pushes/deployment/livewallet.
All457lessons authored; updated sendpoll,SendPollManager,SendPollMessage.
326counterparts=286production+40tests;131absent=83production+48tests.169CTest.
Allcounterparts partial/fullparitynot_verified. Native Main/Minima/durableSQL absent.
Do NOT mark goal complete.

NO LIVE TOOL/BUILD/TEST SESSIONS. All olderLIVE/PENDING belowSUPERSEDED.
BothfullbuildsPASSwithoutwarnings7353/98278. New comparison367programs3216ops
PASSnormal25606 6.75s andsan9574 8.54s. Bothreportsread.
Five regressionsPASSnormal15438 13.69s/san59043 25.31s:
source_mapping326,sendpoll_workers,command_differential12289,
command_runner_differential1265,command_runner_errors_differential1120.
All EIGHT comparisonreportsread including two new+sixregressionreports.
Six distinct selected CTest entriesPASS perbuild, notall169.
Do not rerun passed checks without changes/concreteconcerns.

Added send/sendpoll.cpp/.hpp, support/sendpoll_command.cpp,
support/SendPollCommandOracle.java, tools/sendpoll_command_differential.py,
student-guide/new-tests/send-polling-command.md. LabwrittenBEFOREnewtests.
CMake+CommandRunnerregistered; source map/source-map.json326current.
ReusesactualSendPollManager/SendPollMessageunchanged; wrapper getManager binding
(defaultJavaNPE missingMain),fetchbeforebranches,derefonlywhenused,sourcefirst
case-sensitive literalrewrite,onlyexactsendrejected,emptydirecttextaccepted,
listcopiedqueue,removeUIDevenabsent,unknownactionsuccesswithstillMainlookup.
Exactmetadata/defaultcomplete/freshparams tested.

Fixture JavaUnsafe bypassesmanager/Mainthreadstartingctors; initializesqueue,
lock,runningfields. Native dormant subclass uses existingprotectedctor.
Actualinheritedworker processes actualCommandRunner hash/failure commands with
real1secondpauses; timerpublicationrecorded,notdelivered. Stoppedworkerschedules.
Fullqueuestate,heldcopies,UIDshape/identity,return/errorscompared aftereveryop.
367programs include 100random25step sequences and Cartesianaction/text/Mainstates.
Normalizer checksuppercase16byteUIDandcurrentqueueuniqueness thenstablelabeling.
NPE/CCEproseexcluded; all88initialmismatcheswereexistingCommandgenericcastprose
versusJavatype/loaderdetails. Verifiedall88byCounter; noothermismatches.
Loggeroutputexcluded fromnewfixture; priorrunnererrorsregressionPASS.
Existingmanager nullmessage/nullstoredUIDinvalid_argument+malformedstatusnative
exceptions are known broaderpartialAPIgaps, unchanged. No fullruntimeequivalence.

Initialfullbuildattempt77371failedmissingJavaExceptionsinclude; fixedexplicitinclude,
rebuild75176PASS. Earlytest57789 sourcecompiled/ran butnativeexecutableabsent; terminal.
Initialcomparison41239exit1 only88CCEprose; correctednormalization+documentedgap.
FinalfullbuildandchecksabovePASS. No laterproductionchanges.
Logs /tmp/minima-sendpoll-{full-build,full-san-build,tests,san-tests,regressions,
san-regressions}.log. Finalguideaudit8910links2634anchors457hashes0errors.
Scope5newsourcewhitespace/conflictPASS; gitdiffcheckPASSlimitedbyuntrackedcpp.
README/provenance/REVIEW/courseREADME/indexgeneratorupdated.
Reviewapprovewithsuggestions scopedcommandonly; fullreleaseunapproved.

NEXT BATCH PREREADS (no implementation/lab/tests yet):
Complete pinned Java createfrom.java91,constructfrom.java106,postfrom.java78,
signfrom.java97 under system/commands/send/wallet FULLYREAD. Siblingsrc/tests
search no matching four wrappers. ExistingnativeTxnDB.cpp,TxnRow.hpp,
txnimport.cpp,txnexport.cpp,txncreate.cpp,txndelete.cpp,txnscript.cpp,
SendRecipients.hpp FULLYREAD. Nativechildrunner search no existingwrapperhelper;
CommandRunner.runSingleCommand has empty-arrayfallback, unlike sourcewrappers
runMultiCommand(...).get(0). Must retain ownpublicoverload/virtualtestoverride.
Needcomplete relevantchilddependency/testsreview+reuseannouncementBEFOREcode.

Sourcecreatefrom: parsefromaddress,address,amount,tokenid viaAddressParam default0x00;
burn>0andtoken!=exact0x00 rejectsBEFOREscript. Random32byteUID; childtxncreate;
childtxnaddamount includesliteral ' fromaddress: '+address (spaceaftercolon).
Ifchildstatusfalse ->txndelete thenCommandException child.error. Then txnscript
JSONstringliteral script unescaped,txnmmr,txnexport showtxn:true,txndelete.
Otherchildstatusesignored;response=export.response. No try/finallycleanup.
constructfrom: coinlist/script, toaddress/toamount, changeamount BEFOREchangeaddress,
rawtokenid(default0x00). RandomUID;txncreate; StringTokenizercomma ignoresempty,
trimJava whitespace each; eachtxninputfalse deletes+throws. Outputrecipient,
positivechangeonly,script,mmr,exportshowtxntrue,delete; otherstatusesignored.
postfrom: obtainTxnDB BEFOREdataparse; decodeTxnRow, optionalid notvalidparams;
parsemmr BEFOREtx.getID+insert; parsemine AFTERinsert; optionaltxnmmr;
txnpost minebool,delete;ignorechildstatus; returnpost.response.
signfrom: obtainTxnDB,decoderow,optionalid(valid),getID+insert BEFOREprivatekey
AddressParam/keyusesNumber/postBoolean; txnsign customkey, whenposttrueadd
' txnpostauto:true txnpostmine:true';export(no showtxn),delete; castresponseobject
putpost; ifposttruetrycastsignresponse+copytxpow; catchExceptionlogsandswallows.
responseextractionfailurehappensafterdelete. AllfourgetFunctionfresh, defaultfullhelp.
NeedactualnativeTxnRowreaderlogging/nullcategory existinggapsreviewifexposed.

--- Older checkpoints (superseded where contradictory) ---

# Latest VERIFIED checkpoint — unsigned creation; map325

Goal ACTIVE/incomplete. Currentturn PROGRESS, previousgoalturn PROGRESS. No blocker,
approvalneeded,agents/commits/pushes/deployment. All457teachinglessons authored.
325counterparts=285production+40tests;132absent=84production+48tests.168CTestentries.
Allcounterparts partial/fullparitynot_verified. Native Main/Minima/durableSQL absent.
Goal is NOT complete.

NO LIVE TOOL/BUILD/TEST SESSIONS. EarlierLIVE/PENDING belowSUPERSEDED.
Finalunsigned309programs1248operations20081recordingSQL perbuild PASS:
normal84697 28.74s, sanitizer79841 94.27s. Bothreportsread.
SevenDISTINCT selected entriesPASS perbuild: source_mapping,command_differential,
command_runner_differential,command_runner_errors_differential,
coin_funding_commands_differential,offline_send_commands_differential,
unsigned_send_command_differential. All12 comparisonreportsread. Notall168tests.
Do not rerun passing checks without newchanges/concreteconcerns.

Initial7normal25703exit8 in58.03s andsan57088exit8 in220.24s ONLYbadzero-total
assertion (sanmoduleloadedold307beforecorrection). Other6eachPASS. Correctedunsigned
followups abovecompleteverification. Funding736/3418ops/39016SQL PASS51.66snormal/
186.59ssan; offline201/558/9773SQL PASS16.49s/101.33s; command12289,runner1265,
runnererrors1120,source_mapping325allPASS. Initial304unsignedPASS28.42s before
expanded independentexpectations. Zero-total baseline source/nativebothreject:
build/unsigned-zero-amount-observations.json. Newzero-recipientpositivetotalcase
DOESwritefile; fractionalamount3/split1.5producesoneamount2output; asserted.

Fullbuild94257/80546PASSwithoutwarnings. Logs/tmp/minima-unsigned-full-build.log,
-full-san-build.log,-regressions.log,-san-regressions.log,-tests.log,-san-tests.log.
No productionchangesafterfullbuild. Newsendnosign573Java-linecounterpartregistered.
Source cachedconfirmationrewind,coinage0(omittedvalidparams),fractionalsplit1..20/
no remainderoutputs,strictStringstate,changewithoutkeycheck,unsignburnfalse,
dryrunignored,nosizegate,actualTxPoWfileoutput preserved. Reuse existingselector,
wallet,MMR,generator,MiniFile. nextRecipientToken+requiredToken movedUNCHANGEDfrom
send.cpp tosrc/support/SendRecipients.hpp andusedbybothcommands.

Fixtureusesrealheader/TxPoWreaderstomeasurePRNGoffset+lengthandfullconsumption.
Normalizeonly verified32-bytefilePRNGslice,decodedbodyPRNGandshape-checkedclock
filenames; compareotherbytes/JSON,SQL,logs,dirs,proofs,signaturesandminerqueues.
Actualcreate/view/sign/verify/post plain+customtokenburn flowsPASS. No realwallet/
networknode. Recordingrowsnotdurablestorage; nativeAddressAmountvalue/null/ref
APIs,concurrency,allfilesystem/platformcasesstillunverified.

README/provenance/REVIEW,newlabunsigned-transaction-files.md,sendnosignlesson,
sendlesson,courseREADME/indexgeneratorupdatedfinalPASS. LabwrittenBEFOREtests.
Scopedcode-reviewapprovewithsuggestions; fullnode/APIparitynotapproved.
Scoped9sourcewhitespace/conflictcheckPASS;gitdiffcheckPASSbutuntrackedcpplimit.
FinalguideauditPASS8902links2634lineanchors457hashes0errors. Source map325checkPASS.
Allcurrentbatchworkcomplete; no pendingchecks.

Nextsendpoll.java FULLYREAD. NativeSendPollManager.cpp/.hpp,SendPollMessage.cpp/.hpp
and completeJava manager/message sources, cpp/support/sendpoll.cpp FULLYREAD.
Sibling src/testssearchfoundno matching sendpoll/manager. Main.java constructor
mSendPoll=newSendPollManager andgetSendPollgetterread; nativeCommand completeparam
accessorsseen, needdefaultstate/fullcallerreview. Reuseannouncement stillneeded
BEFOREcode; no nextlab/test/implementationwritten.
Sourceflow: getactiondefaultadd; fetchMain.getSendPoll BEFOREactionbranches; add
replacesFIRSTliteralcase-sensitive sendpoll->send in completecommand,rejectsonly
exactsend;listcopiesmanagerlistandJSON;removeusesuid;unknownactionsreturnempty
responsebutstillrequireMainlookup(managercanbenullifunused). NeedexplicitMain
receiverbinding asothercommandsuntilnativeMainexists; reuseactualmanager.

--- Previous checkpoints (superseded where contradictory) ---

# Current checkpoint — unsigned309 normal PASS; corrected sanitizer running

Goal ACTIVE/incomplete, PROGRESS.325counterparts285prod40test;132absent.168CTest.
All457lessons authored; source map325current. No agents/commits/pushes.
LIVE exec79841 corrected sanitizer unsigned-only CTest, log
/tmp/minima-unsigned-san-tests.log. Poll it; do not restart. NOotherlivehandles.
Normal84697 final309programs1248ops20081SQL PASS28.74s, reportread.
First normal304programsPASS28.42s. Expanded307hadBADTESTexpectationzeroamountfile;
Java/nativebothcorrectlyreject Invalid Minima amount to send..0. Reproduced and
saved build/unsigned-zero-amount-observations.json. Corrected torejecttotal0,
addedzeroandnegativeindividualrecipients inpositivetotalmulti. Zeroindividualfile
andfractional3/1.5->oneamount2output assertionsPASSin309normal.

Normal7batch25703terminalexit8 onlywrongzeroassertion; sixotherchecksPASS:
source_mapping,command12289,runner1265,runnererrors1120,funding736/3418/39016SQL,
offline201/558/9773SQL. Funding51.66s,offline16.49s;batch58.03s. Correctednormal
unsignedfollowupPASS asabove. All6normalreportsread. San7batch57088terminalexit8
onlysameold307expectation (moduleloadedbeforeedit); other6checksPASS, funding
186.59s,offline101.33s,batch220.24s. All5sanregressionreportsread; waitingcorrected
unsigned report. No productionchanges afterfullbuild94257/80546PASSno warnings.
Scoped9sourcewhitespace/conflictcheckPASS;guideaudit8896links2634anchors0errors
beforelastdocwrites, rerunfinal. Allcurrentcode reviewedscoped; Reviewverdictpending
finalsancheck. README/provenance/REVIEW/unsignedlab/sendnosignlesson stillsaypending;
updateafterPASS. Need finalaudit,checkpoint. Previousworkingdetailsbelow.

Nextsendpoll.java FULLYREAD; furtherreusereadsongoing, no implementation/lab/tests.
FullnativeMain/Minima/durableSQLstillmissing; goalNOTcomplete.

--- Previous working and verified checkpoints ---

# Current WORKING checkpoint — sendnosign; map325

Goal ACTIVE/incomplete. Previousturn PROGRESS; currentturn PROGRESS. No blockers,
agents/commits/pushes.325counterparts285prod40test;132absent84prod48test.168CTest.
All457lessons authored. Newlabunsigned-transaction-files.md BEFOREtests.

LIVE: exec25703normal7regressions and57088san7regressions; logs
/tmp/minima-unsigned-regressions.log and-san-regressions.log. Do not restart.
Fullbuild94257/80546PASS; logs/tmp/minima-unsigned-full-build.log,-full-san-build.log.
Initial unsigned304programs1228ops19741SQL PASS28.42s (87951terminal).
Expanded307programs adds fractional/zero/stateassertions, strengthensdryrun,
shortchain,unsignedburn andsignedburn expectations; waitingfinalresults.
Build11751terminalmissingMinimaLoggerinclude; fixed48685PASS. Nootherlivehandles.

New sendnosign.cpp/.hpp translates573-lineJava. Metadataexact; sourceflowkept:
confirmedancestorwalk(cachedintdepth),coinage0butnotvalidparam,fractionalsplit1..20,
no remainderoutputs,strictStringstate,changewithoutkeycheck,unsignburnfalse,
dryrunignored,no sizegate,actualTxPoWfileoutput. Usesexistingselector/generator/
wallet/MMR/MiniFile. nextRecipientToken andrequiredToken EXTRACTEDUNCHANGED from
send.cpp into src/support/SendRecipients.hpp and reusedbybothcommands.

New unsigned_send_corpus.py +drivergroupunsigned/unsigned-send-commandprefix.
Java/native observers read actualheader/TxPoW for eachfile; fileTxPoW recordsJSON,
consumedbytes andactualPRNGoffset/length. Normalizer verifies32-bytefieldmatches
rawslice thennormalizesonlythat slice; otherbytescompared. NormalizesonlybodyJSON
PRNGandshape-checkedunsigned/signedclocknames. SubsequentminerexistingfixesPRNG12
beforemining,queue bytes remaincompared. No realwallet/networknode.

README,newsendnosignlesson,sendlesson,indexgenerator/courseREADME updatedpending
results. Source map325regenerated. Need readfinalreports, code-review, provenance,
lab/perfilefinalevidence, docs/counts, guideaudit/whitespace checks andcheckpoint.
AddressAmount nativevalue/null/referenceAPI notfullparity; Main/Minima/nativeSQL
absent. First304normalPASS doesnotprovecompletefile/node.

--- Previous verified checkpoints ---

# Latest VERIFIED checkpoint — offline transaction files; map324

Goal ACTIVE and incomplete. This turn made PROGRESS, no blocker or permission
needed. No agents/commits/pushes/deployments. All457 teaching lessons authored.
324counterparts=284production+40tests;133absent=85production+48tests.167CTestentries.
All counterparts remain partial and full parity not_verified; native Main/Minima
and durable SQL backend still absent. Do not mark goal complete.

NO LIVE BUILD/TEST/TOOL SESSIONS. Previous LIVE/PENDING below is SUPERSEDED.
Normal12selected30046 PASS133.68s. Sanitizer12selected24374 PASS515.77s.
Distinct selected checks: source_mapping, command_differential,
command_runner_differential, command_runner_errors_differential, utility,
prefs,txndb,transaction_commands,sql_db,minima_db,vault_signing_commands,
offline_send_commands (the last8 use _differential suffix).
All22 comparison reports read, including finalsanvault. Not the full167 inventory.
Do not rerun passing checks without changes or concrete concerns.

Offline201programs558operations9773recordingSQL calls perbuild, PASS23.32s normal
and97.59s sanitizer. Includes512demo lines for empty/nullargs,9TxPoWfixtures,
repeatedMAIN/BURNsigning,exactbytes,verifiedsignatures,snapshot-before-miner,
SQL failures,passwords,missing/truncated/NUL/directory/path cases,recursivefile/
directoryobservations,writeappend/replace/errors. No actual wallet/network used.
Othercounts perbuild: command12289,runner1265,runnererrors1120,utility1361,prefs1130,
txndb2503,transactioncommands1129/6904ops,SQLDB464/1260ops/8352calls,
MinimaDB2367/7519ops/334711calls,vault247/800ops/16275calls+8crossdecrypt.
Vault107.57s normal/515.76s sanitizer. No timeouts/failures in finalbatch.

Fullbuild71598/65544PASSwithoutwarnings. Logs/tmp/minima-offline-full-build.log,
-full-san-build.log,-regressions.log,-san-regressions.log. New sendview/sendpost/
sendsign compiled+registered; sendsignmain512printsmapped. MiniFile base/read/write
now reuseJavaUnixFile and POSIXdescriptorcleanup to preserve source operationorder
andtypedIOException/FileNotFoundException. Missing-read and write-parent baseline
reports retained in cpp/build/offline-send-file-{read,write}-baseline-results.json.
Initial tests also corrected expectPosted namecollision and wrong duplicatewitness
assumption: source signs eachinput; Witnessdeduplicatesroots. SQLfixture supplies
uses0eachquery, so repeatedwrites1prove callcount, notdurableincrement.

README/provenance/REVIEW/lab/sixaffectedlessons updatedwithfinalPASS. Scopedreview
approvewithsuggestions only; fullnodeandbroaderstream/reference/platform APIsstill
unapproved. Currentnewlaboffline-send-files.md waswrittenBEFOREtests. No source
mutationafterfinalbuild. Scoped12filewhitespace/conflictcheckPASS;gitdiffcheckPASS
but untrackedcpp limitsit. FinalguideauditPASS8891links2634lineanchors457sourcehashes0errors; source map324
checkPASS. AuditgeneratedINDEX/coveragejsoncurrent. Nooutstandingbatchwork.

Nextsendnosign.java FULLYREAD; native send.cpp/.hpp andtxnutils.cppFULLYREAD.
Sibling tools/minima-core-cpp src/tests searchfoundnosendnosign/selectCoins/
unsignedtransaction. Complete coin_funding_corpus.py,WalletMiningFixture.hpp,
SearcherOracle.java includingproofFixtures, bothchainharnesssections,Javafunding
fixtures and nativegeneratorgenerateTxPoW31..86 read. Reuseannouncement stillneeded
BEFOREcode. No sendnosignimplementation/test/labcreated. Existing12-nodeMMRfixture
adequateforconfirmedancestorpath; bothplain/custom-tokenfixturesavailable.
Differencesfromsend: walksconfirmdepthparentsBEFOREminer/search;coinage0butnotvalid
param; splitfractional1..20notfloored/no remainderoutputs;outputstorestate true;
statevaluescastString(sendcoerces);nochange-keyconsistencycheck;no sizegate/signing;
burnsign=false;dryrunvalidbutignored; writescompleteTxPoW tospecifiedpathor
unsignedtransaction-millis.txn. Reuse send::selectCoins; preserveotherdifferences.
GeneratedPRNGmustbeobservedandnormalizedwithoutdiscardingfilebytes; existingfunding
normalizer only handlesJSONdryrun PRNG. Existingtimefixtureforcesclockclamp.

--- Previous checkpoints (superseded where contradictory) ---

# Current checkpoint — offline commands verified normal, sanitizer running

Goal ACTIVE/incomplete, PROGRESS. 324counterparts284prod40test;133absent85prod48test.
All457lessons authored, affectedlessons updated.167CTest entries. No agents/commits.
LIVE exec24374 ASAN/UBSAN12selected regression CTest, log
/tmp/minima-offline-san-regressions.log.11checksPASS; only vault stillRUNNING. OfflinePASS97.59s andreportread.
Do not restart. Fullbuild71598/65544PASSwithoutwarnings. Normal30046terminalPASS
all12checks133.68s; all11reportsread. Normaloffline201programs558ops9773SQL23.32s.
Utility1361PASS,MinimaDB2367/7519/334711SQL,prefs1130,txndb2503,transactioncommands
1129/6904,SQLDB464/1260/8352,command12289,runner1265,runnererrors1120,vault247/800/
16275SQL+8crossdecrypt. source_mapping324PASS. Selected12notall167.

sendview/post/sign registered + sendsignmain512printsdone; nullptr/emptyargsboth
compared. MiniFile read/base/writeerrors corrected, baselinespreserved. Normal
199programsinterimPASS15.90s, final201includesdemo. No laterproductionchanges.
Read full current MiniFile/CommandRunner/newcommand source+headers, sourceJava3,
newcorpus/harnesssections and metadata/CMake. Scopedcode-reviewwrittenpending
finalsanreports. README/provenance/lab/REVIEW still say sanRUNNING: updateafterPASS.
Finalguideauditbeforelastdocwrites8886links2634anchors457hashes0errors; rerunafter.
Scoped12source whitespace/conflictcheckPASS;gitdiffcheckPASS(untrackedcpplimit).
Ten sanitizercomparisonreportsread (all exceptvault); no failures.
Need poll24374, inspect11sanreports, finalreviewverdict, docsvalidation, whitespace
checks, finalaudit then VERIFIEDcheckpoint. Nootherlivehandles.

Nextsendnosign.java FULLYREAD; native send.cpp/.hpp, txnutils.cpp FULLYREAD.
Sibling src/testssearchfoundno sendnosign/selectCoins/unsignedtransaction. Search
funding corpus/harness/generator helpersdone, Complete coin_funding_corpus.py, WalletMiningFixture.hpp, SearcherOracle.java
includingproofFixtures, both chainharness sections, Javafundingfixtures and native
generator generateTxPoW overloads31..86 nowread. Reuseannouncement stillneeded
beforecode. No sendnosignimplementation/test/lab.
Keydifferences fromsend: walksconfirmdepthparents BEFOREminer/search, coinage0and
notvalidparam; splitfractional1..20 notfloored,no roundingoutputs, outputstorestate
true; state values castString (sendcoerces); no keyconsistencycheck/changefrom;
no sizegate/nosigning, burntxnwithsign=false; dryrunvalidbutignored; writescomplete
TxPoW specifiedpathorunsignedtransaction-millis.txn. Must preserve these precisely.
FullnativeMain/Minima/durableSQLstillmissing, goalNOTcomplete.

--- Superseded checkpoints ---

# Current WORKING checkpoint — offline file commands; map324

Goal ACTIVE and incomplete. 324counterparts=284production+40tests;133absent.
All457lessons authored. New offline-send-files lab written BEFORE tests; per-file
lessons/docs not yet updated.167CTest registrations. No new agents/commits/pushes.
Previous VERIFIED321 checkpoint below remains applicable to previous code.

New sendview/sendpost/sendsign registered in runner/CMake. Native and Java fixture
load real TxPoW files, compare bytes/replies/SQL/mining/verification. Corpus187.
Missing-file baseline preserved in build/offline-send-file-read-baseline-results.json.
MiniFile read/base fixes match initial missing/NUL/directory checks. Current run
reached184of187: Java IOException Not a directory after signing, C++genericioserror.
Write baseline saved build/offline-send-file-write-baseline-results.json.
MiniFile prepare/write now exclusive POSIXcreate thenoutputopen/write/close, exact
IOException vs FileNotFoundException ordering; NOT YET TESTED. Build51346 terminal
exit2, inspect /tmp/minima-offline-build.log (requested utility target maybewrong).
No liveprocesses at this checkpoint. Sanitizer wallet binary stale for latest fixes.
Otherterminal handles99645PASS,70148/52013testfixtureassertionerrors,37197realwritefail.

Fixture expectation corrected: source signs EVERYinput but Witness deduplicates
by rootpublickey. Assert one retained signature per nonempty main/burn transaction
AND main+burn SQL key-use writes. Source row driver repeatsuses0 so writes1each;
not durableSQL proof. Names normalize wallclockfilename only. Need add directory
observations and explicit failure effects, scope, tests, sharedMiniFile regressions,
normal+sanbuilds/reports, code-review, affectedlessons README/provenance/REVIEW and
finalguideaudit. Native Main/Minima/durableSQL absent; nofullnodeclaim.

--- Previous verified checkpoints ---

# Latest VERIFIED checkpoint — send/funding and REV/LEN; map321

Goal ACTIVE and incomplete. This turn made PROGRESS. No blocker, commits, pushes
or new agents. All457 university lessons authored and affected lessons keptcurrent.
321counterparts=281production+40tests;136absent=88production+48tests.
166CTestregistrations. Allcounterparts remainpartial and paritynot_verified.
Native Main/Minima/durable SQL backend still absent. Do not mark goal complete.

NO LIVE TOOL/BUILD/TEST SESSIONS. Earlier LIVE/PENDING below is SUPERSEDED.
All12 DISTINCT selected CTest entries PASS perbuild:
source_mapping;command;command_runner;command_runner_errors;consensus;
transaction_inputs;transaction_mining;coin_funding;REVTests;rev_original_tests;
LENTests;len_original_tests. Not the full166. Source map was initially stale in
san7reg56960 because REV/LEN files were added during that run; regenerated321
map PASS in normal89457 andsan13124 five-entry original-test runs and finalcheck.
Don't rerun passing suites without new changes or a concrete concern.

Normalfunding4527 PASS47.76s;san64412 PASS229.24s. Each736programs3418ops39016SQL.
Bothfundingreportsread. Normal7reg50466 PASS177.25s;all6reportsread.
Sanreg56960 TERMINALexit8 onlystale-mapcheck;all6behaviorchecksPASS,
consensus543.15s,input224.68s,mining67.35s,command25.78s,runnererrors6.17s,
runner5.54s.All6sancomparisonreportsread. CorrectedmapPASS asabove.
REV/LEN+map5checks89457normalPASS9.41s;13124sanPASS9.24s;all4originalreportsread,
3unchangedJava methods and12assertions perfileperruntime.
18distinctcomparisonreportsread:6reg+funding+2original perbuild.
Fullbuild4159/87171PASSwithoutwarnings, followed byREV/LENtargets8424/79189PASS.
Logs/tmp/minima-funding-full-build.log,-full-san-build.log,
/tmp/minima-rev-len-build.log,-san-build.log. No production changes after builds.

Newcommands send,txnaddamount,txncoinlock implement actual source branches and
register in CommandRunner/CMake. Reuse existingWallet/Searcher/Proofs/Generator/
keys/vault/txnutils minerreceiver; stocksourcequirks preserved. SharedCommand
JSONArraycast nowtyped;Generator size/workrejectbranches logactualMinimaLogger.
Sourcecapacity/caught-exceptiongeneratorlog paths stillgap. NewREV/LENmappedtests
reuseBITGETadapter and originalrunner, productionfunctionsunchanged.
Newlabscoin-selection-and-funding.md andreverse-and-length.md createdBEFOREtests.

README/provenance/REVIEW/lessons updatedwithfinalPASS. Scopedmanualcode-review
approvewithsuggestions only; fullnodeandbroadernull/list/objectkey/demoAPIsnot
approved. Newsourcewhitespace/conflictcheckPASS;gitdiffcheckPASSbutuntrackedcpp
makesitlimited. FinalguideauditPASSafterlastdocwrites:8870links,
2634lineanchors,457hashes0errors. Source map321checkPASS. Noactualwallet/network.

Nextsendview/sendpost/sendsign sourcefiles FULLYREAD. Noimplementationwritten.
Reusegate still needs completion/report. Fullnative txnview/export/import read;
MiniFile.hpp, cppread/write/basefunctions, JavaMiniFile23..140, fullutilitydriver,
JavaUnixFile.hppread. Siblingsearchfoundno correspondingcommands/writer. Existing
MiniFile normalcodec reusescompatiblebytes butgenericioserrors/pathnormalization
needexactfailuretests. Utilitysuite onlynormalpersistence andnormalizeserrors.
Needfinishnativeutilityfixture/callerreadsandreportreuse BEFOREnextcode.
sendsign createsreqsigs butNEVERaddskeys:signseveryinput MAINthenBURN. Optional
unlockiflockedbeforeTxPoWdereference; normal-onlyrelock;writeactualTxPoW to
signedtransaction-currentMillis.txn. sendpostcapturesresponseJSON BEFOREasyncminer.
Detailed implementation, fixture and retainedbaseline notesbelow.

--- Superseded working checkpoints ---

# Current checkpoint — send/funding plus REV/LEN; one sanitizer regression running

Goal ACTIVE, incomplete. This turn made PROGRESS; no blockers/commits/pushes.
321 counterparts=281production+40tests;136absent=88production+48tests.
166CTest entries. All457lessons authored. Guide audit8870links2634anchors0errors.

LIVE: exec session56960 sanitizer seven-regression CTest (log
/tmp/minima-funding-san-regressions.log). Six terminal results: miningPASS67.35s,
commandPASS25.78s, runnererrorsPASS6.17s, runnerPASS5.54s,inputPASS224.68s;
source_mapping failed only because REV/LEN files were being added during the run.
Map regenerated321 and both source_mapping followups PASS in final REV/LEN runs.
Only consensus remains RUNNING; do not restart it. Need wait terminal and inspect
cpp/build-sanitized/consensus-differential-results.json. All other sessions terminal.

PASS normal funding4527 736programs3418operations39016SQL47.76s.
PASS sanitized funding64412 same counts229.24s. Bothreportsread.
PASS normal7reg50466 177.25s, all6comparisonreportsread.
San4earlyreg+input reportsread; consensusstillpending.
PASS REV/LEN+mapping5checks normal89457 9.41s,san13124 9.24s;
all4originalreportsread (3unchangedJava methods,12assertions each perruntime).
12 DISTINCT selectedCTestchecks perbuild when finalsanconsensuspasses:
7reg+funding+4original(native/differentialx2);do notcountmapfollowupasextra.
Full all-target builds4159/87171 PASS withoutwarnings, then newREV/LENtarget
builds8424/79189 PASS. Newtarget initial no-rule error fixed by cmakeconfigure.
Logs /tmp/minima-funding-full-build.log, -full-san-build.log,
/tmp/minima-rev-len-build.log and -san-build.log. No library changes after these.

Newproduction send/send.cpp,hpp;txn/txnaddamount.cpp,hpp;txncoinlock.cpp,hpp.
Source-exactmetadata registeredCommandRunner/CMake. Actualwallet/search/proof/
generator/keys/vault/miner dependencies; Mainreceiver uses txnutils::getMiner.
Preservestableinplacesort, addressgrouping, firstsmallbreak, fullfallbackincluding
reservedcoins;sendfiltersagain,txnaddamountdoesnot. Fundingaddsinputs/locksBEFORE
outputparamparsing;no rollback. Sendactualtokenroundtrip,outputs,state,change-key
check,signing,realburnsigningevendryrun,password/mining ordering.
Shared Command.getJSONArrayParam now typedJavaClassCastException (reuseJSONObject
pattern). Generator size/work rejectbranches nowlogactualMinimaLogger. Other
capacity/caught-exception generatorlogs remainunverified/omitted; documented.
Sendcatch uses NativeThrowable SQLtext, excludesJavaError/bad_alloc likevault.
GenericobjectJSONkeys/publicnullablelocklist/replacement/nullselectorargs,
AddressAmountreferencegraph/demomain stillgaps. Allcounterpartsremainpartial.

Newtest mappingsREVTests/LENTests exact12assertions eachusingBITGETadapter,
actualregistry/factory/Contract;productionREV/LENunchanged. OriginalJava wrappers
and sharedscript_value_original_tests.py/CMakeentries added. SourceREV invaliddomain
is actuallytwoemptyparamswrongarity;LENsampleASCIIonly. SiblingsFunctions.cppREV
compatible butLENcountsUTF8bytes;existingnativeUTF16 implementationreused.

Harness extends WalletCommandsOracle/wallet_commands with selection output plus
sorted input, sharedCoin mutation, coinLock helpers/state snapshots,fundingbool
resetslocks eachprogram, verifyMinedactualTreeKeyverification. New
cpp/tools/coin_funding_corpus.py 736programs +groupfunding inexistingdriver.
Source-derived customtokenMMRchains reuse SearcherOracle.proofFixtures(Token)
overload; default unchanged. Java fundingFixtures returns2tokenscripts variants.
Fixturepool nowfixedPRNG32x0x12 andtime0 BEFOREhash so rejectedIDlogscompare.
DryrunPRNGONLYnormalizedafterlength32assert;vaultcases reusevaultnormalization.
Allotherreply/log/SQL/lock/txnwire/minerfields compared. Noactualwallet/networkused.
Baselines: build/coin-funding-address-assertion-baseline.log (badvalidhexfixture);
coin-funding-generator-log-baseline-results.json (missingnativegeneratorlog);
coin-funding-array-cast-baseline-results.json (JavaClassCastvsbad_variant_access).

Teachinglabs createdBEFOREnewtests: coin-selection-and-funding.md,
reverse-and-length.md. Corresponding5filelessons+Command/TxPoWGenerator/REV/LEN
productionlessons updated. README/provenance/REVIEW updated but some latest
validationtext STILLPENDINGsanconsensus; finalizethoseaftercompletion. REVIEW
needs scoped finalverdict afterchecks. Finalsource-map/audit/whitespace required
afterfinaldocs;gitdiffcheckalreadyPASSbutuntrackedcpptreemeanslimitedcoverage.

Next readableport group: sendview,sendpost,sendsign. Full3pinnedJavafiles JUSTREAD
thisturn; no new implementation yet; reusegate NOTCOMPLETE fornextgroup.
sendview readsMiniFile.createBaseFile/readCompleteFile,convertTxPoW,replynestedtxpow.
sendpost same+calculateid, snapshotsJSON BEFOREactualasyncminer call.
sendsign reads/convert,getsWallet,optionalunlockiflockedBEFOREdereferenceTxPoW;
for MAIN thenBURN inputs addScriptProof then signData every input: reqsigs is
created but NEVERPOPULATED, so source does NOT deduplicate keys. Relock onlynormal
path. Calculateid, write actualobject to signedtransaction-currentMillis.txn,
replyabsolute filename. Demo main512MiniData values. Needs MiniFile currentnative/
file-command helpers/callers/tests +siblingsearch +user reuse report beforecode.

Additional next-group source reads (no code written): full native txnview.cpp,
txnexport.cpp,txnimport.cpp; MiniFile.hpp and MiniFile.cpp relevant read/write/base
helpers; full utility_differential.py and Java MiniFile lines23..140; JavaUnixFile.hpp.
Sibling src/tests search found no sendview/sendpost/sendsign or matching filewriter.
Native MiniFile uses generic ios failures and unnormalized filesystem paths;
next group should check typed FileNotFound/negative-array/path semantics using
existing JavaUnixFile helpers before claiming parity. Existing utility suite
normalizes exceptions toERROR and onlytestsnormalisolatedfilepersistence, so is
not evidence for exactIOerrors. Still need nativeutility/filefixture callerread
and explicitreuse report before nextcode.

--- Earlier checkpoints (superseded) ---

# Latest VERIFIED checkpoint — vault, txnsign, keys, BITGET/BITSET; map316

Goal ACTIVE and incomplete. This continuing turn made concrete PROGRESS. No
blocker, commits, pushes or new agents. Do not mark full goal complete.
316counterparts=278production+38tests;141absent=91production+50tests.
161CTestregistrations.All457lessonsauthored.Allcounterpartsremainpartial,
fullparitynot_verified.Main/Minima/nativeH2MVStore stillmissing.

NO LIVE BUILD/TEST/TOOL SESSIONS. All current checks TERMINAL PASS.
Finalsan20195 keysPASS265.36s132programs262ops7895SQL;normal11494 samePASS51.46s.
Bothkeysreportsread. Finalfullbuild10680/83064PASS, no compilerwarnings/errors.
Latestbuildlogs /tmp/minima-key-null-full-build.log and-full-san-build.log.
Normal18reg47351PASS543.20s;san18reg22283PASS1832.49s;all34reportsread.
Normalvault92942PASS128.01s;san18257PASS560.21s247programs800ops16275SQL8crossdecrypt
bothreportsread. OriginalBITGET/BITSET4entrieseachbuildPASS;272/271assertionsper
runtime3unchangedJavamethodseach;all4originalreportsread. Current24distinctselected
CTestentriesPASSperbuild:18reg+vault+4bit+keys,NOTfull161.42distinctcomparisonreports
read=17reg+vault+2originalbit+keys perbuild. Afterkeysregistration normal54576
3followupsPASS34.12s(runner/errors/mining);san68747twoPASS12.50s(runner/errors);
all5refreshedreportsread,notcountedasseparatedistincttests. Sanmining alreadyrannew
sharedfixtureandpassed. DoNOTrepeatentirepassinggroupswithoutnewchanges/concerns.

New productionvault/txnsign/keysactualsourcebranches/helperbodiesregistered.
Sourcekeyderivation,WalletSQL,PasswordCrypto,BIP39,TreeKey,Witness,txnpost/
MinimaDB/MiniFile reused. ExplicitMainreceiver/postMessage defaultsNPE,needsnative
Mainstartupbinding. Preservelocksave-before-wipe,clearafterrestore,nogeneral
relockfinally,postbeforelateparse/delete,keysseed-private-then-publicchecking.
WitnessnullableaddSignature/isSignedBy;Unicodecasefoldextractedfromexisting
InetSocketAddress;TreeKeyNodearrayboundstypedJavaexception;TreeKeydefaultactual
MinimaLogger. BITGET/BITSET productionunchanged;actualsourceassertionsmapped.

Finalonlyactualproductionfailuresfixed:negativecustomindexRuntimeException:vector
->Javaarraybounds;keysnullfilterbad_variant_access->nullStringreceiverNPE.
Otherfailureswerefixtureassertions:shortchainauto-post(keptrejection+reuse12block
fixtureforsuccess),keyphraseouterspaces/blank(sharedCommandtrims/rejectsblank).
Sanvaultper-request30sectimeoutfixedwithoptionalRunner.response_timeout(default30;
vault/keys180);sameassertionspass. Allbaselinesretained;no oldfailureremainscurrent.
Detailedimplementation/sourceinspection notesandbaselinepathsintheearlier
workingcheckpoints below. TheirLIVE/PENDINGstatesareHISTORICALandSUPERSEDED.

Docs/provenance/README/REVIEWupdatedfinalPASS.Manualcode-reviewcompleteforcurrent
changes,scopedapprovalonly;nativeMain/H2/lifetime/interruption/randomgenkeygaps
retained. Newlabs vault-and-signing,bit-get-set,wallet-keyscreatedBEFOREinitialtests.
Allaffectedlessonsupdated;sendlessonrefinedfromfullsourcereadbutcommandunported.
Finalguideaudit316map,457lessons,8852links2634anchors0errors afterallguidewrites.
Source-mapcheck316PASS. No audit rerun needed for thischeckpoint-onlysentence.
Newfilewhitespace/conflictchecksandgitdiffcheckPASS;untrackedcpptreemeansgitdiff
aloneisnotcompletewhitespacecoverage.No actualuserwalletused;syntheticfixturesonly.

Next actualport: send/txnaddamount/txncoinlock(andremainingfiles). FullpinnedJava
send952lines,txnaddamount,txncoinlockandkeysreadthisturn;keys nowimplemented.
Reusegatefornextsend-family NOTyetcomplete:finishrelatednativeadapter/caller/test
andclosestcandidateinspectionandreportexactreusebeforewriting. Existingnative
keys::checkKey nowavailablewithpositivefixtureseed0xAA/modifier0x00/private/public.
Do notuseoldvaultrow0x12privateasifderivedfrom0xAA(base+modifiercheckwouldfail).

Sendstudy:orderCoins stablesortcaller'slistdescendingthenaddressgroups;selectCoins
scansskippingminer/mempool,breaksfirstsmallcoin,returnsmostrecenteligibleifany;
elseentiregroupedlist(includingreserved). sendcallerfiltersagain;txnaddamount
callerdoesnot. Preservequirks. NativeMinimaDB/miner/wallet/txnutils alreadyavailable.
senddryrunskipsmainunlock/sign/miningbuttokenburnhelperoutsideguardstilldefaultsigns.
Stocksignkeyidentity:Command.getAddressParamabsentreturnsdefaultemptyliteral;
providedblankthrows;0xempty->MiniData.to0xString->BaseConverter.encode16 returns
emptyliteral. Thuscontentemptycheckmatchesstockemptyreturnpaths;testabsent/blank/
0x/nonempty. SubclassoverridesseparateAPIquestion. JSONObjectJavaextendsLinkedHashMap,
writerstringifiesobjectkeys;MiniDataequalsbytesbutnohashCodeoverride. Dryrunbytesize
usesMiniDataobjectkeys,nativeJSONObjectstringonlyneedsreviewespdistinctequalprinted
keys;don'tnormalizeawayduplicateJSONkeys.See sendstudentlessonfollow-up.

--- Historical working checkpoints follow ---

# Current execution status — ONE sanitizer key suite pending

Goal ACTIVE, incomplete; this turn PROGRESS. No blocker/commits/pushes/newagents.
Current316counterparts=278production+38tests;141absent=91production+50tests.
161CTestentries;all457studentlessonsauthored.Allsource-mapstatusespartial/paritynot_verified.

ONLY LIVE SESSION:20195 finalsanitizerwallet_key_commands_differential.
Log /tmp/minima-key-command-correct-san-test.log. Started with final132-casecorpus
andfinalparsedphrase/blankexpectations. No productionchangespending. Poll terminal,
readbuild-sanitized/wallet-key-commands-results.json, fixonlyactualnewfailures.
All otherbuild/testhandles TERMINAL. DoNOTrestartolderrunslistedbelow.

Finalnormalkeys11494 PASS132programs262operations7895SQL51.46s;reportread.
Correctedfullbuild10680/83064PASSnowincludeskeys;no compilerwarnings/errors.
Normal18reg47351PASS543.20s;san18reg22283PASS1832.49s.All34reportsread.
Vault247programs800ops16275SQL8crossdecryptPASSboth(normal128.01s,san560.21s),
bothreportsread. BITGET/BITSET4entriesPASSboth272/271assertionsperruntime,
all4originalreportsread. Afterkeysregistration normal54576threechecksPASS34.12s
(runner,runnererrors,mining);san68747twoPASS12.50s(runner,runnererrors),all5
refreshedreportsread. Do notcounttheseoverlappingchecksasseparatedistincttests.
Normal24distinctselectedCTestentriesPASS;san23with24th(20195)pending,notfull161.
41/42distinctcomparisonreportsread(currentpassedreports),onlyfinalsankeysawaited.

Obsoletekeysruns:17656 normalfailed83.04swrongspaceexpectation;
64336normalfailed76.68sloadedobsoleteblankexpectation;12671sanfailed413.98s
wrongspaceexpectation. Allterminal;productionagreedwithJava. Bothspaceandblank
expectationsnowcorrectafterfullCommand.getParamread:trimU+0000..0020,rejectblank.
Savedlogsbuild/wallet-key-commands-phrase-assertion-baseline.log,
build/wallet-key-commands-blank-assertion-baseline.log,
build-sanitized/wallet-key-commands-phrase-assertion-baseline.log.
Earliernullfilterfailurewasactualproductionmismatch;fixed,bothJSONbaselinesretained.
Newkeyrun20195usesfinalcode+assertions;normal11494alreadyPASSsamefinalsuite.

Documentation updatedthroughnormalkeysresult,all18regbothresults,316map.
Stillneedreplacefinalsan-keyPENDING wording inlesson/lab/README/REVIEW/provenance,
finalreviewverdict,thenre-auditandwritefinalcheckpointwhen20195passes.
Currentguideaudit457lessons316map8852links2634anchors0errors. gitdiffcheck/newfile
whitespace/conflictchecksPASS. Manualcode-review includesnewkeysandchangedfixtures.
FullnativeMain/Minima/H2MVStoremissing;randomno-phrasegenkeynotcoveredbynewgroup.

Exploratory sendlesson nowrefined, no send/txnaddamount/txncoinlockimplementation.
Fullsend.java+keys.java+txnaddamount/txncoinlockreadpreviousbelow. Stocksend signkey
identity can be analyzed fromactualgetAddressParam:absentdefaultemptyliteral;
providedblankthrows;0xemptyconvertsviaMiniData.to0xString->BaseConverter.encode16
whichreturnsemptyliteralwhenbytesempty. Thereforecontentemptycheck should match
stockmethod paths; verifyabsent/blank/0x/nonempty explicitly. Java subclassoverrides
areseparateAPIgap. JSONObject.java1..125 reread:extendsLinkedHashMap(insertionorder,
NOTHashMaparbitraryorder);writerstringifieskeysatoutput. MiniData.equalsObject
comparesbytesbutnohashCodeoverride;distinctequalprintedobjectkeysexpose generic
nativeJSONObjectstringonlygap indryrunbytesize. Don't relyonJSONparsingtoobserve
duplicatekeyserialization. MiniData.to0xString returnsBaseConverter.encode16.
Sendlessonlinksnewvault/keyslabsandrecordstheseobservations;unportedlabelretained.

--- Earlier working checkpoints follow; execution statuses above supersede them ---

# Wallet keys follow-up — map316; checks RUNNING

Newkeys.hpp/.cpp fullyimplemented commandbranches+checkKeyhelper,registered/CMake.
Currentmap316=278production+38tests;141absent91production+50tests.161CTestentries.
GoalACTIVE/unblocked;thisturnPROGRESS.Noagents/commits/pushes. DoNOTmarkcomplete.

LIVE:22283 sanitizer18regressions (16passed,transaction_check thenminingremain);
12671 sanitizerkeys correction run (will hit now-fixed fixturephraseassertion);
64336 normalkeys trimmed-phrase retry (startedjustbeforeblankassertioncorrection;
mayhaveoldcorpusloaded andfailblankexpectation—inspectactualterminal).
Current logs /tmp/minima-vault-san-regressions.log,
/tmp/minima-key-command-final-san-test.log, /tmp/minima-key-command-parsed-test.log.
All otherhandles terminal. Initialkeybuild5935/59034PASS;firstfull29002/82911PASS;
correctedfull10680/83064PASS(no warnings). Initialnormal42442FAILEDcase23(12.47s),
initialsan44950FAILEDsamecase23(64.53s):nullStringreceiver->bad_variant_access.
Bothresults savedwallet-key-commands-null-filter-baseline-results.json perbuild.
ActualkeyslistnowchecksnullableJSONfieldbeforeStringequals;typedNPE sourceparity.
Correctednormal17656 reached extraassertionforouterspaces andFAILED83.04s;
JavaandC++agreedcorrectlytrimmedphrase. Savedbuild/wallet-key-commands-phrase-
assertion-baseline.log. No productionchange needed forfixtureassertions.
Command.getParam source/native reread:trimU+0000..0020 thenrejectblank.
Corpusnowexplicit expectedparsedphrase andblankerror;driverexpectKeyError+genkey
assertionnoSQL/phrase/modifier/script. Needfinishboth132programcomparisons,
retainfailureifoldloadedcorpusfailed,rerunonlynecessarycorrectedkeysuite.

keysreusegatecompletedfullJavafile,fullnativeKeyRow/sourceKeyRow,Wallet.hpp/.cpp
(currentturnbeforecompaction),BIP39methods/TreeKey/Command+getParam reread,
sendcallerfull,txnminepostreceiverfull. SiblingWalletin-memorynoSQLlifecycleandno
matchingkeyscommand;exactreuseannouncedbeforewriting. Actualcodepreserveslist
filterpriority+firstmatch,maxusesinitial0,genkeyusesgetParamtrimwithoutBIP39clean,
checkkeysrequiresbaseavailable/privatepubliconly,newdelegatesWallet,createallkeys
postsMAIN_LOAD_ALL_KEYS(keyuses)thenreply. ExplicitpostMessagedefaultNPEneedsMain.
checkKeyprivate/seedmodifierguardTHENreconstructpublic;missingrowNPE. uint32counts
bitcastretainJavawrap. Allsourcehelp/paramsfactory. JSONObject::getString returns
non-null nativeString viaasString;commandexplicitnullreceivercheckminimaladaptation.

Fixture:WalletMiningFixture keys::postMessage resetdefaultNPE/initbindexistingpost;
message describe addskeyuses onlyifpresent, mirrorsJavaMiningMain messagecapture.
keysCheck opactualstatichelper.Java keyFixtures hashes0xAA seed+0x00modifier then
TreeKey2x2:private0xC4DD9EF0F0591E2CA51FC8E40EF8A9C75520B33B5AF952E5558D7A53F519DA3D,
public0x5D75E30C06C55C848BA7BA34E46BFC30FCE1D0529FEC1FF4C706C434DE31AFFE.
Newwallet_key_corpus132programs,toplevelvaultsnapshot,actualrecords/filter/guards/
SQLfail+typedparams,validderivationvswrongmodifier,mixedprivatepubliccounts,
explicitphrasegenkey,newkey,exactMainpayload-1/0/fractional/wrap andpostfailure.
No randomno-phrasegenkeycase;documentedgap.Runnergroupkeysresponse_timeout180,
prefixwallet-key-commands;CTesttimeout600. ExistingvaultcomparisonsstillPASS247both.
Sharedfixturechangesonlynewkeysops/keyusesfield;oldmessagesunchanged. Oldnormal
18regressionsPASS(all17reportsread);sanstillrunning.Walletkeyschangeslandedwhile
sanregressionlatergroupsrunning;noexistingproductiondependencychangedexceptregistry.
Need short command_runner/error regression afterkeysregistration ifnewconcern,
andfullbuildsfinalalreadyPASS;doNOTrepeatall18onlybecauseaddedkeyscommand.

Newwallet-keys.md labbeforeinitialtest,perkeyslessonupdated,CMake161,
README316counts,provenancekeysection/reviewfollowupPENDINGresults. Guideaudit316map,
457lessons,8850links2634anchors0errors. Updateallpendingdocs/provenance/review/
checkpointwithfinalactualtestresults. Reviewkeysproductionandchangedfixtureparts
performed. No furthercodechanges currentlyplanned untilchecksresolve.

The next earlier checkpoint has vault/bit details, followed by olderhistory.
Its count315 superseded.18257sanvaultPASS560.21s247/800/16275/8 andreportread,
terminal. Afterallchecks expect24distinctselectedCTestentriesperbuild:
18regressions+vault+4bit+keys.42reports total=17reg+vault+2originalbit+keys perbuild.
Currentlyallnormal17reg+vault+2originalbitreportsread;san15completedregreportsneed
finalcollection+vault+2originalbitsread. Don't countstaleresultsasnewpasses.

--- Vault and bit checkpoint follows ---

# Latest — vault/signing and bit-get/set mappings; map315; sanitizer checks RUNNING

Goal ACTIVE, incomplete; this turn PROGRESS. No blocker/commits/pushes/newagents.
All457 student lessons authored.315counterparts=277production+38tests;
142absent=92production+50tests.160CTestregistrations.Allstatusespartial/paritynot_verified.
Main/Minima/nativeH2MVStore absent.

LIVE session:22283 sanitizer18regressions.18257 vault retry nowPASS247programs
800ops16275SQL8crossdecrypt560.21s,reportread,handleterminal.
Logs /tmp/minima-vault-retry-san-test.log and /tmp/minima-vault-san-regressions.log.
Sanregression stillrunning; consensusnowPASS602.39s andwalletstageactive. Do not restart
or claim completed based on prior reports. Normal47351 ALL18PASS543.20s, terminal.
Normal vault92942 PASS247programs800ops16275SQL8crossdecrypt128.01s, reportread.
Sanfirst49492 FAILED141.97s: per-request30second harness timeout case186 during
fullsizecustomsigning; no sanitizerdiagnostic. Terminal. Saved crashbaseline
build-sanitized/vault-signing-commands-timeout-baseline.json. Runner now optional
response_timeout(default30); vault selects180. Retry18257PASS same247programs.
New bit tests37460/93525 ALL4PASS eachbuild (9.98snormal;sanlogfinal),terminal;
4originalreportsread272BITGET/271BITSETassertionsperruntime,3Javamethodseach.
Fullbuild35264/49483 PASS then73662/44100 PASS addingBITGET/BITSET targets;
no compilerwarnings/errors. All handles terminal except22283.

Production newvault.hpp/.cpp,txnsign.hpp/.cpp registry/CMake. Actual wallet,
BIP39,PasswordCrypto,TreeKey,Witness,txnpost,MinimaDB/MiniFile reused. Explicit
vaultMainReceiver defaultnull, needsfutureMainbinding. PreservegetBaseSeed/order,
keycreationguards,confirmation/semicolon onlycommand,passwordencrypted-save-before-
wipe,clearseedafterrebuild,restore/testphrase/resetkeys.Tempfilesonlytests;noactual
walletused. stopkeysrealtwosecondsleep;Java interruptionmodelgap.
txnsignretainsactualrowowners,precomputeIDbeforewallet,auto/explicit/custommodes,
normalrelockandmissingexplicit-keyrelockonly;nofinally,latepost/delete behavior.
WitnessnullableaddSignature rejectsnullwithsourceIllegalArgumentException;isSignedBy
optionalkeyenumeratesallrootsbeforecomparison;vector-heldsignaturealiasgap.
Extracted exactexistingInetSocketAddress unresolvedUnicodefold into support/
JavaStringIgnoreCase.hpp;reuseforphraseequalsIgnoreCase;peerregressions relevant.
TreeKeyNodearraybounds nowtypedJavaArrayIndexOutOfBoundsException index/length,
negativecustomuses=-1 exposedgenericvectorerror;12directnodeboundsfixtures.
TreeKey defaultlog usesactualMinimaLogger; explicitloggerretained.Objectharness
setsno-oponlytreekeycaseasJavaoriginalcapture. Negativeconstructorcategoriesgap.

NewWalletVaultFixture +Java/nativewalletops,toplevelvaultsnapshotsactualbase,
encrypted/decrypted,prefs,4tempstatefiles,Maincalls/flags,STOP_NEWKEYS. RealSQLcalls
withsuppliedqueryrows: NOTdurabledatabase.8crossdecryptprogramsappendfromactual
Java/nativeciphertext;onlyindependentciphertextbytesnormalized,keeplength/wireheader/
otherprefs. Newvault_signing_corpus247 includes typedparams/failSQL/wipe/reset/sign/
post;all8postcombinationsassertsuccess. Initialtestfixtureautomaticpost failedboth
becausechainshort; saved build/vault-signing-post-fixture-baseline.json,retainrejection
anduseexisting12blockwalletchainfortrueautomatic. No productionfixforfixtureerror.
Firstvalidnegativeindexfailure savedbuild/vault-signing-commands-key-index-baseline-results.json.
Old invalidstalebuildtest20039 ignored;oldcorrecttest2473exit8index;81076exit8
fixtureassertion;allterminal. No oldsessions running.

NewmappedBITGETTests.cpp/BITSETTests.cpp reuseBITCOUNTadapter;sourceBitSetbytes
reproducedLSBfirst,positions0..62,two126examples,type+value;allinvalidargs preserved.
No productionBITGET/BITSETchange. NewOriginalTestwrappers +existingrunner mappings
and4CTestentries. Fullsourcetests/functions/Contractgetter/ConstantExpression/HexValue
andregistryread. SiblingFunctions.cpp useswrongbitnumbering/out-of-rangefalse/
truthiness/retainszerotail;single set/get roundtripinsufficient. Exactreuse reported.

Docs affected vault/txnsign/Witness/TreeKey/TreeKeyNode/InetSocketAddressIO/BITGET/
BITSET+2testlessons updated. New labs vault-and-signing.md andbit-get-set.md BEFORE
initialtests. README/index/audit/provenance/REVIEWupdated;sanstatusPENDING.Must
replacependingwithactualfinalresults afterterminal tests. Manualcode-reviewdone,
fullnode/lifetime/interruptiongapsrecorded. Audit315map,457lessons,8842links,
2634anchors,0errors. Newfilewhitespace/conflictchecks pass(gitdiffaloneweakuntracked).
All17normalregreports read and0failures;4completed sanregreportsread(object9464,
KISS4471,peermessages1447,utility1361). Need readremaining13sanregreports+sanvault
whencomplete,updatecheckpoint/docsfinal. Normal23selecteddistinctCTestentries
18reg+vault+4bit;sanexpected23butNOTyetcomplete. Source mappingcheck315done.

Exploratorynextwork ONLY(noimplementation): full pinnedtxncoinlock.java,
txnaddamount.java reread; fullsend.java read inranges1..65,65..330,331..595,
596..798,798..952;fullkeys.java read. NativeKeyRow.hpp/.cppfull andWalletcheckSingleKey,
BIP39getNewWordList/convertWordListToString bodies,txnminepost.hppreceiver read.
Searchcurrent/sibling tools/minima-core-cpp noexistingselectCoins/orderCoins/
txnaddamount/txncoinlock. BroaderProjects/minimarg hitunrelatedoverlongfilenameunder
maxima/minima-common;don'tclaimexhaustivesiblingsearch. Reusegatefornextcommands
NOTyetcomplete;do notreportimplemented.
Nextdependencies: sendneedskeys::checkKey,currentlyabsentcommand. keys::checkKey
checksseed+modifierthenconstructsrow-size/depthTreeKeypublicroot. Needconsistent
fixtureseed/private/modifier/public forsuccessfulsendchange verification(current
vaultrowsmallpublicmatchesprivatebutprivateisnotderivedfrom0xAA base).
send.orderCoins sorts callerlist amountdescending(stable) thenreturnsaddressgroups.
selectCoins checksminer/mempool seekingonecoin>=amount;breakfirstsmall coin even
addressgroupordering;iffoundreturnsmostrecenteligible,elsereturnsentireorderedlist
includingpotentialreservedcoins. sendcallerfiltersreservationsagain;txnaddamount
callerdifferent. Preserve thesequirks,notgenericcoinselectionrewrite.
senddryrun skipsmainsignature/unlock/mining BUTtokenburn helperstilldefaultsigns;
JSONbytesize usesMiniDataobjectsaskeys(nativeJSONObjectcurrentlystringonly),
MiniDataJavaequalsObjectcomparesbytesbutnohashCodeoverridefound. Needownership/
keysemanticsreview. signkey comparison usesJavaStringidentity(!="");inspect
getAddressParamidentitybeforeequatingtocontent/presence. send fullnativeportnotstarted.

--- Earlier checkpoint follows ---

# Latest — mining commands, POW/SIGDIG and BITCOUNT VERIFIED; map311

Goal ACTIVE, incomplete; this goal turn PROGRESS. No blocker, commits, pushes or
new agents. All457 student lessons authored. Current map311=275production+36tests;
146absent=94production+52tests. 155CTest registrations. Main/Minima and native
H2/MVStore absent. All counterparts partial/parity not_verified.

No live build/test/tool sessions remain. All current checks terminal.
Normal group69329 exited8 (15pass+1oldharnessfailure); targeted42109 PASS9.09s
replaces that failure. Normal total258.81s+targeted9.09s. Sanitizer group75041
exited8 (16pass+1oldharnessfailure), total1117.13s including newmining. Targeted
65084 PASS22.83s replaces its protocolfailure. All16selectedregressions now have
passingfinalresults ineachbuild. No new/currentfailures. DoNOTrepeatentiregroups
onlybecausehistoricalpre-fixfailureappearsinlogs.

Latest fullbuild50239/41149 PASS includesBITCOUNT, POW/SIGDIGandcommands; logs
/tmp/minima-bitcount-build.log and-san-build.log have no warning/errorlines.
Earlierfullbuild78471/51330 alsoPASS. No oldsessionsstillrunning; do not restart
oldone-offgenerators. 24distinctselectedCTestentriespassperbuild, notfull155suite:
16regressions+mining+4POW/SIGDIG+3BITCOUNT/KISS. All38comparisonreportsread,
19perbuild=14regressions+mining+3originaltests+KISS. Source mapping andminerworker
arethe2nonreportregressions. No repeatinvocationscountedasseparatechecks.

Production new txnpost/txnmine/txnminepost .hpp/.cpp +registry/CMake. ActualMinimaDB
row, sharedGenerator, txnutils signedburn, actualminer async/sync; post beforelate
parse/delete errors retained. getMiner callbackexisting; newtxnminepostpostMessage
callback defaultJavaNPE, futureMainmustbind. TxPoWMinerentrymethods nowvirtual.
TxnRow/TxPoW defaultconverters log caughtIOException using MinimaLogger and
NativeThrowable, reusingCoinpattern. Firstfailure saved in build/transaction-mining-
commands-converter-baseline-results.json. Productiondecoder changeexposedold
support/transaction_commands.cpp stdoutprotocol issue; nowloggerprintln->stderr
unicode::encodeUTF8, matchingexistingJavaSystem.outredirect. Exactproductionlogretained.

New WalletMiningFixture.hpp plus wallet_commands.cpp/WalletCommandsOracle.java
operationsmining,mineNext,mineState,postHeld,limitSize/proofCoins. Actual dormantminer
bodiesrun; JavaUnsafe avoidsMain/minerconstructors. OnlygeneratedbodyPRNG fixed32
bytes0x12atminerentry; otherheader/txndata unchanged. Calls, fullwire, queue/post
messages, heldowners and allheldmain/burncoinreservations capturedaftereachop.
Failuresfalse/throw/postfailure injectedseparately. Actualconsensus29modes+wallet
12nodechainfixturesreused. Newtransaction_mining_corpus.py +drivergroupmining.
Normal207programs699ops1893SQLPASS54.49s; sanitizer samecountsPASS62.54s. Bothreportsread. Casesrealasync/sync,
importedrow,missingreceivers,typedparams,burn signatures,late deletion,truncations,
bodyless/nullpost,burn+inputproofs and retainedreservationsafterrowdelete.

MappedPOWTests,SIGDIGTests reuse INCTests adapter andunchangedJavamethodsdriver.
4CTestentrieseachbuildPASS5.60normal/6.11san;25/89activeassertionsperruntime.
NoPOW/SIGDIGproductionchange. ExistingSIGDIGsiblingreversedargs/doubleincompatible.
MappedBITCOUNTTests +actualpublicmutable256-entrytable inBITCOUNT.hpp; totalBits
readsentriesviauint32accumulator/bitcastpreservingJavaoverflow insteadpopcount.
Source34successfulJUnitassertioncalls +256conditionaltablepredicates;13hexexamples.
SeparateprocesssupplementalJava/nativeoutputs84,42,-2147483648 comparetablemutation.
BITCOUNT+original+KISSregression3entriesbothbuildsPASS12.90normal/21.74san;
4471KISSprograms(not4950; commentarymistakeimmediatelycorrected). Reportsnormalread,
sanreportsnowread. Nativefullbuildsincludechanges, no otherproductionchanges.

Docs: Newlabs transaction-mining, power-significant-digits, bit-counting (12th-14th)
createdbeforetheirinitialtests. Affectedlessons+README/indexnav+source-map+provenance
+REVIEWupdated. Miningdocs/provenance/README/REVIEWupdatedwithfinalsanPASS. BITCOUNTandPOW/SIGDIGdocs verified. guideauditpasses311map,457lessons.
Manualcode-reviewperformed; findingsrecorded, main/runtime/APIgapsremain.
All19comparisonreportsperbuildreadnoerrors. Lastguideaudit8815links2634anchors,
457hashes/lessons,311counterparts/146absent,0errors afterfinaldocwrites.
Manualreview/whitespacecheckscompleted(newcpptreeuntracked,sogitdiffaloneweak).
No needrepeatpassingregressionsunlessnewfailure; latestBITCOUNTfullbuildcoversall.

ExploratoryONLY, NOTimplemented: rereadtxnsign,txncoinlock,txnaddamountfull;
newreadfullbackup/vault.java, fullnativePasswordCrypto.hpp/.cpp andWallet.hpp/.cpp,
UserDBencryptedseedget/set, MinimaDBsaveUserDBdeclarations/bodylines. Siblingsearch
no matchingvault/Mainmethods. Reusegateforvault/txnsignNOTyetcomplete (remaining
callers/Mainmethods/nativecrypto tests/sourcehelpers). Mainmethodsbodiesnowread:
setHasShutDown477 sets mShuttingdown; restoreReady586.. sets mRestoring and calls
shutdownGenProcs/shutdownFinalProcs (those bodiesNOTnewlyread); getAllKeysCreated840
returns mInitKeysCreated, getAllDefaultKeysSize844 delegatesWallet. Alsofull utility_
differential.py read (existingPasswordCrypto crossdecrypt cases andmalformedtails).
Do not claimvault/signorcoinselectioncommandsported. SourceBITGET/BITSETtestmapping
stillabsent. Continueactualfullport; goalmuststayactive.

--- Earlier checkpoint follows ---

# Latest — txncheck and source INC/DEC tests VERIFIED; map 305

Goal ACTIVE, incomplete. This continuing turn made concrete PROGRESS. No blocker.
No commits, pushes or agents spawned. All457 teaching lessons remain authored.
Current map305 =272production+33tests;152absent =97production+55tests.
148 CTest registrations. All counterpart statuses partial/parity not_verified.
Main/Minima startup and native H2/MVStore still absent.

No live build/test/tool sessions remain. All current checks terminal PASS.
Final full builds5840/22036 PASS with no warning/error lines; logs
/tmp/minima-txn-check-final-build.log and-final-san-build.log.
21regressions eachbuild: normal26664 PASS353.96s, sanitizer75802 PASS898.47s.
Newcommand comparison396programs1049ops +4INC/DEC entries bring this to26selected
checks perbuild, not the full148 suite. All42comparisonreports for this checkpoint
were read:18regressions+newcommand+2originaltests perbuild; no duplicate invocations count.
No current failure or blocker. GoalACTIVE; full native node is unfinished.

Production:
- NEW txncheck.hpp/.cpp, CMake and CommandRunner registration. Exact help/params,
  actual customDB lookup, first-seen tokens, amounts/counts/output JSON; actual
  generator/basic/signature/MMR/script checks. Final validtransaction intentionally
  excludes allsignaturesvalid, matching pinned source. Clears stored monotonic
  flag AFTER generation; uses shared transaction/witness owners.
- TxBody four transaction/witness fields now initialized shared_ptr owners.
  Sequential serializer and checked null errors. NEW in-place reader calls four
  existing readers preserving owners, then clears ID vector and reads IDs.
  Static deserializer delegates. Java held-list aliasing still not represented.
- TxPoW shared getters/setters for all4 owners; existing reference getters checked;
  value setters allocate owners. Native body/header container itself stillvalue.
- TxPoWGenerator shared overloads retain all supplied owners. Existing value
  overloads delegate through new owners. Missing tip now JavaNPE. Zero truncated
  integer hash rate now JavaArithmeticException("BigInteger divide by zero").
- MinimaDB.bindRuntime installs existing consensusServices for generator/checker.
- TxBlock missing token creator access now JavaNPE (source mode17 caught this).
- MiniData negative-BigInteger constructor error now exact source message,
  including "postitive" spelling. Algorithm unchanged.
- support/objects.cpp adjusted for pointer field.

Tests:
WalletCommandsOracle.java/native wallet_commands.cpp gain actual ConsensusOracle
fixtures and validationFixture/generate/held/body/bodyread operations. Capture
stored wire errors and all3 txn flags after every op. Body snapshots preserve
4owner identities, byteoffset, partialwire/errors, PRNG/difficulty/IDlist.
Actual generator/checker/TreeKey run; SQL remains recording, no Main constructor.
NEW transaction_check_corpus.py + driver --group check + CTest timeout600.
Source fixtures87 =29modes(no clearedbody26)*3scripts. Invalid signatures,
MMR/scripts/amounts, typed params, stale flags, hashrates including fractionalzero,
mutations/deletion, owner aliases/nulls/truncation/repeated reads.
PASS396programs1049ops0SQL normal41216 andsan34946(177.57s); both reports read.
Three retained production baselines in build/transaction-check-command-*baseline-results.json:
-token (bad_optional_access), -hashrate (zero divide), -negative-hashrate (message).
Java harness fixes: bodyBytes avoids outer bytes name; ConsensusOracle.wire already
has0x prefix, so no extra prefix. Incorrect witness:true test replaced with actual
clear command; asserts command success, retained identities and changed bytes.

Mapped INCTests.cpp andDECTests.cpp reuse ABSTests.cpp pattern. All11 active source
examples,4constructor+22value+4typed rejection assertions=30perruntimeperclass.
Variant retains integer overload for-1/0/1, string overload fordecimal inputs.
NEW INCOriginalTests.java/DECOriginalTests.java; existing original runner extended.
4CTest entries perbuild PASS final80862normal5.28s/44470san5.11s.
Earlier8.75s/9.03s runs preceded integer-overload correction; final reports read.
No production INC/DEC change. Both newtargets compile after CMakeconfigure.

Regression26664normal ALL21PASS353.96s. All18normal JSONreports read nofailures:
3802primitives,9464objects,2890consensus,1343checkerlogs,105timederrors,
1558txblockowner6219ops,357miner1352ops,384processor1434ops,
1129txncommands6904ops,671inputs2734ops41653SQL,675wallet1907ops50275SQL,
134addresses276ops3693SQL,418witness1128ops8637SQL,
2367MinimaDB7519ops,324chaincommands4096ops,1265runner,1120runnererrors,
189searcher. Native shareddeadline/minerworker andmapping alsoPASS.
Sanitizer75802 ALL21PASS898.47s; all18JSONreports inspected match normal counts.
MinimaDB334711drivercalls andchaincommands0drivercalls eachbuild. Source mapping,
minerworker andshareddeadline are the3non-report checks perbuild.
Previous final production fullbuilds36617/47906 passed no warnings/errors;
final fullbuilds5840/22036 include mapped INC/DEC targets and PASS.

Reuse/review:
Inspected full pinned txncheck/TxBody/TxBlock, generator generatebody, relevant
TxPoW fields/accessors, actual MinimaDBcallbacks/TxnRow/TxnBlockowners,
checker/signatures, walletfixture/harness/driver and complete ConsensusOracle.
Sibling TxBodyvaluefields/differentwireAPI unsuitable for Java identity; retained
existing TxnRow/TxBlockshared pattern. Read MiniData constructors and BigIntdivide
before error fixes. Read complete INC/DEC Java tests/functions, current matching
functions/headers/Contract paramcheck/registry, ABS native+Java wrapper+driver,
sibling test_kissvm runScript and INC/DEC block, Functions requireArgs/numArg/INC/DEC.
Sibling onlytwointegerexamples/differentAPI; reuse currentABS adapter instead.
Exact paths/reuse reported before coding. Manual code-review skill used; changed
production/test code reviewed. Corrected integeroverload test detail duringreview.
Exploratory source reads only, NOT implemented: txnsign,txnpost,txnmine,txncoinlock,
txnaddamount andItemList (lasttwo fullcurrentturn). Don't claim those ported.

Teaching/docs:
New labs transaction-check.md (tenth) andincrement-decrement.md (eleventh), both
created before initialtests. Affected lessons updated: txncheck,TxBody,TxPoW,
Generator,MinimaDB,TxBlock,MiniData,INC/DEC+originaltestlessons; README/navigation,
source-map/provenance/REVIEW updated. Lastguideaudit8774links2634anchors457hashes,
305counterparts/152absent/369productionlessons88testlessons, noerrors.
REVIEW/provenance/labs/mainREADME updated with verified final results and limits.
Continue actualport work; never mark goal complete on counts or component tests.

--- Earlier checkpoint follows ---

# Latest — transaction input and utility construction VERIFIED; map 302

Goal ACTIVE and incomplete. This goal turn made concrete PROGRESS: three mapped
production files, shared syntax repair, meaningful differential checks and lessons.
No live build/test/tool sessions. No commits, pushes or new agents. No blocker.
302 counterparts = 271 production + 31 mapped tests; 155 absent = 98 production +
57 tests. 143 CTest registrations. All existing counterparts remain partial with
parity not_verified. Main/Minima startup and native H2/MVStore remain absent.

## Production changes

New .hpp/.cpp under src/org/minima/system/commands/txn: txninput, txnbasics,
txnutils. CMake includes all; CommandRunner registers the first two. Exact source
help/fullhelp/valid params/factories. txnutils is a helper, not a Command.

txninput uses actual MinimaDB custom TxnDB lookup. Preserves ID lookup before
floating parsing; precedence coinid > coindata > address/amount/token fields.
Search returns a deep copy (important corrected teaching note); optional floating
changes that copy. Field construction always uses ELTOO even with floating:false.
Reuses token scaling, Coin converter, precomputeTransactionCoinID, calculate ID.
Input insertion and ID updates precede scriptmmr parsing/proof construction, so
later exceptions retain mutations. Exact missing coin/token/import errors.

txnbasics calls actual utility permissively, updates output IDs/transaction ID,
returns actual row. Utility implements both proof/script overloads and linked
burn construction. Strict transaction helper resolves inputs with searcher,
keeps original input list; single-coin helper does not resolve. Select exact
ancestor using MiniNumber difference capped at proof history. Proof appended
before script lookup; single-coin always queries script and uses CommandException
for missing script, transaction overload checks existing scripts and uses generic
checked Exception. Repeated proofs append; scripts use existing witness dedup.

Burn helper resolves TxPoWDB, getMiner callback, Wallet, tip in source order.
Actual relevant/simple/unspent search; caller/miner/mempool exclusions; ordered
selection; proof/script attachment; unique public keys; late amount check;
change with storeState:false; link; IDs; optional actual wallet signature.
getMiner is explicit new static std::function returning shared TxPoWMiner, with
unbound default throwing null Main receiver. Future Main must bind this before
workers. It is not a fake successful miner or full Main implementation.
Native helper references still lack complete nullable-argument API parity.

Shared MiniNumber syntax repair: src/support/JavaBigDecimalSyntax.hpp replaces
constructor's generic regex/digit validation. Reads BMP digits using existing
ICU; source-ordered errors for compact <=18 UTF16-unit versus long mantissas,
repeated dots, missing digits, invalid exponent character/length. Empty/missing
exponent yields JavaTypedException with source no-message NumberFormatException.
Existing mantissa/exponent conversion, arithmetic, rounding, limits unchanged.
Extreme exponent overflow/precision/scale remains a known review gap.

## Source and reuse gate

Read complete pinned txninput, txnbasics, txnutils; also inspected txncoinlock
and txnminepost (NOT implemented). Reused current Command/CommandRunner,
txnoutput/txnmmr/txnscript, actual MinimaDB, TxnDB, Coin/CoinProof, Transaction,
Witness/ScriptProof, searcher, generator, wallet script/default/sign methods,
miner construction/membership methods, TxPoWDB/RamDB membership and RamData.
Current Java/native wallet-command harnesses, driver and witness corpus read.
Searched sibling tools/minima-core-cpp src/tests for the new commands/utility
methods: no matching implementation. Read sibling MiniNumber interface and
current numeric parser. Reports given before coding.
After numeric mismatch read installed OpenJDK26 src.zip
java.base/java/math/BigDecimal.java:538-775 + MAX_COMPACT_DIGITS=18, and pinned
MiniNumber(String)/checkLimits. No new library, no web research, no alternate DB.

## Test harness and evidence

Extended existing support/wallet_commands.cpp and WalletCommandsOracle.java;
added tools/transaction_input_corpus.py and --group inputs to existing driver.
New CTest transaction_input_commands_differential, timeout600.
Native fixture now seeds actual MinimaDB.mTxnDB via WalletCommandsFixture friend
in MinimaDB.hpp, matching existing Java reflection. Removes separate runtimeLookup
override; existing MinimaDB binding resolves actual owner. Other wallet groups
regression-tested after this change. Loads chain coins/relevance as well as MMR
bytes. Adds utilities/miner/pool/burn operations. Native dormant miner uses actual
protected constructor without launching worker and actual mineTxPoWAsync to fill
membership. Java Main/miner allocated via Unsafe, mMiningCoins set; actual getters
and membership check run, no constructor/worker. Pool uses real RamData and actual
TxPoWDB complete mempool; both witness types and cascade/onchain flags tested.
Source-generated token fixtures reused. Main/SQL are fixture boundaries only.

First run 64462 FAILED at case341 (342 evaluated of609 planned,1429 matched ops,
20966SQLcalls): amount:bad Java character-specific NumberFormatException versus
native generic MiniNumber: invalid number. Retained
build/transaction-input-commands-syntax-baseline-results.json.
After shared syntax repair 16322 PASS all609 programs,2500ops,39917SQLcalls.
Expanded final suite =671 programs,2734ops,41653SQLcalls both builds; reports read.
Cases: metadata/runner gates; input selection/precedence/spent/floating/token;
wrong JSON types/truncated Coin bytes; proof histories/fractional/empty/truncated
chains; strict/permissive/repeated calls; SQL/runtime injection; ordered burn
exclusions/mining/mempool/signing failures and successes; decimal syntax.
Explicit success assertions ensure burn input/output/proof/signature counts and
link hash, and that seeded pool TxPoWs really are transactions. Full custom DB
wire and monotonic flags captured after every operation. NPE/classcast expression
text and logger prefix/stack frames normalized as existing harness; other errors,
replies, wire/state, SQL calls and logs exact. Random change address single-row
only; no exhaustive aliasing/null/concurrent/durable-storage proof.

## Final checks (all sessions terminal)

Initial wallet build16453 PASS; syntax build23880 PASS.
Both final full builds41640 normal/40217SAN PASS, no compiler warning/errorlines.
New final CTest94638 normal PASS38.68s;14123SAN PASS207.73s (overlapped regressions).
13 regressions92274normal PASS101.73s;20106SAN PASS276.80s,14unique checks perbuild
includingnewtest, notfull143suite. All24regression JSONreports read zero failures:
3802primitives,9464objects,1129transaction programs6904ops,189searcher,
675wallet programs1907ops50275SQLcalls,134address276ops3693SQLcalls,
418witness1128ops8637SQLcalls,12289command,1265runner,1120commanderrors,
357miner1352ops,2367MinimaDB7519ops334711drivercalls. Source mapping also PASS.
No unresolved build/test failure. ps read was sandbox-denied; tool live session
handles were polled directly until terminal. No escalation or permission needed.

Logs /tmp/minima-txn-input-{build,syntax-build,full-build,sanitized-build,test,
test2,final-test,sanitized-test,regressions,sanitized-regressions}.log.
New reports build{,-sanitized}/transaction-input-commands-results.json.

## Teaching/review and next work

Ninth lab student-guide/new-tests/transaction-inputs.md explains logic, failure
mutations, burn selection/signing, numeric mismatch and repaired diagnostics.
Updated three command/helper lessons and MiniNumber lesson, course/index/audit
navigation, README, provenance, current REVIEW. Corrected old txninput lesson's
false suggestion that searchCoin returns shared chain coin: it deep-copies.
All457 guides authored. Final audit is saved in student-guide/coverage-audit.json;
last audit8737links2634anchors zero errors before final result-link additions.

Continue toward full goal: Main/Minima startup and actual native SQL persistence,
98 missing production and57 missing test counterparts, complete API/identity
parity. Do not claim100%. Existing tests provide scoped evidence only. Native
getMiner callback needs real Main binding; nullable helper arguments and numeric
extreme exponent conversion remain explicit gaps. Previous checkpoint gaps below
still apply unless superseded here. No live work to wait on or restart.

---

# Latest — chain inspection commands VERIFIED; map 299

Goal ACTIVE. Previous and current goal turns made concrete PROGRESS.
No live build/test/tool sessions. No commit or push. No new agents spawned.
Map299 =268production+31mappedtests;158absent=101production+57tests.
142CTest registrations; ten selected checks pass in each build this checkpoint.
All457teachingpages authored; audit8718links2634anchors457hash/statuschecks,
zero errors. All counterparts stillpartial and paritynot_verified.
Full native Main/Minima startup and H2/MVStore backend remain absent.

## Implemented

New source-mapped base/block.hpp/cpp, printmmr.hpp/cpp, printtree.hpp/cpp.
CMake includes all three; CommandRunner includes and registers prototypes.
Exact help/fullhelp and fresh factory retained. Direct actual MinimaDB singleton
lookup uses existing checked native pointer dereference, not a captured fixtureDB.
block: absenttip CommandException("NO Blocks yet.."), block/hash/timemilli/date
projection with string numbers and existing javaDateString.
printmmr: actualtipMMR.toJSON; absenttip remainsnull-pointercategory.
printtree: existing javaParseInt(trimmeddepth/default32), exactcascade"true",
cascade rendering BEFORE tree; leadingnewlines and inherited/validparamrules.

TxPowTree.cpp: depth-1 now wraps in32bits using bit_cast<int32_t>(uint32_t(depth)-1u).
Oldcode widenedto64 before subtraction, wrong atINT_MIN. Loopstructure otherwise
unchanged; extreme populatedtree stillbillionsofparentchecks asJava, notrun.
TxHeader.cpp: removed its private localtime_r/strftime dateString; toJSONnowcalls
existing support/JavaDateString.hpp helper UNCHANGED. No newdatealgorithm/library.

## Reuse/source review

FULLJava3commands andnativeCommand/header/renderer implementations read. Native
MinimaDB.hpp/fullconstructor+getters/bindusage andexistingminima_db harness read;
source treeprintTree, cascadeprintCascade, MMRtoJSON, TxHeadertoJSON read incontext.
Existing JavaDateString/JavaCalendarFields/JavaInteger helpers readFULL.
Siblingsrc/chain/TxPowTree.hpp andchain/cascade/Cascade.hpp fullread: different
weights/diagnosticformat, no compatiblecommandwrapper. Existingnativealgorithmsreused.
Java/nativeMinimaDBharnesses anddriver readFULL beforeextending; sameCommandException
mapping as existingwallet_commands harness reused. Reusereportsdeliveredbeforeedits.

## Tests and baselines

Extended support/MinimaDBOracle.java andsupport/minima_db.cpp. Addcommandoperation
withdirect/meta/runner modes. Metaaddsmarkerparambeforefactoryfreshreply. Added
forkedchainparentslist, actualtipMMRtarget, tipTime. Olddefaultchainbehaviorretained.
Nativefixturelogger.println sendsactualbodytostdclog for existingcapture; mirrors
Java prefix/framefiltering. DirectCommandException mappedtocanonicalJavaclassname.
Existing tools/minima_db_differential.py has --group chain-commands. ReusesSQL/Main
recorders, realDBchildclasses, state/identity/lock/filesnapshots andnormalizer.
Originaldatabasegroup stillUTC, samecases. Newgroup runs4freshprocesspairsusing
['env','TZ='+zone] tooverrideRunner'sUTCdefault withoutchangingRunner.

Missing-registry failfastbaseline52653 executesfirstmetadataprogram, fails as
nativecommandlookupnull vsJavaactualmetadata. Retainedbuild/chain-commands-baseline-results.json.
InitialUTC78programs1002ops PASS normal66500=4.16s/SAN20007=6.92s.
First4zone312plannedrun failsLondoncase2 at81evaluatedprograms: cascade1970Jan1
Java01:00GMT vsnative01:00BST. Retainedbuild/chain-commands-zones-baseline-results.json.
InvestigatedinstalledOpenJDKsrc.zip Date.toString1028 andZoneInfo/ZoneInfoFile
transition/display paths. Directactualblockprobe showedsharedjavaDateStringalready
matchesJavaGMT. OlderTxHeaderstrftimehelpercausedmismatch; replacedbysharedhelper.
No changes to JavaDateString or JavaCalendarFields. No newdependency.
Expandedtimecases include fractional1.9,-1.9,1709164800123.75 andprinttreeafter
alltimestampchanges toexerciseheaderdatepath aswellasblock'sdirectdatepath.

Final81programsperzone *4zones =324programs4096operations,0commandSQLdrivercalls.
ZonesUTC,Europe/London,America/New_York,Asia/Tokyo. Comparemetadata/factoryfreshness,
empty/single/linear/forkedchains,boundeddepths,parsedextremesonemptytree,exactflags,
directandrunnererrors,0/1/2/3/7/16/33MMRrecords,dateslonglimits/cutover/fractions,
retainedoldDBafternewsingleton, nullcascade. ActualMaincallbacksrecorded; real
Mainconstructor/workersnotstarted. EnhancedNPE/classcasttext+loggerprefix/frames
normalized; otherJSONtypes,strings,exceptions,loggerbodies/effectscompared.

## Final validation

Both finalfullbuilds28564normal/85659SAN PASS, nocompilerwarning/errorlines.
Finalnewtest38785normalPASS9.69s and97730SANPASS20.22s; reportsbothreadzeroerrors.
Nine regressions52845normalPASS50.23s,23593SANPASS177.75s. All14JSONreportsread:
9464objects,1027tree,2892cascade,2367MinimaDBprograms7519ops334711SQLcalls,
12289command,1265runner,1120commanderrors perbuild. TxHeaderTests/mappingalsoPASS.
Tenuniquechecksperbuild total includingnewtest; notfull142suite.
Source map check/auditpass299/158. gitdiffchecknotstrongbecauseallcppuntracked.
Only initialbaselineharness compilefailedauto& binding getRunner temporary;
fixedtoauto, rebuiltPASSbeforebaseline. No unresolvedbuild/testfailures.

Logs /tmp/minima-chain-commands-{baseline,baseline-build,build,final-build,
date-build,test,zones,final-test,regression}.log andsanitizedvariants.
Most recent fullbuildlogs date-build andsanitized-date-build; latesttestlogs
final-test/sanitized-final-test; regression/sanitized-regression.

## Teaching/review

New eighthlab student-guide/new-tests/chain-commands.md withemptytiptable,
JSONtypes/whitespace, singletonreplacement, recordedreceiverlimits, bothbaselines,
fourzone/datehelperreuse correction, depthwrap andtestlimits. Updated3command
lessons, TxHeader,TxPowTree, course/index/audittool, README, provenance, REVIEW.
Finalaudit8718links2634anchorszeroerrors; sourcehashes/statusesall457checked.
REVIEW approvesSCOPEDwork; fullnode/paritystillincomplete.

## Next work

Continue toward actualnative storage execution andfull startup, plusremaining
101production/57testfiles. Re-readexisting SqlConnection/MinimaDB and bundledH2
source/dependency evidence before choosing an implementation; currentrecording
receivers do not provide a persistence engine. Do not redefinefullnativegoal.
EarlierStateVariable/CoinProof/valueidentitygaps remainlistedbelow.
Additional renderer fidelity issue observed butNOTchanged: TxPowTree.printTree
andCascadeNode.toString fetchdatebycallingwholeTxHeader.toJSON; Javajustformats
time. Malformedunrelatedheaderfields couldcauseextra nativeexceptions. A future
experiment should mutateheaderfields andcompareresultbeforedecouplingthiscall.
Extremepopulatedtree depths intentionallynotrun; no completeparityclaim.

Earlier completed checkpoint follows.

# Latest — readers, MiniStringTests and automine VERIFIED; map296

Goal ACTIVE. Current goal turn PROGRESS. Full native node incomplete.
No live build/test/tool sessions. No commits or pushes. Documentation agents
completed earlier; no agents spawned this turn. All457teachingpagesauthored.
Map296 =265production+31mappedtests;161absent=104prod+57tests.
141CTestregistrations; all29selectedchecks passnormal+SAN after one normal
searcherharness correction. All counterparts stillpartial, paritynot_verified.
Final guideaudit8705links2634anchors457hash/statuschecks,zeroerrors.
source_map --check passes296/161. Gitdiffcheckpassesbutfilesuntracked.

## Coin/proof production correction

Coin.hpp/cpp: shared state-list bridge getSharedState/setSharedState; null list
represented. Legacy vector setState stillcopies. Nullable deepCopy Coin handle
withisNull and JavaNPE field guard. deepCopy I/O errors now actualMinimaLogger+
nullhandle; converteractualMinimaLogger+nullopt. Othererror/loggerfailureescapes.
In-place readerassigns completedfieldsinJavaorder, replacesstatelistbeforecount,
appendscompletestates,resetstokenbeforepresenceflag. Nullreceiverguardbeforeinput.
CoinProof.hpp/cpp: sharedwrapperData; sharedCoinandouterMMRProof handles. Inplace
readerassignscompletedCointhencompletedproof; retainedoldchildrenremainalive.
Converter catchesONLYwireIOException, actuallogs+nullopt.
MMRProof.hpp/cpp: sharedouterData; inplaceblocktime,clearchainBEFOREcount,append
completedchunks. Existingwire/hashalgorithmsretained. ChunksandMMRDatastillvalues.
StateVariable.cpp: readererrormessageincludesactualinvalidtype;writerUNCHANGED.
TxPoWSearcher.cpp: searchCoin/getFloatingCoin mapnullcopiedCoin toemptyoptional.

ReusegateFULL sourceCoin/CoinProof/MMRProof/StateVariable, nativemappedclasses,
ByteIO/JavaIOExceptionText/NativeThrowable/logger, ownershipcallerpaths read.
JavaSearcher source relevantsearch/deepCopyread; nativeSearcherfullread.
Sibling tools/minima-core-cpp/src/objects Coin/CoinProof andmmr/MMRProoffullread:
incompatibleownership,spent/token/prooflayout. StateVariablesiblingtypecodes
incompatible. Existingcodecs/ownership/loggerreused; reportdeliveredbeforeedits.

## Reader experiment and evidence

New support/CoinStreamsOracle.java,coin_streams.cpp,tools/coin_streams_differential.py.
ActualJava4coins4wrappedproofs3MMRproofs; everyprefixzero/full, converters,trailing,
12seededone-bitmutations/fixture, explicitbadlengths. Compare results/logbodies/
consumedoffsets/eightretainedobjects+oldstatelist,thenmutateheldowners/wrapper.
Copy/rename/floating combineoversizedIDs,nullstates,throwingloggerRuntime/Error.
Final3045programs PASSboth normal23.31s/SAN66.03s.
Baseline3029programs2716differences retained build/coin-streams-baseline-results.json.
Intermediate12diffs retained build/coin-streams-reader-baseline-results.json:
4invalidStateVariablediagnostic +8harnesssnapshotorder. SourceObjectOracle computes
JSONBEFOREwire; nativeharnessfixedsequencing, notproduction. Floatingadds16cases.
No JavaNPEexpression/stackframe/prefix comparison; nestedidentity/fullnodeexcluded.

Normalbroad99820:20/21pass242.66s. Searcherharness551stdoutlinesvs189expected:
newactualconverterloggermixedwithJSON. JavaSearcherOraclealreadyredirectsdiagnostics
tostderr. Native support/searcher.cpp now getRuntime().println ->stderr, retaining
actualloggerbehavior. Targetbuildsbothpass. Normalrerun32368PASS189programs6468queries
9.58s. SANbroad57305PASS20/20 in711.13s, includescorrectedsearcher;ownershipexcluded.
FinalSANsequential69968PASS4/4 in171.05s: mapping0.22,command19.70,
ownership146.76,error4.36. Ownership1558programs6219ops, unchanged240limit.
No SAN timeout this turn. All36 broadcomparisonreportsread, zero differences.
Perbuild9464objects,2890consensus,1027tree,2892cascade,315ram,135hashmap,2503txndb,
1057IBD,3600MMRsync,1129txncommands6904ops,189search6468queries,1350CoinDB4318ops,
357miner1352ops,384processor1434ops,1343checkerlogs,418witness1128ops,4471KISSVM,
1558ownership6219ops. TransactionTests/TxHeaderTests/mappingalsoPASS.

## MiniStringTests mapped; production unchanged

ReadFULL sourceJavaMiniStringTests+MiniString, nativeMiniStringhpp/cpp,
siblingtypes/MiniStringhpp/cpp, UTF8Spans, primitive/originaltestdriver+FLOORprecedent.
Reusedcurrentproductionunchanged; siblingunboundeddecoder/UTF8incompatible.
Mapped cpp/test/org/minima/objects/base/MiniStringTests.cpp retains2annotated
methods5successpathnonnullassertions andunannotatedhelper1assertion separately.
StaticReadFromStreamresultdiscardedinsource;preservedweakcheck. Nativevalue-return
nonnullchecks materializeownedresults, no contentequalityclaim.
JavaMiniStringOriginalTests checksannotationmembershipthendirectcalls unchanged
methods(existingconvention);noJUnitCore (Hamcrestnotinexistingjars).
Shared script_value_original_tests.py extendedclass/sourcepath/helperreport.
BothbuildsPASS; fourchecksnormal8.63s/SAN10.72s:MiniStringnative+originalJava,
FLOORsharedrunnerbranch30assertions, primitives3802comparisons. All6reportsread.
Lessonexplainsannotationmembership, weakchecks, discardedread, absenterrorcoverage.

## automine mapped and registered

FULL Javaautominebody: allminingbranchesCOMMENTED. ActivegetJSONReply,
getParam(enable,empty),returnreply;freshfactory;inheritedvalidparamsempty.
ReadnativeCommandhpp/cpp,CommandRunnerfull,randomprecedent,Java getParam and
source registry; bothcommandrunnerharnesses+driverfull. Siblingsrcsearchnomatching
commandimplementation. Reusereportdelivered. Addedautominehpp/cpp, CMake+registry.
No miningbehaviorinvented. Existingharnessdirect/metaoperations+driver32programs:
13directparams,1metadata/factoryreply,18runner=9strings*2callerIDs.
Baseline75010:1233oldpass+32newfail,1265total. Retained build/automine-baseline-results.json.
Correctednormal64781PASS2.88s/SAN87848PASS3.65s,1265/1265both. Normaladditional
command12289/error1120/mapping31381PASS3/3 in8.72s; SANsame+ownershipfinalabove.
DirectmalformedparametererrorsnormalizedERROR, exactexceptiontextnotcertified.
Both finalfullbuilds9051normal/60512SANPASS, no compilerwarning/errorlines.

## Teaching and review

Seventhlab student-guide/new-tests/coin-streams.md. UpdatedCoin/CoinProof/MMRProof/
StateVariable/Searcher/MiniStringTests/automine lessons, earlierownershiplab,
course/index/audittool, README,provenance,REVIEW. Audit8705links2634anchorsPASS.
REVIEW approvesSCOPEDchanges; MAJOR fullparitygaps andMINOR StateVariablegapremain.
All29selecteduniquetestsperbuild verifiedfromCTestlogs withnounresolvedfailure.

Logs /tmp/minima-coin-streams-{final,regression,searcher-rerun}.log;
/tmp/minima-coin-streams-sanitized-{final,regression}.log;
/tmp/minima-ministring-{tests,build}.log andsanitizedvariants;
/tmp/minima-automine-{baseline,build,test,regression}.log andsanitizedbuild/test;
/tmp/minima-readers-ministring-automine-{full-build,sanitized-full-build}.log;
/tmp/minima-readers-ownership-command-sanitized-final.log.

## Next full-goal work

StateVariable sourcewriter hasNOinvalid-typeelse(nativecurrentlythrows);
sourceinplacereader exposesrejectedtype/partialstate(nativeAPIabsent). Needs
retainedStateVariable-reader/writerexperiment; source/siblingfullreadthisturn.
Nestedproofchunks/MMRData, primitivefields/tokens, transaction-to-outputstatelist
sharing remainincomplete. CoinTests stillabsent: amount.equals(two) isJavaidentity,
notnumericcompare; MiniNumberhasnoequals override. Do notportbyweakeningassertion.
Main/Minima startup and nativeH2/MVStoreabsent; other104production+57tests unported.
Candidate commands block.java60lines andprintmmr.java46lines FULLreadthisturn,
notported/reusegatenotfinished. blockusesDBtip,time/date;printmmrusesDBtip/MMRJSON.
Continue source-first implementation and teacherlessons; no100percentclaim.

Earlier completed checkpoint follows.

# Latest — Coin shared ownership VERIFIED; mapping remains 294

Goal ACTIVE / full native node incomplete. Previous and current turns PROGRESS.
No live tool/build/test processes. No commits or pushes. Documentation agents all
completed (confirmed list_agents); no new agents spawned.
Map unchanged 294 = 264 production + 30 mapped tests; 163 absent =105+58.
138 CTest cases. All counterparts still partial; full parity not_verified.
All 457 teaching pages authored. Final audit 8,685 links /2,634 anchors /457 hashes
and statuses, zero errors. source_map --check passes294/163.

## Production correction

Only Coin.hpp/Coin.cpp production edits: move all existing fields into nested
Data owned by shared_ptr. Ordinary Coin copies now share the same field storage;
constructors and static deserialise allocate independently. Existing wire, JSON,
hashing, getSameCoinWithCoinID and wire-based deepCopy algorithms retained.
Transaction vectors/CoinProof/Witness automatically keep the same Coin handle.
No borrowed pointers into owning vectors introduced. Retain Coin by value to keep
a Java-like object reference; a Coin& to vector element still invalidates normally.
A moved-from native handle can have null storage and must not be used (ordinary
C++ move convention). Null Java Coin/field graphs not added in this milestone.

Reuse gate read FULL Java/native Coin, CoinProof, native Transaction/Witness,
TxBlock, TxPoWTreeNode, BlockMMR, MegaMMR, TxPoWSearcher; Java Transaction add/get,
TxBlock.calculateCoins, tree/MegaMMR deep-copy transitions. Searched all native
Coin setters/callers. precomputeTransactionCoinID already copies the base MiniData
by value before mutating outputs, preserving its source captured base ID.
Explicit BlockMMR and searcher deepCopy boundaries remain unchanged.
Sibling /Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/objects/
Coin.hpp/Coin.cpp/CoinProof.hpp fully read: value ownership and incompatible spent/
token codecs, not substituted. Reuse report delivered before any edits.
Existing ObjectOracle.java, objects.cpp, CoinFixture.hpp, object_differential.py
read; native TxBlock shared ownership used as smallest ownership precedent.

## Tests and evidence

Extended existing ObjectOracle + objects.cpp with coinalias method/dispatch.
Driver adds220cases =5coin indexes(0,1,4,7,31)*4growth(0,1,17,257)*11targets.
Owners: original,local alias,held input,removed input,remaining duplicate input,
output,CoinProof,witness coin,deep copy,renamed deep copy,wire-decoded proof coin.
Growth/removal precede selecting short-lived mutation pointer. Setters mutateID,
tokenID,spent/store flags,created,MMRentry2^100,token,state. Snapshot12objects via
wire+JSON including MMRleaf. Clear txn/witness, retained handle resetsstate/token,
rebindlocalalias; second snapshot7objects. Java must produce38fields, notERROR.
No claims for MMRProof alias, cross-coin state-list identity or failed inplace reads.

Baseline new tests: full native target build93083 passes. Differential17665 fails
expected9244/9464; all220newcases fail. Retained build/coin-alias-baseline-results.json.
First coinalias0/0/0: Java original/alias/proofcoin ID allAABB; native originalAABB
but alias/proof01. Both explicitdeepcopy IDs01. No preexisting9244regressionfailures.

Full corrected builds normal85683/SAN73336 pass, no compiler warning/error lines.
Normal83266:21/21 selectedPASS303.51s.
SAN84171:20/21 PASS703.82s, ownership timeout240.03s; no sanitizer fault reported.
Sandbox denied ps during timeout cleanup; first manual ps denied, escalated approved
ps inspected test-process rows: no orphan ownership child. Isolated escalated CTest
5017 same240limit/corpus passes1558programs6219ops in141.60s (total141.61s).
Both concurrency and permission changed, so do NOT assert sole cause of timeout.
No CMake/test timeout changes. All36JSONreports read after final completion.

Per runtime final reports:
9464objects incl220aliases;2890consensus;1027tree;2892cascade;315ram;135hashmap;
2503TxnDB;189searcher;4471KISSVM;1129txncommands6904ops;418witness1128ops;
1350CoinDB4318ops;1558blockownership6219ops;357miner1352ops;384processor1434ops;
1343checkerlogs;3600MMRsync;1057IBD. TransactionTests/TxHeaderTests/source_mapping
also pass. Only21selected of138 tests ran this milestone, not full suite.
Logs /tmp/minima-coin-alias-{build,regression}.log and
/tmp/minima-coin-alias-sanitized-{build,regression,ownership-rerun}.log.
Baseline logs /tmp/minima-coin-alias-baseline{,-build}.log.

## Teaching and review

New sixth lab student-guide/new-tests/coin-ownership.md: actual failing ID table,
shared ownership diagram, exact corpus/math, preserved deepCopy, field38guard,
normal+SAN evidence including timeout/rerun and limitations. Updated Coin,
CoinProof,Transaction,Witness,txnmmr lessons and earlier witness lab; course/index
links. Updated README/provenance coin_ownership/REVIEW scoped approval with
remaining MAJOR full-parity gaps. Final guide audit8685links2634anchors457checksPASS.
Python finalize helper /tmp/minima_finalize_coin_ownership.py executed ONCE;
DO NOT rerun (would duplicate README/REVIEW entries). Native edits helper
/tmp/minima_add_coin_alias_tests.py also must NOT be rerun (would duplicate tests).

FULL Java test/org/minima/objects/CoinTests.java read this turn, NOT ported:
3getter assertions +2nonnull assertions =5 successful path;2IOExceptionfailure
assertions; commented resetID inactive. Static reader result discarded, original
nonnull checked. MiniData.equals delegates isEqual (source141read); MiniNumber
has NOequals override (rgsearched), so amount.equals(two) is identity-sensitive.
CoinTests lesson corrected to explain these weak checks and distinction. Need
faithful native in-place API and constructor value-object identity assessment
before porting this original test; do not replace identity with numeric equality
and call it equivalent. Its mapped cpp remains absent.

## Next full-goal work

Coin/CoinProof still need source in-place read semantics/partial mutation and
conversion logging/error fidelity. CoinProof converter currently catchesALL
std::exception silently, source catchesONLYIOException and actualMinimaLogger.
Coin converter catchesIOException callback but defaultsimpleclog; deepCopythrows
onIO vsJava logs+null. State-list sharing acrossdifferentCoins, retained primitive/
token objects, MMRProof value ownership and container identities remain open.
Before broader ownership changes inspect source/callers and add failing Java/native
experiments; current Coin handles establish shared field mutation only.
Main/Minima entry point and native H2/MVStore remain absent. No complete goal claim.

Earlier verified checkpoint follows.

# Latest — 294 counterparts; witness commands VERIFIED

Previous goal turn PROGRESS; current PROGRESS. Goal ACTIVE/fullnode incomplete.
Map294=264production+30tests;163absent=105production+58tests.138CTestcases.
NO LIVE processes. Final9suite normal7018PASS44.49s;SAN33131PASS182.20s.
Finalfullbuild56443normal/64846SAN PASS no warning/errorlines. Priorbuildmissing
GlobalParamsinclude fixed. All16reportsreadAFTERterminalsuccess:418witness1128ops8637SQL;
675wallet1907ops50275SQL;134address276ops3693SQL;12289command;1233runner;1120runnererrors;
1129txncommands6904ops;2503TxnDB both. SourceMappingPASS294/163.
Logs /tmp/minima-witness-commands{,-sanitized}-{build,regression}.log.
FinaldocsnowupdatedREADME/REVIEW/provenance/lab+3perfilepages/currentintro. Final
structuralguideaudit/source_map rerun PASS8671links2634lineanchors457hash/statuschecksZEROerrors.

New nativecommands txn/txnmmr,txnscript cpp+hpp completebodies+registry+CMake.
ActualruntimeTxnDB / MinimaDBchain / Wallet / MMR / ScriptProof / Witness reused.
txnmmr choosesmax(tip-history,allinputcreation), skipsexistingproofCOUNT(notidentity),
lazyhistoricalnodeNPE onlyifnewproofneeded. Sourceexistingproofsuntouched. txnscript
parsesid/object/auto beforelookup; auto queriesmissingscripts; manualinsertionorder,
emptyproofplainScriptProof/rawscriptwithoutcleaning. Partialscriptadditionspreserved.
LocaladdedScripts listretainedthoughnotreturned. TxnRowJSONmayprecomputeoutputIDs.

Productionfix Command.cppgetJSONObjectParam nowget_if+JavaClassCastException for
wrongnonnulltype; missing/null/default/sharedreference preserved. ARRAYgetterstill
bad_variant_access forwrongtype, notfixedthisturn. KnownCoinProofstoresCoinvalue;
Javaaliaspostconstructionmutations NOTpreserved orproved; clear innewlessons.
DoNOTmarkparityverified. FullMain/nativeH2/MVStorestillabsent.

ReusegateFULLread Java txnutils356,txninput190,txncheck221,txnbasics80 (allNOTported,
txnutilsburnneedsMainminer),txnmmr105,txnscript144. FULLnative/JavaScriptProof,
MMRProof andnativeWitness,JavaWitnessadd/getScript;TxnDB/TxnRow nativehpps/bodies,
Transaction/Coinhpps, nativeCommand.cppFULL, sourcegetJSONObjectgetter;MMRgetProofToPeak
both;nodegetPastNodeboth/nativefirst58+tree65;Walletscriptlookupboth,ScriptRowhppcpp;
MinimaDBconstructor/bindRuntime/services, TxPoWGeneratorfull, TxPoWMinerhpp. Native
existingtxnoutput/import/state, runscript fullread. SiblingWitnessscriptstruct/address
andaddScriptProof inspected (lacksdedupe,differenthashpattern), no siblingtxncommands.
ReuseReporttolduserexactpaths. MinimaDB constructorBOTHleaveTxnDBnull: notnativebug.

Harnessreuse WalletCommandsOracle.java/supportwallet_commands.cpp/driver:
third--groupwitness independentcompile/reportdirs; sourcesalsoSearcherOracle.java+
ArchiveCacheOracle.java (existingJavafixtureprooffullread). NewwitnessFixturesrequest
Java-provided12nodechain,coins,branches,script-addresscoins. Nativechainconstructs
actualTxPoWTreeNode/MMR,addChild/setRoot. opchain resetsactualtree+proofhistory;
op txn creates actualTransaction/Witness, optionalcoincreated/entry, existingproofs/
scripts. InitrealTxnDB(Javaexistingreflectionfieldsetup/nativeexistingruntimeLookup)
reusesTransactionCommandsOracle convention, NOnewprodMinimaDBAPI. Resetownerrebinds.
SnapshotfulltxnDBwire+monotonicflagsaftereachop (all3groups), SQL/log/cache/prefsasbefore.
InitialJavafixtureNPEuninitializedTxnDB thennativeUBSANsnapshotNPE: correctedby
reusefixtureinit; SANwallet-witness-commands-crash.json retainsnativecase0diagnostic.

Newtools/witness_commands_corpus.py418programs: proofcounts0..5/inputs0..4+repeated,
correct/wrong/duplicateexistingproofs,history-1/0/1/2/8/11/12/256/1.5,created-1/0/3/11/12/3.5,
missing/gappednodes,entry-1..99/2^64,manualraw/Unicode/empty/bad/prooftruncations,
walletchecked/runtimefailurepositions,missingscripts,duplicates,null/typedparams,
permissions/dispatchstop,castbeforeauto/lookup. Successguardsproof/scriptcounts.

Firstcompletedcomparison124of392firstfailureindex123: EOFexceptionlabelincorrect
inNATIVEHARNESS (sqlExceptionText); fixedtouseexistingnativeExceptionText, production
wireEOFalreadycorrect. Retainedcpp/build/wallet-witness-commands-io-baseline-results.json.
Second346of394firstfailureindex345: scripts:true CCEvsbad_variant_access; fixedshared
CommandgetJSONObject asabove, added24cast-beforeauto/missingrowcases=>418. Retained
wallet-witness-commands-object-baseline-results.json. Direct+dispatchEOFcases added.
Finalnew418PASSnormal15.16s/SAN31.70s; full9suitesPASSboth.
Allnew4cmdcpp/hpp+fullchangedJava/C++harness+Pythonshared/newcorpusreadback.

Studentnewfifthlab new-tests/witness-commands.md explainsbothbaselines+setupfailures,
positionalproofskips,rawscripts,partialmutation andscope. Updatedtxnmmr/txnscript/
Commandpages; README/indexnav/audittool/coursecounts294/163; cppREADMEcount+REVIEWcount.
Finalguideaudit8671links2634lineanchors457hash/statuschecksZEROerrors afterfinal.
FinalmilestoneappendedtoREVIEW/provenance/README. No agentsrunning orspawned.
No commits/pushes. gitdiffcheckcleanbutcppuntrackedsoactualreviewreadfiles.
FULLJavaCoinProof.java readatfinalreview: constructorretainscoin/proofrefs; convertercatchesonlyIOException+logs (nativeCoinProof catchesallstdexceptions silently), inplace read swapsobjects.
Next fullgoalworkcanaddressknownCoin/CoinProof reference ownership beforeextending
moretransactioncommands; don'tinventborrowedvectorpointersbecausegrowthinvalidates.
Earlier verifiedcheckpoint below.

# Latest — 292 counterparts; address commands and MIN/MAX original tests VERIFIED

Goal ACTIVE/incomplete. This turn PROGRESS, no blockers, no live build/test sessions.
Map292=262production+30testfiles;165absent=107production+58tests.137CTestcases.
Full Main/Minima startup and native H2/MVStore engine ABSENT. No full-parity claims.
All457teaching pages authored (369production+88test explanations); docs agents all
completed, verified with list_agents. No new agents spawned. No commits/pushes.

Address milestone: new base/newaddress,getaddress,coinnotify,slavenode .cpp/.hpp
added to registry/CMake. Actual MinimaDB/Wallet/UserDB owners reused. Source metadata
and factories retained. newaddress creates default-sized TreeKey before querying
key count before row JSON (null row count order retained). getaddress direct raw
createall presence takes empty branch; type read unused; dispatch rejects both
undeclared options. coinnotify address parsing before action validation; volatile
set only. slavenode exacttrue enables/needs host; disable ignores host; query saves.

Full sources4commands+txncoinlock read; latter NOTported, depends txnaddamount.
Shared relevant Wallet/getDefaultAddress/createNewSimpleAddress/createNewKey,
MinimaDB saveUserDB+notify, UserDB accessors, JsonDB/MiniFile, Command helpers,
JavaRandom48 and existing shared SQL fixture/oracles allread. Sibling searched4
commands no matches; sibling Wallet previouslyfullread incompatibleSQL/scripts/
single-usekeyscheme. No new fake servicefor production commands.

Shared WalletCommandsOracle.java/support wallet_commands.cpp/driver extended with
--group addresses and independent compilation/report directories. Actual temporary
userprefs.db bytes/currentpref/notification/cache snapshots each op; allowSave,
reloadPrefs,ownerrecreate operations. Not fullnode restart. File reset perprogram
ensures Java/native isolation. Existing675script/sign corpus unchanged inputs.
134addressprograms276ops3693SQLcalls PASS both. Explicit success guard verifies
newaddress status/simple/default/track/publickeylength/total1 (suppliedrowcount,
notdurability). Java-produced Mx200104JMFSAS and lowercase alias0x01 checked by
add/check/remove/check guards. Zero/one/identicaldup rows bound randomselection;
notdistinctrowdistribution. Checked/runtimeSQLfailpositions,locked/unloaded,
invalidparams,nonadmin,direct-vs-dispatch,disabledsavingandreload tested.
Initial126programPASS beforeaddingMx/unusedhost/explicitguards; final134PASS.

Full normalbuild38678/SAN5243 PASS no warning/errorlines.6selected normal87888
PASS28.41s;SAN53889 PASS156.12s. Finalreportsread both:134address276ops3693SQL;
675wallet1907ops50275SQL;1233runner;1120runnererrors;1130prefs. SourcemappingPASS.
Logs /tmp/minima-address-commands{,-sanitized}-{build,regression}.log.
All4commandguides updated; new-tests/address-commands.md fourthlab; README/index
navigation,provenance/REVIEW/courseupdated. Corrected initialdraft falseclaim of
intercepting UserDB setter/save/getter order (actualfixturesnapshotonly); no FS
failure injection. Full storage/restart stillmissing. Allcode/harnessbodyreadback.

MIN/MAX mappedtest milestone: two .cpp testcounterparts, supportMINOriginalTests.java
andMAXOriginalTests.java. FULLboth345lineJavasourcesread, native+JavaMIN/MAXbodies,
ConstantExpression, nativeNumberValue, relevantMiniNumbercompare/Contractinstruction
andMinimaFunctionvalidation/registry read. FULLFLOORTests.cppandsharedoriginaldriver
read. SiblingFunctions.cpp MIN/MAX onlytwoargs; siblingtestonly2argcases, notcompatible
withsource13/25argtests. ReuseexistingnativeMIN/MAX unchanged. No productionchanges.

Eachtest4constructor+2zero-result+6*10000*2boundaryresult+2count+15type=120023
successful assertions guardednativecounter. ValidcontractlimitINT32MAX retained;
800002constant evaluations for sourcevalidlists. Freshfunctioneachtrial; boundary
first/last/middle +int/long sourceorder. 15typeorderedpairs skiponlyNumber/Number;
ExecutionExceptionrequired; separateinvalidContract. Emptydomainblockstaysinactive.
OriginalJava3methods invokedunchanged, JUnit4.13bundle. Native test samples use
existingJavaRandom48.nextInt(65536), unsignedbitassembly2/4parts+bit_cast to32/64,
fresh deterministicseedpertrial. NOTidenticalunseededJavaRandominputs, NOTPRNGparity;
sameproperty/ranges/counts explicitlystatedinreports/lessons. No prodRNGchange.

Shared script_value_original_tests.py addsMIN/MAXchoices/packagepaths/assertcounts,
independentoracles/reports. Newclassprocesslimit180s, CTest400s; oldclasses30s unchanged.
No testiterationcuts. CMake137cases. Fullnormal10763/SAN69384 PASS; no compiler
warning/errorlines.12selectednormal50948PASS36.91s;SAN57987PASS69.38s. All18reports
read:MIN3methods120023assertions60000trials;MAXsame;FLOOR3/30,CEIL3/26,ABS3/23,
MinimaFunction2/11,Script1/10,Hex2/35,Number4/99+7supplemental bothbuilds. FinalSAN
MAXreportreadAFTERterminalsuccess. Logs/tmp/minima-extrema-tests{,-sanitized}-{build,regression}.log.
New2testCPP+2driverJava+sharedPythonscript readbackFULL. MIN/MAXproduction+test4lessons
updated detailedcounts/table/randomadaptation, provenance/REVIEW/README/currentintro.

Final structuralguideaudit/source_map rerun afterfinalstatusupdates PASS. Audit
8652links2634lineanchors,457hash/statuschecksZEROerrors292/165. Rootownsnew4base
commandpages+addresslab+MIN/MAX4pages. Existingfilesuntrackedcpp/,upstream-study/,
graphify-out/ plusDS_Store/.gitattributes; gitdiffcheckdoesnotinspectuntrackedwork.
Next continue107missingproduction+58missingtestfiles andfullAPI/runtime/H2work.
Earlier history below.

# Latest — 286 counterparts; four wallet commands VERIFIED

GoalACTIVE/incomplete, previous turn and this turn PROGRESS. Map286=258production
+28testfiles,171absent=111production+60tests.132CTestcases. NO LIVE builds/tests.
All457studentlessons stillauthored; newthird lab wallet-commands.md. Finalrootaudit
8621local links2634lineanchors457sourcehash/statuschecksZEROerrors. Source_mapcheckPASS.
No agentsrunning. Full Main/nativeH2/MVStoreabsent; neverclaimportcomplete.

Newmappedproduction (.cpp/.hpp):system/commands/scripts/scripts,newscript,
removescript andsignatures/sign. ActualMinimaDB.getDB().getWallet APIs used,
no newserviceplaceholder. NativeWallet SQLdriver stillabstract. Registered4commands
andCMake sources. Metadata/exacthelp/factory/validparams copiedpinnedsource;
removescripthelp starts carriage-return andscriptshelpendsliteraln preserved.
FULL4Javasourcesandnativefilesread. FULLnativeWallet,Wallet.hpp,SQLrecordingfixture,
SqlDBOracle,WalletOracle,Walletnativeharness,Walletdriver,SqlDBdriver/normalize,
Command.hpp/cpp+FULLJavaCommand,ScriptRow.hpp/cpp+JavaScriptRow read. MinimaDB
constructor/getWalletsource/native andactualmethods, ContractcleanScriptfull bothread.
SiblingWallet.hppfullread incompatiblemissingSQL/scripts andsingle-usekeyscheme;
noalternativereplacement. Reuse reportgivenbeforeimplementation.

3sharedcorrections drivenbynewcomparisons:
1) NativeThrowable.hpp addsSqlConnection.hpp andSqlException/SqlRuntimeException
text forwardingtoexisting sqlExceptionText; noSQLexceptionclasshierarchychange.
Baselinefirstfailureindex7 =>8/626programs:IllegalStateExceptionbecameRuntimeException
inreplyandlogger. Retainedcpp/build/wallet-commands-baseline-results.json.
Expandedexistingcommand_runner_errors harnessSQL/sqlruntime adds224=>1120cases.
Original450/896baselineincommandrunnerlab retainedashistorical; current1120zero.
2) Contract.cppcleanScript tokenandparse-errorlogs nowactualMinimaLogger; include
MinimaLogger.hpp. Baselinefirstfailureindex635=>636/637programs malformedquote
missinglog. ReturnoriginalinputonMinimaParseException unchanged. Retained
wallet-commands-cleaning-baseline-results.json.8directclean caseslogon/offadded.
3) Command.cppgetParam nowunicode::encodeUTF8(getParamUTF16(name)), reusingtyped
null/classcast/trimchecks. Baselinefirstfailureindex637=>638/667programs nullscript
bad_variant_access vsNPE. Retainedwallet-commands-parameter-baseline-results.json.
30null/nonstringdirectparamcases across6fieldsadded. UnpairedUTF16stillunverified.

Newfixture3files fullreadpostchanges:
support/WalletCommandsOracle.java,support/wallet_commands.cpp,
tools/wallet_commands_differential.py. ActualJava/nativecommands+registry+MinimaDB+
Wallet/ScriptRow/Address/Contract/TreeKey, onlySQLrecordingdriver. Capturesreply,
SQLcalls/logs,3cacheprobesincludingTRUE_ADDRESS,metadata. Failurepositions1..31,
checked/runtime; empty/duplicate/alteredrows,nullresultsets,unloadedwallet,permission/
invalidparams/batchgates,cleanflags,malformedaddresses,lockedseed,use0/1/3/4signatures.
SuppliedSQLrowsindependentofpriorwrites; NOTdurabilityproof. Synthetickeys only.
NPE/classcastsourceexpressionnormalization+loggerprefix/stackremoved;otherscompared.

Builds: initial4073failedfixturegetCommandOnlycalledstatic; fixedtogetRunner().
50240PASS; initialnewlogger52885failedmissinginclude; addedincludeandrebuild8419PASS
BEFORErunningtest. Both correctionsfollowedfullfinalbuild98817normal/68701SANPASS.
No warning/errorlines in final/tmp/minima-wallet-commands{,-sanitized}-build.log.
13selectednormal57375PASS36.37s;SAN56134PASS151.68s. BothlocalHTTPsuitesrunwith
approvedescalation/socketpermission (no finaltestfailure). Logs
/tmp/minima-wallet-commands{,-sanitized}-regression.log.
All20JSONreportsreadAFTERterminalsuccessbothbuilds:
walletcommands675programs1907ops50275SQLcalls;commanderrors1120;
runner1233;command12289;txncommands1129programs6904ops;HTTP2106requests;
notify527programs2667ops;VM4471;Wallet1433programs6521ops85452effects;logger2016.
Source_mapping,ContractTests,native_stack_tracealsoPASS.

Docsfinalupdated:4commandguides,Command,Contract,CommandRunner,oldcommanderrorlab,
newwalletcommandlab,how-to-study,README/INDEX/audittools+aggregate,cppREADME/REVIEW/
provenance. Labexplains3first-failurebaselines andSQLrecordinglimits. Fullport
remainsunverified; source mapallpartial/not_verified asbefore. No commits/pushes.

Next: continue111missingproduction+60missingtestfiles andfullAPI/runtime/H2work.
Canextendwallet-backedcommands usingactualWallet nowbounddirectly (newaddress,
getaddress etcneedsourcegate). Mainfullruntime/nativeH2stillmajorcompletiongap.
Earlierhistorybelow.

# Latest — 282 counterparts; all 457 teaching pages complete and audited

Goal ACTIVE, full native node incomplete. Map282=254production+28testfiles,
175absent=115production+60tests.131CTestcases. No live builds/tests.
Docs agent finished all369production+88testlessons, zero drafts. Root reran final
audit after last intro link:457unique guides,8594local links,2634lineanchors,
embedded source SHA/map/status checks allPASS, zero errors. Source_map--checkPASS.
Agent appended6missing root-testSHAonlyasauthorized; allagentsended.
Gitdiff--checkclean but entirecpp/untracked so not a full untracked-code review.
Root read audit_guides.py full inclnewhashcheck, README, how-to-study, Wallet and
TxPoWProcessor lessons for sampled review. Root introcountsupdated282/175.

CommandRunner milestone VERIFIED:
Read whole pinned645line runner, nativebody/headers/Command/missingcmd, oldoracles/
driver, actualLogger/nativeexceptionhelpers, CheckerLogs capture, TxPoWChecker
Error boundary. Teaching exposed actualgenericcatchgap. New896cases baseline450
mismatches. Fixgenericcatch actualMinimaLogger+NativeThrowable, nativeExceptionText
forIOException, explicitJavaError/bad_alloc rethrow. New3support/driverfiles:
CommandRunnerErrorsOracle.java,command_runner_errors.cpp,command_runner_errors_differential.py.
Baseline file cpp/build/command-runner-errors-baseline-results.json retained.
Fullbuilds46198normal/55385SANPASS.9selectedtests initial63221/56966eightPASS,
cmd_handler sandboxloopbackbindfailure. Requiredescalatedreruns98578/92882PASS
4.96s/5.52s. Therefore9individual selectedsuitesPASSeachbuild, NOTinitial9/9run.
ReportsreadAFTERcompletionboth:896new,1233runner,12289command,1129txnprograms6904ops,
2106HTTPrequests,527notify2667ops,2016logger; source_mapping/native_stack_tracePASS.
Logs/tmp/minima-command-errors{,-sanitized}-{build,regression}.log.
new-tests/command-runner-errors.md+CommandRunner.mdfinalized; README/REVIEW/provenance
updated. Scope excludesprefix/frames, fullregistry/Main/wallet stillabsent.

Rounding mappedtests VERIFIED:
FullpinnedFLOORTests/CEILTests, pairedproductionfloor/ceilhppcpp, ABSTests+driver+
sharedrunnerread; MiniNumberfloor/ceil/constructors/readsourcewrappers. Existing
MiniNumber BigInt/scaleconstructor normalizesnegative scales; nofloorbuginferred.
SiblingFunctionscandidatesread methods; nativewrapperreuseunchanged.
NewFLOORTests30asserts=4registry+22values+4exceptions. CEIL26=4+18+4. Retainwhole
numericconstructors/fractionstringconstructors/sourceorder/freshfunction/shared
methodContract anddistinctinvalidContract. Sourceemptydomainandmissingtwoargtest
explicit. Nativeassertcounterguard. Twooriginaldriversinvoke3unchangedJUnitmethods.
Sharedscript_value_original_tests.pyadds2classes+packagepaths. CMake4newtests.
Fullbuild70549normal/80143SANPASS.10selected42538/20740PASS13.21s/13.07s.
All14originalreportsreadAFTERterminalpass:FLOOR3/30,CEIL3/26,ABS3/23,MF2/11,
Script1/10,Hex2/35,Number4/99+7supplemental eachruntime.
Logs/tmp/minima-rounding-tests{,-sanitized}-{build,regression}.log.
All4command/roundingfullbuildlog warning/errorsearch empty. Newtest2cpp+2Java
readback; CMake/runnereditsreviewed. Test/prodFLOORCEIL4lessonsfinalizedandSHAadded.
README/REVIEW/provenance final282. Rootnewteachingownership addsFLOOR/CEIL
production+tests; no othercodechangesforthismilestone.

Requested457teacherfirstpass COMPLETE. Allfilelessonsand2newtestlabs indexed.
Next: continuefullport ACTIVE (Main/nativeH2/MVStoreand115productiondestinations absent).
No commits/pushes, no publicnode/walletactions. Userauthorizedongoingfullport.
Earlierhistorybelow.

# Latest —280 counterparts; Number and function tests VERIFIED; guides IN PROGRESS

GoalACTIVE/incomplete. Count280=254production+26testfiles (includesDUMMYFUNCTION
helper),177absent=115production+62tests.126CTestcases. NO LIVE builds/tests.
User explicitlyrequestedsideagent457teacherbackfill. Main/nativeH2/MVStoreabsent.
Mainagent /root/student_guide_backfill +2helpers live. Latest308authoredproduction
+88testexplanations,61drafts (countincreasing). Do notclaimfullguidecompleteyet.

NumberValue milestone:
Full builds89767normal/38777SAN PASS no warnings/errors.16normal94386PASS193.70s;
16SAN78670PASS528.87s. Logs/tmp/minima-number-value{,-sanitized}-{build,regression}.log.
New JavaArithmeticException extends existingJavaTypedException. MiniNumber.cpp
zero denominator typed +0/0 Division undefined vsnonzero/0 Division byzero. Installed
JDK26BigDecimal.divide(MathContext) andpinnedcallerfullyread. NewNumberValueTests
99sourceassertions=7+38+48+6; sourcecase3commented causescomparisonloopsstopafter2,
keepunreachable4-6inactive. NumericexpectedaboveLongMAX usesMiniNumber notoverflow.
Original4JUnitmethods unchanged plus7supplementalexactJava/nativeerrorobservations:
0,1,-1 dividedby0,0.000 and0.pow(-1). Sharedscript_value_original_tests.py reused.
AllreportsnormalSANread:primitive3802,value1451,VM4471,objects9244,checker1343,
logger2016,consensus2890zero failures; finalSANconsensusreadAFTERterminalPASS.

Functionmilestone:
Newmapped DUMMYFUNCTION.cpp/.hpp(testonlyclass,notregistry),MinimaFunctionTests.cpp
(11success-pathasserts), maths/ABSTests.cpp(23=4construct14valid5exceptions).
NativeproductionUNCHANGED. FULLpairedABS,MinimaFunction,ConstantExpression,
sourceExpression andContractgetNumberParam,MiniNumberabs read. Testhelpercomplete5
methods mappedbutrunFunction/minimum metadata notexecuted byoriginaltest;doclimit.
SharedoriginalrunneraddsABSTests/MinimaFunctionTests packagepathsandDUMMYJavadependency.
2originalJavadrivers executeunchangedmethods. CMake4newcases,helperlinkedonlytest.
Fullbuild11491normal/75754SAN PASS no warning/errorlines.
9normal15412PASS10.55s;9SAN23947PASS10.83s. Logs/tmp/minima-function-tests{,-sanitized}
-{build,regression}.log. All10originalreportsread:ABS3methods23asserts,MF2/11,
Script1/10,Hex2/35,Number4/99+7supplemental. Source_map280/177checkpassed.
README,REVIEW,provenance,rootteachingguides updated finalresults.

ROOT TEACHING OWNERSHIP (agentsmustavoid):
how-to-study-this-port.md,new-tests/checker-rejections.md;
production objects/Transaction,TxPoW,Coin; system/brains/TimedChecker,TxPoWChecker;
kissvm/Contract; utils/ZipExtractor,Streamable; kissvm/values/HexValue,NumberValue;
kissvm/functions/MinimaFunction,functions/number/ABS;
system/params/GeneralParams,GlobalParams,TestParams,ParamConfigurer;
Minima.md andsystem/Main.md (bothAUTHOREDexplicitunported,full1655source linesread);
NOWCLAIMED system/commands/CommandRunner.md (NOTyetread/authoredthisstep).
Root testguides:ScriptValueTests,HexValueTests,NumberValueTests,
functions/DUMMYFUNCTION,MinimaFunctionTests,maths/ABSTests.
RootCoin+4paramslessons authoredbasedfullsource/native/fixtures; params719existing
cases67handlers4HTTP3processreportsnormalSANread (no newparamrun). Paramsreset
initialvsresetdifferences,profilelazycapture,enumidentityorder,checkParamssideeffects
andbroadnativeErrorcatchgap documented. RootMain/Minimaexplicitproposedtests,
fullnodeNOTclaimed. MainnativeDB/miner/processor/ChainServices headersread.
Latestrootmanual7pages163links zero errors; need auditlaterrootadditions.
Sideagentaggregateaudit/status ongoing, notifiedallfinalresults280.
Helper scopes remainingtxn/mmrsync andsend/sendpoll commands; mainagentDB/brains/
objects. AgentfoundtreeI/Ocatch/listaliasgapsdocumentednotfixed (seemessagehistory).

NEXT: RootfullreadCommandRunner.java/cpp/headers/harnesses thenauthorguide; coordinate
all457guidecompletion withagent. Continueactualfullportafter/besidesteacherwork.
No commits/pushes, no publicnode/walletactions, no new approvals. Incidentalpsread
sandboxdeniedandnotneeded; didn'tescalate unrelateddiagnostic. Previoushistorybelow.

# Latest — NumberValueTests added; builds running; teacher backfill active

Goal ACTIVE/incomplete. Map277 counterparts=254 production+23 tests;180 absent.
Normal full build89767 and SAN38777 RUNNING; do not start tests until success.
Logs /tmp/minima-number-value{,-sanitized}-build.log. Changed JavaExceptions.hpp
(new JavaArithmeticException extends JavaTypedException), MiniNumber divideDecimal
zero denominator category + messages: 0/0 Division undefined, nonzero/0 Division by
zero. JDK26 BigDecimal.divide(MathContext) full method read; source MiniNumber call
read. Original NumberValueTests.java full plus paired NumberValue bodies read.
New mapped test99 active assertions (7+38+48+6), source comparison case3 absent
means routes4-6 unreachable; native preserves only2 iterations. Supplemental7
error category/message outputs Java/native compared exactly via shared runner:
0,1,-1 divided by0,0.000 and pow(0,-1). New original driver invokes4unchangedJUnit
methods. CMake2 newtests; shared original runner NumberValue choice added.
Root authored Coin guide from full source/native/fixture; agent latest194 authored
production+88 test explanations,175drafts (before subsequent ongoing work).
Root now owns production NumberValue.md + test NumberValueTests.md; updated both
for new correction with validation explicitly PENDING. Agent notified map277.
Helper54utilities finished, now owns51remaining commands backup/search/base.
Need finish builds, run affected normal/SAN regression, read reports, review,
update docs/provenance/guide+agent with final evidence. All prior tests below are
terminal; don't restart them unless new numeric correction affects that scope.

# Latest — 276 counterparts; two mapped value tests VERIFIED; guide IN PROGRESS

GoalACTIVE/incomplete. Count276=254production+22mappedtests,181absent=115prod66tests.
Main/nativeH2/MVStoreabsent. Sideagents remainlivefor457fileteachingguides.
ALL457pagesexistbutmanyproductiondraftsremain; do NOTclaimdocumentationcomplete.
Latestagentreport137authoredproduction232drafts+88testexplanations; countcontinues
increasing. SideagentownsINDEX/coverage/README; rootowns7prodguides+2testguidesand
course-methodology/newcheckerlab. Root7:Transaction,TxPoW,TimedChecker,TxPoWChecker,
Contract,ZipExtractor,Streamable. Also nowrootHexValueproductionguide becausefix;
testScriptValueTests andHexValueTests rootupdatedmatchingnewmappedcounterparts.

NEW MAPPED TESTS:
ScriptValueTests.cpp all10sourceassertions. OriginalJavaScriptValueOriginalTests
invokesunchangedpinnedJUnitmethod; existingbundledlib/junit-4.13.jar used.
HexValueTests.cpp all35executedassertions across2source methods,6constructorroutes
inbothcomparisonloops; preservehextext10vsdecimal10andduplicatedinequalitydirection.
DoNOTactivatecommentedsourceorderingchecks. HexValue numericconstructor nowthrows
existingJavaNumberFormatException fornegative/fractionalvsoldinvalid_argument.
Nootherproductionchangeafterpriorchecker milestone. NewHexValueOriginalTests calls
originalmethods. tools/script_value_original_tests.py generalized--test-class
ScriptValueTests(default)/HexValueTests withseparatecompile/reportpaths.
CMakeadds2mappedtargets+2original-testcomparisons; total120testsnow (IDs shifted).

FINAL NEW VALUE BUILD/TEST:
Scriptfullnormal59933PASS,SAN39363PASS logs/tmp/minima-script-value{,-sanitized}-build.log.
Scriptnormal38884andSAN363693/3PASS;originalmethod1/10PASS2.02s/1.91s.
Hexfullnormal29883PASS,SAN47517PASS logs/tmp/minima-hex-value{,-sanitized}-build.log.
Hexnormal746608/8PASS23.81s;SAN711088/8PASS85.21s logs
/tmp/minima-hex-value{,-sanitized}-regression.log. NO LIVEbuild/tests.
Alllatestvalue/VM/object/originalreportsNORMAL+SANreadzero failures:
Script1method10asserts,Hex2methods35asserts,value1451,VM4471,object9244.
FinalSANobject9244readAFTER71108completion. Source_map276/181regenerated/checkPASS.
BuildlogsnoC++warnings/errors. README,REVIEW,provenance,rootguides updatedfinalstats.
Agentnotifiedcountrelease276/181 andfixedrootHexValueguidebrokenrelativepath via
os.path.relpath beforeitsreport. Need finalguideaudit onceagentsfinishdraftbackfill.

READ-ONLY NEXT SOURCES (NOimplementationyet): NumberValueTests.java FULL+paired
NumberValue.java/cpp/hpp,MiniNumber.div examined. Sourcecomparison switchcase3is
commented, defaultresetscounterandbreaks; cases4–6unreachable, don'tactivatebyport.
NumberValue divisionexceptioncategoryneedsdedicatedreview(native divideDecimal).
FullItemList.java read; sourcequirks:twoStringctor this.sp=s, getArray Object[] cast
alwaysClassCastException, emptyexactseparator splitloopsforever;multiToken uses
StringTokenizer dropsruns. ExistingJavaList.hpp FULLread lacks indexedadd/rawobject
casts. ItemList nativeNOTcreated. UtilitydochelperownsItemList, notifiedquirks.
ZipExtractor/Streamable remainunported; priorread-onlystudybelow.

# Latest — checker VERIFIED; 457-file student guide still IN PROGRESS

Goal ACTIVE/incomplete,274counterparts183absent,Main/nativeH2/MVStoreabsent.
User addedfull457teacher-guide request,sideagentbackfill explicitlyauthorized.
Liveagents /root/student_guide_backfill ownsstudent-guide exceptrootfilesbelow;
itshelperowns88testguides. No livebuild/tests. Do notstopagentsaswholeguidehasdrafts.
All457guidepages nowexist;53builtin lessons authored +5root complex lessons;
remainingproductiondraftsunderagentreview. Test88guidesgeneratedwith20native-specific
adaptations/68unported,helperpolishing. PagecountNOTfinishedteachercoverage.
Root owns how-to-study-this-port.md,new-tests/checker-rejections.md andfiveguides:
files/src/org/minima/{objects/Transaction,objects/TxPoW,system/brains/TimedChecker,
system/brains/TxPoWChecker,kissvm/Contract}. Allfiveauthoredinplace,sourcesfullyread,
actualtestevidence+examples+specificremaininggaps. Temporarysourcebodiesalsoin
/tmp/minima-student-root-lessons/*.md. Agentinstructedneveroverwriteauthoredpages;
aggregateauditmustcountroot5. Need finishguidebackfill+coverage/linkaudit.

CHECKER FINAL BUILD/TEST EVIDENCE:
16374normalfullbuildPASS /tmp/minima-checker-corrections-build.log;
75120SANfullbuildPASS /tmp/minima-checker-sanitized-build.log.
36776normal2targetedPASS:1343checkercomparisons100fixtures12.83s,timed105cases19.48s.
84412normal12broaderPASS152.58s /tmp/minima-checker-normal-regression.log.
24780SAN14/14PASS492.87s /tmp/minima-checker-sanitized-regression.log.
All8reportseachnormalSANreadzero fail:checker1343/100,timed105,object9244,VM4471,
consensus2890,HashMap135,blockownership1558programs6219ops,processor384/1434.
SANchecker31.07s,timed29.31s,consensus492.87s,ownership146.63s,processor49.99s.
Fourteen distinctaffectednormalANDsanitizersuitespass. No newchangesafterbuilds.
README,REVIEW,provenance,rootlab finalresultsupdated. Need finaldoclinkaudit once
agentfilesstable. Previousbuild68047FAILEDonlyfixture nonexistentresetRuntime,
fixedtoexistingsetRuntime({})BEFOREtests. Initial1185checkerfailed78thenfixedreal
Transactionlogs,getterexceptions. Expandedfixture100/1343 includesduplicateHashSet
capacityboundaries,invalidInputamount,nullSimpleMMRpointerforwarding.

NEXT PORT READ-ONLY STUDY alreadydone (NOedits/designyet): fullJavaZipExtractor.java
andStreamable.java, nativeGzip.hpp/GzipInput.hpp/JavaUnixFile.hpp/MiniFile.cpp+.hpp/
ByteIO.hpp read. Relevant siblings searchfoundnoexistingnativeZipExtractor. Broad
rgencounteredlongfilenameerrorinmaxima/minima-common; targetedtools/minima-core-cpp
searchsuccessful. InstalledJDK26src.zip ZipInputStream.java705lines read30–220 and
380–705 (notwholefile); rawinflate/CRC/dataDescriptor/ZIP64 andcanonicalpathsneeded.
NativeGzipInput rawinflate structure andJavaUnixFile child/mkdir helpers are candidates.
No ZipExtractor counterpart created; do NOTcount. Source ZipExtractor has no callers
inpin rgsearch. File.isChild nativeMiniFile onlylexicalprefix,NOTsuitablezipcanonical
containment. Need fullreusegate/testsourceinspection beforeanynewZipcode.

# Latest current state — checker + student guides IN PROGRESS

User additionally requires teacher-quality explanations for all457files, with
logic/tests, logical subfolders; explicitly authorized sideagent for backfill.
Agent /root/student_guide_backfill owns cpp/student-guide/ EXCEPT root-owned
new-tests/checker-rejections.md. Agent's one helper owns files/test88guides.
Root lab documents current checker tests; no fullnode completion claim.
Normal fullcorrectionbuild16374PASS /tmp/minima-checker-corrections-build.log;
previous68047FAILED onlyfixture nonexistent resetRuntime call, fixedsetRuntime({}),
no testsrunagainstfailedbuild. AddedTransaction.checkValid realLogger/Hashtable,
TxPoW bodygetterNPE,SignatureemptyrootIndexOutOfBounds,ContractsetGlobalsnulltokenNPE.
Checker nullableSimple overload forwardsMMRpointer without dereferencingnull;
blockcallersuse.get(),newnullSimple tests. Object fixture suppressesvalidationlogger
likeoriginalJavaObjectOracle. Originalchecker fixtures1185failed78, correctedexpanded
100fixtures1343comparisonsPASSnormal36776(12.83s),timederrors105PASS19.48s.
Reports inspectedzero failures. Sourcecount274/183unchanged.
LIVE84412normal12regressions /tmp/minima-checker-normal-regression.log.
LIVE75120fullSANbuild /tmp/minima-checker-sanitized-build.log.
NOotherlivesessions. SANcurrentchangesNOTyettested. Need pollhandles, launchaffected
SANtestsAFTERsuccessfulbuild,review/updateREADME/provenance/REVIEW/lab/checkpoint.

# Current continuation — checker rejection behavior (IN PROGRESS)

Goal ACTIVE, 274 counterparts, 183 absent; full node/storage incomplete.
Current production edits: TxPoWChecker.cpp/.hpp restores basic/simple/scripts/MMR/
signature logs and empty reconstructed MMR root Java NPE, JavaHashMap unique IDs.
New CheckerLogsOracle.java/checker_logs.cpp/checker_logs_differential.py/CMake#116.
Expanded TimedCheckerErrors fixture empty-mmr adds 14 cases (105 total).
Normal full build 99188 PASS /tmp/minima-checker-logs-build.log.
42114 TERMINAL: timed_checker_errors_differential PASS19.70s; checker_logs failed
78 of1185 comparisons. Causes missing Transaction.checkValid logs, missing token
Contract.setGlobals bad_optional, TxPoW body getter bad_optional, Signature empty
root getter out_of_range. Source methods inspected; correcting native categories
and real logger behavior next. No SAN build/run for current edits yet.
NO LIVE sessions at this checkpoint. No source count changes.
Previous verified milestone follows (applies to previous production version).

# Latest continuation — 274 counterparts; timed checker and block catches (VERIFIED)

Goal ACTIVE/unbudgeted/incomplete. Previous NIO turn verified progress. Pin and
counterpart count unchanged274=254production+20tests;183absent. FullMain/H2/MVStore
absent. No agents/commits/pushes/productionnode/approvalprompts/rejections.

NO LIVE SESSIONS. FINALtimedmilestoneVERIFIED;goalstillincomplete.
83285 SANall13PASS557.30s:consensus2890cases557.29s,processor55.37s,
valuedeadline122.94s,uncaught10.25s,processorworker2.71s,timedchecks0.87s,
logger5.32s,nativethread0.45s,sourcemap0.54s,shareddeadline121.32s,
first91errors39.34s,ownership182.99s,newErrorDeadline125.72s.
84201 finalrelease-signal SAN91-case directrunnerPASS,zero failures;
 /tmp/minima-timed-barrier-sanitized-regression.log. 85476SANtargetbuildPASS
 /tmp/minima-timed-barrier-sanitized-build.log. Normal56661targetbuildPASS
 /tmp/minima-timed-barrier-build.log;49613final91casesPASS20.19s.
Normal59405terminal(exit8ONLYoldprocessorfixture),consensus2890PASS170.82s,
newErrorDeadlinePASS122.95s,ownership1558programs6219opsPASS62.12s,
processorworker0.92s,existingdeadlinesboth120.78s. Correctednormalprocessor59347
PASS384programs1434ops(/tmp/minima-timed-processor-regression.log).
Normalold91cases91888PASS23.54s;originalnormaltimedcheck1.08s,nativethread0.73s,
sourcemap0.22s. ELEVENdistinctnormalaffectedtestslatestresultsallPASS.
Allnewtargeted/deadline/consensus/processor/ownershipJSONreports explicitlyinspected
normal+SAN,zero failures. Sixfinalbuildlogs noC++warnings/errors. Finalsource_map
274/183 andgitdiffcheckPASS. README/REVIEW/provenance finalresultsupdated.
No productionchangesafteroriginalalltargetbuilds;laterfixture-onlychangesverified
bothbuilds. No repeatsneededforpassed120secondchecksmerelybecauseoldaggregatehad
fixturefailure. Normal/SANfreshErrorcaseusedsameproductionbeforefinalstallfixture
change;final91regularcaseswererunwithrelease-signalfixturebothbuilds.
FINAL91FIXTURE:fixed100msstallreplacedbysignalreleasedONLYAFTERcheckingcallreturns.
JavaSemaphore.acquireUninterruptibly;nativepromise/shared_future wait. Reused
ExecutorUncaughtOracle.java/executor_uncaught.cpp release-after-observationpattern
(bothFULLreadbeforeadaptation). Nativeinterruptionstillnotimplemented/tested;only
sourceuninterruptibleworkeractioncontrolled. RebuildSANonlyafter115reportedPASS,
so no livecheckerbinaryrewritten. No agents or production actions.

NEW PRODUCTION:
TimedChecker.cpp actualworkerusesNativeThread unnamednameallocatedbeforelaunch;
keepscapturedstate,parent,block,sharedlist,ancestors. WorkerJavaError/std::bad_alloc
escape;ordinaryExceptionlogs 'Block failed to process : '+nativeExceptionText then
validfalse;finishedtrueonlyaftertry/catchcompletes. OuterError/std::bad_allocescape,
ordinaryexceptionrealMinimaLogger+NativeThrowablebeforevalidfalse. FinalBLOCK_LOGS
usesrealLogger,outsidecatch,explicitnullblockNPEinsteadofsilentlogskip. Sourceflags
notresetonreuse. Nativeinterruption/threadgroups remainUNIMPLEMENTED; source120000ms
and50mspoll unchanged. HeaderonlyaddsfriendTimedCheckerFixtureforstateinspection.
TxPoWChecker.cpp sharedheaderValid rejects nowrealLogger. checkTxPoWBlock and
checkTxBlockOnly catchExceptionvsError distinction,exactsourcecatchprefixthen
exceptionlog;catchloggerfailureescapes. Restoresmissingmaxcount,duplicateIDs,and
MMRrootmismatch logs. BlockcollectsALL main/burn proofIDstrings THEN constructsset
andchecksuniqueness,matchingJava:laternulltransactionvisitedevenafterearlierduplicate.
ExistingcollectCoins helperforbasicuntouched. Otherchecker/script/MMRloggingand
fullAPIsemanticsstillpartial. Nootherproductionfileschanged.

NEW TESTS:
support/TimedCheckerErrorsOracle.java reusesNIOClientOracle reflection and
ConsensusOracle serialization/duplicatefixture. ActualJavaTxPoWGeneratorproduces
parent/block/duplicatetxnwire once. SourceMinimaDB memoryfixture/noMain/nativenode.
support/timed_checker_errors.cpp deserializessamesourcebytes,actualchecker+worker,
existingtimed.cpp parentleafMMRsetup. Usesactualpriorcheckerinvocationtoprimeflags;
sourcejoinsstoredworker;nativeobservescapturedsharedlist/blockownerretirementand
readsatomicstate+lockedlogs. ExplicitdefaulttesthandlerpreventsOOMfatalprocess.
tools/timed_checker_errors_differential.py:91regularcases and1actual120secondfresh
Errorcase. Java/C++runconcurrentlypercasewithPython2workerThreadPoolExecutor(test
processes,NOsubagents). FiltersautoStackframes,generatedThreadcounter,logelapsedms,
JVM-generatedNPEdiagnostic(notcategory/order). Allotherlogs/state/returnscompared.
CMake#114regularTIMEOUT300,#115deadlineTIMEOUT180;separateoracledirs preventparallel
compileoverwrites. Existingtimed2ownershipdeadlinesstillusedunchanged.

TEST DISCOVERY/FIX:
Initial82-case comparison81957 failed7 reusedtimedreturnvalues:Javaworkerfinished
before50ms,nativeworkerafter50ms;identicallogs/finalflags. Preservedsourceproduction
semantics,correctedfixtures:knownmatchinginitial/finalvaluesforordinaryinvalid
reusedcalls,primedTRUEspecificallyforescapingfailures,explicit100msuninterruptible
loggerstallprovessourcereturnTRUEwhileworkerultimatelysetsFALSE. SourceCPUspinin
stallignoresThread.interruptlikeactualchecker(native100mssleep). TestdoesNOTclaim
nativeinterruptionsemantics. Finalfixtureusesreleasesignal,seeLIVEsectionabove. Freshvalid/invalidchecksalsocoverinitialfalseflags.
Final91regularcases91888 PASS23.54s,reportpassedtruezeroFAIL;initialoriginaltimed
checksPASS1.08s,nativeThreadPASS0.73s,source_mappingPASS0.22s.

EXISTING FIXTURE UPDATE:
Fullcpp/support/txpow_processor.cpp,TxPoWProcessorFixture.hpp andsourceoraclethrough
line115readbeforeadaptation;JavaOwner ignoresMINIMALOGnotifications. Nativefixture
previouslyredirectedstdclogonly,sochangedactualLoggeroutputsescapedcapture. Added
MinimaLogger::Runtime.printlnreceiveratmainthatstrips ] : prefixandautomaticframes
thenwritesbodytoexistingperoperationstdclogcapture. Javaoracleandexpectationsunchanged.
NoTxPoWProcessor productionchange. Bothfixturetargetbuilds82717/17361PASS.

BUILDS:
Alltargetnormal97535PASS. Afterfixturetimingadjustmentalltarget2804PASS
 /tmp/minima-timed-errors-build.log. AlltargetSAN21393PASS
 /tmp/minima-timed-errors-sanitized-build.log. Finalsinglefixturetargetnormal82717
 /tmp/minima-timed-processor-build.log andSAN17361
 /tmp/minima-timed-processor-sanitized-build.log PASS. NoC++warnings/errorsin4final
 logs;gitdiffcheckPASS. Sourcecount274/183verifiedinitialtests.

REUSEGATE deliveredexactpathsBEFOREproductionedits. FULLTimedCheckerJava/native/
header,timed.cpp,ConsensusOracle.java/consensus.cpp/comparatorreadpreviousprepand
currentcontext. CurrentnativeCheckerfullfilehasbeenreadacrosschunks;sourcefull
checkTxPoWBlock+checkTxBlockOnlybodiesread. Header/ConsensusServices/getParentschain
reviewed;NativeThread/NativeThrowable/Logger reusedunchanged. Siblingscanfoundno
TimedChecker. NativeGenerator1-100also inspectedforfixturecontext. Newprocessor
fixturegateexplainedexactmissedlogandpreservedJavaexpectationsbeforeedit.

NEXT:
No livehandles orpendingvalidation. Normal59405oldfailedfixtureisresolvedby
corrected59347PASS. DoNOTrerunpassed120secondtestsjustbecauseaggregatefailed.
README/REVIEW/provenanceandthischeckpointupdatedwithfinalresults. Preserveuntrackedworktree,noGOALcompleteclaim. Neednativefullnode
Main/H2/MVStore/183absent+allpartialAPIverification. Remainingnativeworkers,source
interruptions,Threadidentityfields,OSthreadcreation/resourcefailurebehaviorstill
unverified. NIOpreviousmilestoneresultsbelowallterminal.
NEWconcreteremainingbugdiscoveredread-only:NOEDITyet. SourceMMR.java601-632FULL
getRoot() returnsnullfornopeaks. NativecheckTxPoWBlock rootnull currentlythrows
invalid_argument("Empty block MMR") vsJavaNPEonderef. Toverifyaddempty-mmrmutation
AFTERcheckerprime:source set(tip,"mMMR",newMMR()),native *tip->getMMR()=MMR();
actualemptyblockthenreachesnullroot. Needsourcecompanionconstructionreviewbeforefix.
ThisturnFULLnativeTxBlock.cppreadplusnativeTxPoWTreeNode.cppconstructMMR1-56 chunk;
applyBlockMMRbody/sourcecounterpartsNOTfullyrereadthisturn. No newrootfix/test yet.
Thisknownremainingdifferenceisrecordedprovenance/REVIEW.

# Latest continuation — 274 counterparts; NIO server and connector boundaries

Goal ACTIVE/unbudgeted/incomplete. Source pin unchanged.274=254production+20tests;
183absent. Full Main/nativeH2/MVStore/node absent. No subagents,commits,pushes,
production node/wallet/remote network actions or approval prompts/rejections.

NO LIVE SESSIONS. Final13SAN suites23477 PASS285.25s. Source_mapping0.19,
nio_client113.02,nio_server3.40,nio_manager_transport26.64,nio_manager_loopback0.85,
nio_manager_state15.34,nio_message86.69,network_manager11.11,minima_logger3.51,
minima_uncaught7.41,native_thread0.50,nio_server_errors11.93,nio_manager_errors4.66.
Both newSANreports explicitlyread passed=true,zerofailures. README/REVIEW/provenance
updated with finalverifiedresults. No production changes sincefullnormal/SAN builds.

CHANGES:
- NIOServer.cpp now rethrows JavaError/std::bad_alloc from source Exception catches
  (send,broadcast,accept,event processing,outer run); preserves Error in cancelled
  key runtime catch. All server logs use MinimaLogger/NativeThrowable; trace uses
  notify=false. Fatal bind logs before _Exit(0),so logger failure canescape. Invalid
  port diagnostic matches Java 'Port value out of range: <port>'.
- NIOClient.cpp disconnect rethrows JavaError/std::bad_alloc;ordinary exceptions
  remain silent per source. No other NIOClient read/write changes.
- NIOManager.cpp actual startServer/connectAttempt factories use NativeThread with
  unnamed native names prepared beforethread launch,retainownerthroughdispatch.
  Connector source success/failure MinimaLogger + Exception-vs-Error order; success
  log Exception enters reconnect catch,Error bypasses;catch loggerfailure stops
  reconnectposting. Other manager methods untouched.
- support/NIOServerErrorsOracle.java,nio_server_errors.cpp,
  tools/nio_server_errors_differential.py,CMake#112:63cases actual pinnedserver/client
  +realnative selector/loopback:startup,accept/outgoingregister,read,explicitdisconnect,
  direct/broadcastsend,keycancel;runtime/Error/OOM/IO+catchloggererrors,trace notify,
  events/running/clients/register/disconnectlist state. Controlled receiver/key.
  Threadjoinedbefore snapshots;source callbacks deliberatelyshutdown beforethrow to
  observe ordinary recovery without indefinite select. SourceMain receiver controlled
  fornotification;notfullMainstartup. Automaticframes/clientUIDs excluded.
- support/NIOConnectorErrorsOracle.java,nio_manager_errors.cpp,
  tools/nio_manager_errors_differential.py,CMake#113:12actual privateJavaconnectAttempt
  comparisons(nativeactualconnectorrunnable),invalidport/loopbacksuccess+success/catch
  loggerfailures of4types;pendingremoval/registration/reconnect/defaultdispatch.
  SourceUnsafeManager bypasses constructor only; pre-shutdown NIOServer creates
  selector. Nativebaremanagerfriend retains actual launchers. 4additionalnative
  serverfactory invalidport catchloggerfailures. Generatedthreadcounter/portsnormalized.
  Nativefixture waits forconnectorownerrelease;registrationinspection takesactual
  mutex,message/eventinspection locked. Explicit handlerspreventOOMfatalprocess.

VERIFICATION(all sessions terminal):
- initial fullbuild9476 failed ONLY newfixture missingunistd/MiniData stringctor;
  correctedfixture, no tests run againstfailedbuild.
- finalnormalalltarget86932 PASS /tmp/minima-nio-errors-build.log.
- 83603 fiveNIOsuitesPASS80.67s:client44.72,server2.82,managertransport24.85,
  managerloopback0.47,new63servercases7.80.
- newmanagerfixturefullbuild80338PASS;reviewaddregistrationobservationlock;
  finalnormal50829PASS /tmp/minima-nio-manager-errors-build.log.
- 57007 fiveadditionalsuitesPASS47.98s:source_mapping0.22,NIOstate7.70,
  NIOmessage30.46,networkmanager5.26,new12connector+4factory4.34.
- FULLSAN10346PASS;no warnings/errors in3finalbuildlogs.
- bothnormalnewJSONreports explicitlyread passed=true,zerofailures.
- source_map --check274/183 andgitdiffcheckPASS;worktreeuntrackedunchangedlist.
- README/REVIEW/provenance have newmilestone with final13SAN PASS285.25s.
- Final all4newJSONreports passed=true/zerofailures;no remainingpendingtestmarkers.

REUSE/REVIEW:
Exactpathreport deliveredbeforeedits. Full sourceNIOServer.java+nativecpp/hpp,
sourceclientdisconnect+nativefullclientcpp,nativefullmanagercpp/hpp,sourceconstructor/
fullconnectAttemptread. FullcurrentNIOServerOracle/nio_server fixtures,clientoracle,
HTTPfailurefixtures/comparator/NativeThread/NativeThrowable/MinimaLogger,NIOConnections,
InetSocketAddress,NIOSocket header andSocketWakeup header read/reused. Sibling
/Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/network/NIOServer.hpp FULL
read;blockingIPv4behaviorcannotreplacepinnedselector. code-reviewskill reread;review
record inREVIEW.md. No productionNativeThread/NativeThrowable modifications.

REMAIN/NEXT:
NIOServer closes native selector atnormalshutdown whileJavasourcekeepsopen;resource
retention/exceptionalcleanup/reentry/cancellationracespartial. NativeOSdiagnostics
notallJDK. NIOClient read/write/oversize logs andotherNIOManagercatches legacy.
createNIOMessage onlycatchesstdbadalloc notexplicitJavaOutOfMemoryError;
sendSerialized trafficcatch andsendPingMessage genericstdcatchesneedsourceErroraudit.
No changes tothese paths.
TimedChecker PREPONLYthisturn:no edits. FULLnativeTimedChecker.cpp/.hpp,
FULLsourceTimedChecker.java,FULLsupport/timed.cpp reread. Nativecheckerworkerrawthread,
stdcatch setsvalidfalse+finishedtrue wronglyforError;outerstdcatchalso broad;logsclog.
SourceMAX120000ms,50mspoll;source DOESNOTresetvalid/finishedon reuse;source interrupts
workerafterpoll (native nointerrupt). Outerloggingderefsnulltxpow ifBLOCK_LOGS true;
nativecurrentlyskipslogifnull. Nativeancestorownership deliberateexistinghelper.
TxPoWChecker.cpp1-125 and source1-238 read;FULLcheckTxPoWBlocksourcebody inspected.
NativeheaderValid rejectionlogsclog;block catchstd logsone line vs source two calls
'ERROR checking TxPoW Block..' thenexception. Sourceinvalidchain firstlog useful
controlledfailureinjection;primedchecker canexpose stale valid/finished withoutnew
120seconddelay. FullConsensusOracle.java andsupport/consensus.cpp reread, comparator
NOTyetfullyreread thisturn. Siblingscan didnotfindTimedChecker;needsappropriate
reuse reportandcomplete dependency scopebeforeedit. Existingtimed.cpp buildsvalid
blockviaTxPoWGeneratorfixture andtestsactual120second owned-value/shared-listtimeouts.

# Latest continuation — 274 counterparts; HTTP and executor thread integration

Goal ACTIVE/unbudgeted/incomplete/unblocked. Previous turn was verified progress;
this turn also made verified runtime progress. NO LIVE SESSIONS. No agents,
commits,pushes,production node/wallet/remote webhooks or approval prompts/rejections.
All existing work preserved. Source pin5564b9d78978efcaf23679190d05253961fc6e69.
Counterpart count UNCHANGED274=254production+20mappedtests;183absent.
Full node/Main/H2/MVStore not implemented. Do not equate scoped tests with completion.

NEW/CHANGED PRODUCTION:
- src/org/minima/system/network/rpc/HTTPServer.cpp: listener factory and detached
  request workers now use NativeThread, with source unnamed listener and exact
  'Socket Handler @ <configured port>' request names. Removes outer generic
  std::exception swallowing and request-thread local catches. Source catches are
  BindException(always log),SocketException(log only mRunning),IOException(always
  log even after shutdown). All HTTP logging uses real MinimaLogger/NativeThrowable.
  Logger failures propagate through native thread boundary. Invalid port message
  matches source 'Port value out of range: <port>'. Explicit shutdown preserves
  Error/std::bad_alloc propagation while logging ordinary close/wakeup errors.
- src/support/JavaSocketExceptions.hpp: concrete wire::SocketException and its
  BindException subtype reuse TypedIOException and original diagnostic/frame code.
  Socket syscall helper in HTTPServer throws SocketException; bind collisions use
  BindException. No ByteIO/NativeThread/NativeThrowable source changes this turn.
- src/support/FixedTaskExecutor.hpp: keeps lazy workers,FIFO,shutdown/drain,lifetime.
  NativeThread wraps worker; pending exception_ptr survives task clear and worker
  removal/replacement before rethrow to default handler. Replacement-start failure
  replaces original pending task failure. Waiters notified after worker accounting.
  DefaultThreadFactory-style pool-N-thread-M names; global pool/per-pool worker
  counters preserve signed32bit wrap. Termination can precede default-handler return,
  as Java does. Removed local print-and-swallow task/replacement catches.

TESTS/ADAPTERS:
- Existing support/http_server.cpp now installs actual MinimaLogger println receiver
  to keep control JSON stdout clean, while retaining P2PFunctions::logger spy for
  still-legacy CMDHandler logs. Existing Java fixture unchanged. Normal HTTP binary
  concurrency/RPC parsing/network manager tests passed after change.
- support/HTTPServerErrorsOracle.java + http_server_errors.cpp +
  tools/http_server_errors_differential.py:30programs use actual pinned HTTPServer
  and real native listener/daemon request threads/loopback sockets. Factories and
  handlers throw runtime,LinkageError,OOM,IO,Socket,Bind errors; covers stopped
  factory catches,logging failures,direct/async invalid ports,bind collision logger
  failure and a throwing default handler. Compares source event/log order,names,
  running,bound,completion state. Explicit test handler prevents OOM process halt.
  Automatic JVM/native frame lines and generated Thread counter excluded.
- support/ExecutorUncaughtOracle.java + executor_uncaught.cpp +
  tools/executor_uncaught_differential.py:10programs compare actual JDK fixed pool.
  Runtime,Error,OOM,IO,throwing-default modes with/without queued successor; worker
  replacement before default handler,late default rebinding,exact first worker
  name,distinct successor thread,queue drain and awaitTermination while handler
  blocks. Existing fixed_task_executor test retains4workers/FIFO/owner destruction.
- CMake new tests#110http_server_errors_differential and#111executor_uncaught_differential.
  New support fixtures are NOT mapped Java source counterparts and NOT node binaries.

VERIFICATION(all sessions terminal):
96301 full normal HTTP build PASS(/tmp/minima-http-errors-build.log).
52311 fourHTTP/RPC/network/failure suites PASS13.77s;newHTTP30cases4.53s.
20423 full final normal build PASS(/tmp/minima-worker-boundaries-build.log).
84870 fourexecutor/NIOtransport+loopback suites PASS29.58s;newexecutor10cases2.83s.
18628 fiveadditionalsource_map/NIOstate/message/network/native-thread suitesPASS43.11s.
These are13distinctnormal suites. 95140 full finalSAN build PASS
(/tmp/minima-worker-boundaries-sanitized-build.log). No production changes since.
14761 sixteenSAN suitesPASS173.36s;HTTP30cases6.76s,executor10cases3.19s;
all new reports explicitly inspected passed=true,zero failures.
16SAN suites:source_mapping,nio_manager_transport_differential,fixed_task_executor,
nio_manager_loopback,nio_manager_state_differential,nio_message_differential,
http_server_differential,cmd_handler_differential,network_manager_native,
network_manager_differential,minima_logger_differential,minima_logger_worker,
minima_uncaught_differential,native_thread_checks,http_server_errors_differential,
executor_uncaught_differential. Build logs no compiler warnings/errors.
README/REVIEW/provenance updated with verified scopes/remaining work.
Final source_map --check and gitdiffcheck PASS. Worktree still only untracked
.DS_Store,.gitattributes,cpp/,graphify-out/,upstream-study/; no source-pin changes.

REUSE/REVIEW:
Reuse-proven-code/code-review skills continued (already read in this continuous
conversation). Exact source-path report delivered before edits. Current+sibling
search tools/minima-core-cpp/src found no alternate uncaught-handler implementation.
Full HTTP source/header/native implementation,HTTP fixtures/comparator,NativeThread,
NativeThrowable,NIOSocket implementation/header,SocketWakeup header,TypedIOException
and JavaIOExceptionText reused/read. Original source HTTPServer83lines full read
previous turn and retained in context; actual native state revalidated this turn.
Full FixedTaskExecutor and fixed_task_executor test reread. Installed primary
OpenJDK26.0.1 source zip read for complete runWorker,processWorkerExit,addWorker,
addWorkerFailed,execute,DefaultThreadFactory (no JDK implementation copied).
Sourcezip /opt/homebrew/opt/openjdk/libexec/openjdk.jdk/Contents/Home/lib/src.zip.

LIMITS/NEXT(no implementation of following yet):
- HTTP unusual close-error logging callbacks still run under mSocketLock; callback
  reentry/resource failures and repeated concurrent starts not fully proven. Native
  socket diagnostic strings may differ from JDK outside explicit carrier fixtures.
  Source/JVM frame names,groups,priorities,daemon process-liveness,interruptions and
  all ExecutorService APIs still incomplete. Full Main/nativeH2/MVStore absent.
- NIOManager::startServer and connectAttempt still raw std::thread factories. This
  turn read FULL native NIOManager.cpp lines1-45 (rest not reread) and relevant Java
  constructor lines100-165 and FULL connectAttempt body in815-910. Native connector
  still catches all std::exception (incl JavaError),logs clog and posts reconnect.
  Source catches Exception only, uses MinimaLogger success/failure,then reconnect.
- FULL native NIOServer.cpp read this turn. Its own broad catches still swallow
  JavaError/std::bad_alloc and set running=false; need source NIOServer.java catch
  scopes and existing native/Java fixtures review BEFORE adapting. Merely wrapping
  NIOManager launcher is insufficient for source Error propagation. Current new
  socket error carriers could be reused if actual source error types match.
- TimedChecker.cpp/.hpp and TxPoWChecker.hpp reread; earlier turn FULLJavaTimedChecker
  and support/timed.cpp read. This turn TxPoWChecker.cpp lines35-95 and Java
  TxPoWChecker.java50-220 read. Its checkTxPoWBlock catches std::exception and logs
  clog; Java catches Exception,logs 'ERROR checking TxPoW Block..' then exception.
  Need fix Error propagation in underlying checker too, not merely timed wrapper.
  Existing source tests/fixtures ConsensusOracle.java,consensus.cpp and dependencies
  require full review for modifications. Source TimedChecker does NOT reset
  finished/valid on each call; reuse of a primed checker can expose stale flags and
  allow Error tests without changing its real120second deadline. Existing native
  timed.cpp has valid/invalid and two actual120second ownership deadline cases.
- P2PSwapLink.java FULL read: all41lines are // comments, no active Java class.
  Conservative read-only scan confirms this is the ONLY wholly //-commented source
  among457tracked Java files. It still lacks its mapped CPP file. Do NOT activate
  its commented implementation or count it as live protocol logic. No manifest or
  source change made for it this turn. Future final1:1 audit should explicitly
  represent the inactive file rather than invent functionality.
- Remain115absent production paths(95commands+20others incl the inactive file) and
  68absent mapped tests. No goal completion or blockage declaration appropriate.

# Latest continuation — 274 counterparts; uncaught exception handling

Goal ACTIVE/unbudgeted/incomplete/unblocked. 274 = 254 production +20 mapped tests;
183 absent. All partial/not_verified. Full node NOT complete. Pin unchanged
5564b9d78978efcaf23679190d05253961fc6e69. No agents, commits, pushes, production
node/wallet/remote webhooks. No approval prompt or rejection; established CTest
prefix approvals worked. Workspace remains untracked cpp/, graphify-out/,
upstream-study/, .DS_Store, .gitattributes. Preserve all work.

NO LIVE SESSIONS. Final all-target builds44399/97983 passed; no production changes
since. Normal63265 all28PASS145.07s; sanitizer37771 same28PASS281.49s.
Both latest uncaught reports:121programs+6fatalprocesschecks+13worker scenarios,
allPASS, nofailures. NativeThreadChecks also PASS. README/REVIEW/provenance updated
with final scoped verification. Do not claim full runtime or storage parity.

NEW IMPLEMENTATION:
- src/org/minima/utils/MinimaUncaughtException.hpp/.cpp: source first toString,
  nullable lastError initialized***, duplicate return BEFORE thread dereference,
  logger+logUncaught ordering, lastError assigned only after successful logging,
  three dynamic fatal types -> native _Exit(0), nullable captured-default NPE.
  Captured default is STATIC and replaced on EACH constructor; lastError is per
  instance. Short locks avoid C++ races but never span callbacks. Halt callback
  may be supplied for controlled platform tests; default bypasses exit hooks.
- src/support/NativeThread.hpp/.cpp: default-handler registry with snapshots,
  retiring owners released outside locks, late lookup at escaping exception,
  actual native thread identity/name, stderr fallback. A handler exception is
  confined to thread boundary; direct calls still propagate. Unknown C++ throws
  explicitly named native.UnknownException with no fabricated catch-site frames.
- MinimaLogger::Throwable gets Kind classification (default ordinary Exception).
  NativeThrowable derives kind from native types/subclasses, not rendered names;
  std::bad_alloc maps OutOfMemory. JavaError, JavaOutOfMemoryError,
  JavaMVStoreException and JavaTimeoutException carriers added. MVStore carrier
  is NOT an H2 backend. New carriers preserve supplied empty messages as ': ';
  old two-arg JavaTypedException behavior unchanged via optional preserveEmpty.
- MessageProcessor actual factory wraps run with NativeThread. Removed incorrect
  outer swallow-and-complete catches. Error vs Exception uses actual logger and
  captured frames; finally cleanup runs if catch logging throws, and cleanup
  exceptions replace pending failures. START/SHUTDOWN/timeout now use logger;
  PostTimerMessage null inputs/source missing timer use NPE. Running/shutdown
  flags preserve source failure paths. Thread names prepared before launch then
  moved into worker so name copying does not allocate before its boundary.
- TimerProcessor factory uses named native boundary. run no longer swallows
  exceptions. Orphan receiver raises NPE before queue replacement; failed thread
  retains running flag. Null-timer diagnostics use actual logger/new native stack;
  printAllMessages uses notify=false; stopped-receiver diagnostic uses logger.

TESTS:
- support/MinimaUncaughtOracle.java reuses MinimaLoggerOracle/NIOClientOracle:
  compiles actual unchanged pinned source, controls logger/date/Main/Throwable
  frames and default handler only. Native support/minima_uncaught.cpp counterpart.
- tools/minima_uncaught_differential.py:121 programs compare per-op exact trace,
  initial***, repeats, first-vs-second toString differences, NULL error/thread/
  text/frames/elements, Unicode/NUL, print/date/post/stack/default failures,
  constructor changing global captured default and recursively captured handlers.
  Heap and automatic NPE details normalized as existing logger comparator does.
- Six isolated process checks (3fatal types x direct/thread) require exit0,
  source shutdown log, NO graceful hook/return/delegation. Native _Exit exercised;
  no production node. Actual native carriers and Java counterparts thrown on
  real threads; stack symbols/counts differ and are not falsely compared.
- support/UncaughtWorkersOracle.java / uncaught_workers.cpp:13 modes use actual
  source/native MessageProcessor and TimerProcessor: normal,ordinary,Error,OOM,
  MVStore,Timeout,cleanup,catch logging,both logging+cleanup,start/end log failure,
  throwing default handler,orphan timer. Exact log/cleanup/default event order,
  lastMessage/running/shutdown/pending state. Database release callback controlled;
  no DB storage started. START handshake synchronizes empty queue before post;
  elapsed time/automatic frame lines/native-generated name counter normalized.
- native_thread_checks:subtypes/std::bad_alloc/spoofed-name classification,empty
  messages,actual executing thread identity,default replacement while worker waits,
  both registry retirements reenter without lock deadlock,injected halt returns
  and duplicate suppression,foreign throw/no invented frames,stderr fallback.
- CMake adds native sources and tests#108uncaught_differential/#109thread_checks.
  source_map regenerated274/183. Final source-map --check and gitdiffcheck PASS.

SESSION HISTORY (all terminal):
13806 first build FAIL onlyfixtureasLong/JSONArray.at API;fixed;58495PASS.
22619PASS;95917PASS after unknown-throw frame fix/native tests.
32443 tests FAIL only endlog START queue count race;121handler programs and6fatal
checks alreadyPASS;native checksPASS. Fixtures synchronized START then post.
79992 full normalPASS;25884 four focused suitesPASS6.98s.88672 fullSANPASS.
44399/97983 final all-target normal/SANPASS after empty-message fix and13mode
worker expansion. Build logs /tmp/minima-uncaught-build.log and
/tmp/minima-uncaught-sanitized-build.log have no compiler warnings/errors.
63265 28normal suitesPASS145.07s;newuncaught4.81/nativechecks0.31.
37771 same28SANPASS281.49s;newuncaught6.10/nativechecks0.48.
28suites:source_mapping,java_differential,utility_differential,message_differential,
message_workers,peer_state_differential,nio_client_differential,nio_loopback,
nio_manager_loopback,nio_connections,peers_checker_native,p2p_manager_native,
p2p2_worker,network_manager_native,txpow_miner_worker,txpow_processor_differential,
txpow_processor_worker,notify_manager_differential,notify_manager_worker,
rpc_client_differential,minima_logger_differential,minima_logger_worker,
native_stack_trace,minima_rpc_cli_differential,param_configurer_differential,
ParamConfigurerTest,minima_uncaught_differential,native_thread_checks.

REUSE/REVIEW/NEXT:
Reuse and code-review skills continued; skill files reread; graphify/minima-ref
previously applied continuous task. Full pinned uncaught/message/timer and native
logger/throwable/shared exceptions/native workers/tests read. Current+sibling
search tools/minima-core-cpp/src and core/minima-core-meg found no native uncaught
handler. Exact reuse report delivered before edits. Read all changed files.
New source not full Thread/JVM implementation: causes/suppressed,per-thread
handler overrides,groups,VM stderr,resource exhaustion/interruptions/memory-model
parity incomplete. Timer weak receiver lifetime and nonrecursive callback locks
remain partial. No native Main/H2/MVStore; no SQLite/JVM substitution authorized.

Prepared but NOT edited next thread work:
- FULL cpp/src/support/FixedTaskExecutor.hpp46lines read. Existing replacement
  worker behavior tested by support/fixed_task_executor.cpp (FULL read later this
  turn). Still swallows tasks instead of uncaught dispatch.
- FULL JavaHTTPServer.java83 + native HTTPServer.cpp56/.hpp and
  support/http_server.cpp,HTTPServerOracle.java,tools/http_server_differential.py
  read. Current factory mThreads.emplace_back and daemon std::thread bypass native
  boundary. Current outercatch std::exception swallows runtime failures absent
  from Java; logs via P2PFunctions instead of exact MinimaLogger exception overload.
  Existing native HTTP fixture installs P2PFunctions::logger spy; adapting real
  logger requires corresponding fixture receiver adaptation and revalidation.
- FULL JavaTimedChecker.java87 + native TimedChecker.cpp50 read. Worker catches
  std::exception (incl native Errors), logs clog, sets finished even when source
  Error escapes; no default boundary. Source catches Exception only. Its actual
  TimedChecker.hpp and support/timed.cpp FULL read later this turn: normal valid/
  invalid plus actual120second value/shared-list lifetime deadline tests. Further
  checker service dependencies require inspection BEFORE implementation.
- NIOManager.cpp thread startServer/connectAttempt found via rg; NOT fully read
  this turn. JavaNIOManager1114lines; need actual caller/dependency review.
- Main.java ONLY lines1-150 reread this turn; earlier continuous task read more.
  Native Main startup still unported. There are115absent production files and68
  absent mapped tests. Of production:95commands;20others including Main,Minima,
  MySQLConnect/ArchiveServer,10sphincs,Streamable,ZipExtractor,AesUtil,jnifunctions,
  ItemList,P2PSwapLink. FULL Streamable.java24 and ZipExtractor.java64 reread as
  future preparation only; no reuse gate/implementation yet. Do not mark done.

# Latest continuation — 273 counterparts; native startup configuration

Goal ACTIVE/unbudgeted/incomplete/unblocked.273=253production+20mappedtests;
184absent. Allpartial/not_verified. FullnodeNOTcomplete. Pinunchanged5564b9d78978efcaf23679190d05253961fc6e69.
NO LIVE SESSIONS. Noagents/commits/pushes/productionnode/wallet/remotewebhooks.
Previous270RPCturnwasverifiedprogress. ThisturnactualnativeParamConfigurer/
TestParams/mappedParamConfigurerTestandsharedcorrectnessfixesverifiedprogress.
Allhandlesbelowterminal. ApprovedCTestprefixesusedwithoutactualprompt/rejection;
no selfapproval. Gitstatusonlyuntracked.DS_Store,.gitattributes,cpp/,graphify-out/,
upstream-study/. Preserveeverything. Skillsreuse/code-reviewcontinued;graphify/
minima-referenceinstructionsreread(theirfullreferenceswereconsultedearlierinthis
continuoustask). No graph rebuild requested/performed.

NEWPRODUCTION:
- src/org/minima/system/params/TestParams.hpp/.cpp. Lazyvalues()patternreusedfrom
  P2PTestParams; firstuseMINIMA_VERSIONcapturescurrentGlobalParams.MINIMA_VERSION
  +"-TEST"; source10assignmentsetTestParamsorder; alloriginalmutablefielddefaults.
- ParamConfigurer.hpp/.cpp +ParamConfigurerKeys.inc/ParamConfigurerConsumers.inc.
  All67ACTIVEenumkeys/helpstrings/consumerbodytranslations. OptionalUTF16args,
  Environmentvectorofalready-formed-mapentries; fluentusingConfFile/Env/Args,
  configure,isDaemon,isShutDownHook,checkParams(String/Args). Sourceargumentlookahead
  andunknown-/NULLfailureorder; flagsnotfollowedbyvaluegettrue; nextNULLthrows;
  negativeCLIvalueparsedasunknownflag. EnvdefaultlocaleUnicode-lowercaseprefix,
  duplicatecollectionBEFOREputAll,unknown-keyduplicateerrorspreserved. Configfirst
  -confonly,no-trimkeys/values,String.split=droptrailingempty,all-equalslinearray
  error,duplicatecollectionbeforeputAll; checkedopenIOprintln+exit1vsunchecked
  traversalIO. AllconsumerschangeACTUALGeneral/Global/Test/P2Pfields andnativeRPC.
  Directorycreation sourcepathspellingstored; mkdirresultignored. Sourcehelp
  exact%-20s%-15sformat/status1. checkParamsmutatesglobals asactualsource.
  Defaultstdout/std::exit; enumidentityhashstableOSrandomperprocess67keys;
  RuntimeenumHashreceiverallowsactualJavahashbindingfortests. JavaHashMap reused;
  ordinal-paddedhash-equalitykeysreplicateEnum.compareTotiesratherthannameorder.
  configuredetectsnewkeymodcountafterconsumerloop,butfullreentrantiterationNOT
  equivalent(see gaps). No fakeMain/DB/SSL/JNIimplementations; flagsmerelysetsource
  globalsasParamConfigurerdoes.
- ConfigfileIOnowPOSIXopen/fstat/read+RAIIclose; captureinitialfilefenceforpositive
  size<=INTMAXasFiles.linesFileChannelSpliterator. Nativefstreamearlierincorrectly
  treateddirectoryreadasEOF; POSIXreportsIOExceptionIsadirectorywrappedunchecked.
- support/UTF8Spans.hpp extractedEXACTexistingMiniStringmalformedprefixloop;
  MiniString::normalizeUTF8 nowconsumesitwithsameU+FFFDreplacement.
- support/StrictUTF8CharacterReader.hpp: wrappedReadableByteChannelStreamDecoder
  behavior,8192bytebuffer,validateentirefetchedblockBEFOREpublishingchars; incomplete
  prefixcarriedtofollowingread; ifcharsproducedreturnswithoutanotherchannelread
  (sourceinReadyfalseforwrappedRBC). TypedMalformedInputExceptionlengthpreserved.
  ReusesJavaUnicode/MiniStringvaliddecoding. SocketCharacterReader gainsoptional
  decoded-charactercallback soFiledecoderreusesALLexistingCRLF/readLinerules.
- support/JavaInteger.hpp testedBMPu_digitaccumulatorextractedtemplateandextended
  tojavaParseLong; signedMINhandledwithoutoverflow. ExistingjavaParseIntretained.
- MiniNumber.cpp stringctor mapsBMPUnicode decimaldigitstotestedASCIIgrammar,
  rejectsothernonASCII/supplementarycharssameasBigDecimalcharparser; invalidnumber/
  exponent/scaleNumberFormatException; exponentusesjavaParseIntreplacingstoi.
  Existingnumericmath/rounding/rangealgorithmsreused. Fullresource/extreme-scale
  behaviorstillunverified(e.g.regex/verylargedecimalinputs).
- JavaLowerCase.hpp UTF16overloadextracted; previousUTF8APIwrapsitunchanged.
- JavaConcurrentMap.hpp UTF16keyget/put;reserveForPutAllJDKpresizing+firsttablebelow16;
  mAllocateddistinguishesneverallocatedfromclearedexistingtable; correctfloor.75
  thresholdforsmalltables. PreviousordinaryString/ConcurrentMapAPIretained.
- JavaUnixFile.hpp javaUnixMkdirs: exists/mkdirfirst,weakly_canonicalabsolute,
  recurseparentthenmkdircanonicalpath; noextra"dot"dirfordot/../inside;rootguard;
  NUL/emptyfailboolean. Existingpathconstructorsreused. OldMiniFile/SqlDBother
  directcreate_directoriescallsNOTchangedbythismilestone.
- JavaExceptions.hpp JavaTypedException originalclassname+ownedfullmessage inclNUL
  +constructionstack;javaExceptionTextrendersit. ParamUnknownArgumentException/
  ConfigurationExceptionderiveit. Otheroldtypedclassesunchanged.

REUSEGATE/READS:
FullpinnedParamConfigurer553,TestParams81andParamConfigurerTest5methodsread;Minima
startupcaller96..140read. FullnativeGeneralParams/GlobalParams/P2PTestParams,
JavaConcurrentMap/JavaHashtable/JavaInteger/LowerCase/FinalSigma/UnixFile/MiniFile/
MiniString/decoder/RPC/loggerandrelatedfixturesread; siblingtools/minima-core-cpp
searchednoParamConfigurer/TestParamsimplementation. Exactreusereportbeforeedits.
OriginalMinimaNumberstringconstructor/checkLimitsandnativefullrelevantparserread
whencomparisonfoundUnicode/exceptiongap; existingJavaIntegerreusedafterreport.
InstalledJDKsrc.zipreadHashMap.putMapEntries,File.mkdirs,Files.lines+factory,
FileChannelLinesSpliteratorreadLine/getBufferedReader,StreamDecoderreadBytes/
implRead/inReady/constructorsandBufferedReaderfill/readLine. Nativeadaptationsuse
existingportandPOSIX/std/ICU; productionnevercallsJava/JNI.
Do NOTrerun/tmp/minima_param_adapters.py,/tmp/minima_param_generate.py,
/tmp/minima_param_fixture_generate.py: one-shotwritersandnowstale.

TESTS:
- support/ParamConfigurerOracle.java compilesALLunchangedpinnedproduction. Actual
  ParamConfigurer/TestParams; reflectioncapture/restoreGeneral+Global+Teststatic
  fields andP2PLOG_LEVEL. ReflectionprivateparameterMapentries; firstmetadata
  returnsactual67enumidentityhashes/helpstrings. English/TurkishLocaleperrequest.
- support/param_configurer.cpp usesactualnativeglobals/defaultHTTP/loggersand
  ParamConfigurer; generatedsupport/ParamSnapshots.hpp reflectsnativefields;
  friendParamConfigurerTestAccessreadsmap; JavaenumhashessuppliedthroughRuntime
  forcomparison. No differentproductionmapalgorithm. Errorrendererfinalusesactual
  nativeExceptionText (oldfixturemanualwhat()couldtruncateNUL;removed+caseadded).
- tools/param_configurer_differential.py #106TIMEOUT300:719programsPASSBOTH,
  67handlers,4actualHTTPrequestsPERruntime,3processchecks. Fullsettings/maporder/
  errors/sideeffects,repeatconfigure,English/Turkishenvcase,duplicatecollection,
  args/env/conffileprecedence,32/64bitbounds,BMP/supplementarydigits,checkParams,
  TestParamsmutations/application,constantcaptureonfirstclassuse.119ishconf
  inputsincl96padding/UTF8boundarycases8177..8193/16383,duplicatevsdecodeerror
  precedence,BOM,CR/LF,NUL/dirread. Separateownedtemp"effects"treeclearedbetween
  Java/native; exactdirectoryentrysnapshotsincludeordinary/dot/symlink/file-parent/
  NULpaths. P2PnodesactualHTTP200/404/drop+retry+errorlogging. Noexternalpeers.
  ProcesshelpersParamConfigurerProcess.java/param_configurer_process.cpp compare
  exacthelpstdout/status1,missingconfstdout/status1,first-useversionbefore-TEST
  remainsafterGlobalversionchanges. Thesehelpersaretestfixtures,NOTnodeexecutables.
  SelectedVMnumeric/NPE/indexdiagnosticTEXTnormalized; classes/state/sourcecontent
  retained. Javaidentityhashvaluesarecontrolledfororderequivalence,notclaimed
  identicalnativehashgeneration. Fullnode/JVMsemanticsnotproven.
- test/org/minima/system/params/ParamConfigurerTest.cpp mappedALL5originalmethods,
  runtimeassertions(notNDEBUGassert),freshGeneralParamsbetweenmethods;PASSboth.
- support/ParamConfigurerOriginalTests.java invokesactualunchangedJUnitannotated
  source5methodswithfreshGeneralParams; expectedexceptionannotationsrespected.
  upstreamlib/junit-4.13.jaravailable; noJunitdependencydownloadrequired. PASSboth.

BUILDS/TESTS(allterminal):
17183initialfullnormalPASS;54737fixturesPASS;18579initialfullsanPASS.
50271FAILcase235:MiniNumberinvalidtextwrongexception;fixedtyped+Unicode.
59199decimalbuildPASS;53561MapresizePASS;76451typedexception/mappedfullPASS;
83765SANcurrentPASS;81991processversiontargetPASS.
78953FAILcase427extra"dot"directory;nativeoriginal5mappedPASS0.34s.
55409mkdirfullPASS;47960rootguard/enumtiesfullPASS;23836reviewedsanPASS.
51907FAILcase502decodererrorprecedence;JDKbuffersinspectedandstrictreaderadded.
39184bufferfullPASS;72446710casesPASS4.82;38965sanbufferfullPASS.
37868expandedpathfullPASS;82587FAILcase610directoryreadsilentlyEOF;44607sanpathPASS.
23113POSIXfullnormalPASS;72060POSIXfullsanPASS.64993eighteenNORMALsuitesPASS149.99s;
23407same18SANPASS259.21s. Suites:source_mapping,java_differential,json,kiss,
 kissvm,utility,prefs,hashmap,command,command_runner,peer_state,nio_client,
 http_server,cmd_handler,RPC,minima_rpc_cli,param_configurer,ParamConfigurerTest.
NormalParam718cases4.63/mapped0.35;SANParam7187.48/mapped0.42. NIO44.00/107.89;
peer_state37.96/55.23;hashmap16.94/20.05. AllpassafterFINALproductionchanges.
Finalfixtureusesproductionrenderer:48879/54709targetbuildPASS;24080normal719cases
PASS5.07;47358SAN719PASS7.88. No laterproductionchanges. Logsfinalalltargetbuilds:
/tmp/minima-config-posix-build.log and/tmp/minima-config-posix-sanitized-build.log.
README/REVIEW/provenance/source-mapupdated273/184. Finalgitdiffcheck/source_mapPASS.

LIMITS/NEXT:
FullnativeMinima/Main/H2MVStore/nodeABSENT. Existingglobalparameterfieldsstill
nativevalueString/MiniNumber,notallNULL/unpairedUTF16/referencealias/concurrent
updates. APIEnvvectorrepresentsunique-mapentrysnapshot; futureSystem.getenv
adaptermustconstructthatmap. Reentrantconfigurationtraversalpartiallydetected,
notfullJavaHashMapliveiteration. Huge/special/changingconfigfiles/JVM-global
locale/PrintStream/error/resourcebehaviornotfullyestablished. RAIIclosesfile
whenunreachable; sourceunclosedstreamcleanupcollectiontimingunspecified.
PotentialnextnativeMinimaUncaughtException/defaultthreadhandlerworkhasNOTbeen
implemented. FULLsource60lineswasreadprevious270turn; searchcurrent+siblingthen
foundnoUncaught/OOM/MVStore/nativeThreadboundary. NeedreadactualMessageProcessor/
Timer/MinimaThreadcallersandtests+reusegatebeforedesigning/implementingintegration.
Previous270checkpointbelowrecordsUncaughtsourcealgorithm. ParamConfigurer/TestParams
areNOWimplemented; do notredoitsoldpreparation. Mainpinned1379linesnotported;
H2nativebackendstillabsent,noSQLite/JVMsubstitutionauthorized. Keepgoalactive.

# Latest continuation — 270 counterparts; native RPC CLI and exception frames

Goal ACTIVE/unbudgeted/incomplete/unblocked.270=251production+19mappedtests;
187absent. Allpartial/not_verified. FullnodeNOTcomplete. Pinunchanged5564b9d78978efcaf23679190d05253961fc6e69.
NO LIVE SESSIONS. Noagents/commits/pushes/productionnode/wallet/remotewebhooks.
Allhandlesbelowterminal. Skillsreuse/graphify/minima-reference/code-reviewcontinued;
reuse/code-reviewrereadthisturn. Userasleep; approvedCTestprefixesusedwithoutactual
prompt/rejection; no selfapproval. Gitstatusonlyuntracked.DS_Store,.gitattributes,
cpp/,graphify-out/,upstream-study/. Preserveeverything.

NEWPRODUCTION:
- src/org/minima/utils/MinimaRPCClient.hpp/.cpp andsupport/minima_rpc_main.cpp.
  Executablecpp/build/minima-rpc fullynative. Optionalvector<optionalUTF16> args;
  NULLarray/flag/host/commanderrororder; sourceflags/help/unknown/missingvalues;
  hostslash; defaultminima/passwordsentEVENwithoutpasswordflag; exact13linebanner.
  AllsourceACTIVEalgorithmtranslated; SSLsetupcommentedinsource -> remainsfalse.
  OnecommandNOTtrimmed; interactivechecknonemptyBEFOREtrimcodeunits<=32; spaces
  sendrootrequest; exitlocal; quitbreakONLYafterrequest+parseobject+pretty succeed.
  UsesactualRPCClient::sendGETBasicAuth/JSONParser/MiniFormat/MinimaLoggerdefaults.
  Scalar/arrayJSONCCE,NULLJSONNPE,ParseExceptiononlycaughtsinglemode/stderr;
  interactive catchesstdexceptionandlogsnativeThrowable; closeIOstringonly.
  URLEncoderUTF8adaptationusesexistingJavaUnicodeencodeUTF8+safeASCII.-*_alnum,
  spaceplus,uppercasepercentotherbytes; unpairedUTF16replacement?then%3F.
  RuntimeboundaryInput/readLine/twocloseforms,println,printStackTrace; omittednative
  defaults. StandardInputusesPOSIXreadavailablebytes,EINTRretry,oneidempotentclose.
  mainreturnsSystem.exitstatus; executabletop-levelprintsuncaughterror+frames.
- support/SocketCharacterReader.hpp extractonlybyte-readcallback; existing
  socketconstructorwrapssamenioReadSome. NewreadLineUTF16 preservescodeunits;
  existingUTF8readLinewrapsit. OptionalretryEOF=falsekeepssocketbehavior;
  stdinrequestsTRUEeachlinebecauseCtrl-DtemporaryEOFmustpermitlaterinput.
  ReviewfoundstickyEOFbugandfixedafterreadingJDKStreamDecoder.implRead (EOFis
  localflag/resetdecoder). Native/JavaptyCtrl-DthenexitbothPASS.
- support/NativeStackTrace.hpp: usesinstalledexecinfo backtrace construction-time
  capture,noinlinehelper,grows64bufferuntillongerthanstack(intAPIlimit). Captured
  addressesretainedthroughcopies/rethrows; backtrace_symbolsfreedwithRAII.
- ByteIO.hpp IOException,sharedJavaExceptions.hpp typedclasses,andParseException
  nowderiveNativeStackTrace. AddedJavaIllegalArgumentException/ArrayIndexOutOfBounds;
  RPCcheckHeaderthrowscapturedIllegalArgument (sameoldrenderertext).
- support/NativeThrowable.hpp snapshotoriginalexceptiontext+recordednativeframes;
  IOusesownedmessagepreservingNUL,ParseExceptionclass+getMessage,othersrenderer.
  ImplementsloggerThrowableandstderrprintStackTrace. No manufacturedcatchframes.
  Nativeframecount/namesnotliteralJVM; standarduncapturedexceptionsremaingap.

REUSEGATE/READS:
FullpinnedMinimaRPCClient197linesreadrepeatedlybeforeedits. Fullrelevantexisting
RPCClient/nativefixtures,MiniFormat,MiniString/JavaUnicode,JSONparser/value/exception,
MinimaLogger,JavaExceptions/ByteIO/IOExceptionrenderer,SocketCharacterReader/
NIOSocketandNIOreadercomparisonsread. Siblingtools/minima-core-cppsearchednoURL
encoder/stackcapture. Exactreusereportprovidedbeforeedits. InstalledSDKexecinfo.h
FULLread; onlylinkedAPIused,noimplementationcopied. PriorJDKURLEncoderFULLread;
thisturnStreamDecoder.implReadreadfromOpenJDK26.0.1src.ziptoresolveEOF.
ExistingRPCHandler/Server/Runnerreused; Servergainsoptionalhandler(defaultunchanged).

TESTS:
- support/MinimaRPCClientOracle.java actualunchangedpinnedmain,nullableUTF16args,
  controlledFilterInputStreamoverbytes,readfailonce/closefail,stdout/stderrcapture.
- support/minima_rpc_cli.cpp mirrorsinputsusingactualsharedreader; production
  HTTP/JSON/logger. RequestsconsumeexistingJSONlineRunnerprotocol.
- tools/minima_rpc_cli_differential.py #105 TIMEOUT300:89cases/70HTTPrequestsPER
  runtime,10realexecutablechecks. Bothnormal/sanitizedPASS. Missing/NULLargs,
  repeatedflags,defaultauth/null/emptyunicodecredentials,safeASCII/percentUTF8/
  unpairedsurrogates/NUL/control/8200bytecommands;object/scalar/NULL/invalidJSON,
  statuses,sourcebanner,whitespace/CR/LF/UTF8inputboundaries,quitfailurethencontinue,
  readerrorrecoveryandcloseerrors. NativepipeprocessrespondsbeforeEOF;permanent
  EOFretainsloop(explicitlykillfixture); actualptyCtrl-Dthenexitresumesandstops.
  Heap/dateprefixandVMdiagnostics/nativeframecount/textnormalized. Reviewfixed
  overlybroad5-spacefilter: onlylogger-prefixedframesandstderrframesremoved,
  JSON/controlwhitespacepreserved; deepnestedJSONcaseadded. FramePRESENCEcompared.
- support/native_stack_trace.cpp #104 TIMEOUT30:160+recursiveframes,originalthrow
  symbolpresence,copy/throwaddressesexact,fullNULIOmessage,parser/typedstackcapture.
  normal0.19/san0.60PASS. NoTSAN. Cause/suppressed/standardexceptionsnotcomplete.

BUILDS/TESTS(allterminal):
38971initialfullnormalPASS;92116newfixturesfullPASS.33568fullsanPASS(log/tmp/minima-cli-sanitized-build.log).
90102native_stackPASS/JavafixtureFAILrawJSONArray.toArrayreturnsObject[]; fixedcast.
17690CLI83cases64requests8processPASS4.60s.1227tightenednormalizer85cases66requests
PASS2.93s.43406tenaffectednormalPASS53.79s:malformed,source_map,primitives,json,kiss,
NIOclient714cases13972ops41.47s,nioloopback,RPC,loggercomparison/worker.
2299/77229readfailurefixturebuildPASS.40330expandedCLI89cases70requests8processPASS3.31s.
69577twelveaffectedSANPASS128.36s,NIO107.97s,RPC4.69,logger3.40/worker0.69,
stack0.60,CLI4.35(rebuiltfixturebeforetestreachedit).
FinalEOFcorrectionbothALLtargetbuilds97966/40357PASS;logs/tmp/minima-cli-final-build.log
and/tmp/minima-cli-final-sanitized-build.log.6459normalfourPASS13.32s;85561SANfour
PASS14.11s:nioloopback,http_server,cmd_handler,CLI. FinalCLI89/70/10PASS5.47s/5.75s.
DocsREADME/REVIEW/provenance/source-mapupdated270/187. GoalNOTcompleted.

REMAINING/LIMITS:
Main/nativeH2MVStore/fullnodeabsent. Sharedtypedexceptionscaptureframesbutnotall
standardexceptions,causes/suppressed,fullJVMErrorclassification/resource/PrintStream
semantics. POSIX/nativeframeportabilitytestedonlymacOShere. Mutablecharset and
RPCURL/header/TLS/globalnetworkgapsremain. No claimsfullAPIparity/fullnode.

NEXTREADONLYPREPARATION(noadditionalimplementationswritten):
FULLpinnedMinimaUncaughtException.java60lines,TestParams.java81lines,ParamConfigurer
.java553linesread. FULLnativeGlobalParams.hpp/.cpp,P2PTestParams.hpp/.cppread.
Searchcurrent+siblingUncaught/Timeout/OutOfMemory/MVStoreexception/nativeThread
adapterfoundonlyloggerandmessageprocessorstdthreadnames; noexistinguncaughthandler.
MinimaUncaughtException hasstaticdefault capturedatEACHconstructor,perinstance
lastErrorinitial***; throwable.toStringbeforecheck; NULLlastErrorlaterNPE;
logthreadname+logUncaught,thenassignlast;OOM/MVStore/Timeoutlogspecific+Runtime.halt0;
elsecallstaticdefaultWITHOUTnullcheck(sourceNPEifabsent). Needsactualnativethread/
throwabletype/default-handler/haltintegration; don'tfakeH2backend.
ParamConfigurerdependsTestParams andalreadyportedRPC/Logger/General/P2Pparams.
usingConfFile first-conf+nextvalue,split=droptrailingempty,untrimmedkeys/values,
collectduplicatesfailBEFOREputAll; IOExceptionprint+exit1. Envdefaultlocale
lowercase,minima_prefix,stripping/duplicatecollection. argslookaheaddashesunknown
canthrow;configureHashMapENUMidentityhashiterationorderaffectsmultiplesetting
sideeffects. Full553activeconsumersread(includeshelp,mkdirs,P2PURLfetch,test/solo,
SSL/JNIflags). Needreusegate+candidates/dependencies/testsreportbeforewriting.
TestParamsversioncapturesGlobalParams.MINIMA_VERSION+-TESTatclassfirstuse;
existingnativeP2PTestParams::values()lazyfieldsisreusablepattern. NativeGlobalParams
currentlymutablevaluestaticstrings/MiniNumbers; NULLreferenceparitynotprovided.
Siblingsearchattemptsrc/paramsdirectoryabsent; broaderfilenamequeryfoundnone.
No codewrittenforthesenextclasses.

# Latest continuation — 269 counterparts; native logger

Goal ACTIVE/unbudgeted/incomplete/unblocked.269=250production+19mappedtests;
188absent; allpartial/not_verified. FullnodeNOTcomplete. Pinunchanged5564b9d78978efcaf23679190d05253961fc6e69.
Noagents/commits/pushes/productionstartup/remotewebhooks. NO LIVE SESSIONS.
Allhandlesbelowterminal; do NOTpoll/restart. Prior268RPCturnwasverifiedprogress.
Skillsreuse/graphify/minima-reference/code-reviewcontinued. GraphJSONinventoryread
forMinimaLogger/MinimaRPCClient/MiniFormat/MiniUtil nodes (actualsourcepaths/lines).

NEWPRODUCTION:
- src/org/minima/utils/MinimaLogger.hpp/.cpp.
  MINIMALOGconstant; fixedDateFormatdd/MM/yyyyHH:mm:sscapturesinitialdefaultzone.
  AllsourceString/Exception/logUncaughtalgorithmtranslated. UTF16optionalString
  preservesNULLandcodeunits; UTF8/constchar*adapters. logStringdefaultnotifytrue.
  Runtime.totalMemory THENfreeMemory THENcurrentTime/dateformat THENprintln;
  signedlongheapdifferenceusesunsignedarithmetic+bitcast. FullprefixMiniFormat.
  Ifnotify&&getMainnonnull,allocateJSONObject(message=fullUTF16),SECONDgetMain;
  secondNULLthrowsJavaNPE,elsePostNotifyEvent(MINIMALOG,SAMEsharedJSONObject).
  ThrowableinterfacevirtualtoString()->nullableUTF16 andgetStackTrace()->nullable
  vectorofnullableUTF16frameStrings. logExceptioncallslog(toString),THENfetchframes,
  logeachwith5spaces;NULLexception/list/elementsourceNPE. Uncaughtprefixexact.
  ThisinterfaceconsumesRECORDEDframes; automaticthrow-sitecapturefornativeexceptions
  NOTimplemented. No inventedcatch-siteframes/noJVMproduction.
  Runtimeget/setservices fillomitteddefaults: nativeclockstdchrono,allocatorheap,
  UTF8stdoutlineunderoutputmutex,absentMaininitiallyreturnsnullptr. macOSmalloc_zone_
  statistics(nullptr)sumallzones:size_allocatedandallocated-size_in_use;glibcmallinfo2
  alternatebranchUNTESTEDhere. Otherplatformrequiresheapbinding(throws,notfake0).
  Registryimmutable sharedRuntime snapshots undermutex; callbacks copiedandold
  capturedownersdestroyedOUTSIDElock. Reviewfixedpotentialreentrantdestructordeadlock.
- support/JavaCalendarFields.hpp extractedexistingtestedJavaDateString calendar
  calculationunchanged;optionaltimezoneIDargument. ReturnsownedICUcalendar,tm,
  year/month/day/offset/raw/dst. ExistingJavaDateString.hpp consumesfieldsandkeeps
  originaldisplay-namebehavior. Loggerusesnumericfields. MiniUtilunchanged.
- NotifyManager.cpp nowdefaultlogcallsMinimaLogger; onlylistener+releaseLocks
  required,existingnativeRPC/UserDBdefaults. Mainreceiverruntimebindingstillneeded.

REUSEGATE/EVIDENCE:
FULLpinnedMinimaLogger.java77linesandMinimaRPCClient.java197linesreadagainbefore
work. Mainnotificationmethods1310..1408read. ExistingMiniUtil.hpp/.cpp,MiniFormat
.hpp/.cpp,JavaDateString.hpp,JavaUnicode/JSONValue/JSONObject/ParseException,
SocketCharacterReader,JavaExceptions,MinimaDBglobalreferencesandservices,Notify
manager/workerandNIOClientOraclehelpersread. Completeprefs/utilitycomparisoncode
read;relevantNIOtimezonecomparisonread. Siblingtools/minima-core-cpp searched
logger/date/heap/URLencoder candidates,noequivalentlogger. Reusereportbeforeedits
namedexactpaths,selectedcalendar/formatSize/JSON/worker/globalreferences.
InstalledSDK/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/malloc/
malloc.hlines526..540/600..618read: statisticfieldsandnullzonesumsallzones.
NoJDKsourcecopied. InstalledJDKURLEncoder.java328linesFULLreadforNEXTCLIpreparation,
notimplementedyet (detailsbelow). DatecalculationpriorJDKderivationalreadydocumented.

NEWTESTS:
- support/MinimaLoggerOracle.java extends existingNIOClientOracleUnsafehelper.
  CompilesALLunchangedpinnedproduction. MainOwnerconstructorbypassed;controlled
  PostNotifyEventstoresSAMEdata,canmutate/rebind/throw/nest. DateFormatfinalfield
  replacedtestonlyusingUnsafewithSimpleDateFormatsubclasscontrolledclock;original
  formatterretainedfordirectdatedifferential. PrintStreamprintln(String)controlled
  beforeencodingcapturesUTF16/NULLprecisely. ExceptionoverridestoString/getStackTrace
  controlled,actualStackTraceElementsuseFixture.<name>(Fixture.java:12).
  NativeheapdefaultnotJVMheap: dynamicheapportionnormalizedexternally.
- support/minima_logger.cpp actualnativeLoggerfixtureusingequivalentreceivers.
  JSONsharedpayloadretainedviaJSONValue.value=sharedptr (notcopyofJSONObject).
- tools/minima_logger_differential.py (#102 TIMEOUT240):2016casesPASSBOTH. FourTZ
  processesUTC/London/NewYork/Tokyo;384directoriginalDateFormatcasesinclLongMIN/MAX,
  pre1582/pre1900,2^53boundary,seededrandomsignedlong. Remaining1632loggingcases:
  default/explicitnotify,String/Exception/Uncaught,NULLtext/error/frames/elements,
  UTF16surrogates/NUL/CRLF,callbackfailures/toString/getStackTraceorder,Mainset/clear
  atprint/post,nestedlog,sharedpayloadmutation. NormalizeONLYheap-prefixportionand
  automaticNPEdiagnostics;sourcecontent/trace/dateexact.
- support/minima_logger_worker.cpp (#103 TIMEOUT30):400concurrentprint-before-notify
  pairs,800Mainlookups;secondlookupdisappearsafterprintthrows;heapwrapMIN-1->MAX,
  total/free/time/printorder;actualnativeheap/clock/stdoutdefaultswithabsentMain.
  RuntimeFinalLogcapturedownerdestructorlogswhenlastownerretired; proveslockreleased
  beforedestructionandnewruntimepublished. ActualNotifyManager(noHTTP/logoverride),
  UserDBemptyhooks,listenerthrowsTEST -> defaultLogger -> controlledMainreceiver
  buildsnotification -> actualworkerreceivesMINIMALOG.2tasks,latch,join,oneoutput
  andoneMINIMALOGdelivery. MainitselfNOTimplemented. NoTSAN.

FIXES/VALIDATION(allterminal):
Initialproductionfull27859PASS. ExplicitnewtargetbeforecmakeregenfailedNoRule;
configured. Initialfixture54474FAILsharedptrcannotimplicitJSONValue;sourceAPIread,
fixedvalue.value=data.49226testFAILmissingnativebinary(runstartedafterfailedbuild;
notbehavioral). Initialsan91158samefixturecompilefailure. Fixednormal4445PASS,
san90435PASS. #102normal27716PASS3.05s/san10650PASS3.73s(2016cases).
AfterNotifydefaultlog+worker,full33928/91204PASS. Sevenregressionsnormal68324
PASS57.40s,san4936PASS130.06s:source_mapping,nio_client,notify_managercomparison/
worker,rpc_client,loggercomparison/worker. NIO714cases/13972opsPASS44.82s/115.27s.
Duringreviewruntimecallbackreleaseunderlockfixed;full62886/73034PASS. Normalfinal
loggerpair85071PASS3.20s(2.72s/0.48s). Sanitizedseven-suitehandle4936wasstillrunning
NIOwhenfinalcorrectedbinarybuilt;laterlogger3.47s/worker0.79spassedthatbinary.
Sanworkerfilemtime13:38:14,LastTest.logloggerworker13:38BSTPASS. No rerun needed.
Source_map269/188PASS;gitdiffcheckPASS. README/REVIEW/provenanceupdated. Entirecpp
untrackedunchangedgitstatusshape. No actualapprovalrejection/blocker. Approved
CTestescalationusedforRPCloopbackregression. Allbuild/testsessionsclosed.

REVIEWLIMITS:
LoggerThrowablesneedoriginalnativeexceptionframecapture; fullmutableSimpleDateFormat
API/timezonechangesandJavaresource/PrintStream/globalstateerrorsNOTcomplete.
Fixedpattern/captureddefaultzoneiscovered; nativeDateFormatinitializationtiming
acrossC++globalinitializersversusJavalazyclassloadingnotfullproof. glibcallocator
branchuntested. RuntimebindingsmustclearonorderlyMainshutdownbeforereceiverstate
isgone. Main/fullMinimaRPCClient/nativeH2/MVStore/fullnodeabsent. AllpreviousRPC,
JSON/UserDBcrossworkerandotherglobalparitylimitsremain. CounterpartcountNOT100%.

NEXTMinimaRPCClientpreparation (NOCLIcodewritten):
Fullpinnedsourceaboveandprevious268checkpoint. ExistingRPCClientnativevalidated;
MiniFormat.JSONPretty/jsonparseractualsource+testsread. SocketCharacterReader FULL
read:8192buffer,callbackcurrentlytiedsharedPosixNIOSocket/nioReadSome; UTF8framing
+UTF16surrogate/readLineCRLF/skipLF alreadytested. Couldextractbyte-readerboundary
forstdinreuse; DONOTuseistream.read(8192)thatwaitsforfullbufferoninteractiveinput.
ExistingPosixreadreturnsavailablebytes; productionstdinread(0,..)appropriateonce
sourceinputwrapper/close/errorsemanticsinspected. Noactualreaderadaptationwritten.
NativeURLEncodercandidateabsent incurrent+siblingsearch. InstalledJDK
java.base/java/net/URLEncoder.java FULL328linesreadfromOpenJDK26.0.1lib/src.zip:
UTF8safeASCIIA-Za-z0-9 .-*_ (space ->+);allotherUTF8bytespercentUPPERHEX;isolated
surrogatescharsetreplacement? thenpercent3F. SourceusesCharsetencoderREPLACE,
8-charbufferandpairskepttogether;MiniString/JavaUnicodeexistingencodingreusable.
NoJDKimplementationcopied. BeforewritingCLI,reportexactreuse/minimumadaptations.
ImportantCLIsemantics: -sslpubkeyparsed/displayedBUTSSLsetupcommentedsoalwayssslfalse;
defaultminima/passwordsentEVENbpassfalse;bpassonlybanner. Hostslashadded. Onecommand
NOTtrimmed;interactivechecksnotnull/notemptyBEFOREtrim,soall-spaceslinecallsrootRPC;
exitcheckaftertrim;quitcheckafterURLencodingANDsuccessfulJSONPretty. EOFspinsassource,
notexit. Help/unknownparameterSystem.exit(1);missingvalueuncheckedarraybounds.
Exceptionlogging/printStackTraceoriginalnativecaptureisstillmissingruntimework;
donotpretendcatch-sitebacktracesarethrow-siteframesorremoveerrorpathsforpassingtests.

# Latest continuation — 268 counterparts; native RPC and live webhooks

Goal ACTIVE/unbudgeted/incomplete/unblocked. 268=249 production+19 mapped tests;
189 absent; all counterparts partial/not_verified. Full native node NOT complete.
Pin remains5564b9d78978efcaf23679190d05253961fc6e69 (457Java,369production/88tests).
No agents, commits, pushes, production node/wallet/miner or remote webhooks.
NO LIVE BUILD/TEST/EXEC SESSIONS. Every handle below is terminal; do not repoll.
Skills reuse/graphify/minima-reference/code-review applied; reuse/code-review reread.

NEW PRODUCTION:
- org/minima/utils/RPCClient.hpp/.cpp: ten active source public method forms.
  Optional string null/empty distinction; source literal https dispatch/casts,
  Basic UTF8/Base64 and literal auth token, GET/PUT/POST, nullable customtype.
  200-only body MiniString UTF8/readLine concatenation; other GET/PUT print/empty;
  POST IOException retains originalURL/parameters even after POST->GET redirect.
  HTTP1.1,10s connect,no read timeout,no environmentproxy,sourceUA/Connectionclose.
  Native SSLContext is CURL connection-options callback (CAINFO tested), NOT full
  JDK provider API; default cert+hostname verification retained. sendGETSSL delegates
  NULLcontext and throws as source. POST NULLparams connects before its NPE.
  Manual20redirectlimit,sameprotocol300..307except304/306. POSTexcept307 resetsALL
  headers/method (UA Java/26.0.1,Connectionkeep-alive), othercrosshost/port dropsauth.
  305 usesproxyhost/port andcachedrequesttarget; suppresscurlProxy-Connectionheader.
  URLuserinfo neverautomaticallyauthenticates (CURLOPT_HTTPAUTH CURLAUTH_NONE).
  Emptyquery/pathdot/CR/rawUTF8/fragment handling uses CURLU+initialrawtarget.
  NULURL preserveduntilsourceproxy-selectionphasefailure, notsilentlytruncated.
  Droppedconnectionretryonce;101retrythenProtocolExceptionexact;PARTIAL_FILE maps
  exactIOExceptionPrematureEOF; typedconnect/socket/TLSerrors(providertextnormalized).
  USER_AGENT now mutex snapshot/assignment adapter, copied atrequestconstruction.
- support/CurlTransport.hpp header-only extraction from actualtestedP2PEnvironment:
  singlecurlglobalinit,RAIIhandles,headers/status/location/bodycallbacks,URLparts.
  P2PEnvironment retainsoldheader-onlybehavior throughsharedhelpers. RPCbodyread
  only200;callbackabortsnon200headers and101. P2Pregressionpassednormal/san.
- support/ByteIO.hpp IOException ownsfullstdstringmessage; inheritedwhat()onlyfor
  conventionalstdexception. message() preservesNUL. TypedIOException retainsJava
  className. JavaIOExceptionText consumesfullmessage; existingEOFformatunchanged.
- NotifyManager.cpp nowdefaultsendPOST callsnativeRPCClient; stillrequireslistener,
  log,releaseLocks; actualMinimaDBUserDBdefault remains. HTTPinjectablefortests.

NEW TESTS:
- support/rpc_client.cpp JSONlinefixture invokesactualRPCclient withcapturedstdout,
  customCAviaSSLContextcallback,nullableargs/agent,typedIOexceptionrender.
- support/RPCClientOracle.java actualunchangedALLpinnedproduction compiled; only
  oraclecode controlsargs/capturedstdout andbuildstestedCAKeyStore/SSLContext.
- tools/rpc_client_differential.py (#101 TIMEOUT300):477cases/597requestsPERruntime.
  Threeephemeralloopbackservers(HTTPorigin,otherhostport,HTTPS); realTLSCAgenerated
  opensslselfsigned2daylocalhost/IPcert inbuild/rpc-client-tls (testonlykey).
  Casesallmethod/statusmatrix,redirectmethod/headerreset,authstripping,same/cross
  scheme/port,userinfo,305proxy,20looplimit,drop/retry,101,chunked,truncatedbody,
  empty/binary/invalidUTF8/BOM/CRLF/NULbody,nullableauth+APIordering,UAfold/newline,
  URLNUL/CR/LF/emptyquery/dotpath/rawUTF8/space/fragment,TLSreject+configuredCA.
  Comparemethod/path/headerVALUEdict/bodybytes andoutput; headerORDERnotcompared.
  NPE/CCE/MalformedURL/ConnectException/SocketException/SSLHandshakeException
  diagnostictextsnormalizedtoCLASS; sourcePOST/Protocol/PrematureEOFtextsexact.
- support/notify_http_worker.cpp compiledtarget invokedbysamePythonfixture:
  actualownedNotifyworker+UserDB+Message/JSON, NOsendPOSToverride. Twoeventswith
  listenerJSONmutation,filtering,404failure/recovery,307redirect ->7actualHTTPPOSTs
  and2exactloggedfailures. Workerjoinbeforeoutput. Added4threads/4000USER_AGENT
  assignments+snapshotreads(8192-bytealternatingstrings),thenresetdefaultbeforeHTTP.
  NoTSAN. NativeintegrationbehaviorassertionsindependentofJavaNotify; separate
  #99Notify527programs/2667operationsalreadycompareactualJavamethods.

REUSE/PRIMARYSOURCEEVIDENCE:
CompletepinnedRPCClient/MinimaRPCClient/MinimaLogger/NotifyManager read; production
callerblocks ParamConfigurer322..352,base/tokenvalidate90..158,incentivecash35..90,
network/peers98..142/182..219,Mainnotification1310..1400/listener67..84.
NativeP2PEnvironment.hpp/.cpp,Base64,MiniString,Notify/worker/UserDB/globalMinimaDB
snapshotpattern inspected. ExistingP2Pfunctionsloopback+peersCheckerRunnerreused.
Siblingtools/minima-core-cpp searched,noRPC/Notifyequivalent. Reusereportdelivered
beforetransportextraction, beforeNotifyandbeforeUSER_AGENTsnapshotadaptation.
JDK26.0.1zip:/opt/homebrew/Cellar/openjdk/26.0.1/libexec/openjdk.jdk/Contents/Home/lib/src.zip
RelevantHttpURLConnection URL/header/write/IO/redirect/auth/retry/proxy source read;
HttpClientparse/getURLFile/101,NetworkClientencoding,MessageHeaderprint,
ParseUtiltoURI/createURI/quotecomposition,MeteredStreamfirst195lines read.
Onlybehaviorstudied; noJDKimplementationcopied. curl8.7.1macOSSecureTransport/
LibreSSL3.3.6MultiSSL. OfficialcurlSSL_CTX_FUNCTION/CAINFO/CAINFO_BLOB docsconfirmed
SecureTransportdoesnotexposeSSL_CTXcallback; choseCAINFOfilecallbacknativeadapter.
AlsoofficialCURLU/curl_url_set/REQUEST_TARGET docsread. Links inprovenance.json.

FAILURES FIXED (do notremovefixturecases):
First#101case43POST201embeddedNULerrortruncatedviawhat(),bodyalreadycorrect.
FixedIOExceptionfullmessage; transporterrorclasses andPrematureEOFadded.
412casefirstcorpuspassed;extendedcase412URLuserinfoautomaticcurlBasicfixed.
Case418empty?lostbycurlURLfixedwithrawtarget/emptyquerytracking;spacesneeded
CURLU_ALLOW_SPACEandCURLOPT_CURLU. 452cases+notifyintegrationthenpassed.
Extendedcase420305extraProxy-Connectionremoved;101retry/rejectionfixedfromJDK.
Case422NULURLJavathrowsIOExceptionFailedtoselectaproxy(norequest)whileC++sent
truncatedtarget. FixedsourceconnectionphaseerrorandCR/LFhandling;477passed.
ReviewidentifiedmutableUSER_AGENTC++race; mutexsnapshotadapteradded+stresspassed.
Oneapply_patchinvalidduplicatefileoperationchangednothing; retriedcorrectly.
GuessednonexistentJavaAtomicReference.hpp and globBindingsearch changednothing;
reusedactualMinimaDBmutexpattern. No approval rejection/blocker.

VALIDATION (allterminal):
InitialsanRPCbuild76163PASS. IOExceptionrebuildnormal73689PASS; transporttarget14544
PASS;san25164PASS. RPCnormal57921PASS412/542/8.50s. Extended19027FAILuserinfo;
55956targetPASS;99270FAILemptyquery. 1069normal/60097sanPASS;89951PASS452/577/4.55s.
15472FAIL305header;83266targetPASS;91082FAILNULURL.17522targetPASS;99277PASS477/597
7.73s. Fullall-targetnormal9508/san50940PASS. Tenregressions77365PASS62.58s and
88021PASS72.02s:malformed_streams,source_mapping,prefs,message_differential,
message_workers,p2p_functions,sql_db,notify_manager_differential/worker,rpc_client.
AfterUSER_AGENTfix,fullall-target24042/66919PASS. Finalfour96615normalPASS10.67s,
34534sanPASS12.97s (source_mapping,notifycomparison,notifyworker,RPC). RPCfinal
5.33s/5.35s,477cases/597requests,realnotify7posts/2errors+UAstress. Source_mapping
268/189PASS. gitdiffcheckPASS(cppentirelyuntracked). Review/README/provenanceupdated.
Testsrequireloopbackbind; sandboxPythoninitialattemptfailedPermissionError.
Approved CTestprefixeswithrequire_escalatedrunwithoutfurtherprompts; use those
forfuturelocalTCPtests, do notcircumventpermissions. InitialfailedPython45337
startednooracles(serverscreationfailed). Allsessionsclosed; noorphansidentified.

REMAINING/SCOPE:
FullMain/MinimaLogger/MinimaRPCClient/nativeH2/MVStore/nodeabsent. FullRPCnotverified:
NULcustomheadersstillcurlC-stringgap; uncommonURLsyntax/redirectedgecases,
allresponseparsercases,nonUTF8/defaultmutablecharset/UTF16surrogates,globalJDK
ProxySelector/Authenticator/CookieHandler/ResponseCache/properties; fullSSLContext
provider/key/trust/cipher/sessionpolicy andexactprovidererror/stacktracebehavior.
SourceRPCreadtimeoutinfinite; donotaddunsolicitedtimeout. Loggingfullnodebinds
stillpending. JSON/UserDBcrossworkercontractsunchanged. CounterpartcountNOTcompletion.

NEXT suggestedusefulwork: MinimaLogger thenMinimaRPCClient nativeentrypoint/runtime
integration, whilecontinuingfullsourceandH2work. BothpinnedfilesFULLrereadthisturn.
MinimaLogger77lines: staticMINIMALOG,mutablepublicSimpleDateFormat(dd/MM/yyyy HH:mm:ss,
Locale.ENGLISH);log nullableString buildsdate+RuntimeheapusedMiniFormat prefixprints,
ifnotify&&Main.getInstance()!=null allocateJSONObject(message=fulllog),SECONDMain
getInstance().PostNotifyEvent; rebind/NPEorderobservable. Exception/uncaught methods
logtoString theneachStackTraceElement,separatelogevents. Needreuseexistingdate,
formatSize,exception/thread/runtimehooks BEFOREdesign; newreusegateforloggernotdone.
MinimaRPCClient197lines: mainargs parsing default127.0.0.1:9005/minima/password,
sslsetupENTIRELYCOMMENTED sofalsealways;ensurehostslash,interactivebannerMinimaLogger;
onecommandURLEncoderUTF8,actualGETBasicAuth,JSONObjectcast+JSONPretty,catchonlyparse
errorprintstack;interactiveBufferedReaderUTF8,ignoreempty/null(EOFspinsassource),
trim,exitbeforeencode,GETauth,JSONPretty,quitafterSUCCESSonly;catchExceptionlog,
closebothreader+inputatend. NoCLIimplementationwritten; logger/URLencoderreusepending.

# Latest continuation — 267 counterparts; notification manager and command

Goal ACTIVE/unbudgeted/incomplete/unblocked. 267=248 production+19 mapped tests;
190 absent; all counterparts partial/not_verified. Full native node NOT complete.
Pin unchanged5564b9d78978efcaf23679190d05253961fc6e69 (457Java). No agents, commits,
pushes or production node/wallet/miner startup. NO LIVE BUILD/TEST/EXEC SESSIONS.
All handles below terminal; do not poll or restart them. Skills reuse/graphify/
minima-reference/code-review applied; code-review and reuse skill reread this turn.

First completed prior265 processor finalsanitizer session8183: consensus462.05s,
archive-cache27.04s bothPASS. Priorprocessorcheckpoint below now accurate.

NEW production:
- src/org/minima/system/network/webhooks/NotifyManager.hpp/.cpp; all pinned methods.
  Required releaseLocks/getListener/sendPOST/log(text,bool); optional getUserDB
  defaults actualcurrentMinimaDB. Owned MessageProcessor create starts afterctor.
  Constructor loadsUserDB hooks; PostEvent retains SAME JSONObject viaMessageObject.
  Nullable event captured asUTF16 BEFORE listener; listener can mutate JSON and/or
  replace messagepayload; serialization usesoriginalnotify afterlistener. Listener
  exception logs error+Message.toString notifytrue, thencontinuesunlesslogthrows.
  SnapshotgetAllWebHooks afterlistener; FIRST no#hook returnsENTIREhandler. Stop
  checkedafterparse; emptyfilterdoesnotdereferenceNULL event. Perhook stdexception
  logs onlyWEBHOOKS_ERROR_LOGS,notifyfalse. NativeUTF16substring handlesisolatedsurrogates.
  Hooks recursive_mutex; returnsnewcopy, anynullentrythrows; addNULLonlythrowswhen
  absent (existingnullcontains shortcircuits), blank/duplicate stillsaves. Remove
  FIRSTmatching, alwayssave; clearalwayssave; userDB lookupafterlistmutationinside
  sourcemonitor. No silentHTTP/logger no-op defaults.
- src/org/minima/system/commands/network/webhooks.hpp/.cpp; exactmetadata/help/valid
  params/action defaults, addfilter#hook, removeEXACTsuppliedstring, clear, errorlogs
  secondcurrentmanagerlookup, unknownactionlist. runtimeLookup defaultthrowsunbound
  JavaNPE; mustMainbind. RegisteredactualCommandRunner ALL_COMMANDS. UsesUTF16params.
- MiniUtil::UTF16StringList/convertUTF16ArrayList/convertUTF16JSONArray and UserDB
  getWebHooksUTF16/setWebHooksUTF16; minimaloldconversionadapt preservingJavaUTF16.
  ExistingStringList stdstring adaptersunchanged foroldcallers.
- Command::getParamUTF16 required/default usespinnedtrimcodeunits<=32, NULLNPE and
  nonstringCCE, exactCommandExceptionmissing/blank. ExistinggetParamunchanged.
- support/JavaExceptions.hpp JavaClassCastException+renderadded.

REUSE gate reports deliveredbeforeNotify/command. FULLsourceNotify147lines,
RPCClient,MinimaLogger,MinimaRPCClient,webhooks command read; Main notification
methods1310..1400 andstaticlistener67..84 read. ExistingMessageProcessor/Message/
MessageStack/Listener/JSONObject/JsonDB/UserDB/MiniUtil read withPrefs tests and
message_workers. ExistingCommand/CommandRunner/connect andcommand tests read.
Siblingtools/minima-core-cpp searched, no Notify/webhook/RPC equivalent. Graph
inventoryhasNotifyManagerandwebhooks. No dependenciesadded orJava sourceedits.

NEW tests:
- support/notify_manager.cpp manualprotectedmanager realmethods, twoactualUserDBs,
  retainedJSON/messages, perop DB lookupfailure/null, callbackmutations/errors,
  actualCommandRunner+directwebhooks/metadata, queue/listcopies/rebinding snapshots.
- support/NotifyManagerOracle.java actualALLpinnedproduction compiled with reused
  NIOClientOracle Unsafe helper. ActualMinimaDB subclass and UserDB. Mainconstructor
  bypassed; getsnotifycurrentmanager andlognotifycontrolled. Notifyconstructorworker
  bypassed andfieldsinitializedas source. ActualRPCClient runs againsttestonly
  URLStreamHandlerFactory/http HttpURLConnection capturingbody/getResponseCode;
  noexternalnetwork. RawJVMlogprefixremoved, body+notifybool/eventorderretained.
- tools/notify_manager_differential.py (#99):527programs/2667operations PASS BOTH.
  Header/eventNULL,wrongtype,NULL/duplicatehooks,listindependentcopy,post/listener
  mutation/replacement,old-eventfilter,new-JSONbody,stop,snapshot,DBfailure/rebinding,
  loggingexceptionorder,UTF16surrogatefiltermembership,validURLfragments,command
  metadata/parser/adminMiniDappcaller,paramsNULL/typed/blank andnullablemanagerorder.
  NPE/CCE autoexpressionsnormalized. Onlyvalidhttp://fixture target URLs used;
  malformedURL/TLS/nativeRPC transport NOTvalidated. # separator-free hooks AREtested.
- support/notify_manager_worker.cpp (#100):400posts/4threads withadd/removehooks;
  malformedinitialmessage+per5thHTTPfailure, workerrecovery/finallycleanup,actualJSON
  identity, stopinsideblockedlistener/dropoutsideowners, reentrantgetUserDBinspect.
  No TSAN; testdoesnotconcurrentlymutatesharedJSON orUserDBoutsidehookmutex.

FIXES DURINGTESTING:
- Javafixtureinstall() collidedwithNIOClientOracle staticinstall returntype; renamed
  installHTTP. Broadscriptaccidentallyaddedcommandcasestomutatehelper; removed/read.
- Actualcomparisoncase268 foundUTF8hookadapterlosesisolatedhighsurrogate; fixedvia
  UTF16paths. Case274 wasmalformedtarget#http andnativeHTTPfixturedoesnotparseURLs;
  separatedRPCscope, retainedvalidfragmentcaseforfirst#split. No productionURLclaim.
- commandfixturewrongstaticrunMultiCommand; nowgetRunner().runMultiCommand.
- stalecommandfixturebinarygenericCommandException mapfixed/rebuilt; finalpass.
- FirstASANworkerfalsefailure: weakmanager expiresBEFOREmembersfinishdestruction.
  PollwaitnowchecksBOTHweakmanagerandweakJSON, diagnosticassertsseparate. 10sanitized
  repetitions PASS1.92s. No productionlifetimecodechangedforthisfixtureassertion.

VALIDATION:
Initialtargetbuild73791PASS; UTF16target82122PASS. Initialfullnormal53002PASS and
fullsan8015PASS. Initialnormal4suites9529PASS6.57s; missingregexnamescorrected via
normal94678(map/message)PASS4.79s. Initialsan6suites92612FAILonlyworkerassertion,
other5pass. Oldcompare8379compilefail;12600case268fail;80264case274fail;20334PASS431/
2519. Laterfull30875/99238failedonlyfixturestaticcall. Fixedtarget26808/74942PASS.
Workerrepeat13652PASS10/1.92s. Java9817compilefail;10248stalefixtureerrorclassfailure.
Full48965/67855PASS. Final8normal46807PASS16.05s,8san37477PASS26.05s (441programs).
Afteradditional86negativecommandcases,target27010/3178PASS;final#99normal37989
PASS3.86s/san14802PASS4.21s:527/2667. Finalall-target18564/24474PASS. Allterminal.
Source_mapregenerated267/190; gitdiffcheckPASS (cppentirelyuntracked). Reviewand
provenanceupdated; nofull-portcompleteclaim.

REMAINING: Main/RPCClient/MinimaLogger/nativeH2/MVStore/fullnodeabsent. Notifyhook
mutex doesNOTsynchronize externalUserDB or sharedJSONObject mutation. stdstring
adapterselsewhere loseisolatedsurrogates. Completecollectionidentity/resourceerrors,
wrongtypedinitialhookarray andconstructorworkertimingnotverified. SeeREVIEW.

NEXT RPCClient preparation (NO CODE/REUSE REPORT fortransportyet):
FULL pinnedRPCClient.java/MinimaRPCClient.java/MinimaLogger.javaalreadyread. Native
P2PEnvironment.hpp/.cpp fullyread (curlheader-onlyabort;cannotreturnRPCbody). Existing
cpp/tools/p2p_functions_differential.py FULLread: Handler/ProxyHandler andtransport
loopback40-ish cases compareactualrequestpaths,305proxy/retry20redirects; reuse.
RPCcallerblocks read ParamConfigurer322..352, base/tokenvalidate90..158,
base/incentivecash35..90, network/peers98..142,182..219; Notify andCLI fullread.
OpenJDK26.0.1sourcesavailable:
/opt/homebrew/Cellar/openjdk/26.0.1/libexec/openjdk.jdk/Contents/Home/lib/src.zip
andlib/security/cacerts. ReadHttpURLConnection.java2426..2605 fromzip:
followRedirect accepts300..307except304/306, sameprotocol only; badabsoluteLocation
retriedrelative. 305proxysameURL/method/requestURI. POST->GETexcept307 unless
http.strictPostRedirect=true; RESETSALLrequestheaders (thereforecustomUA/Connection/
Content-Type canchange!), posterNULL. Othermethodsretainheaders; differenthostor
port dropsAuthorization/Cookie/Cookie2. SourcegetInputStream0 at1314 notreadfull;
writeRequests/defaultheaders/auth/retry/timeouts/getResponseCode stillneeded. TLS
trust/certs/SSLContext adapterdetailsnotresolved; doNOTinventtrustall. RPC SSLNULL
context sourceNPE important. 10sconnect,no readtimeout. FullRPCmethods summary in
previous265checkpoint andoriginalpinnedsource.

# Latest continuation — 265 counterparts; TxPoWProcessor compiled and compared

Goal ACTIVE/unbudgeted/incomplete/unblocked. 265=246 production+19 mapped tests;
192 Java files absent. Counterparts remain partial/not_verified. Full node absent.
Pin unchanged; no agents, commits, pushes or production startup. NO LIVE SESSIONS.
This checkpoint supersedes the 264 note below about missing processor/TxBlock values.

TxBlock now owns shared TxPoW. Owned transaction-list checker/timed overloads retain
original headers/list/elements and timed-out worker ownership. Legacy vector adapters
retain vector even when empty. Source readDataStream partial mutation preserved.
Ownership comparison: 1558 programs/6219 operations normal and ASAN/UBSAN. Ten
selected suites pass264.38s/589.95s including actual120s deadline tests.

NEW system/brains/TxPoWProcessor.hpp/.cpp translates all active pinned method bodies:
actual TxPoWDB/RAM, shared TxBlockDB, tree, timed checker, MMR, cascade, NIO protocol;
source full/TxBlock processing, LIFO parents/children, recalculation, IBD/sync/archive,
missing transaction requests, shutdown flags and notifications. Required Main/lock
receivers; getDB/NIO defaults use actual native runtime. Factory starts owned worker.
Finally replacement semantics preserved. Header rejection logs corrected in checker;
TxBlockDB null argument now source NPE after acquiring method mutex.

NEW support/TxPoWProcessorFixture.hpp, txpow_processor.cpp,
TxPoWProcessorOracle.java, txpow_processor_worker.cpp and
 tools/txpow_processor_differential.py; CMake tests97/98.
Actual pinned Java oracle generates eight mined block wires (six-block main chain,
fork, referenced empty transaction). Actual processor/checker/tree/MMR/RAM with
controlled archive/SQL index/wallet/external receivers, not disk persistence.
384 programs/1434 operations pass normal24.05s/sanitized45.93s. Native18-message
worker cleanup/error recovery/owner release and24strict clock boundaries pass.
Both full all-target builds pass. Final five suites52.33s/135.70s; final normal
consensus/archive-cache/worker148.36s; final sanitized consensus/archive-cache462.05s
(archive27.04s). Last session8183 terminal PASS; do not poll/restart.
Earlier processor mismatches corrected: unqualified fixture bind resolved std::bind;
Java root needed actual mining; TxBlockDB wrong null exception; missing headerlogs.

MAJOR REMAINING: no native H2/MVStore, Main or complete node. Shared lifetime does
not synchronize mutable tree/TxPoW access across workers. Collection aliases,
wrong-message cast identities, IBD/TxBlock Object identity renderers, full checker
logging/exception order and source Crypto.hashObject null-on-error API incomplete.
No TSAN. No full runtime integration claim. See REVIEW/provenance for scoped checks.

NEXT NotifyManager/RPCClient/Main dependencies — preparatory reads, NO code yet:
FULL pinned NotifyManager.java, RPCClient.java, MinimaLogger.java,
MinimaRPCClient.java, commands/network/webhooks.java read. Main notification methods
1310..1400 and relevant callers read. Native P2PEnvironment.hpp/.cpp (existing curl
transport) fully read; it aborts after headers and cannot directly provide RPC body
semantics. Native UserDB webhook accessors/MiniUtil conversion read. Sibling/current
search found no compatible Notify/RPC counterpart. Remaining RPC caller blocks:
ParamConfigurer339, tokenvalidate135, incentivecash66, peers123/200. Relevant curl
transport tests and nullable SSL context behavior need inspect before implementation.
Notify source retains original JSON through Message; listener can mutate it before
serialization, filter event is captured before listener. First hook without # returns
entire handler. Hook copy null throws. Required current Main listener/RPC receiver
must be bound honestly until those counterparts exist. RPC SSL APIs dereference
null SSLContext in source; do not silently replace with default trust. GET/PUT only
HTTP200 return stripped readLine contents, other statuses print+empty. POST non200
throws. No read timeout; connect10000. User-agent mutable Minima/1.0.

# Latest continuation — 264 counterparts; TxPoWMiner compiled and compared

Full goal ACTIVE/unbudgeted/incomplete/unblocked. PROGRESS this turn: prior MMR
ownership/restoration completed, then new native TxPoWMiner.264=245production+19tests;
193absent; all counterparts partial/not_verified. Pin unchanged. No subagents,
commits,pushes,Java source edits or production node/wallet/miner startup.
NO ACTIVE BUILD/TEST/EXEC SESSIONS. All listed handles terminal; do not rerun them.

NEW src/org/minima/system/brains/TxPoWMiner.hpp/.cpp:
- Owned MessageProcessor factory; required releaseLocks/postMain/automineTimer/clock.
  Default getTip uses actual current MinimaDB, generate uses actual TxPoWGenerator,
  broadcast actual NIOManager MSG_TXBLOCKMINE21. Optional callbacks for replay.
- Exact START_NONCE_BYTES string, BigInt.toJavaBytes copied into serializedheader
  offset4, BigIntincrement-beforecopy, SHA3(strict txnDifficulty), decodedfinalnonce.
- Async add coins before bodyhash/queue; actual Message shared TxPoW identity. Main
  MININGstart thenend thenTXPOWMINED. End hasNOautomine;sourcewritesstartagain.
- Async stoppedloopstillsetsZERO/calcID/posts/removescoins. SyncignoresisRunning,
  >10000failureclockcheck/strictelapsed>limit, timeoutreturnfalseRETainscoinsandSTART
  nonce/bodyhash. Exceptionreservations/partialmessage state preserved.
- Regular+burninput IDs uniqueunder shortmutex; removeonceperoccurrence matches
  sequential source. Source overlappingtransactions are not reservationrefcounted.
- Minepulse blocksTXBLOCK_NODE,queuesactualgeneratorif tipelse30000timer.
  TXBLOCKMINER generatesandbroadcastsunlessTXBLOCK_NODE. Automine timerreadtwice,
  seconddiv2signedtruncate. Hashrateold hashesMiniDatawire; newpatchesTxHeaderbytes;
  originalMiniNumbercount dividedbyelapsedclamped>=1. Optionalexplicitclockadapter.

REUSE gate userreport delivered beforecode. FULL pinned465lineminer read; sibling
/Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/mining/{TxPoWMiner,
MiningManager}.hpp and tests/mining/test_mining.cpp fullyread. Incompatibleinteger
nonce/difficulty/directchainmutation, notcopied. CurrentCrypto/BigInt/TxHeader,
TxPoWGenerator,MessageProcessor/Stack,TimerMessage,P2P2Managerfactory reused.
Relevantcaller txnmine/sendpost/hashtest FULL, txnpost/NIOMessage/Main relevantblocks.

NEW support/{txpow_miner.cpp,TxPoWMinerOracle.java,txpow_miner_worker.cpp},
tools/txpow_miner_differential.py,CMake93differential/94worker:
- actualpinnedJavaallproduction compiled; UnsafeviaNIOClientOracle initializes
  minerstack/running/coinswithoutconstructorworker; MainOwner capturesposteﬀects.
  MinimaDBactualconstructor emptytree; no SQL used. Native protectedconstructor
  manualfixture, actualqueue and inheritedprocess method. Generator/no-tip defaults
  notfullyintegrated in oracle. NativegetTip fixture returnsnull; nodeflag skips
  TXBLOCKMINER. Separate native routing uses suppliedgenerator/broadcastcallbacks.
- Reused ArchiveFixture/ArchiveManagerOracle txpow, CoinFixture/ObjectOracle coin,
  SqlRecordingFixtureJSONadapters, Runner,SqlDBnormalize. Firstfixturebuildfailed
  nonexistentJSONasObject/asArray; correctedtoreusehelpers andincludearray.
-357programs/1352operations compareactualSHA3/wire/ID/size/nonce, sourcecoinlists,
  duplicateoverlap,burninputs,queuedmutation,heldmessage state,postfail1..4,nulls,
  timeoutrecovery,stoppedloop,logs,timerdelayextremesLONGmin/max. Clockwindows
  validatedBEFOREnormalization;NPEexpressions/logprefix/stackexcluded.
- Native worker:16minedrealTxPoWs/FIFO/identity/finallycleanup; controlledcallback
  cancellation+outsideownerrelease; exceptionthencontinue; impossible-targetclock
  equalitythenexpiry; nativehashrateclockarithmetic; generatedqueue/broadcastidentity.
  No simultaneoussharedTxPoWread/writetest andnoTSAN.
- Fullnormalbuild74254exit0; initialfixture73969failed; fixed55003exit0; fullwith
  worker19583exit0. FullASANbuild34804exit0. First93normal12587PASS10.24s,
  worker4338PASS1.94s. Final6normal50379PASS38.54s:map,objects,messages,workers,
  minercomparison9.17s/minerworker1.52s. Same6ASAN23392PASS126.45s:
  object82.78,msg6.78,workers1.78,miner28.78,minerworker6.03. ALLTERMINAL.

Important remaining miner/API gaps: MainandTxPoWProcessor absent/fullnodeunwired.
SharedTxPoW lifetime doesNOTsynchronize Main/startnotification readers against miner
mutation; mustcompleteobjectaccesscontractbeforefullruntime. GeneralParamsmutation
andconcurrentminingofsameworknotverified. GenericwrongMessagepayloadcastnames,
header-only processMsg/malformedheaders/resourceerrors incomplete. Actualsource
Crypto.hashObject catchesException/logs/returnsNULL, nativehashObjecttemplate throws
andTxHeaderhashfields nonnullable. DoNOTjustthrowNPEbeforeheaderbodyhash: source
assignsnullandcontinuesuntilsubsequentfailure. NeedsharedAPIfix. Staticbenchmarkdemo
mainunmapped; benchmarkrandom/timingonlynativeclockadapters,notJavaexactcomparison.

NEXT TxPoWProcessor PREPARATORY READS ONLY — NO CODE/REUSE REPORT YET:
- FULL upstream-study/src/org/minima/system/brains/TxPoWProcessor.java1056lines.
- FULL pinnedtest/brains/TxPoWProcessorTest.java: bothtestbodiescommentedout.
- FULL siblingtools/minima-core-cpp/src/system/TxPoWProcessor.hpp (~274lines),
  siblingtests/system/test_processor.cpp1..185+281..390 relevanttests; remainder
  and dependencycallers notfullyread. Siblingimplementationnotcompatible:
  basicPoWonlychecksheight/time, isBlockusesheight, basicTxnCheckreturnsTRUE,
  trimhardcoded32, MMRaddsTxPoWIDleaf, differentDB/IBD. DoNOTreuseasvalidprocessor.
- NativeTxPoWDB/TxBlock/TxPoWTreeNode/TxPowTree/TxBlockDB/IBD/TxPoWChecker/Stack
  headersread. Theirimplementationbodiesreadearliermilestones butfinishrelevant
  gate beforewritingnewprocessor. ActualTxBlock stillownsTxPoWbyvalue andchecker
  takesvector<TxPoW>; alias/nullable conversion needsresolve, nothiddencloning.
- Graphify graph inventoryfoundprocessor node. rgcallersMain,NIOMessage,backup
  commandsdone, relevantfullcallerreadstillneeded. No newprocessorreport.

README/REVIEW/provenance/source-map current. PriorMMR263 milestone follows.

# Latest continuation — 263 counterparts; MMR ownership/restoration verified

Goal ACTIVE/incomplete/unblocked. Seven selected normal and ASAN/UBSAN suites all
PASS (94.39s /229.34s); all builds terminal successful. No active handles, agents,
commits, pushes or production node/miner startup. Map remains263=244production+19test,
194absent; all partial/not_verified.

MMR::megaMMRLookup now returns shared_ptr<MMR>; getEntry retains owner through walk.
MinimaDB::bindRuntime supplies current global MegaMMR shared MMR. MegaMMR now owns
shared MMR; clear/readDataStream replace pointer at source assignment point.
MiniFile typed I/O logs use JavaIOExceptionText; source EOF diagnostic corrected
after an actual truncated cascade mismatch. javaUnixFileLength extracted unchanged
from MinimaDB; MegaMMR load-size log restored. Search fixture callback adapted.

MinimaDB actual-Java/native fixture extended: MMR local/parent/global/cutoff lookup,
held references across clear/load, nonempty tree/cascade object restoration, MegaMMR
time boundaries, file sizes,48 truncated state-file cases and partial snapshots.
Final2367programs/7519operations/334711SQLcalls PASS34.83s/53.50s. Separate CTest92
minima_db_ownership passes held reset, last-owner transfer,7000lookups during1000
fully constructed pointer replacements. Seven suites: objects,tree,utility,mmrsync,
searcher,minima_db_differential,minima_db_ownership. No TSAN; concurrent clear/load
of one MegaMMR, full coin-table aliasing and arbitrary null callback remain open.

Next missing component TxPoWMiner preparatory reads completed for pinned465-line
source, sibling mining/TxPoWMiner.hpp and MiningManager.hpp+mining tests; native
MessageProcessor/Stack,P2P2Manager,Crypto,TxHeader,TxPoWGenerator. No miner code or
reuse report yet. Finish relevant callers/test/dependency gate before implementing.
Sibling miner incompatible: integer reserialization/different difficulty and direct
chain processing; pinned miner patches BigInteger bytes into header offset4, strict
transaction difficulty, exact Main/timer events and 10001-failure Maxima time checks.
Source preserves coin reservations on timeout/exception, missing automine field on
end notification, and ZERO final nonce if async loop stopped before finding hash.
Native BigInt.toJavaBytes supplies nonce bytes (NOT MiniNumber.toBytes). Reuse actual
Crypto, TxHeader, MessageProcessor factory and required service boundary pattern.

Historical milestone sections below are preserved; their pending MMR/empty-chain
claims predate this turn and are superseded above. README/REVIEW/provenance updated.

# Latest continuation — 263 counterparts; MinimaDB coordination compiled and compared

Full 457-file native goal ACTIVE/unbudgeted/incomplete/unblocked. This goal turn
made PROGRESS: MinimaDB production translation, nested native locks, ownership fixes,
Java/native lifecycle and concurrent validation. 263=244 production+19 mapped tests;
194 absent. All counterparts remain implemented_partial/not_verified. Pin unchanged.
NO ACTIVE BUILD/TEST/EXEC SESSIONS. No subagents, commits, pushes, upstream edits or
production node/wallet/miner startup. New map/README/REVIEW/provenance current.

NEW production src/org/minima/database/MinimaDB.hpp/.cpp plus
src/support/ReentrantReadWriteLock.hpp:
- Required SqlDBServices and Main shutdown/postMessage/postTimer/postNotify/startupError/
  halt/currentThreadName callbacks. No native H2 provider or Main implementation yet.
- Actual existing archive/TxPoW/tree/cascade/user/wallet/TxBlock/P2P/P2P2/MegaMMR children.
  Shared singleton publication after construction; getters/reset/replacement retain
  source old-object identity. TxnDB null until loadAllDB. Short ownership mutexes.
- Full source load/recovery/delete/recreate order; restore/reload/restart/save/shutdown;
  source catch/finally differences retained. saveSQL wallet compacttrue always,
  TxPoW/archive caller flag, CoinDBfalse. saveState/read-lock, saveP2P no lock, allowgate.
- Source rescue timer1000 MAIN_DO_RESCUE confirmed in Main.java line112; mobile/JNLP
  startupError callback; halt callback must not return. Tests never invoke real halt.
- Locks support nonfair readers/writers, reentrancy/downgrade/no read upgrade.
  Thread-local shared identity tokens prevent OS thread-ID reuse inheriting abandoned
  locks. safeRelease drops only ONE write and ONE read; static diagnostics untouched.
  Wrong read unlock logs; wrong write unlock propagates before state assignment.
  JavaIllegalMonitorStateException mapping added; JavaExceptions now includes string.
- JavaUnixFile.hpp child helper follows inspected JDK File parent/child semantics:
  empty parent resolves from '/', absolute child remains below parent, normalized
  slashes/trailing separators. getDBFile nullname throws JavaNPE. File size usesstat.
- TxnDB UserDB resolver now consults CURRENT global owner each save/load, even heldold
  custom DB after MinimaDB replacement. bindUserDB clears resolver explicitly.
- MinimaDB::bindRuntime assigns TxPoWTreeNode ChainServices, TxnDB and TxPoWSearcher.
  ibdServices/consensusServices expose owners; not all global services are bound yet.
- Review corrected newly introduced dangling-reference risk: TxnDB::runtimeLookup/
  getRuntimeDB now shared_ptr; 8 txn command callers hold owner through operation;
  TxPoWSearcher::Services.getMegaMMR now shared_ptr, both MegaMMR loops retain it.
  support/{transaction_commands,commands_cli,searcher}.cpp adjusted to same contract.

REUSE gate completed before production: full pinned MinimaDB, actual child DBs,
serializers/runtime interfaces/tests and sibling tools/minima-core-cpp/src/database/
MinimaDB.hpp plus relevant database/integration tests inspected. Sibling SQLite
bootstrap/addBlock flow differs; reused current native children. User informed exact
paths/adaptations. Full installed JDK26.0.1 ReentrantReadWriteLock inspected lastturn;
File/UnixFileSystem parent helpers inspected thisturn. No JDK source copied.

NEW support/{minima_db.cpp,MinimaDBOracle.java,minima_db_lock.cpp},
tools/minima_db_differential.py and CMake #90 differential/#91 native locks:
- Actual pinned MinimaDB plus all production Java children compiled with SqlDBOracle
  recording driver. Unsafe Owner fixture avoids Main constructor/worker; logs and
  Main rescue/startup effects captured. Native reuses SqlRecordingFixture/Runner.
- Final2253programs/6696operations/316180drivercalls PASSnormal35.99s,ASAN45.48s.
  First1953/5796/256362PASS27.65s before restart matrix was added. Current finalcorpus
  injects SQL/runtime failures1..150 per lifecycle method, null driver objects,
  component identity snapshots, paths, nullable notification set, nested locks/ignore,
  fixed seed, encryptedURLs, savegates, held TxnDB globalUserDB resolver and real filebytes.
- Timer deadlines checked before+1000..after+1000; restart setLong timestamps checked
  before+100000..after+100000 BEFORE normalization. NPE expression/lockidentity/
  logger prefix and stack text excluded. Files only in fixture TemporaryDirectory.
- CoinDB bootstrapped per program, then event/prepared IDs reset consistently bothsides.
  Identity registry intentionally retains old objects for ID comparison; do not infer
  lifetime-release stress from this fixture. Source wronglock-state semantics compared.
- Native lockfixture4096operations across8threads, sharedreaders whileanotherholds,
  writer-to-reader downgrade excludes writer, wrongthreadunlock, terminatedowner
  tokens. ASAN/UBSAN passes; exact nonfair scheduling/TSAN not established.
- All-target normal initial67667exit0; ownership-fix fullbuild8522exit0; sanitized
  fullbuild1332exit0. Native lock firstfailed because JavaExceptions missing<string>,
  corrected and finalalltargetsuccess. Do not rerun stale build handles.
- Five selected suites #50transactioncommands,#51searcher,#82SqlDB,#90MinimaDB,#91locks
  PASSnormal73998 total58.47s; ASAN92750 total90.26s. ALL SESSIONS TERMINAL.
- Map regeneration/check263/194 and gitdiffcheck PASS; cpp remainsUNTRACKED.

NEXT: continue the full goal, including remaining MinimaDB/runtime integration and
194 missingmappedfiles. Important unresolved owner work (do NOT call this finished):
1. MinimaDB file-size getters, nonempty/corrupt persisted cascade/tree/MegaMMR,
   invalidcustomTxn data, real subprocess halt and null/UTF16/error edge paths need
   broader differential coverage. Current lifecycle fixture uses empty/missingchain.
2. ReentrantReadWriteLock extreme count throws std::overflow_error, JDK26 throws
   Java Error; fullresource-error distinction notyetrepresented. No JavaError class
   found by rg in current src. Exactnonfairqueuebehavior notcertified.
3. ChainServices coin vectors and addTxBlock/addSQLTxPoW still valuecopy some source
   aliases/nulls. MMR::megaMMRLookup binding still missing; API returns rawpointer
   and needs ownership audit like searcher/TxnDB. Consensus/IBD bindings exposed but
   full Main/processor/miner/Greeting/Pulse integration not complete. Concurrent
   singleton creation currently reassigns static callbacks: intended startupbefore
   workers only; concurrentreplacement/rebinding notverified.
4. Native H2 execution/MVStore/encryption/recovery absent. Do not replace with SQLite
   or JVM. Backend preparation/verified H2source archive details are retained below.

Historical ArchiveManager milestone and preparatory reads follow. Its 'NEXT MinimaDB'
section describes BEFORE this milestone; do not redo already-completed implementation.

# Latest continuation — 262 counterparts; ArchiveManager compiled and compared

Full 457-file native goal ACTIVE/unbudgeted/incomplete/unblocked. Current goal turn
PROGRESS: ArchiveManager implementation, codec/log corrections, normal/sanitized
comparisons. 262=243 production+19 mapped tests;195 absent;all partial/not_verified;
CLI24. Pin unchanged5564b9d78978efcaf23679190d05253961fc6e69.
NO ACTIVE BUILD/TEST/EXEC SESSIONS. No subagents/commits/pushes/pinnedJava edits or
production node/wallet/miner startup. README/REVIEW/provenance/source-map current.

NEW production src/org/minima/database/archive/ArchiveManager.hpp/.cpp:
- All active non-demo source methods, two tables/index/fifteen prepared statements
  translated, required SqlDBServices/shutdown/startupDebug/clock, recursive SQL monitor.
- Source FIRST=highest, LAST=lowest two-query paths; reference-to-prepared-field helper
  observes checkOpen replacement. Save retries exactly once after any std exception.
  SQL-only catch paths return null/partial lists; loadCascade has no checkOpen/catch.
- Sync window lower=max(start-256,1), shutdown after rows.next before getBytes;
  ranges sort stably, default descending, null entries retained until comparator.
- checkCascadeRequired validates only when no cascade is stored. Replacement path
  deliberately skips validation. saveCascade ignores DELETE failure JSON, serializes,
  binds tip then bytes. Source concurrent unsynchronized lifecycle left explicit gap.
- IMPORTANT CORRECTION to old continuation: GeneralParams.NUMBER_DAYS_ARCHIVE is
  Java LONG (source line201), not int. MAX_KEEP_BLOCKS long product and overflow
  preserved via uint64/bit_cast; constructor default100000 tested incl extremes.
- TxBlock converter previously caught all std exceptions; now I/O-only plus callback.
  Cascade converter callback and exact validation level/parent logs restored.
  Default codecs log through clog; archive connects SqlDB logger. Successful wire/
  hashing/pruning/validation decisions unchanged. Source destructive demo main absent.

Tests NEW support/archive_manager.cpp,ArchiveManagerOracle.java,
tools/archive_manager_differential.py (#89 TIMEOUT240):
- Actual pinned ArchiveManager/SqlDB/TxBlock/Cascade bodies with recording JDBC only.
  Reuse SqlRecordingFixture/SqlDBOracle/NIOClientOracle/ArchiveFixture. Java deterministic
  fixture copied from ArchiveCacheOracle.txpow because that old test class relies on
  a cache-test replacement TxPoW; this oracle compiles actual upstream TxPoW.
- Final3333 programs/10831 operations/125655 driver effects PASSnormal30.70s,
  ASAN/UBSAN92.71s. Covers query/prepared identity/order, retry/reopen, partial/null/
  SQL/runtime failures, shutdown mid-range, stable distinct equal-height blocks,
  cascade validation/replacement/DELETE failures, bad-hash serialization failures,
  all truncated prefixes of3blocks+3cascades, long retention and cutoff arithmetic.
- Live clock parameters checked against before/after windows, native call counts
  checked, then normalized; existing NPE expression/logger prefix/stack exclusions.
- Initial3273 programs passed30.46s after fixture overhead fix. First run timedout
  at240s because all unused fixtures constructed every program. Diagnosed native CPU
  and measured10fixture-onlyrequests0.686s; request() now constructs only referenced
  inputs. No production behavior changed to make tests pass.
- All-target normal20691/1580 and sanitized46696 exit0; finalfixture38532/62887 exit0.
- Nine selected normal suites51866 PASS171.61s: #2malformed,#3map,#5objects,#25cascade,
  #39archivecache,#40IBD,#43rawarchive,#82SqlDB,#89ArchiveManager.
- Eight supporting sanitized93340 PASS422.25s; final archive69472 PASS92.71s.
  ALL sessions terminal. No newTSAN/consensus run. Map--check262/195 and ten changed
  source/build-file whitespace checks PASS. gitdiffcheck passes butcppisUNTRACKED.

NEXT MinimaDB runtime owner. PREPARATORY READS THIS TURN (NO MinimaDB native code yet;
NO MinimaDB reuse report yet; finish gate/report before writing):
- FULL upstream-study/src/org/minima/database/MinimaDB.java via1-255,256-545,546-850.
- FULL sibling tools/minima-core-cpp/src/database/MinimaDB.hpp. Incompatible: own
  SQLite openPersistence/bootstrap pipeline, different block/coin/MMR APIs, no pinned
  wallet/archive lifecycle or reentrant read/write lock behavior. Do not substitute.
- Sibling integration/test_integration.cpp FULL; database/test_database.cpp relevant
 150-425 (MinimaDB/basic/persistence),540-800 (CascadeMegaMMR) read. Other portions not
 fully reread this turn; prior wallet gate read relevant wallet tests.
- FULL native TxPoWDB.hpp/.cpp,TxnDB.hpp/.cpp,TxPowTree.hpp/.cpp,TxPoWTreeNode.hpp,
  MegaMMR.hpp/.cpp,UserDB.hpp/.cpp,P2PDB.hpp/.cpp,P2P2DB.hpp/.cpp,CoinDB.hpp/.cpp,
  JsonDB.hpp/.cpp,MiniFile.hpp/.cpp,JavaUnixFile.hpp,ChainServices/IBDServices/
  ConsensusServices,TxPoWSearcher.hpp,MessageProcessor.cpp,MessageStack.hpp/.cpp.
  Wallet full read previous milestone. Other dependency full source earlier porting.
- Source Main279-350 startup+492-540 shutdown; MessageProcessor144-203 finally cleanup;
  TxPoWSearcher144-224 MEGAMMR read-lock calls. Other callers rg only this turn.
- Graph inventory MinimaDB nodes found in graphify-out/graph.json; rg native/sibling
  source found NO reentrant read/write adapter. std shared_mutex alone incompatible
  with required reentrancy/writer-read nesting/downgrade.
- FULL JDK26.0.1 ReentrantReadWriteLock.java read from
  /opt/homebrew/opt/openjdk/libexec/openjdk.jdk/Contents/Home/lib/src.zip
  java.base/java/util/concurrent/locks/ReentrantReadWriteLock.java,60647chars in3
  contiguous chunks0:26000,26000:47000,47000:end. NO JDK code copied into repo.
  Default nonfair; writer barging, readers block heuristically behind head writer;
  reader reentrancy allowed; writer can read, reader cannot upgrade; wrong read
  unlock throws IllegalMonitorStateException with exact text; wrong write unlock
  same class no text. JDK26 maxcount Integer.MAX_VALUE (NOT old65535); AQLS(long).
  getRWLockInfo suffix '[Write locks = n, Read locks = n]' plus object identity.
  Source header GPL2+Classpath and original DougLea public-domain notice inspected;
  preserve applicable notices if incorporating code. No AQLS dependency source read.
- MinimaDB safeReleaseReadWriteLock releases at most ONE write and ONE read hold,
  catches exceptions separately, does NOT update static diagnostic fields.
  readLock(false) catches/logs wrong unlock; writeLock(false) propagates and only
  then sets current state=false. Ignore-locks early return for all3methods.
- MinimaDB constructor creates archive/txpow/tree/cascade/user/wallet/txblock/P2P/P2P2/
  lock/coinNotify/MegaMMR; TxnDB remainsnull until loadAllDB. createDB publication
  AFTERconstructor. Getters/replace/reset preserve shared old objects.
- loadAllDB wallet then archive recovery-delete/recreate thenTxPoW recovery,UserDB,
  TxnDB,CoinDB folder wipe/create,Cascade/Tree/P2P/P2P2 checks,MegaMMR checks,Archive
  cascade check. Error rescue timer1000 ormobile/JNLP startup-error orprocesshalt0.
  Native test must use controlled required Main services for termination/rescue;
  never start/delete actual user node databases during tests.
- loadArchiveAndTxPoWDB(resetWallet),loadDBsForRestoreSync,fullDBRestartMemFree differ
  in source order/catches. saveSQL wallet compacttrue ALWAYS,TxPoW/archive callerflag,
  CoinDBfalse, writeunlock finally. saveState readlock,TxnDB/User/P2Ps/cascade/tree/
  optionalMegaMMR; saveUserDB readlock; saveP2PDB no lock; allowSaveState early gates.
- Future runtime binding: TxPoWTreeNode::services static ChainServices; TxnDB::runtimeLookup;
  TxPoWSearcher::services. Existing ChainServices coin lists value-based cannot represent
  nullable CoinDB rows; full alias/null behavior remains a documented integration gap.
  MessageProcessor owns name but no native thread-local Java name helper yet. Required
  Main post/message/timer/startup-error/shutdown/log services needed for real owner.

Older milestones and H2 native-backend preparation follow. Do not rerun old temporary
source-writing scripts. Native H2 execution/MVStore/encryption/recovery still absent.

---

# Latest continuation — 261 counterparts; Wallet compiled and compared

Full457-file native goal ACTIVE/unbudgeted/incomplete/unblocked. Current goalturn
PROGRESS: Wallet implementation plus meaningful Java/native tests and sharedfixes.
261=242production+19mappedtests;196absent;allpartial/not_verified;CLI24. Pinunchanged.
No subagents/commits/pushes/pinnedJavaedits orrealwallet/node/minerstartup.
NO ACTIVE BUILD/TEST/EXEC SESSIONS. README/REVIEW/provenance/mapcurrent.

NEW Wallet.hpp/.cpp under src/org/minima/database/wallet:
- Allactivepinnedmethods translated:3schemas/18preparedstatements,seedinit/update/
  wipe/reset/privatekeycheck, scriptadd/remove/list/cachemembership,defaultkeygates,
  realTreeKeycreate andsign+persistuses. RequiredSqlDBServices;noactualH2backend.
- UsesexistingSqlDB/Rows/BIP39/Crypto/Address/TreeKey/JavaRandom48 directly. Fields
  mBaseSeedsharedSeedRow,mAllKeys/tracked/simple set<optional<string>>,stop/shutdown
  flagsandNUMBER_GETADDRESS_KEYSatomic. RecursiveSQLmonitorfor source synchronized.
- Keeps source removeScript cacheentries, returnedKeyRowmodifierliteral0x00 while
  SQLstoredmodifierderivedfromcachecount, generatedwordsbeforeprovidedphrase,
  seedparamclearingbeforeinsert, signaturewithheldifSQLuseupdatefails,partiallists
  andgetAllDefaultAddressesALLexceptioncatch. MostsourcewalletmethodsDO NOTcheckOpen.
- JavaStringTrim.hpp extracts BIP39 helper unchanged; BIP39callsit. MultisigContract.hpp
  copiesexact sourceconstant ONLY; multisigcommandstillabsent/unclaimed.
- TreeKey.hpp/.cpp: source exhaustion/too-many-signature-level logging restoredwith
  loggercallback(defaultclog), WalletsignconnectsSqlDBlog. Sourcemaximumconstructor
  leveldiagnosticcorrected. sign(MiniData*) acceptsnullandthrowsNPEonlyatactualdata
  signingpoint; constrefwrapperretained. Hash/signaturealgorithmunchanged.
- Majorremaining: realnativeH2durability/encryption/recovery,Main/MinimaDB/TxPoWDB
  runtimebinding, unsynchronizedlifecycle/seed/cacheownership/concurrency, fullJava
  nullable/UTF16/publicAPI/logger/pathologicalTreeKeybehavior. No productionwalletuse.

TESTS:
- NEW support/wallet.cpp,WalletOracle.java,tools/wallet_differential.py #88. Reuses
  SqlRecordingFixture+SqlDBOracle; actualpinnedWallet+row/BIP39/Address/TreeKey/
  Winternitzbodies. NoJava runtime dependencyinproduction.
- Final1433programs/6521operations/85452JDBC effects PASSnormal23.19s/san144.49s.
  1427initialprogramscope passedfirsttry75907(15.39s); added3keyinsertfailures,
  realdefaultkeyinit,2constructorstickinessprograms. No normalizedmismatchfixes.
- Seed/heldrowidentity+mutation,schema/query/preparedidentity,partialSQL/runtime
  failures,nulls,cachemembership,cleanup/reopen,sourceflags/defaulttarget,persisted
  useupdateorder andsignatureBYTES verified. Standarddefault64x3TreeKeygeneration
  runsrealC++; signcases2x2keys coveruses0/3/4 includingresetlog/nullsigning.
- Privatecaches sortedformembership ONLY (HashSetiterationnotclaimed). Randomdefault
  selectiontests0/1candidates; generatedseedphrasecorpususesprovidedfixturephrase.
  NPEexpressionsandloggerprefix/stackexclusionsexistingunchanged.
- Alltargetnormal72558initialPASS; final97965normal/74221sanexit0. Finalfixture
  targets87584normal/7067sanexit0. Eight-suite normal6311ALLPASS67.49s,
  san85756ALLPASS272.07s,allTERMINAL. #3map,#5objects,#13kissvm,#44walletrows,
  #45bip39,#48command,#82SqlDB,#88wallet. No newTSAN/consensusrun.
- source_map--check261/196PASS;gitdiffcheckPASSbutcppuntracked;actual10changedfiles
  whitespacePASS. Reviewrequestsfullportchangesduetoengine/runtime/concurrency/API.

NEXT ArchiveManager thenremainingruntime/nativeH2backend. NOArchiveManagercounterpart
yet. PreparatoryREADSCOMPLETEDthisturn:
- FULL upstream-study/src/org/minima/database/archive/ArchiveManager.java via1-280,
  281-560,561-840 coversend(808). Includesstaticdemomain; notexecuted.
- FULLnativeTxBlock.hpp/.cpp,Cascade.hpp/.cpp,IBDServices.hpp,TxBlockDB.hpp/.cpp.
- SourceTxBlock.convertMiniDataVersion281-end/Cascade.convertMiniDataVersion230-248
  excerptsread: I/O-onlycatch+MinimaLogger. NativeTxBlockstillallstdcatchincorrect;
  CascadealreadyIOonlybutneedslogcallbackforSQL. ReuseCoin/TxPoWcodecpattern.
- rg finds siblingpersistence/BlockStore.hpp+chain/BlockStore.hpp; persistencecandidate
  fullyreadinpreviousSQLmilestones; chaincandidateNOTyetreadthisscope. NoArchivemethod
  reusegate/reportyet; finishbeforecode. CurrentSqlDBfixturesavailable.

Archivefacts:15preparedstatements,2tables+index. MAX_KEEP_BLOCKS=2000*GeneralParams
NUMBER_DAYS_ARCHIVE is Java long; product is long (corrected in ArchiveManager milestone). createSQLdebuglogsMainflagbefore
statementcreationandafterclosebeforeprepared. loadFirstBlock meansHIGHblock(DESC),
loadLastBlockLOW(ASC),each2querieswith-1sentinel. loadSyncrange start-256 clamp<=1
to1,checksMain.isShuttingDownAFTERrows.nextBEFOREgetBytes,sortsDESCoutsideSQLcatch.
loadBlockRange strictstart/endbounds,defaultDESC,sortoutsideSQLcatchpreservenulls/
comparisonerrors. saveBlock retries_intSaveBlockONCEonANYException(noextraerrorlog).
intSaveBlockcheckOpen thenMiniData.getMiniDataVersion(logI/Oandnull),bindID,height,
clock,bytes andexecute. checkCascadeRequired/saveCascade/loadCascadeDO NOTalways
checkOpen;saveCascadeignoresgenericDELETEfailureJSONbeforebindingtip+blob. loadCascade
SQLexceptionpropagates. checkForCleanDB skipswhenGeneralParams.ARCHIVEtrue,else
cutoff=highestblock-MAX_KEEP_BLOCKSviaMiniNumber thenlong. hackShutrawconn.close.
closeAndReopen saveDB(false). FullsourceMain/MinimaDBcaller/reusefixtureinspection
andmethodteststrategy stillneeded. ExistingIBDArchiveinterfacealreadysharedTxBlock
lists/Cascade; compatibleownerbindinglater.

H2 sourcearchive/inspection/provenance unchanged frompreviousmilestone; realbackend
ABSENT. Neverrerunold/tmp sourcewriters. GoalremainsACTIVE;donotnarrowcompletion.

--- Previous verified milestone ---

# Latest continuation — 260 counterparts; CoinDB compiled and compared

Full457-file native goal ACTIVE/unbudgeted/incomplete/unblocked. Current turn PROGRESS:
TxPoWDB finished its verification, then CoinDB implemented/compiled/compared.
260=241production+19mappedtests;197absent;all rows partial/not_verified;CLI24.
Pin5564b9d78978efcaf23679190d05253961fc6e69 unchanged. No agents/commits/pushes,
pinned Java edits, real node/wallet/miner startup or remote-peer interaction.
NO ACTIVE BUILD/TEST/EXEC SESSIONS. README/REVIEW/provenance/map updated.

NEW CoinDB.hpp/.cpp in src/org/minima/database/txpowtree:
- Exact four prepared statements/schema, insert/getAll/clearOld/getSize/reopen.
- Private ctor removes old .mv.db (ignores deletion result), then loadDB; catches
  only SQLException. Static replacement only after ctor returns; SQLfailed instance
  replaces old, runtimefailure preservesold. Shortmutex protectsnative sharedptr
  publication. Retained instances remainalive; synchronizedmethods inheritedrecursive.
- Required SqlDBServices (real H2 provider STILL ABSENT). Nullable MiniData/Coin/number
  pointer overloads. getAllCoins vector<sharedCoin> retainsnulldecodedrows/partialSQLlists.
  ChainServices currentlyvector<Coin>, actualnullablecoin-listbinding stillneeded.
- Coin.convertMiniDataVersion corrected broadcatch to wireIOExceptiononly; callback
  mirrorsTxPoW. Defaultstderr callback remainsnotfullMinimaLogger. Insertion logs
  serialisationIOfailure thenbinds ID/block and throws onnullbytes exactlysource.
- JavaIOExceptionText.hpp extracts EXACToldTxPoWSqlDB codec formatter and reusedbyboth.
  JavaUnixAbsoluteFile helper extracts unchanged SqlDB absolute helper. Successful
  Coin encoding/numeric algorithms unchanged.
- CoinFixture.hpp extracts old objects.cpp coin(int) unchanged; bothobject/newSQL
  probesreuseit. JavausesexistingObjectOracle.coin. SharedSQLfixturesunchanged.

TESTS:
- #87coin_db_differential:1350programs/4318ops/28803effects PASSnormal7.19s/san9.34s.
  ActualpinnedCoinDB/Coin methods, recordingdriver, realtempfilesystem. Alltruncated
  prefixes3coinsincltokens/state, malformednumbers/hashes,oversizedhashserialise,
  checked/runtimefailure order,nullandpartialrows,staticpublication/heldinstance,
  SQLcompact/reopen andoldfiledeletion(empty/nonemptydirs). Onlybuildtempfilesused.
  ExistingNPEexpression/loggerprefix/stackexclusionsunchanged.
- Firstfailurecase507 fixedfixture nullconnection: reuseSqlDBfixtureDriverManager
  SQLException(No suitable driver), notnullreturn. Secondcase719 fixednewCoinDB
  logger toexistingI/Oformatter. Productionresultsnotmaskedbynormalization.
- ALLTARGETbuilds27635/49419exit0. Fixturebuilds87533/81837exit0. FINALalltarget
  builds31469/77390exit0 afterformatterextraction.
- #87normal35545PASS. Other7normal88498ALLPASS68.28s. All8san80792ALLPASS201.07s.
  #3map,#5objects,#82sql,#83concurrent,#84onchain,#85txpowsql,#86txpowdb,#87coin.
  No newTSAN/consensusrun. source_map--check260/197PASS; gitdiffcheckPASSbutcpp
  untracked; actual12changedfileswhitespacescanPASS.
- Earlier same-turnTxPoWDB#86 1355/7407/80960 effects passedboth;10suites
  normal4343ALLPASS149.23s/san13095ALLPASS399.91s, terminalhandles.

NEXT Wallet/nativebackend/runtime completion. No Wallet counterpart written yet.
PREPARATORYREADS completed for next scope:
- FULL upstream-study/src/org/minima/database/wallet/Wallet.java (1-280,281-590,
  591-900), KeyRow/SeedRow/ScriptRow.java, TreeKey.java (1-240+241-end).
- FULLnativeRow.hpp/.cppx3, SqlRow,SqlConnection(alreadyinheritsSqlRow),SqlDB;
  TreeKey/TreeKeyNode/Winternitz/Signature.hpp/.cpp; Address/Crypto.hpp/.cpp;
  JavaRandom48.hpp, BIP39.hpp + BIP39.cpp executablemethods265-end read. Wordlist
  middletruncatedinlatestcat (priorBIP39milestonehadverifiedcopiedfullwordlist).
- FULLnativewallet_rows.cpp + WalletRowsOracle.java +wallet_rows_differential.py;
  ObjectOracle.javafull andobjects.cpp relevantcoinfixturedeps.
- Sibling tools/minima-core-cpp/src/wallet/Wallet.hpp FULLread; relevant tests
  tests/database/test_database.cpp210-270+integration/test_integration.cpp90-125.
  Its in-memory random single-WOTS keys/one-useflag differ from pinned SQL wallet,
  deterministicmodifier+baseseed TreeKey, scriptAddress andpersistedusecounts.
  Underlying siblingWinternitzalreadyreviewed incompatible BC scheme.
- Source multisig.java1-78 includes exact MULTISIG_CONTRACT constant; fullcommand
  NOTread/ported. Wallet.createSQL requiresconstant. No fabricatedmultisigcounterpart.
- NativeBIP39 trim helpercandidate found only BIP39.cpp265; mayextractunchanged.
  Wallet runtime binding and complete reuse report NOTyet sent; finishgate beforecode.

Walletsourcefacts toretain: constructor TEST_PARAMS sets static NUMBER_GETADDRESS_KEYS8
(sticky, normalctor doesnotreset64);createSQL18preparedstatements+3tables,replaces
HashSets before getAllKeys/getAllAddresses/initBaseSeed+addbaseMultisigifabsent.
initBaseSeed stillcallsgetNewWordList evenprovidedphrase; clears GeneralParams.SEED_PHRASE
beforeinsert. updateSeedRow assignsnewSeedRow onlyafterexecuteUpdate. wipebaseseed
updatesrows,assigns0x00,savecompact,checkOpen. Mostwalletmethods DO NOTcallcheckOpen.
removeScript doesnotremovecachedaddress. createNewKey storesmodifiercomputedfrom
cachecount butreturnedKeyRowmodifier isliteral0x00. signData reconstructsTreeKey,
signs,thenpersistsuses before returnsignature. getAllDefaultAddresses catchesALL
Exception whileotherlistmethodsSQLonly. TreeKey logger/exactexception/API gaps
visible in source (exhaustionlog absentnative;levelsdiagnostic differs), needaudit
whennewwalletcomparisonreaches them. No active Walletimplementationchoice yet.

H2driver/nativeSQLengine still absent. Reference archive /tmp/minima-h2-2.4.240-sources.jar,
SHA256a853be74b3f6d63438c53d19e2c368f158ee622499422ba586d1f8bc17b843e2;842Java
sources. Driver.java andJdbcConnection.java FULLread; officiallicensepage FULLread.
No H2codeincorporated. MVStore/session/parserdependenciesstillneedsourceinspection.
Neverrerunold/tmp sourcewriter scripts. FullgoalremainsACTIVE; donotdowngrade.

--- Previous verified milestone ---

# Latest continuation — 259 counterparts; owning TxPoWDB compiled and compared

Full457-file native goal ACTIVE/unbudgeted/incomplete/unblocked. Progress this turn.
259=240production+19tests;198absent. Pin unchanged. No agents/commits/pushes or node startup.
TxPoWDB.hpp/.cpp now composes actual RAM/SQL/onchain stores with required Main message
and wallet resolver callbacks. Exact lookup/duplicate/relevance/lifecycle sequence,
shared objects and map identity. JavaUnixFile normalizer extracted unchanged from
SqlDB. Four TxPoW transaction getters now throw typed JavaNullPointerException for
header-only TxPoW; other body accessors still pending.

#86:1355 programs/7407 operations/80960 effects PASS normal and ASAN/UBSAN. Actual
pinned methods; driver/owner boundaries are fixtures, no native H2 engine or node.
All-target builds normal8389/san19607exit0; final fixture rebuild40061/22555exit0.
Ten suites normal4343 ALLPASS149.23s; san13095 ALLPASS399.91s, TERMINALexit0.
No live sessions at this checkpoint. Source map regenerated259. README/REVIEW/provenance
updated; final mapping/whitespace check pending. No new TSAN.

Next CoinDB: FULL pinned txpowtree/CoinDB.java, Coin.java, nativeCoin.hpp/.cpp,
TxPoWTreeNode.cpp/ChainServices and sibling persistence/UTxOStore.hpp read. Reuse
gate/report sent. Coin converter incorrectly catches all stdexceptions; reuse
TxPoW's I/O-only callback pattern. Preserve constructor old .mv.db file deletion
and static replacement timing, four prepared statements, partial/null rows and
SQL-only catches. All test files must remain within build temp dirs. Native H2
engine and fullruntime remain major work. H2 Driver.java and JdbcConnection.java
fully read; license page fully read; no H2 code incorporated. Never rerun old /tmp
source writers. Continue without narrowing goal.

--- Previous verified milestone ---

# Latest continuation — 258 counterparts; TxPoWSqlDB verified at driver boundary

Full457-file native goal ACTIVE/unbudgeted/incomplete/unblocked. Current goal turn
is PROGRESS: implementation, both builds, differential tests and malformed-data fixes.
258=239production+19mappedtests;199absent;allrows partial/not_verified;CLI24. Pin
5564b9d78978efcaf23679190d05253961fc6e69 unchanged. No subagents, commits/pushes,
pinned Java edits or real node/wallet/remote-peer startup. NO ACTIVE EXEC/BUILD/TEST
SESSIONS. Current README/REVIEW/provenance/source map reflect this milestone.

NEW PRODUCTION:
- src/org/minima/database/txpowdb/sql/TxPoWSqlDB.hpp/.cpp: exact table/index and ten
  prepared statements, wipe(no index/statement recreation as in pin), add, first
  lookup, child IDs, counts, relevant/latest pagination, custom filtering, cleanup,
  closeAndReopen. Reuses SqlDB recursive lock, lifecycle, log, native TxPoW converter /
  serializer, TimerMessage clock, JavaLowerCase. Required SqlDBServices; no driver.
- maxSQLMilli() returns functionlocal static atomic long initialized from current
  GeneralParams.NUMBER_DAYS_SQLTXPOWDB at first access, ctor forces use. Default
  259200000 verified against actual Java class initializer. Native accessor versus
  Java public field is documented API adaptation. MAX_RELEVANT_TXPOW MiniNumber100
  static and LOG_ADD_BLOCKS/LOG_ADD_TRANSACTION atomicfalse. Demo main notported.
- SQL exceptions caught exactly(source exists() does not log); partial vector results
  and null decoded entries retained. References to prepared fields survive replacement
  in checkOpen. Clean preserves unsigned-wrap long arithmetic and hard second clock.
- Source query filter performs independent single passes for ;/update/delete/insert.
  Fixed initial erase-loop defect that removed newly formed matches in same pass.
- add serialise I/O failure logs then continues firstfive bindings, readsclock, throws
  null serialized value before setBytes, preserving MiniData.getMiniDataVersion timing.
- SqlConnection interfaces + shared native/Java fixtures extended setInt/setBytes /
  getBytes. Only fixtures implement these interfaces: still NO NATIVE H2 backend.
- TxPoW.convertMiniDataVersion now catches only wireIOException (was allstdexception)
  with optional error callback overload. SqlDB uses callback; default helper stderr
  does not implement full MinimaLogger yet. Runtime errors propagate as Java.
- MiniData.cpp: exact hash read/write I/O messages; negative ordinary array length
  uses JavaNegativeArraySizeException(number). MiniNumber.cpp: source invalid wire
  length message and existingJavaNumberFormatException for zero-length BigInteger.
  Numeric algorithms/successful bytes unchanged. Other errors/API gaps stillopen.

TESTS/REVIEW:
- support/txpow_sql_db.cpp + TxPoWSqlDBOracle.java reuse shared SQL proxies and existing
  ArchiveFixture/ArchiveCacheOracle and ram.cpp/RamOracle transaction fixture. Java
  runs actual pinned class bodies; H2 drivers deregistered. No runtime Java dependency.
- tools/txpow_sql_db_differential.py #85:2592programs/8729ops/86333drivercalls pass
  normal andASAN/UBSAN. Exact SQL, identities, params, real serialized bytes andIDs,
  nulls/partialrows, integerbounds/pagination, alltruncatedprefixes4TxPoWs inclnonempty
  transaction, malformed number/data/hash fields, oversizedhash serializationfailure,
  transactionlogs/reopen/filtercases. Liveclock validation before normalization;
  NPEexpressions/loggerprefix+stacks excluded; defaultretention checkednotfabricated.
- Initial2156programscope passed; expandedcase2441 exposed hashwrite diagnostic
  mismatch, fixedproduction. Finalsuiteincludesruntimeprimitiveerrors tocheck new
  TxPoW I/O-onlycatch. No mismatches hidden by newnormalization.
- ALLTARGET final builds normal35603/san53960exit0.12suite normal34563ALLPASS106.38s;
  san25413ALLPASS255.35s. #2malformed,#3map,#4java,#5objects,#39archivecache,
  #61NIOtransport,#63NIOloopback,#66NIOmessage,#82SQL,#83concurrent,#84onchain,#85new.
  New#85normal15.74s/san36.72s. #66normal28.35s/san79.71s. No newTSAN/consensus#29run.
- source_map.py regenerated + --check passes258/199. gitdiffcheckpassesbutcppuntracked;
  separate whitespace scan ofchangedSQLcode+fixtures also passes. Reviewrequests
  changes for FULLPORT because engine/runtime/API remain absent.

NEXT:
Continue TxPoWDB (owning RAM+SQL+onchain store), then real backend and remaining
MinimaDB/Wallet/archive/coin/processor/miner/Main/commands; do not downgrade goal.
No TxPoWDB.cpp exists yet. PREPARATORY READS already done this turn:
- FULL pinned database/txpowdb/TxPoWDB.java, RAM/RamDB.java+RamData.java, nativeRamDB.hpp
  /.cpp+RamData.hpp/.cpp, TxPoWSearcher.hpp/.cpp, support/ChainServices.hpp, full
  support/SearcherOracle.java, fullRamOracle.java+ram.cpp, currentSQLclasses/fixtures.
- MinimaDB.java relevant ctor/getters+loadSQL paths/restart/save/cleanup read (NOTfull
  class):1-215,300-365,455-565,580-660. Main987-1038auto-backupmessagehandlerread.
  Main.MAIN_AUTOBACKUP_TXPOW string found. Other Main/runtime fullscope stillneeded.
- Repository search confirms no existing owningTxPoWDB C++; sibling full
  tools/minima-core-cpp/src/mempool/Mempool.hpp read: FIFO eviction, duplicate/double
  spend rejection differs from pinnedRamDB. Full sibling persistence/BlockStore.hpp,
  Database.hpp and tests/persistence/test_persistence.cpp read; SQLite/schema/replacement
  differs. nested minima-core-cpp/tests/mempool/test_mempool.cpp read. Do not substitute.
- SearcherOracle uses actualTxPoWSearcher/Wallet hashsets via reflection,MinimaDB.createDB;
  can reuse to test relevance. NIOMessageOracle/MainFixture+Unsafe candidates identified
  by rg for capturing Main.PostMessage without node startup, relevant excerpts only;
  FULL relevant fixture/deps MUSTbe read before reuse. No implementation choice/report
  for TxPoWDB sent yet: finish reuse gate/report then port. Source add posts autobackup
  even duplicate and catches all messageexceptions, thenRAMexist/add,SQLexist/relevance/
  add; SQL add return ignored. getTxPoW RAMfirst thenSQL(no read-through cache).
  Childblocks SQLidlist -> getAllTxPoW preservingduplicates, skipnull decodedobjects.

H2 source archive and older milestones follow unchanged. Never rerun old /tmp source
writer scripts. Current goal must remainACTIVE; no genuine blocker or approvalneeded.

--- Previous verified milestone ---

# Latest continuation — 257 counterparts; TxPoWOnChainDB verified at driver boundary

Full457-file native goal ACTIVE/unbudgeted/incomplete/unblocked. Previous goal turn
is PROGRESS: code, builds, cross-language tests and a MiniNumber correction. Do not
mark done, blocked or replace goal with a narrower wrapper/SQLite/JVM outcome.
257=238production+19mappedtests;200absent;allrows partial/not_verified;CLI24. Pin
5564b9d78978efcaf23679190d05253961fc6e69 unchanged. No subagents authorized/used,
no commits/pushes/pinned Java edits or real wallet/node/remote peer startup.
NO ACTIVE BUILD/TEST/EXEC SESSIONS. README/REVIEW/provenance/source map current.

NEW:
- cpp/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.hpp/.cpp directly translates
  schema/eight prepared statements, wipe, add, transaction lookup, both block query
  overloads, first/last/date, count, clean overloads. REQUIRED SqlDBServices provider
  and clock(defaultexistingTimerMessage::currentTimeMillis). No actual SQL engine yet.
- Uses inherited recursive mSQLLock; SQL-only catches preserve partial row/list
  results and propagate runtime failures. Helper arguments reference prepared fields
  so checkOpen() recreation updates fields before use. wipe keeps prepared statements.
- MAX_ONCHAINSQL_MILLI atomic long default500654080: original int product overflow,
  verified against actual Java static initialization (not manually assigned oracle
  expectation). Cleanup uses unsigned arithmetic+bit_cast for long wrap, reads clock
  then retention, and reads clock a second time for hard=true. Optional strings and
  pointer MiniNumber retain null timing. Source demonstration main remains absent.
- cpp/src/support/SqlConnection.hpp now includes SqlPreparedStatement(clearParameters,
  setString,setLong,execute(),executeQuery(),executeUpdate()) and Connection.prepare /
  ResultSet.getLong purevirtuals. Existing fixtures extended to implement required APIs.
  SqlDB anonymous null helper extracted unchanged to sqlRequired; logger madeprotected.
- cpp/support/SqlRecordingFixture.hpp extracts actual prior sql_db.cpp recorders with
  typed rows, prepared IDs and SQL/runtime failure injection. sql_db.cpp reuses it.
  SqlDBOracle.java generalized similarly; real H2 drivers deregistered before fixtures.
  Main/MinimaDB/TxPoWDB/native engine remain absent; never call these real persistence.

#84 onchain_db_differential:
- support/OnChainDBOracle.java subclasses pinned TxPoWOnChainDB only to expose create /
  clear methods; actual production bodies run. Existing SqlDBOracle proxies reused.
- support/onchain_db.cpp and tools/onchain_db_differential.py:855programs,3639operations,
  29167drivercalls pass bothbuilds. Exact schema/query strings, prepared identities,
  null/signed typed rows, partial creation/results, SQL-vs-runtime failures, dateformat,
  bothqueryoverloads andcleanup. Fixtureclock fixed(default1700000000000 or signed
  extremes); Java Systemclocklive. Python validates actual setLong timestamps against
  before/after windows with exact retention arithmetic BEFORE normalization. NPE/class
  diagnostic expression text and logger date/memory/stack prefixes excluded. RealH2
  SQL execution/order/constraints/prepared invalidation andpersistence unverified.
- Existing #82 still464/1260/8352; #83 1024concurrent+2reentrantcalls pass regressions.

FOUND/FIXED existing MiniNumber mismatch:
- First #84 normal/san failedatcase598 for1e40 rejected BEFORE DB call: JavaNumberFormat
  exactlarge message vs nativegenericrange. No test normalization/substitution added.
- Read full pinnedMiniNumber.java, relevant nativeconstructor/checkLimits, full
  MMREntryNumber.cpp/.hpp, PrimitiveOracle/primitives/differential tests and callers.
- MiniNumber.cpp nowthrows existingJavaNumberFormatException withpositiveBigDecimal
  diagnostic andnegativefixedmessage. Existing MMREntryNumber::toBigDecimalString
  extracted tosupport/JavaBigDecimalText.hpp and reused unchangedbehavior. Added
  positivenegativebounds, scientific, fractional trailingzeros,70digitprecision and
  44decimalclamping cases to #84. OtherMiniNumber constructor/malformed/API parity gaps
  remain open. Numerical arithmetic unchanged; did NOT rerun expensive consensus#29.

Final ALL-TARGET builds normal53328/sanitized64421 exit0.
Final8suite normal61040: #4java1.64s,#13kissvm4.47,#25cascade39.65,#48command7.96,
  #49runner2.31,#82sql2.55,#83concurrent.36,#84onchain4.02; ALLPASS62.97s.
San94782: #4 2.93s,#13 10.70,#25 114.41,#48 18.65,#49 2.52,#82 2.82,#83 .42,
  #84 5.37; ALLPASS157.81s. Allsessions terminal. NoTSANthisscope.
Source map regeneratedandchecked257. Reviewreadallchangednewcode;docsupdated.

NEXT:
1. Pinned upstream-study/src/org/minima/database/txpowdb/sql/TxPoWSqlDB.java FULLY READ
   THIS TURN (602lines; older missingfirst100 warning nowobsolete). No C++ writtenyet.
   Needs SqlPreparedStatement.setInt/setBytes and ResultSet.getBytes, actual TxPoW
   wire conversion/null behavior, JavaLowerCase, GeneralParamsdays and log flags.
   Tenpreparedstmts, unused SQL_SELECT_OLDEST field, MAX_SQL_MILLI class-load-time
   dependsGeneralParams.NUMBER_DAYS_SQLTXPOWDB, MAX_RELEVANT_TXPOW=HUNDRED. Wipe drops
   all/recreates table butNOTfastsearchindex orpreparedhandles; preserve pin.
   customSizeQuery lowercase/removes ; thenupdate/delete/insert literals; no stmtclose.
   exists swallowsSQLExceptionWITHOUTlog. closeAndReopen save(false),conn=null,
   checkOpen(false) underrecursive lock. Read dependencies/candidates/callers and
   issue exactreuse report before newimplementation. Existing sharedSQLfixtures ready.
2. Full pinned TxPoWDB.java and commands/search/txpow.java READTHIS TURN. TxPoWDBclass
   missing; commandmissing. Main.java1379 notreadthisSQLscope. MinimaDB782 previouslyread.
   Wallet/ArchiveManager/CoinDB/nativeH2engine/fullMain stillneedimplementing.
3. Sibling Database.hpp/BlockStore.hpp and persistence/test_persistence.cpp fullyread
   THIS TURN; SQLite + serialized block replacements differsfrompin; notsubstituted.
4. Native engine source preparation: javap Constants fromactual upstream-study/jar/
   minima.jar confirms H2 2.4.240 (BUILD_DATE2025-09-22), sameaslib/h2-2.4.240.jar.
   Downloaded matchingSOURCEjar viaapprovedcurl to /tmp/minima-h2-2.4.240-sources.jar
   URL https://repo.maven.apache.org/maven2/com/h2database/h2/2.4.240/h2-2.4.240-sources.jar
   localSHA256 a853be74b3f6d63438c53d19e2c368f158ee622499422ba586d1f8bc17b843e2
   publishedSHA1 checkedmatch e75d1bac93e36cf12928d56423bdc5d4ecc57fe0.
   Maven.sha256 URL404 (not sandbox); .sha1 succeeded. Noapprovalsneeded/requested.
   Source org/h2/Driver.java FULLREAD viaunzip: accepts jdbc:h2:->newJdbcConnection;
   nullURLsqlerror; defaultconnectionthreadlocal; DriverManagerregistration etc.
   Header says MPL2.0/EPL1.0 multiplelicense; READfulllicensebeforeincorporatingcode.
   ArchiveNOTextracted/copiedintoprod; JdbcConnection/engine/MVStore NOTreadthisscope.
   H2referenceprovenance entry recordsURI/hash/version. NoJava/JNI runtimeintroduced.

Previous checkpoint follows (superseded counts/read-status):

# Latest continuation — 256 counterparts; SqlDB wrapper verified

Full 457-file goal remains ACTIVE, unbudgeted, incomplete and unblocked. Do not mark
complete or count interfaces/scoped tests as a working database/node. Last goal query
this turn: 3,291,453 tokens / 25,106 seconds, active. User away; continue autonomous
routine work. No subagents authorized/used. No commits, pushes, Java source edits,
remote peers, real wallet databases or full node started. No active build/test sessions.

256 counterparts = 237 production + 19 mapped tests; 201 absent. Source map regenerated
and checked. All rows still partial/not_verified. CLI remains 24 commands. Pin unchanged:
5564b9d78978efcaf23679190d05253961fc6e69. README/REVIEW/provenance updated through SqlDB.

NEW production:
- cpp/src/org/minima/utils/SqlDB.hpp/.cpp translates all pinned methods: load/encrypted
  load, sticky encrypted reopen, isOpen/checkOpen, hardClose/save, backup/restore,
  JSON/MiniData conversions, synchronized generic SQL metadata/row/CLOB handling.
- cpp/src/support/SqlConnection.hpp extends existing SqlRow with explicit native
  SqlConnection/SqlStatement/SqlResultSet/metadata/object/Clob interfaces; SqlException
  and SqlRuntimeException preserve checked SQL versus runtime categories.
- REQUIRED SqlDBServices.connect accepts exact H2 URL/user/password. There is NO actual
  native provider, SQL execution, MVStore, encryption or backup engine yet. Constructor
  rejects absent provider. NO SQLite fallback or production no-op driver.
- Source encrypted flag remains true after later plain load. Connection assigned before
  autocommit/createSQL. save omits explicit statement.close, result sets not explicitly
  closed, SQL NULL columns omitted before class-name lookup, Clob special case only when
  metadata class equals java.sql.Clob, length narrows signed32, close errors reset results.
- Virtual methods/forwarding overloads preserve subclass dispatch; protected recursive
  mSQLLock for synchronized subclass APIs. Other lifecycle methods source-unsynchronized:
  arbitrary concurrent close/load still needs native safety review.
- Review fixed Java Unix File normalization/root/bare parent and NUL filesystem guards.
  Repeated/trailing slashes collapse without resolving dot segments. JDK26.0.1 local
  lib/src.zip java.base/java/io/UnixFileSystem.java fully read; no JDK source copied.
  Logging retains bodies and notify flag; complete MinimaLogger/Main debug binding absent.

Tests:
- #82 sql_db_differential: support/SqlDBOracle.java runs actual pinned SqlDB against
  JDBC dynamic proxies; deregisters real drivers first. Native support/sql_db.cpp uses
  equivalent recording interfaces. Both record connection/statement/metadata/object
  call order, configurable failures, result values, file state, credentials, logger bodies.
  Test-only temporary paths; no real SQL database created. Null returned Driver.connect
  follows actual DriverManager no-suitable-driver SQLException in native fixture.
- cpp/tools/sql_db_differential.py reuses existing Runner and WalletRowsOracle proxy
  pattern; 464 programs / 1260 operations / 8352 driver calls. Exact except Java NPE/
  class-cast message expressions and logger stack/date/memory prefixes. Valid JSON/null,
  wrong casts, malformed UTF8 before parser, sticky encryption, quoted backup names,
  NUL paths, SQL NULL, duplicates, CLOB lengths including signed truncation, close failures.
- #83 support/sql_db_concurrent.cpp: eight real threads, 1024 generic SQL calls and
  two reentrant createSQL calls (including reopen from locked execute), verifies no
  overlapping statement lifetime, two connections/schemas, no failures.
- Final ALL-TARGET builds: normal session85510 exit0; sanitized67115 exit0.
- Final tests normal17201: #82 3.10s, #83 .27s; bothPASS3.38s.
  Sanitized51364: #82 3.31s, #83 .51s; bothPASS3.83s.
- No new TSAN run. Previous TSAN #73 scope only remains valid. No production shared
  dependencies altered except adding standalone SqlDB and connection interfaces.
- All changed new code reviewed; major remaining native engine gap explicitly in REVIEW.

NEXT:
1. Continue real native SQL backend and database subclasses, then MinimaDB/Main.
   Full pinned upstream-study/src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.java
   was read THIS TURN, no corresponding C++ written yet. It needs prepared statements,
   ResultSet.getLong, MiniNumber.getAsLong, JavaDateString, clock and logger. Preserve
   partial-result mutation and SQLException-only catches. MAX_ONCHAINSQL_MILLI source
   is an int-expression widened to long (1000*60*60*24*1000 overflows signed32 before
   widening); do not accidentally replace with literal 86,400,000,000.
2. Pinned SqlDB.java fully reread; TxPoWDB.java and MinimaDB.java fully read previous
   scope. TxPoWSqlDB.java first ~100 lines still need a complete reread before porting
   (earlier combined output truncated). Wallet/ArchiveManager/CoinDB missing.
3. Reuse gate: sibling tools/minima-core-cpp/src/persistence/Database.hpp and its
   tests/persistence/test_persistence.cpp fully read; SQLite differs from H2 file,
   dialect, AES and SCRIPT/RUNSCRIPT. Sibling MinimaDB.hpp fully read; different schema/
   control flow. Current SqlRow, MiniFile full implementation, JSON interfaces, MiniData/
   MiniString, WalletRowsOracle/native harness/differential and Runner fully inspected.
   Search/read exact remaining candidates/callers before new code and state reuse report.
4. Main.java (1379 lines) NOT read this current SQL scope yet. Full Main runtime absent.
   Do not substitute a Java process/JNI, mock database, existing incompatible legacy
   C++ node or stub counterpart for the full native goal.

Earlier Network/RPC checkpoint follows for context:

# Latest continuation — 255 counterparts; native NetworkManager verified

Full457-file goal ACTIVE/unbudgeted/incomplete/unblocked. get_goal now reportsACTIVE
(supersedes stale paused notices),tokens3205163/time24366s at lastquery. DoNOTmarkdone.
255=236production+19tests;202absent. Allrows partial/not_verified;CLI24;fullMainabsent.
Pin5564b9d78978efcaf23679190d05253961fc6e69. NoJavaedits/commits/pushes/remotenodes.
No active builds/tests. No subagents. Userofftobed requestscontinuedautonomouswork.

New native NetworkManager.hpp/.cpp: allsourcebodies, actualNIO/P2P/P2P2/RPC ownership,
dummy peer MessageProcessor workers when disabled. RequiredNetworkManagerServices
contains existingNIO/P2P/P2P2services + getUserDB/clock/interfaces. Factory self before
init; weaknetworkcallbackforNIOpostP2P; invalidpeer callbacksactualP2PFunctions.
IMPORTANT NIOisShuttingDown remains requiredMAIN callback, NOTnetwork.mShuttingDown.
Reviewcaught/fixedinitialmisbinding; nativeworkerregressionguardsvalidCONNECT payload
withport-1 whileMainshutdowntrue thenchecksno pendingclient andnetworkflagfalse.
Ownerrefs protectedshortmutex; initialatomic<shared_ptr> unsupportedApplelibc++fixed.
RPCstart/stopmutex, actualCMDHandler; stopjoinslistener. Destructorcleanspartialinit.
join nativeownershipbarrier joinsNIO/peers andstops+joinschecker. SourcehardShutdown
doesNOTstopRPC orsetnetworkflag; regularshutdown does. Testsassertbothdifferences.
StatusactualNIOmaps/traffic; JavaDuration floornegative millis→seconds then /60;
MIN_LONG/-1 guard; preservesunusedgetUserDBreceiver incl nullignored/failureorder.
P2PEnvironment newHostInterface/localHostInterfaces reusesgetifaddrs/numeric; BSD
JDK prependsIPv4groups/addresses; isUp meansIFF_UP+IFF_RUNNING. IPv6groups irrelevant
tohostselection. LocalplatformhostselectioncomparesJava; otherOS/aliasesstillopen.
NativeJDKsource /tmp/minima-jdk26-NetworkInterface.c downloadedwithcurl (webfailed);
URL/hashinprovenance. Its enumIPv4/addif/isUp read. NotcompiledorcopiedJNIcode.

#80 support/network_manager_native.cpp reusesnio_manager_loopback/p2p2_worker patterns.
8actualworker combinations(P2P/P2P2/hard), NIO_INIT→P2Pqueue, P2P2DBinit, actualRPC
hash+restart, realNIOMessageping+traffic, soft/hardshutdown, ownerexpiry. Global
lookupsinstalled explicitlywithweaknetwork; realUserDB/P2PDB/P2P2DB/TxPowTree. No
externalpeers;NOCONNECTtrue, port0; nofullnativeMainorJava lifecycleparityclaim.
#81 support/network_manager.cpp,NetworkManagerOracle.java,tools/network_manager_
differential.py:540programs actualcalculateHostIP/getStatus, NIOmaps/NIOTraffic,
negativeages/signedrates, configfields, null/failingDB/NIO. ReusesPeerRuntimeFixture
andNIOClientOracle.install/Unsafe, separateprotocolstdout. JavaTrafficgetStartTime
useslive now-age; nativefixedclock1700000000000. Agesawayfromminuteboundaries,
renderedtraffic.fromnormalizedonly; fullDatealreadyhasseparateparitytests.

Final ALL-TARGETbuilds39466normal/64842san ended0. LatesttestfixturevalidCONNECT
guard change rebuilt60926/99388 both0 (no subsequentproductionchanges).
Final normalCTest22633:#63 1.06s,#64 1.58,#67 35.02,#78 2.63,#79 3.97,#80 .72,
#81 5.14;all7PASS50.13s. San50131:#63 .88,#64 1.51,#67 36.75,#78 2.76,
#79 4.48,#80 1.17,#81 9.47;all7PASS57.02s. Sourcemap255 regenerated+checked.
README/REVIEW/provenance currentthroughNetworkManager. RPC#79still2106requests.
NoTSANrunfornewNetwork/RPC; prior#73TSANscopeonly remainsvalid.

NEXT:
- Complete native database/Main integration and remaining202counterparts. DoNOT
  confuse current255counterparts/tests withfullworkingnodeor100%semanticparity.
- FullyREADthisturn pinnedupstream-study/src/org/minima/database/MinimaDB.java782lines
  and siblingtools/minima-core-cpp/src/database/MinimaDB.hpp(full). PinMain.java1379
  lines NOTreadthisscope yet. MinimaDB requires Wallet,ArchiveManager,TxPoWDB,
  TxPoWSqlDB,TxPoWOnChainDB,CoinDB, SQLDB and reentrantRWlock; these mostlyabsent.
  NativeUserDB,TxnDB,Cascade,TxPowTree,MegaMMR/P2P DBs alreadyexistandtested.
  SiblingMinimaDB usesdifferenttypes/schema/controlflow; doNOTsubstitutefullclass.
  Siblingpersistence/Database.hpp andnativeSqlRow.hpp readthisturn fornextreuse.
  NeedreadSQLsources/candidates/deps/tests and reuse reportbeforeimplementation.
- ExactJavaNetworkconstructor/shutdown differential, Wifi/alias/otherOSinterface
  cases, arbitraryconcurrentDB/globalmutation, fullMinimaLogger+quit remainopen.
- Actualchecker→manager promotionoverTCP stillneedsbothworkers exercise.
- Neverrerunoldtemporarysourcewriters; donotreruncostlyconsensus#29withoutchange.

# Latest continuation — 254 counterparts; actual RPC commands verified

254=235prod+19tests;203absent. Full457-file native goal incomplete/unblocked.
CMDHandler.cpp/SocketCharacterReader/JavaURLDecoder compile with nioReadSome adapter.
Productionfirstcompile passed. Existing HTTPServer native/Javafixtures now rpc mode;
actual CommandRunner+Authorizer. #79 cmd_handler_differential:2106requests/sixconfigs
exactresponses exceptDate, contentlength, UTF16counts, headers/auth/malformed/
fragmentedinput. Finalnormal82861 #782.14s/#793.98s; san10898 #782.36/#794.58 pass.
Earliernormalstatusfixture failed, thenrerun+3repeats passed; finaloracle redirects
logs tostderr withseparate protocolstdout using SearcherOracle pattern. Initial
#64nio_connections normal1.50/san1.92 pass. No activebuild/tests. SourceMap254.
Docs/review/provenance throughCMD. All-targets lastbuilt BEFORE CMD (52406/90440);
selected minima_http_server/nio_connections rebuilt both afterward32586/33455.
Next NetworkManager: fullyread pinned NetworkManager.java; native NIOManager/
services/P2PManager/P2P2Manager/MessageProcessor/P2PEnvironment, nio_manager_loopback
fixture; sibling noNetworkManager (NIOServer is simpler incompatible orchestration).
Need reuse report before implementing NetworkManager. Host discovery can reuse
getifaddrs/numeric from P2PEnvironment but needsordered IPv4interfaces. Downloaded
OpenJDK26u native NetworkInterface.c to /tmp/minima-jdk26-NetworkInterface.c; read
enumIPv4 BSD andaddif: groupsandaddresses prepend, IPv6 newgroups ignored byMinima.
Need inspectisUp flags and preserve wl-prefixfirstaddress selection, lastotherIPv4.
FullMinimaDB/Main/logger/quitcommand remainabsent. Earliercheckpoints below.

# Latest continuation — 253 counterparts; RPC listener verified, CMDHandler in progress

253=234prod+19tests;204 absent. Full native457-file goal incomplete/unblocked.
RPC Server/HTTPServer and shared SocketWakeup extracted from NIOServer compile.
#78 http_server_differential3programs/10TCPhandlers perimplementation passes normal
2.01s/sanitized2.22s. #58/#63 regressions pass both. All-target builds52406/90440
ended0. Docs/review/provenance through listener. CMDHandler/SocketCharacterReader/
JavaURLDecoder and nioReadSome now WRITTEN, NOT YET compiled/tested. HTTP fixtures
extended to optionally use actual CMDHandler. Need new RPC differential corpus,
builds, review, mapping update and continued full runtime port.
No active sessions at this checkpoint. Prior checkpoints below remain valid.

# Latest continuation — 251 counterparts; P2P2 helpers/manager verified

Full native 457-file objective incomplete/unblocked. 251=232prod+19tests;206absent.
Pin5564b9d78978efcaf23679190d05253961fc6e69. All rows partial/not_verified; CLI24.
No Java source edits, commits/pushes or remote node/peer started. No active sessions.

New prod P2P2Functions.hpp/.cpp and P2P2Manager.hpp/.cpp in native CMake library.
- Helpers reuse compatible P2PFunctions predicates/NIO/availability; independent
  invalid set and hostname cache; P2P2 logs unconditional as source. Ownenum/constants.
- Manager required services releaseLocks/getP2P2DB/getP2PDB; actualfactory/timerworker.
  Public numeric fields atomic; mutable static strings still non-null C++ strings.
- Source itself is unfinished: INIT adds324.45.45.45:9901 twice/dedup then9902 and
  reads first_startup without changing it; FASTLOOP schedulesfirst then availability,
  count and fresh randompeer but sendsNOconnection; SLOWLOOP/checkWhichConnect empty;
  convertOldP2P reads host/port only; declared P2P2_SHUTDOWN message hasNO branch.
- #76 support/p2p2_runtime.cpp, P2P2RuntimeOracle.java,
  tools/p2p2_runtime_differential.py:837programs/3374ops normal4.92s/san7.03s pass.
  Reuses P2PFunctionsFixture, P2PManagerOracle.stack, P2PFunctionsOracle.runPeer(empty
  ops) to installreceivers. DB logs excluded explicitly; manager/helper logs compared.
  Randommultiple400 draws checked membership + eachcount40..160, nosequenceclaim.
- #77 p2p2_worker.cpp actualTimerProcessor/factoryworker; semaphore releaseLocks
  completion barrier, init entries, real1msfastloop, ignoredshutdownmessage and
  explicitshutdown/join/weakownershiprelease. Normal1.31/san1.54 pass.
- All-target builds85588normal/10593san ended0. #59db regression normal4.03/san5.12
  passes alongside#77. Newprod compiledfirsttry; support JSONarrayptrderef corrected.
- Sourcemap regenerated251. README/REVIEW/provenance current throughthisscope.
- Risks: existing P2P2DB sharedvector externalconcurrency, mutable staticstringnulls,
  full MinimaDB/NetworkManager/logger binding. Don't call anything100%complete.

Next: native RPC Server/HTTPServer/CMDHandler before NetworkManager ownershipbinding.
Already fully read pinned NetworkManager.java, rpc/Server.java,HTTPServer.java,
CMDHandler.java and p2p2sources. Native authorizer/CommandRunner exist; socket support
NIOSocket.hpp/.cpp and NIOConnections.hpp/.cpp exist (NIOChannel.hpp doesNOT).
Sibling NIOServer.hpp exists; no matching RPCserver/NetworkManager implementation.
Need full relevant native/sibling/socket/command dependencies read + reuse report
before implementation. CMDHandler preserves source parsing/auth/body/read/URLdecode,
CRLFbehavior, actual CommandRunner, and quit runtimehalt afterclosing sockets.
Prior manager/concurrency/discovery checkpoint below remains valid.

# Latest continuation — 249 counterparts; manager parity, discovery and concurrency verified

Full457-file native objective incomplete and unblocked; pin5564b9d78978efcaf23679190d05253961fc6e69.
249 counterparts =230 production+19 tests;208 absent. All map rows partial/not_verified.
CLI24; full Main/node absent. No Java source edits/commits/pushes/remote node started.
User went to bed and authorized routine autonomous decisions. Cannot self-approve
system-enforced prompts. get_goal last reported paused after user interruption;
update_goal cannot resume goals. Continue authorized work; never mark complete early.

This head supersedes stale manager/concurrency gaps below. No active build/test sessions.
- P2PState.hpp scalar/reference access short-locks mStateMutex; toJson retains strong
  references; DNS address creation outside lock. Manager public counters/timer atomic.
- #73 peer_state_concurrent:2000 state replacements +2000 actual manager updates +
  16000 reads. Normal0.79s, ASAN/UBSAN1.85s, TSAN2.59s pass. Explicit checker stop/join
  before resetting callback references. Globals frozen; no full race-freedom claim.
- TSAN build cpp/build-thread-sanitized configured with -fsanitize=thread for compile
  and executable linking; entire native library + peer_state_concurrent built/passed.
- Extracted support/P2PFunctionsFixture.hpp from p2p_functions.cpp and
  support/P2PManagerFixture.hpp from p2p_manager_native.cpp. Existing tested code reused.
- #74 support/p2p_manager.cpp, P2PManagerOracle.java, tools/p2p_manager_differential.py:
  actual pinned/native methods, all maps/flags, peer sets, DB contents, queues,timers,
  failure order, routing payloads/lengths/logs. 405programs/2915ops/46packets pass both.
  Original success inputs using string DB/greeting/path peers were corrected to source
  address objects; positive greeting/walk assertions added. Final counts above only.
  Initial C++ fixture compilation corrected JSONObject pointer dereference; no prodfix.
- #75 tools/p2p_discovery_differential.py reuses checker Runner/Responder incl barrier,
  manager harness, PeersCheckerOracle.runChecker encode branch. ActualTCP/defaultNIO
  ping, real5sec pauses;31programs/84ops/39pings perbuild. Guard/no-default/no-connect,
  stopped, malformed reply/peer list, success and max3attempts. Bothpass.
- Final manager+discovery normal22222: #74 6.64s/#75 42.01s;48.66s total.
  Sanitized10077: #74 11.48s/#75 42.11s;53.59s total. Bothterminal0.
- State regressions normal17120 #54/#68/#69/#72 pass43.50s total;
  sanitized28694 samepass68.22s. Postextraction regressions normal67703
  #67/#68/#69/#72 pass45.88s;san41965 #67/#68/#69/#72/#74 pass63.77s.
- Fullnormal/san builds passed before support extraction; changed executables rebuilt
  in both afterward. Source map regenerated/check249; README/REVIEW/provenance updated.

Next work:
1. Remaining network-manager dependencies before native NetworkManager/Main binding.
   Read in current task: upstream NetworkManager.java, p2p2/P2P2Manager.java,
   P2P2Functions.java, rpc/Server.java, HTTPServer.java; native P2PFunctions.hpp/.cpp,
   P2P2DB.hpp/.cpp and NIOManager.hpp. Sibling search finds P2PSync (differentprotocol),
   no matching network or P2P2 manager. Sibling MinimaDB.hpp exists for later DBreuse.
2. P2P2Functions largely duplicates P2PFunctions but requires independent invalid set
   and local cache, different unconditional P2P2 logging. Reuse compatible helpers;
   do not alias independent Java statics. Full peer helper tests available.
3. P2P2Manager source itself unfinished: checkWhichConnect is empty, convertOldP2P
   reads peer host/port only; INIT inserts literal invalid addresses twice/deduplicates,
   FASTLOOP schedules then availability/count/random selection but no connect call,
   SLOWLOOP and declared shutdown message have no process action. Preserve pin.
4. Actual checker-to-manager promotion with both workers running and full lifecycle
   remains pending; prior checkerTCP tests and managerworker fixtures are reusebases.
5. Finish all missing production/test files and full native runtime, not just counts.

Do not rerun old temporary source-writing scripts. Do not rerun costly consensus#29
without a consensus change. Native binaries never use Java; Java only test oracles.

# Latest continuation — 249 counterparts; native P2PManager and real lifecycle

Full457-file native goal ACTIVE, unbudgeted, incomplete, unblocked.
Pin5564b9d78978efcaf23679190d05253961fc6e69 (1.1.2.8).
249 =230 production +19 mapped tests;208 absent. All rows partial/not_verified.
CLI24; full Main/node absent. No Java source edits, commits/pushes, wallet/miner,
remote peer/node started. This head supersedes older counts and manager status.

New native source:
- src/org/minima/system/network/p2p/P2PManager.hpp/.cpp in native CMake library.
  All pinned method bodies translated: initialize/init/discovery, processMessage,
  assessConnectivity/processJsonMessages/processLoop/walk routing, connect/status,
  sendMessages/updateP2PPeersList/shutdown/haveAnyPeers and getters/constants.
- P2PManagerServices in manager header: required releaseLocks/getP2PDB/saveP2PDB/getTip;
  currentTimeMillis defaults actual TimerMessage clock. Missing functions fail at
  use; no fake default DB. Full Main/MinimaDB/NetworkManager binding still absent.
- Protected constructor creates state before initialize applies TestParams. Factory
  gets shared ownership, initializes real checker and three timers, then starts its
  own worker. initialize and createPeersChecker protected allow unstarted fixtures.
  Destructor stops/joins owned checker, including factory initialization failure.
  Base destructor handles manager ownership barrier. Parent pointer in checker weak.
- Manager private static rand=JavaRandom48; fixture friend can seed it. Discovery
  default real NIOManager.sendPingMessage; protected pauseDiscovery sleeps5s.
- PeerShuffle.hpp EXTRACTS existing P2PGreeting OS-entropy Fisher-Yates code unchanged.
  Both greeting and manager discovery now reuse it; no new shuffle algorithm.
- All source quirks retained: initial saved count before addnodes, forced checker
  checks even noconnect, state before test params, init only setsHostSet iftrue,
  comma token trim / rootnode split trailing-empty removal, firstgo debug peer,
  discovery loop only when flag alreadytrue (init/loop doNOT enable it),3attempts,
  no-default3min logthrottle, cap blocks even existingpeer readd, clear>50 preserves
  three timer types, float client division allowsInf/NaN, shutdown setsversion and
  updateslist but doesNOT saveDB, save only>0 and>=INITIAL/2.
- NOCONNECT keeps source containsValue check then per-entry required .equals (null
  entry may throw only after a match exists), reclassifies matching UIDs to allLinks
  address. JSON source branch ordering preserved. Explicit host/port evaluation
  before native address use avoids null receiver UB in new methods.

#72 p2p_manager_native:
- support/p2p_manager_native.cpp. Reuses PeerRuntimeFixture.hpp, real P2PDB/state,
  actual NIOManager/NIOClient maps/traffic/serialization. P2PManagerFixture executes
  actual manager method bodies but captures timer requests and creates IdleChecker
  (actual class, unstarted queue). Native-only assertions, no Java parity claim.
- Covers three initialtimers, forced savedpeer messages/order, originalDBcount/flags,
  addressslash stripping, cap/peer copy, persistence threshold/savecount, overload
  clear exclusions, connectivity assessment, seeded loop delay, IEEEfloat count,
  actual connect greeting serialization, greeting checker admission/flag/state,
  statuscounts, failed-connection reclassification/disconnect maps and shutdown.
- Testmode check: state noneP2Pmax100 created beforeTestParams setsglobal2, initlater
  setsstate2; connectivity timer60sec. Test settings singleton resolves minima_one
  but no connection is started (fixture checker unstarted; actual workerNOCONNECT).
- Separate actual P2PManager::create starts actual manager ANDchecker workers; actual
  TimerProcessor thread dispatches1ms P2P_ADD_PEER. Wait checks synchronized known
  peer set as well as getLastMessage to avoid treating message start as completion.
  Actual checker rejects127.0.0.1, manager P2P_SHUTDOWN stopsboth; join completes,
  weak manager/checker references expire even while pending timers remain.
- Noactual manager discovery ping or checker→manager promotion overTCP yet; those
  need end-to-end integration using earlier checker responder. No remotepeer sockets.
- Initial fixture compile failed explicit std::function MessageObject constructor,
  nonexistent map.isEmpty/JSONValue.asInteger; corrected fixture, production compiled
  first try. First test1.73sec passed before addedtestmode+strongerwaitpredicate.

Final verification (no active sessions):
- Both CMakeconfigured. ALL-TARGET builds38136(normal),71396(sanitized) ended0.
- Final normal CTest76739: #54PASS32.75s,#68PASS4.91s,#72PASS1.42s;39.09s total.
- Final sanitized CTest90141: #54PASS50.23s,#68PASS8.37s,#72PASS1.72s;60.32s total.
- #54 still874programs/26348ops; #68 still651/1959. Shared greeting shuffle extraction
  is the existing-production change tested by these regressions.
- Earlier #67/#69/#70/#71 prior checkpoint valid, production not changed this turn.
  No consensus change; doNOT rerun costly #29 just for linkage/newmanager.
- Source map regenerated249/457 andcheckpassed. README/REVIEW/provenance updated
  with native-only manager evidence and explicit concurrency/Java/runtime gaps.

NEXT REQUIRED WORK:
1. Fix shared-state C++ races before full runtime wiring. P2PManager now runs a real
   worker; P2PState scalar fields and address/shared-pointer fields are plainstorage.
   Concurrent getStatus/getP2PAddress/getClients with worker updates can be UB. Public
   manager INITIAL_PEERS_LIST_NUM/LAST_SAVED_PEERS_AMOUNT/HEALTHtimer fields need
   corresponding review. Current worker test only reads synchronized getPeersCopy
   while active, so sanitizer pass is NOT proof of concurrent status safety.
   Read pinned/native P2PState and existing atomic/mutex patterns before changes;
   preserve nonblocking getters, don't hold one broad lock acrossnetwork waits.
2. Add P2PManager Java differential using actual pinned class. Extend existing
   P2PFunctionsOracle/NIOClientOracle and native PeerRuntimeFixture; extract/reuse
   manager fixture from p2p_manager_native if suitable. P2PManagerFixture is friend;
   state/private rand accessible in fixture. Java Unsafe allocation bypassesworker,
   required fields: state=P2PState, mPeersChecker=checkerfixture, EXCLUDE_FROM_CLEAR,
   allpubliccounters/timers, mLastNotifyNoPeers=0, NOTIFY_NOPEERS_TIMER180000, firstgo.
   MinimaDB subclass getP2PDB/saveP2PDB capture effects but use actual P2PDB contents.
   Pinned protected init/processJson/processMessage accessiblefromsubclass; private
   loop/send/discovery reachable through actual messages or reflection.
   Compare state/toJson+maps/flags, queuedchecker messages, timers/delays, DB contents,
   log bodies, actual NIOqueue/traffic/serialization and failure ordering. Seed manager
   rand by existing reflection as Walk/Util; generatedsecrets/shuffles need declared
   canonicalization consistent with earlier suites. Don't replace actual behavior.
3. Real discovery/parent integration: reuse tools/peers_checker_differential.py TCP
   Responder including barrier! Java PeersCheckerOracle encode utility supplies framed
   Greeting containing peers-list. No editing pinned Java. Discovery cases should
   exercise actual NIOManager sendPing,3attempts, malformed peers, success clearing
   discovery, verified+known membership and missingpeers retry. Java Thread.sleep
   failure path5s real; testtimeout accordingly or inject interrupt meaningfully.
4. Aftermanager validation, continueNetworkManager/Main/database/processors/miner,
   remainingcommands and mappedtests. DoNOT count requiredcallbacks as implementations.

Relevant reads this turn:
- Full pinned P2PManager.java (661lines) reread, pinned P2PDB/connect. GeneralParams
  field/default checks. Full native P2PManager newfiles, P2PGreeting/SwapLinksFunctions/
  WalkLinksFuncs bodies, allrelatedheaders, PeerOperations, NIOManagerServices,
  Message/MessageProcessor/MessageStack, TimerProcessor.hpp/.cpp, nativechecker test.
- Current/sibling searches: no native P2PManager, pinned SwapLinks/WalkLinks tests;
  sibling tools/minima-core-cpp P2PSync header1..100 inspected confirmsdifferentflow
  (wholecandidate tested/read earlier task). No delegation authorized.
- Never rerun old temporary source-writing scripts. Keep fullgoal active.

# Latest continuation — 248 counterparts; checker Java/TCP and test settings verified

Full 457-file native goal ACTIVE, unbudgeted, incomplete and unblocked.
Pin 5564b9d78978efcaf23679190d05253961fc6e69 (1.1.2.8).
248 =229 production +19 mapped tests;209 absent. All map rows partial/not_verified.
CLI24; full Main/node absent. No Java source edits, commits/pushes, wallet/miner,
remote peer/node started. This head supersedes earlier statuses and counts.

New production:
- src/org/minima/system/network/p2p/params/P2PTestParams.hpp/.cpp, native library.
  Full pinned defaults and setTestParams assignment order. Singleton values() exposes
  mutable settings corresponding to Java static fields; use values().FIELD in C++.
  First access initializes the fixed-size JavaList containing InetSocketAddress
  minima_one:9001, adapting existing Crypto::getInstance's function-static pattern.
  No eager DNS lookup merely by linking the class. setTestParams copies scalar
  values and SHARES DEFAULT_NODE_LIST; repeated/replaced/null lists supported.
- P2PParams.LOG_LEVEL and test LOG_LEVEL now optional<P2PLogLevel>. Java null enum
  comparisons disable all three filtered logging methods; native now represents it.
  Existing P2PFunctions comparison bodies unchanged. All dependent targets rebuilt.
- Reuse gate completed: pinned TestParams, sibling core/Minima TestParams identical;
  current P2PParams/JavaList/InetSocketAddress/Crypto fully read. Existing peer-state
  tests/oracles fully read and extended. Sibling tools/minima-core-cpp search finds
  P2PSync rather than test settings; no suitable native test-settings implementation.

Checker verification (#71 peers_checker_differential):
- support/PeerRuntimeFixture.hpp extracts existing p2p_functions.cpp manager/client
  fixture unchanged, adding TimerMessage delay accessor via existing friend.
- support/peers_checker.cpp invokes actual checker processMessage/default real
  NIOManager ping/default fresh JavaRandom48. Unstarted checker captures queue/timer
  requests, real manager traffic/maps; parent Queue retained. No fake ping/random.
- support/PeersCheckerOracle.java extends P2PFunctionsOracle/NIOClientOracle. Unsafe
  allocates checker/parent/database receivers to avoid full worker startup; actual
  pinned processMessage and STATIC NIOManager.sendPingMessage execute unchanged.
  Java-generated TxPoW blocks build actual Java/native TxPowTree/TxPoWTreeNode.
- tools/peers_checker_differential.py uses persistent processes, temporary stderr
  files and binary buffered stdout. Controlled loopback responder sends exact Java
  Greeting frames; records full request frames. Offline HTTPS availability hook.
- 339 programs /758 operations /329 real pings agree: exact bytes, traffic, peer
  sets/order, parent messages, timer delays, logs and tree lookup effects. Null
  receivers/addresses/flags, valid/wrong/missing/malformed network/chain data,
  promotion/removal/caps, disconnected retry, loop/force and alias checks included.
- Fresh-Random admission:500 trials per implementation with 15..100 admissions and
  queue/state invariants. Counts legitimately differ; DO NOT compare stochastic
  counts exactly. Nondeterministic multi-peer eviction not proved exact.
- First sanitizer run found a FIXTURE trace race: absent Main makes ping fail after
  writing but before reading, so responder could record Java requests after reset
  for native. State agreed but trace counts1/3. Added fixture-only control connection
  (-1 length, ! ack) to drain earlier connections before snapshot/reconfiguration.
  Final both pass329pings. OLD build-sanitized/peers-checker-failure.json is stale;
  final results/logs pass, not a current failure.
- #70 retains separate actual worker startup/stop/join/shutdown ownership coverage.
  Full parent, Main/DB/logger binding and actual timer dispatch remain outstanding.

Parameter verification (#54 peer_state_differential):
- Existing support/peer_state.cpp and PeerStateOracle.java reuse extracted active
  parameterSnapshot. New testParams programs compare all settings before/after
  assignment, every scalar boundary, nullable logging/metrics URL, fixed-list
  failures, element mutations, replaced/null/resizable lists and shared identity.
- 76 new programs /230 operations; full suite874 programs /26348 operations passes
  normal/ASAN/UBSAN. Active null-list snapshot guarded before iteration.
- #67 actual logging suite adds null level. Now672 programs /3670 operations plus
 39 real HTTP loopback cases. P2PFunctionsOracle/p2p_functions support nullable level.

Final builds/tests (no active sessions):
- CMake configured both; final ALL-TARGET builds91095 and10014 ended0.
- Final normal CTest80319 all6PASS: #54 31.83s,#67 34.78s,#68 4.59s,#69 4.58s,
  #70 0.37s,#71 6.13s;82.27s total.
- Final sanitized CTest9018 all6PASS: #54 50.88s,#67 35.81s,#68 7.95s,#69 7.77s,
  #70 0.68s,#71 10.71s;113.81s total.
- Earlier all-target builds75215/93559 and targeted50666/95711 also ended0 before
  nullable-enum change; superseded by final builds above.
- Source map regenerated248/457; --check passes. Python AST/JSON parse/gitdiffcheck
  passed. README/REVIEW/provenance updated. Source files remain partial; no full
  runtime completion claimed. Consensus code unchanged; do not rerun costly #29.

Next: P2PManager, then NetworkManager/Main/database/processors/miner/commands/tests.
- Pinned P2PManager.java full read across full file +225..405 retry after truncated
  combined output. No native P2PManager exists. Don't confuse this with new fixture.
- Current dependency reads: P2PState.hpp/.cpp, P2PDB.hpp/.cpp and mapped P2PDBTest,
  PeerOperations.hpp, SwapLinksFunctions.hpp, WalkLinksFuncs.hpp, P2PPeersChecker.hpp,
  MessageProcessor.hpp/.cpp, MessageStack.hpp, SocketAddressSet.hpp, JavaRandom48.hpp,
  connect.hpp/.cpp, P2PParams and TestParams. Source GeneralParams/pinned dependency
  implementations and relevant tests/callers should be completed before reuse report
  and writing manager. No manager-specific pinned tests found in prior search.
- Read NetworkManager startup45..105 and shutdown234..287; NIOManager startup310..330,
  disconnect/reconnect458..510/608..624 and removal918..940. NIOManager native already
  emits real postP2P callbacks; full NetworkManager wiring remains absent.
- Manager constructor state field initializes BEFORE it applies TestParams. Preserve
  that source ordering, then construct checker and schedule initial10sec loop,
  connectivity delay,5min save. Shared factory needed before timers/start workers.
- init reads actualP2PDB/version/peers, posts forced checker checks, handles configured
  addnodes with existing connect.createConnectMessage, assigns flags/limits, chooses
  root/saved peer or discovery. INITIAL_PEERS_LIST_NUM is captured before addnodes.
- doDiscoveryPing returns/log-throttles if defaults empty; otherwise loops only when
  discovery flag AND isRunning, max3 attempts, actual ping, shuffle/admit both sets,
  sleeps5sec on null greeting. No implicit discovery flag change in init/loop.
- processMessage health clear>50 preserves3timer message types; save updatesDB then
  saves and schedules; connected/disconnected/json/loop/reset/random/noConnect and
  add/remove peer branches translate existing helpers. Add peer ignores even existing
  peer if alreadyatcap. NoConnect moves matching inlink IDs into notaccepting map.
- processJsonMessages obtains current NIOClientInfo before JSON access, null client
  returnsdisconnect. Greeting queuesknown/sourcepeer tochecker thenSwap processing;
  req/resIP, notaccepting map reset, walk and do-swap paths have exact source order.
- processLoop random delay and removemyaddress occur even noconnect; preservedoutlink
  selection/scale/inlinks/loadbalance branches and DB reload beforediscovery.
- getClients uses float division (zero ->IEEE behavior). getStatus measuresactual
  connectedinfos. updateP2PPeersList stores only size>0 and size>=INITIAL/2. shutdown
  setsDBversion, updateslist, stopschecker, logs, stopsmanager; doesNOT saveDB itself.
- Reuse established required service callback boundaries only for missing MinimaDB/
  Main binding; don't count callbacks as those implementations. Full parent lifetime,
  shared concurrent state, generic exceptions/resource behavior remain toverify.
- No subagents authorized. Never rerun old temporary source-writing scripts.

# Latest continuation — 247 counterparts; checker native implementation, Java validation pending

Full457-file native goal ACTIVE, unbudgeted, incomplete, unblocked.
Pin5564b9d78978efcaf23679190d05253961fc6e69 (1.1.2.8).
247 = 228 production +19 mapped tests;210 absent. All rows partial/not_verified.
CLI24, full Main/node absent. No Java source edits, commits/pushes, wallet/miner,
remote peer/node started. This head overrides all earlier counts/statuses.

New production files:
- src/org/minima/system/network/p2p/P2PPeersChecker.hpp/.cpp in native CMake library.
  Methods/branches fully translated, but NOT Java-differential verified yet.
- Factory starts inherited MessageProcessor after shared construction; constructor
  sets full logging false and queues PEERS_INIT. Existing set getters return same
  shared mutable sets. MAX_VERIFIED_PEERS atomic<int32> default250; LOOP_TIMER21600000.
- Uses existing SocketAddressSet, Message/TimerMessage, native P2PFunctions and
  NIOManager.sendPingMessage, Greeting/MiniNumber/TxPoWTreeNode.getPastNode.
- Protected virtual ping defaults to real native TCP; protected newRandomInt defaults
  to a freshly constructed JavaRandom48 per call. Fixture overrides only for native
  tests. Actual Java new Random nondeterminism still requires validation.
- Required mTip callback used only for nonempty remote50block/hash with valid network
  version; inherited releaseLocks callback required on construction.
- Parent weak_ptr<MessageStack> allows actual P2PManager as receiver without fake
  mapped P2PManager now; avoids ownership cycle. Java holds parent strongly; full
  lifetime parity and complete P2PManager/MinimaDB/Main lifecycle not proven.
- Exact source quirks: force-full-check sets forcelog, not force; ADDPEERS obtains
  local addresses even unused; source 10% overflow admission; verified-at-cap removal
  occurs before replacing candidate; choosing null from verified set does NOT remove
  null; accepts no tip/below remote check block; rejects missing/hash-mismatched past
  node; failed verified peer ->unverified+30min retry, disconnected check->60sec retry.

#70 peers_checker_native:
- support/peers_checker_native.cpp. Native-only, NO Java comparison count claimed.
- Checker subclass invokes actual processMessage, captures actual TimerMessage delay
  via existing friend NIOManagerFixture, overrides ping and random selection. Queue
  subclasses expose protected drain only in fixtures; no production API widening.
- Assertions admission/dedup/local reject, promotion, invalid verified/unverified,
  89/90 random boundary, cap1 eviction, null-selection overflow, force flags, loop
  online/offline behavior, exact30min/6hour delay. No actual network in this test.
- Separately uses actual P2PPeersChecker.create worker and waits on releaseLocks
  notification, stop/join, shutdown flag and weak expiration. This is actual native
  worker execution, not a simulated callback.
- Normal PASS0.55s; ASAN/UBSANPASS0.59s. Final ALL-TARGET builds94191/17743 ended0.
  No active sessions. First builds failed missing SocketAddressSet include, then
  fixture protected queue access; both fixed before final successful build/tests.
- Source map regenerated247/457; README/REVIEW/provenance updated with pending Java
  verification, chain checks, real transport, timer dispatch and parent lifetime gaps.
- No existing peer/differential code changed; prior #67/#68/#69 passing results below.

NEXT REQUIRED WORK: checker Java differential and integration, then P2PManager.
- Do not restart checker production translation; file exists/compiles. Do NOT claim
  #70 establishes parity. Native fixture currently supplies null tip and fake reply.
- Actual Java checker calls STATIC NIOManager.sendPingMessage, so a meaningful Java
  suite needs a controlled local TCP responder rather than overriding checker ping.
  Use existing P2PFunctionsOracle/NIOClientOracle manager/Main fixtures, real traffic.
  A Python responder can serve framed MiniByte12+Greeting using same local port for
  Java and native. Drive persistent oracle processes per program with configured
  reply before each request; capture stderr safely to avoid full-pipe deadlocks.
  Do not modify pinned Java to inject fake static calls.
- P2PFunctionsOracle nested fixture can subclass P2PPeersChecker and allocate via
  Unsafe to bypass worker creation; override PostMessage/PostTimerMessage captures,
  call actual superclass processMessage. Parent fixture subclasses P2PManager via
  Unsafe, captures PostMessage. Set its private peer sets and parent through existing
  field helpers. Java mDelay reflected for exact scheduled durations as prior suites.
- Existing support/p2p_functions.cpp could be extended with checker ops, real native
  queues, set snapshots, actual default ping. Or create separate harness reusing
  existing classes/helpers; prefer minimal reuse, not duplicated fake manager logic.
- NIOMessageOracle already has MinimaDB fixture overriding getTxPoWTree and uses
  TxPoWTreeNode(decode(raw)) test constructor. Native equivalent TxPoWTreeNode(TxPoW)
  exists and creates empty MMR; use it for exact chain identities instead of fake
  tip arithmetic. Existing NIOMessage/ArchiveCache power fixture generation reusable.
- FULL relevant reads this turn: MessageProcessor.hpp/.cpp, TimerMessage.hpp/.cpp,
  SendPollManager.hpp/.cpp, ChainServices.hpp, NIOManager.hpp, actual sendPing body,
  Greeting.hpp, JSONObject.hpp/.cpp, TxPoWTreeNode.hpp and cpp1..58, TxBlock.hpp and
  JavaTxBlock1..65. P2PManager checker callers70..105/143..166/620..656 read plus rg.
  No P2PPeersChecker tests found in upstream/test. Sibling only P2PSync different flow.
- Random new-per-call admission/eviction is nondeterministic in pinned Java; test
  deterministic one-choice/cap cases plus meaningful stochastic invariants where
  necessary, rather than changing Java or forcing matching random outputs.
- Typed exception/logging, full concurrent set iteration, generic reference lifetime
  semantics remain open for full goal. No blocker; keep active and continue.

# Latest continuation — 246 counterparts; WalkLinksFuncs

Full 457-file native goal ACTIVE, unbudgeted, incomplete and unblocked.
Pin 5564b9d78978efcaf23679190d05253961fc6e69 (1.1.2.8).
246 = 227 production + 19 mapped tests; 211 absent. All map rows partial/not_verified.
Native CLI24 commands, full Main/node absent. No Java source edits, commit/push,
wallet/miner/remote peer/node started. This head overrides all earlier sections.

Implemented:
- src/org/minima/system/network/p2p/WalkLinksFuncs.hpp/.cpp, CMake native library.
  All10 methods: map filtering, out/in walk routing, next hop, swap generation,
  UID-to-address search, reverse response routing, returned walk/connect attempts,
  returned load-balance and load swaps. Existing peer maps/codecs/NIOManager execute.
- support/PeerOperations.hpp extracts existing SwapLinksFunctions required/put/
  remove/values/send/count helpers unchanged, plus nullable map get; Swap reuses them.
- P2PFunctions::log made public native adapter (same implementation/callback), reused
  for direct MinimaLogger body calls. Full MinimaLogger prefixes/MINIMALOG pending.
- Separate JavaRandom48 Walk random preserves generator's source nextInt(size-1),
  including excluding final HashMap key and failure when only1 key. UtilFuncs random
  remains separate. Source9-hop cutoff, signed32 slot subtraction and first-match
  address removal retained. Null-own address arguments safely compare unequal.
- Review corrected nullable destination host/port call evaluation: explicitly require
  receiver, read host then port, then checkConnect. Tests reject a local address then
  reach null next entry, retaining Java partial path/log state rather than native UB.

#69 walk_links_differential:
- tools/walk_links_differential.py extends existing support/p2p_functions.cpp and
  support/P2PFunctionsOracle.java with w_* ops and persistent nullable walk snapshot.
  Fresh all369Java + existing NIOClientOracle fixture; native uses actual peer maps,
  flags, codecs and manager queue. No actual node, miner, DNS peers or remote sockets.
- 883 programs /2153 operations PASS normal4.67s and ASAN/UBSAN7.82s.
  Groups address16,filter24,generate180,load300,map-failures8,next-response60,
  null-receivers8,own-null10,returned144,routing128,null-after-rejection2,walk-secret3.
- Capture path/secret/returning/client/slots/joining/in/UID plus full peer state,
  log notifications and connection messages. Generated load-swap secrets normalized
  by stable length/identity tokens; fixed walk secrets byte exact. Path-changing
  observations: 329 in the final matching normal/sanitized reports.
- Guards require walk/do-swap messages, actual NIO_CONNECT, returning flags, multiple
  result kinds/errors/path mutations, one-key random failure and final-key exclusion.
  Initial comparisons all agreed but last-key guard assumed numeric UID order;
  corrected to independently captured Java map bucket order (c7 need not be last).
- Fixture noMain log capture now skips notification body events to match Java's
  absent Main PostNotifyEvent receiver. Real logger console/prefix parity still open.
- Generic typed exceptions/log text, concurrent mutation, allocation/resource limits
  and full lifecycle remain unverified. Source-map status must remain partial.

Validation:
- Final ALL-TARGET normal74223 and sanitized28077 ended0. CMake configured both.
  No active sessions. No production/fixture changes since final builds/tests.
- Final CTests normal38555: #67PASS34.38s, #68PASS4.73s, #69PASS4.67s (43.77s total).
  Sanitized45670: #67PASS36.47s, #68PASS8.14s, #69PASS7.82s (52.44s total).
- Normal/sanitized #69 reports agree; source map regenerated246/457. AST/changed-source
  whitespace and diff checks pass. README/REVIEW/provenance updated with scoped facts.
- Earlier costly consensus/dispatcher checks unchanged and not rerun unnecessarily.

Next:
- Continue P2PPeersChecker and P2PManager, then NetworkManager/Main/db/processor/miner,
  remaining commands and mapped tests; do not count callbacks as those implementations.
- FULL pinned P2PPeersChecker.java read this turn. Constructor starts MessageProcessor,
  posts PEERS_INIT. Methods haveAnyPeers,checkUnverifiedPeer,processMessage,private
  removeRandomItem. Peers sets SocketAddressSet already tested; NIOManager sendPing
  native already implemented. Needs actual tip lookup and manager PostMessage binding.
- Preserve checker quirks: checkUnverifiedPeer caps unverified, verified-at-cap uses
  NEW Random().nextInt(100) 10% admission; random removal may select null and then
  not remove it; PEERS_FORCEFULLCHECK messages set forcelog but NOT force; ADDPEERS
  gets cached local addresses even unused; TEST substring network check; chain check
  accepts tip below remote block or no local tip, rejects absent checknode/hash mismatch;
  verified-at-cap removal occurs before replacing/adding current address.
- Main MessageProcessor startup/ownership/timers must use existing shared worker
  patterns; keep full runtime distinction. Random new-per-call requires meaningful
  stochastic assertions or deterministic single-choice cases; do not edit pinned Java.
- P2PManager caller blocks500..543 and WalkLinksFuncsTest FULL read; prior native
  P2PDoSwap.hpp/.cpp plus pinned P2PDoSwap.java FULL read. QuickState/QuickInetLists
  read earlier. Their mapped tests/helpers still missing, not replaced by oracle.
- Reuse gate for new checker scope still needs current/sibling search, dependencies/
  callers/tests inspection and exact reuse report before code. No subagents authorized.
- Do not rerun old temporary source-writing scripts. Full goal has no blocker.

# Latest continuation — 245 counterparts; SwapLinksFunctions

Full 457-file native goal ACTIVE, unbudgeted, incomplete, unblocked.
Pin 5564b9d78978efcaf23679190d05253961fc6e69 (1.1.2.8).
245 = 226 production + 19 mapped tests; 212 absent. All map rows partial/not_verified.
Native CLI24 commands; complete Main/node runtime absent. No source Java edits,
commit/push, wallet/miner, remote peer/node started. These facts override prior heads.

New implementation:
- src/org/minima/system/network/p2p/SwapLinksFunctions.hpp/.cpp in CMake native library.
  All pinned methods translated: wrapP2PMsg, onConnected, load-balance request,
  onDisconnected, processGreeting, request/response IP, joinScaleOutLinks, requestInLinks.
- Uses actual P2PState/maps, P2PFunctions/UtilFuncs, P2PGreeting/P2PWalkLinks, MiniData,
  Message and native NIOManager/NIOServer/NIOClient flags. Required mainPresent
  std::function distinguishes absent Main from absent manager. No silent fake Main.
- Exact source quirks retained: duplicate incoming host:0 still inserted after
  disconnect request; outgoing duplicates still receive greeting; balancing condition
  tests not-accepting count but multiplier uses inLinks/maxP2P; zero divisor throws;
  discovery reset occurs before disconnect; greeting removes none map before runtime
  lookup/flag; null client evaluates greeting JSON before debug-level filter.
- wrapP2PMsg holds original JSONObject shared pointer; explicit alias test mutates
  inner JSON after wrapping and observes mutation through wrapper.
- No mapped SwapLinksFunctionsTest/QuickState/QuickInetLists native files yet. Their
  Java sources fully inspected; don't claim differential suite is their mapping.

Test #68 swap_links_differential:
- tools/swap_links_differential.py extends existing support/p2p_functions.cpp and
  support/P2PFunctionsOracle.java. No duplicate fake manager or Java runtime in native.
  Freshly compiles all369Java plus NIOClientOracle and P2PFunctionsOracle.
- Root swap flag activates state config/snapshots and new s_* ops; old #67 unchanged
  when flag absent. Capture nullable maps, exact address snapshots, current IP secret,
  known-peer list, discovery/host flags, actual NIO P2P-greeting flag and posted queue.
- 651 programs / 1959 operations PASS normal4.76s and ASAN/UBSAN8.02s.
  Groups connected16,connected-errors32,greetings256,greeting-errors32,disconnect1,
  disconnect-partial12,ip-request40,ip-response40,response-partial12,walks200,
  walk-errors8,wrap1,wrap-identity1. 28 IP-request and232 walk message observations.
- Deterministic outputs and partial effects exact. OS-generated secrets canonicalized
  only as stable tokens preserving length/equality/reuse. Corpus fixed secrets short,
  so not normalized. Independently shuffled greeting knownPeers compared as multiset;
  state known-peer order remains exact. Guards require send/disconnect/NIO_DISCONNECT,
  true/false greeting flags, success/error results and actual secret/walk output.
- First comparison detected fixture temporary list expiration: for(*list(...)) used
  a temporary shared owner. Fixed by retaining local known list before range loop.
  Generic exceptions/log-prefixes, allocation/receiver failures and full concurrent
  interleavings remain unverified. Main/P2PManager/logger bindings still required.

Build/check status:
- Final ALL-TARGET normal55011 and sanitized95441 ended0 after all production/fixture
  edits. CMake both configured with #68. No active sessions.
- Final CTests normal2616: #67PASS34.36s, #68PASS4.76s (39.13s total).
  Sanitized5408: #67PASS36.07s, #68PASS8.02s (44.10s total).
- New normal/sanitized report JSONs agree; source-map regeneration and --check PASS
 245/457. Python AST/changed-source whitespace/git diff --check PASS.
- README/REVIEW/provenance updated. Previous expensive consensus/dispatcher suites
  not rerun: their production logic unchanged and prior validated results below.

Next:
- Continue WalkLinksFuncs.java then P2PManager, peers checker, NetworkManager/Main/
  databases/processor/miner and remaining commands/tests. All still required.
- FULL WalkLinksFuncs.java read this turn (267lines). Its tests/callers/dependency
  P2PDoSwap still need relevant full reads before code. Prior native P2PWalkLinks and
  P2PState/UtilFuncs/Message dependencies fully inspected and tested. Search siblings
  found only P2PSync different sync loop, not matching native helper implementation.
- Preserve WalkLinks source quirks: generateDoSwap picks random.nextInt(uids.size()-1)
  (excludes last key, throws when only1), path limit <9, removeIPs null x method
  reference throws even if B empty, onReturnedWalkMsg removes FIRST matching address
  object rather than unconditional last element, checkConnect side effects before
  path removal. Client-slot arithmetic needs defined Java signed32 overflow.
- Walk helpers need direct MinimaLogger body logging (P2PFunctions.log is private);
  reuse a shared logging boundary or port actual MinimaLogger, no fake mapped class.
- Do not rerun old /tmp write scripts or duplicate completed counterparts. Goal has
  no blocker and must remain active until full compiled native node/API parity.

# Latest continuation — 244 counterparts; P2PFunctions / UtilFuncs

Full 457-file native goal remains ACTIVE, unbudgeted, incomplete and unblocked.
Pin 5564b9d78978efcaf23679190d05253961fc6e69 (1.1.2.8).
244 = 225 production + 19 mapped tests; 213 absent. Every source-map row remains
partial/not_verified. Native CLI still 24 commands; complete Main/node absent.
No Java source edits, commit, push, wallet, miner or remote peer/node started.
These facts override 242-and-earlier sections below.

Implemented:
- src/org/minima/system/network/p2p/P2PFunctions.hpp/.cpp: constants, invalid-peer
  set, exact prefix/colon checks, connect/check/disconnect, actual manager/client
  queries, type7 MiniString/JSON sends, three log-level filters, cached interface
  set and actual native HTTPS availability default. Required manager runtimeLookup;
  optional logger/platform callbacks for tests. Full Main/logger binding pending.
- src/org/minima/system/network/p2p/UtilFuncs.hpp/.cpp: actual JavaHashMap key-order
  search, in/out/not-accepting/none precedence and random address selection.
- support/JavaRandom48.hpp: atomic 48-bit LCG, power-of-two and signed32 rejection
  bounded nextInt matching installed JDK source. Default seed from MiniData OS RNG.
- support/P2PEnvironment.hpp/.cpp: native getifaddrs/getnameinfo/getaddrinfo hostname
  enumeration with reverse+forward confirmation and existing numeric IPv6 rendering.
  Native libcurl HTTP(S) URL adapter closes after headers, checks >=400 explicitly,
  follows same-protocol 300..307 except304/306, max20 responses. Receive failure/empty
  response retry once (fresh socket). HTTP305 proxy uses original URL AND cached
  origin request path, matching actual JDK behavior. Regular redirect clears proxy.
- CMake now requires CURL7.85+ (SDK libcurl8.7.1 tested), links CURL::libcurl publicly.
  P2PLogLevel remains shared enum aliased by P2PFunctions::Level.

#67 p2p_functions_differential:
- support/p2p_functions.cpp, support/P2PFunctionsOracle.java and
  tools/p2p_functions_differential.py. Reuses NIOClientOracle Unsafe fixture only
  to bypass actual Main/worker construction; executes pinned actual helpers. Native
  uses actual manager queues/maps/client sends/traffic. Freshly compiles all369Java.
- 671 programs / 3666 operations PASS normal35.49s and ASAN/UBSAN36.49s.
  connection-policy576; invalid-first48; queries2; send4; logging3; invalid-set1;
  availability5; cached-locals1; real-locals1; map-precedence6; map-search18;
  random-selection6 (seeds0,1,-1,MAX/MINlong,5564,193ops each).
- Coverage requires connect/disconnect effects, actual type7 packets, true/false/
  ERROR/OK results. UTF16 sorted nullable sets, shared cached-set identity, null
  receivers, local prefix oddities, missing manager, seeded rejection bounds covered.
- 39 actual loopback HTTP cases compare BOTH outcomes AND exact request traces:
  status codes, drop, redirect loop, redirects, self305proxy and successful separate
  proxy. Failed initial runs caught missing drop retry and incorrect absolute-form
  proxy path; both fixed, no normalization of traces. No real Google request.
- Availability method itself uses offline https URL provider to compare open/connect/
  getInputStream/close order and failures; separate root-url probe exercises real
  native/JDK HTTP transport. Oracle installs https handler, so it cannot currently
  validate real HTTPS without changing that setup.
- Native default TLS trust/proxy/authenticator/cache/resource semantics remain
  incomplete/unverified. Curl default connect timeout300s differs from Java default
  unlimited; response stream close/drain/malformed-body semantics still open. Do not
  claim complete URLConnection parity. Exact MinimaLogger prefix/MINIMALOG pending.
- Local hostname set matches this machine, other network/provider failures untested.
  Default Random seed construction differs but seeded algorithm matches. P2PFunctions
  diagnostic Java main has no standalone native mapped entry point yet.

Build/verification:
- CMake normal and sanitized configured. Final ALL-TARGET builds28684(normal),
  35335(sanitized) ended0 after all production edits. No active tool sessions.
- Final #67 CTests30986/79515 ended0; report JSONs agree exactly.
- source_map.py and --check PASS244/457; Python AST and provenance JSON PASS.
- README/REVIEW/provenance updated with tested scope and unresolved findings.
- Prior #29consensus (expensive), #65manager and #66dispatcher already passed normal
  and sanitized last turn; no production changes to their logic this turn, not rerun.

Next:
- Continue actual remaining P2P runtime components. SwapLinksFunctions.java FULL205
  and its upstream SwapLinksFunctionsTest.java FULL120 read this turn; not implemented.
  Native P2PState.hpp/.cpp, P2PGreeting.hpp/.cpp, P2PWalkLinks.hpp/.cpp and
  support/PeerHarness.hpp fully read for reuse. Full Message.hpp read as dependency.
- P2PManager.java caller locations inspected with rg only; read relevant call blocks
  before implementing SwapLinks. P2P_SEND_MSG and P2P_SEND_DISCONNECT exact constants
  at45/48. No native P2PManager yet; do not create a fake mapped Manager just constants.
- Sibling tools/minima-core-cpp src/network/P2PSync.hpp and its test inspected during
  this checkpoint; different IBD batch synchronization, not compatible peer helpers.
  Searches for SwapLinks/current+core/Minima found Java candidates only; no reusable
  native SwapLinks. User reuse gate needs exact new-scope report before next edits.
- Swap processGreeting calls Main!=null then manager/server/client received-P2P flag;
  preserve absence semantics and actual client update. Random secrets/peer shuffle
  need oracle canonicalization only for genuinely nondeterministic values, preserving
  size/identity/reuse assertions. Source onConnectedLoadBalanceRequest division uses
  inLinks.size/maxP2P even though condition checks not-accepting count.
- WalkLinksFuncs.java/P2PManager.java then NetworkManager/Main/db/processor/miner and
  remaining commands/tests still required. Do not count callback captures as runtime.

# Latest continuation — 242 counterparts; native NIOMessage dispatcher

Full 457-file native goal remains ACTIVE, unbudgeted, incomplete and unblocked.
Pin 5564b9d78978efcaf23679190d05253961fc6e69 (1.1.2.8), verified again this turn.
242 = 223 production + 19 mapped tests; 215 absent. Every map row remains partial/
not_verified. Native CLI still 24 commands; full Main/node runtime still absent.
No Java source edits, commits, pushes, wallet, miner or remote node started.
These facts override historical 241-and-earlier sections below.

New implementation:
- src/org/minima/system/network/minima/NIOMessage.hpp/.cpp, compiled in CMake.
  All active pinned branches translated: greeting negotiation/client mutations,
  IBD validity/heavier-chain/rescue, TxPoWID/request, full TxPoW consensus/relay/
  mempool/parent and tree scans, pulse crossover/request ordering, generic messages,
  P2P JSON, ping replies, sync/archive requests, block node/mine and MegaMMR request.
  Archive-single body remains a no-op because all pinned body code is commented.
  Unknown types (including MAXIMA and pong responses) match pin's fallback branch.
- support/NIOMessageServices.hpp declares required external Main/MinimaDB/P2P/
  processor/miner/megammrsync receivers. Actual native codecs, IBD, tree/searcher,
  checker/policies and NIOManager transport execute in the dispatcher. External
  callbacks are NOT a completed Main/database/processor/miner implementation.
- NIOMessage.dispatch matches NIOManagerServices.processIncoming directly. No JVM
  in native executable, no silent default binding. Run clears payload on body
  exits and caught exceptions; null payload or failing shutdown receiver before
  the try retains Java exception order. bad_alloc rethrows through RAII cleanup.
- JavaStringSet composes the existing JavaHashMap/SocketAddressSet synchronization
  pattern. Individual contains/add remain separate operations. LAST_TXBLOCKMINE_MSG
  uses atomic load/store separately; this is not an atomic check-and-update.
- Difficulty ratio reuses full inspected MMREntryNumber.dividePositive(...,7).
  Decimal comparison to .01 preserves Java DECIMAL32-to-double threshold decision.
  Five observable relay threshold cases straddle its decimal rounding boundary.
- P2P client count uses float JSON via javaFloating(float), avoiding double expansion.
  Ping ancestry intentionally uses getParent(100) with keys named 50block/50hash.
- MessageProcessor.PostTimerMessage now virtual, matching Java override behavior.

Review correction:
- TxPoWChecker.hpp/.cpp adds const MMR* script/MMR overloads. Existing reference
  overloads delegate. Java script checker ignores its MMR argument; checkMMR only
  dereferences it for an actual proof. Native NIOMessage originally required a
  nonnull MMR before scripts, now passes pointer and checks only at proof use.
  Fixtures cover valid/null/empty MMR, empty witnesses, failing proof, monotonic
  and non-monotonic scripts. No replacement empty MMR is invented by production.
- NIOMessage checks nullable server/traffic pointers before native dereference.
  Other older manager null receiver paths and complete resource failures still
  need review. Do not claim that all generic Java-null behavior is fixed.
- NIOMessageFixture friend added to TxPoWTreeNode/TxPoWGenerator for fixture MMR/
  mempool observation/setup only. NIOMessage own friend sees payload cleanup.

#66 nio_message_differential:
- support/nio_message.cpp, support/NIOMessageOracle.java,
  tools/nio_message_differential.py. Reuses full NIOClientOracle/manager-state/IBD
  fixture patterns, ConsensusOracle.fixture (actual signed/script/coin/MMR
  payloads), and ArchiveCacheOracle deterministic PRNG/zero-super-parent setup.
- Java Unsafe bypasses node/worker/miner constructors only. Calls actual pinned
  NIOMessage.run with real client queues, NIOTraffic, TxPowTree/Cascade/IBD and
  consensus methods; required runtime receiver effects captured. Native uses
  actual MessageStack queue, drained into cumulative snapshots after each packet.
- 1918 programs, 4677 packets PASS NORMAL AND ASAN/UBSAN.
  Groups: typebytes5 (all256 codes + null/empty under shutdown/sync/slave/trace),
  payload510, greetingrepeat448, peerjson8, ping32, idflags8, pow4, mining4,
  recentmine4, ibdreq4, truncated102, consensus720, blockrelay28, ratio5,
  treescan16, parentscan4, pingancestor4, heavierchain12.
- Chain fixtures cover main-tree counts1/255/256/257 and parent DB counts1/511/
  512/513, repeated packet throttle and internal0x00/0x01/null UIDs. Valid/invalid
  cascade and heavier IBD paths, rescue timers, invalid full-address nulls included.
- Coverage guards require actual response types0/1/2/3/4/12/14/16/19/20, P2P
  CONNECTED/NOCONNECT/MESSAGE/REMOVE_PEER, ibd/sync/pow/mine/txblock effects,
  2sdisconnect timer, specific ratio decisions, MDS_HEAVIER_CHAIN and MAIN_DO_RESCUE.
  A guard caught fixture cascades with empty superparents instead of zero parents;
  both implementations had only rejected these. Fixed fixture generation and
  verified actual notification/rescue effects; do not remove those guards.
- MainFixture excludes MINIMALOG explicitly. It previously captured logger events
  with timestamps/heap sizes, exposing the known native logger gap. Generic caught
  error text/stack traces/logs are not compared. MDS_HEAVIER_CHAIN is compared.
- No successful MegaMMR service fixture/binding, exact live-clock boundary replay,
  complete injected receiver failures, concurrent global mutation or persistence.
  MINIMALOG notifications need real native logger integration before full parity.

Actual TCP integration (#63):
- support/nio_manager_loopback.cpp now routes through NIOMessage.dispatch after
  original32frames/four-held-workers drain. Missing TxPoWID produces exact type3
  wire; SINGLE_PING produces decoded type12 Greeting, exact framed read accounting.
  Protocol branch runs outside fixture lock; runtime service binding published
  under the state lock, reset only after manager shutdown/worker ownership barrier.
- Existing connect/greeting/queries/32frame/concurrency/disconnect/EOF/weak-owner
  expiration checks retained. PASS normal0.53s, sanitized0.77s (approved loopback).

Build/check status:
- Latest ALL-TARGET normal52622/sanitized59987 ended0. Afterwards only loopback
  fixture changed and rebuilt both (61538/5097 ended0). No production changes after.
- Normal #29 consensus2890 PASS138.20s, #37 workersPASS1.58s, #65 managerPASS7.32s.
- Sanitized #29 consensus2890 PASS431.71s, #37 workersPASS4.64s.
- Normal final #66 PASS27.92s after cascade fixture correction.
- Sanitized selected CTest session73863 ended0: #65PASS13.99s, #66PASS80.32s;
  total530.66s including #29/#37. No active tool sessions. Do not rerun #29;
  it already passed and is expensive under ASAN.
- Source map regenerated + --check PASS; syntax/JSON/changed-source whitespace and
  git diff --check PASS. README/REVIEW/provenance updated with final
  successful results; normal/sanitized #66 reports agree exactly.

Next work:
- Validation and docs finished for this checkpoint. Goal remains active. No blocker.
- Continue remaining runtime components (P2PFunctions/P2PManager, NetworkManager,
  Main, databases, processors/miner) plus remaining commands/tests. Do not count
  callback capture as their implementation. NIOMessage now EXISTS; don't restart it.
- Pending dispatcher review: successful MegaMMR handler through actual native
  megammrsync (not ported), logger/MINIMALOG, bandwidth limit boundaries, exact
  time/rate-limit boundaries, receiver/resource failures, mutable static settings.
- Main must clear mHaveSentIBDRecently at pinned startup/reset/timer locations;
  manager clearClientSync binding must remove both static request maps on disconnect.
- Wildcard Socket.connect vs SocketChannel.open address substitution remains a
  separate adapter gap. Local JDK NioSocketImpl source already inspected; no native
  getLocalHost helper found. Manager process-exit semantics still require Main to
  keep outstanding detached work alive after its 8s await timeout returns.

No agents authorized. Keep reuse gate and commentary <=60sec. Do not rerun old
/tmp code-writing scripts. Configure after new targets, await builds before tests.
Socket binds require approved CTest escalation. No goal blocker.

---
Historical checkpoints follow; latest facts above supersede them.

# Latest continuation — 241 counterparts; manager state review complete for tested scope

Active full 457-file goal remains INCOMPLETE, unbudgeted and unblocked. This turn
made progress (new evidence and production fixes). Do not mark complete or blocked.
Still 241 counterparts = 222 production +19 tests, 216 absent, all rows partial.
Pin unchanged 5564b9d78978efcaf23679190d05253961fc6e69. No Java edits/commits/pushes.
No active tool sessions. Current facts below override historical pending notes.

New completed work:
- #65 nio_manager_state_differential; support/nio_manager_state.cpp,
  support/NIOManagerStateOracle.java, tools/nio_manager_state_differential.py.
- Actual pinned Java NIOManager.processMessage, disconnect and query methods,
  with Unsafe singleton startup fixture, actual clients/maps/packet queues, and
  required DB/P2P/timer receivers. Uses NIOClientOracle, NetworkStartupOracle and
  IBDOracle patterns already inspected. Native fixture subclasses NIOManager;
  adds friend NIOManagerFixture to NIOServer.hpp and TimerMessage.hpp for map/
  timer observation. No production-only stub dispatch added.
- Initial client UIDs set BEFORE map insertion. Generated UIDs never mutated:
  labels projected by object identity; state/map output sorted by projected label.
  Native manager PostMessage uses real MessageStack; snapshot drains into retained
  posted vector. Java overrides PostMessage; timer schedules captured (source
  delays read via reflection/native friend), failures injected after capture.
  Server selector intentionally null in this manual fixture: actual DISCONNECT
  retains queued UID then throws on wake; both languages compare this state.
- 1449 programs, 3879 operations PASS NORMAL AND ASAN/UBSAN. Groups:
  reconnect432 + limit12, disconnected576, connect24, startup21, newconnection24,
  lastread40, removepeer16, disconnect3, disconnectall1, cancelattempt1,
  nullclient4, guard36 (includes HEALTHCHECK/unknown/null), hardshutdown1,
  txpowrequest64, archivesync192, query2 (many operations each).
- Coverage guards ensure every config succeeds and real packet types0/3/13,
  expected timers(5s/10s/30s/120s), P2P_INIT/NOCONNECT/DISCONNECTED/REMOVE_PEER,
  and both successful and failing operations actually occur. Matching empty or
  error-only responses cannot satisfy the fixture.
- Generic exceptions normalized, IO/packet/state/call data compared. Stale-read
  ages are far from the live-clock threshold (500000/700000ms etc), not a claim
  of exact boundary timing. No real connector or NIOMessage dispatcher in #65.
  Native #63 already tests actual manager connection/dispatch threads overloopback.

Production fixes discovered/reviewed:
1. NIOManagerServices.txPoWExists changed from string to optional<string>.
   NIO_TXPOWREQ previously evaluated id.value() BEFORE calling receiver; Java
   passes null to getTxPoWDB().exists first. New code calls txPoWExists(id),
   logs with id.value_or("null"), then constructs MiniData (null still throws
   there if lookup returned false). This preserves lookup side effects/errors.
   Failing comparison1199 revealed missing lookup call on null ID. Fixtures
   include lookup true/false and injected error. All affected callbacks rebuilt.
2. NIOManager destructor now catches only server shutdown wake exception before
   unconditional stopMessageProcessor/joinServer/joinMessageProcessor. A thrown
   wake no longer skips later ownership barriers. No blanket swallow of join
   failures. Exhaustive resource failure injection remains pending.

Validation after final fixes:
- Both ALL-TARGET builds PASS (normal session17005/sanitized20587 ended0).
- Final selected #61,#63,#65 all PASS in both builds:
  normal32.06s (transport24.46 / loopback.40 / state7.20),
  sanitized41.18s (transport26.42 / loopback.59 / state14.17).
- #61 still640serialization/send +17ping cases. #63 still32frames/fourworkers.
- Earlier #65 corpus1433/3847 passed; final1449/3879 adds healthcheck and limits.
- Initial fixture compile error scheduled.add(sharedJSONObject) fixed to *row.
  Initial runtime fixture missing boolean defaults fixed using holds_alternative.
  These were fixture issues, not production fixes. Final coverage guards pass.
- Source map --check PASS (no new mapped class), Python syntax/provenanceJSON/
  explicit changed-source whitespace/git diff --check PASS. README/REVIEW and
  provenance updated. No remote peer, full node, miner or wallet started.

Source study this turn:
- Re-read pinned NIOManager1..790 and893..943 and exactTXPOWREQ; previously FULL
 1114line source read remains applicable. P2P invalid-peer helper/NetworkManager
 getter/Main flags, NIOMessage sync-map types, TimerMessage/Message ownership,
 native manager/deps and existing fixtures inspected. Sibling tests searched;
 no existing manager-state implementation to reuse beyond known fixtures.
- Re-read NIOMessage1..280, native ConsensusServices.hpp, TxPoWChecker.hpp,
 Pulse.hpp at turn end to prepare native protocol handler. Entire1311line
 NIOMessage source was read in previous goal turn; do not claim complete new
 read this turn. Native NIOMessage still DOES NOT EXIST.

NEXT WORK:
- Native NIOMessage1311line dispatcher is the next main missing component;
 reuse now-tested NIOManager, Greeting, IBD, Pulse, TxPoW/TxBlock, consensus
 checks, MegaMMR wrappers. Existing support/NIOProtocol.hpp already pins names.
 Study complete relevant service contracts/callers before implementation;
 source-study summary from earlier context describes all branches.
- Wildcard Socket.connect vs SocketChannel.open adapter gap remains (details
 below). No native InetAddress.getLocalHost helper found in current/sibling
 search. InetSocketAddress full source already inspected. Address resolution
 must distinguish ping's Socket behavior before claiming full parity.
- Manager state differential now exists; don't restart it from scratch. Remaining
 manager review: multiple global managers, source clock boundaries, asynchronous
 connection failure/cancellation interleavings, all resource failures and full
 NIOMessage/Main/NetworkManager/P2P/DB bindings. Incoming callback collection is
 not native protocol parsing. Generic API/null/log gaps elsewhere remain.
- Java non-daemon executor workers keep JVM alive; detached native workers do not
 keep process alive. Pending Main shutdown must preserve work after 8s wait
 returns. Existing shared ownership only preserves object lifetime within process.

No subagents authorized; keep reuse gate and progress commentary <=60s. Don't
rerun old /tmp code-writing scripts. Configure after new targets, await builds
before tests. Loopback CTest prefixes already approved; use escalation for binds.

---
Historical 241 checkpoint follows; current facts supersede pending-state notes.

# Active full-port continuation — 241 counterparts

Full 457-file native node goal ACTIVE, unbudgeted, incomplete and unblocked.
Pin 5564b9d78978efcaf23679190d05253961fc6e69, v1.1.2.8. Java unchanged.
241 = 222 production + 19 mapped tests; 216 absent. All mapping rows remain
partial/not_verified. CLI 24 commands, full node still absent. No commits/pushes.
Current facts override historical 240-and-earlier notes below. No active sessions.

Latest implementation / validation:
- Mapped NIOManager.hpp/.cpp now compiled in minima_file_port. All active pinned
  message branches translated, static create/send/ping helpers, native factory,
  server thread and message processor ownership. Required NIOManagerServices
  receivers await full Main/NetworkManager/P2PFunctions/NIOMessage bindings.
  Do NOT call processIncoming callback collection a native NIOMessage port.
- NIOServer receiver now std::function; prior shared<MessageStack> constructor
  delegates to it. Manager uses weak callback; running server/processor threads
  retain manager. Manager lifecycle fixture verifies weak ownership expires after
  shutdown even while client objects remain referenced.
- support/FixedTaskExecutor.hpp adapts existing native/sibling CV message queue
  to four lazy workers, unbounded FIFO, shutdown drain and timed non-canceling
  wait, task-exception worker replacement. execute after shutdown rejects.
- support/NIOConnections.hpp/.cpp adapts existing owned PosixNIOSocket + address
  resolver. Blocking SocketChannel.open path; timed connect, full write/read.
  Review fixed EINTR restarting the full timeout: monotonic deadline retained
  across poll interruptions/spurious readiness, renewed after positive read.
  Zero timeout waits indefinitely. Descriptor copying disabled.
- support/NIOProtocol.hpp holds actual pinned convertMessageType names for reuse
  in pending NIOMessage port. Type11 => UNKNOWN_11, type10 => MAXIMA, pong etc MSG_.

New tests, all PASS normal AND ASAN/UBSAN:
- #61 nio_manager_transport_differential: 640 serialization/send cases, 17 actual
  loopback ping cases, freshly compiles actual 369 production Java sources.
  Native support/nio_manager_transport.cpp, Java NIOManagerTransportOracle.java
  reusing NIOClientOracle singleton fixture; tools/nio_manager_transport_differential.py.
  All 256 type names, MiniData/MiniString/Greeting bytes, null/failing serializer
  timing, injected OOM -> IOException, empty-server send/broadcast accounting.
  Ping verifies exact outgoing SINGLE_PING + MiniData.ZERO_TXPOWID, arbitrary
  response type acceptance (0/11/12/255), fragmenting, extra trailing bytes,
  empty/type-only/truncated replies, negative/>512MiB length and 10-second timeout.
  Initial fixture assumed ZERO_TXPOWID32 bytes; actual pinned and native source
  both use one byte 0x00. Fixed expected request (6 bytes total); no production
  protocol change was required. Generic exceptions normalized; IO strings exact.
- #62 fixed_task_executor: four held workers, 100 queued tasks, timeout leaves
  work alive, rejection, throwing worker replaced while shutdown drains FIFO,
  repeated shutdown, closure lifetime across owner destruction (30 queued tasks).
- #63 nio_manager_loopback: real NIOManager.create/server/processor/timer threads;
  bind port zero, incoming registration; connect command actually posts and
  establishes outgoing connection, actual outgoing greeting wire/metadata;
  client queries and valid outgoing filter; 32 one-byte frames queued behind
  four held processIncoming callbacks, exact payload/no duplicates/metadata,
  160-byte total read traffic; disconnect clearSync/P2P event; shutdown peer EOF
  and weak manager expires. This is native integration, NOT Java differential
  state-machine coverage or NIOMessage processing.
- #64 nio_connections: socketpair, 50 SIGUSR1 interruptions preserve an 80ms
  deadline; progress renews per-read200ms timeout across total400ms transfer;
  zero timeout waits and EOF distinguished. Test restores signal handler and
  joins producers before checking failures.
- Existing #58 nio_server_differential re-run after receiver change, passes both
  14 lifecycle checkpoints + four startup cases.
- Final selected suites (#58,#61..64): all five PASS normal29.57s, sanitized32.28s.
- Final all-target normal and sanitized builds PASS after timeout fix. Source map
  regenerated and --check PASS; Python syntax and git diff --check PASS. Native
  binaries do not invoke Java. No remote peer/node/miner/wallet started.

Review/source evidence:
- Prior turn fully read pinned NIOManager1114 and NIOMessage1311 lines (see below).
  Current turn re-read full native manager, NIOServer, executor/connector and
  relevant exact pinned manager transport methods, Greeting/dependencies.
- Reused actual NIOClientOracle/NIOServer differential and native loopback
  infrastructure; re-read full sibling tests/network/test_network.cpp. Closest
  sibling message envelopes differ from pinned protocol; no substitute used.
- Installed OpenJDK26.0.1 src.zip Executors.newFixedThreadPool,
  ThreadPoolExecutor processWorkerExit/getTask/runWorker and NioSocketImpl
  timedRead/read/connect inspected. No JDK implementation source copied.
- README, REVIEW, provenance updated with results and explicit remaining gaps.

NEXT REQUIRED WORK (no blocker):
1. Java/native NIOManager state transitions differential: reconnect attempts,
   P2Pdisabled/TXBLOCK/lonepeer/invalid peer branches and mutation order;
   disconnected reconnect force/suppression, timers, startup connect-list,
   shutdown/restoring guard, stale reads, TXPOWREQ and SYNCTXBLOCK. Current
   transport/lifecycle evidence does not cover these policies comprehensively.
   NIOManagerFixture friend can construct protected manager without starting
   threads. Native virtual schedule/connectAttempt seams permit timer capture.
   Java Unsafe subclass + reflection avoids node startup; PostMessage and
   PostTimerMessage overridable. Java private connectAttempt cannot override;
   avoid triggering it in manual differential, or use real loopback separately.
   Random UIDs should be projected by object identity, never mutate map keys
   after insertion. TimerMessage.getTimer exists; delay field is private.
   NIOClientFixture friend exists; NIOServer has no fixture friend yet.
2. NIOConnections wildcard destination difference uncovered in source review:
   JDK NioSocketImpl Socket.connect substitutes any-local address using
   InetAddress.getLocalHost; SocketChannel.open differs. Current shared connector
   uses supplied resolved numeric address. Search found no existing getLocalHost
   native helper. InetSocketAddress.hpp/.cpp fully re-read this turn; implements
   numeric/DNS resolution, not local-host lookup. Resolve this before full parity.
   Need distinguish Socket ping from SocketChannel connect behavior, not globally
   rewrite connector resolution. Custom JDK properties/other platforms unverified.
3. NIOMessage actual1311-line dispatch port remains absent. All source already
   read; full Main/NetworkManager/P2P/SQL/wallet runtime and many commands remain.
4. Exceptional resource failure lifetime still review: one caught exception in
   NIOManager destructor could skip later stop/join calls if server wake throws.
   Fixed executor native detached closures retain state but, unlike Java non-daemon
   workers, do not themselves prevent process exit. Full Main shutdown must honor
   unfinished work after manager's source-compatible 8s await returns. No native
   Main exists yet, so no claim of complete process-lifetime equivalence.

No subagents authorized. Keep reuse gate/report before new implementation.
Continue existing active goal; do not mark complete from file counts/tests.
Never rerun old /tmp write/extend/minima_*_once.py scripts: overwrite fixes.
Approved ctest prefixes cover real loopback tests outside sandbox. Configure after
adding targets; wait for builds before dependent tests. Progress updates <=60s.

---
Historical 240 checkpoint follows; current facts above supersede stale absences.

# Active full-port continuation — 240 counterparts

The full 457-file native node goal remains ACTIVE, unbudgeted, incomplete and
unblocked. This turn made progress; do not mark complete. 240 counterparts =
221 production + 19 tests; 217 absent. All mapping rows partial/not_verified.
Pin 5564b9d78978efcaf23679190d05253961fc6e69 (v1.1.2.8). Java source unchanged.
No commits or pushes requested. CLI now 24 counterparts; connect needs runtime.
Current facts here supersede historical 239/237 notes below.

Latest completed code (239 -> 240 plus previously-mapped API completion):
- Greeting::createGreeting in src/org/minima/objects/Greeting.hpp/.cpp.
  Added support/GreetingServices.hpp following ChainServices/IBDServices; required
  readLock(bool), getWelcome()->optional<string>, getTip()->shared<TxPoWTreeNode>,
  getPeersCopy()->SocketAddressList. Bind before use; no silent default receivers.
- src/org/minima/system/commands/network/connect.hpp/.cpp. Full command/help/
  params/getFunction plus static createConnectMessage. Static required postMessage
  callback represents Main.getNIOManager().PostMessage; full NIOManager absent.
  Registered in CommandRunner; new CLI command has no native network binding yet.
- support/JavaExceptions.hpp now has JavaCheckedException to preserve generic
  checked java.lang.Exception text in CommandRunner, leaving old mappings intact.
- support/NetworkStartupOracle.java, support/network_startup.cpp,
  tools/network_startup_differential.py, CMake #60.

Verification completed normal AND ASAN/UBSAN:
- network_startup_differential 532 programs / 1764 greeting-create calls PASS.
  304 connect helper/direct-command/runner cases plus 228 greeting programs.
  Source constructors/methods are actual pinned Java, freshly compiled with all
  369 production Java files. Only singleton/service receivers replaced in oracle.
- object_differential 9244 PASS, command_runner_differential 1233 PASS.
- Native library, network startup runner, object/command runners and CLI built
  both configurations. Final all-target builds also PASS in both configurations.
  No running tool sessions at final checkpoint.
- Source map/check PASS. CLI --help enumerates connect plus the existing23.
- Corrected an intermediate commentary typo: 532 programs, never 660.
- No remote connections, node, miner or wallet started. The connect fixture only
  collects real Message objects; missing actual runtime is not represented as done.

Behavior / review:
- Greeting locks BEFORE try, catches body std::exception and logs, unlocks AFTER
  catch. Lock/unlock failures propagate. Tests compare lock calls and remaining
  depth under injected failures, exact serialized bytes and JSON/order.
- Nullable welcome; host inserted only when IS_HOST_SET (not removed on subsequent
  unset calls); port stored as string; absent tip -> top -1; walk actual parents
  tip-to-root, append IDs WITHOUT CLEARING existing chain. Repeated creates append.
  P2P peer array uses InetSocketAddressIO::addressesListToJSONArray (returns VALUE).
  Null/invalid peer list fails after chain/metadata mutations, preserving prior peers.
- Full Main/MinimaDB/P2P binding, nullable GeneralParams.MINIMA_HOST, all stream
  overloads and reference-aliasing API still pending. Native date/log generic gaps
  from earlier notes remain. Greeting create method is implemented, not whole-node.
- connect first ASCII colon, same trim <=32 as CommandRunner, javaParseInt accepts
  BMP Unicode digits and signed INT32, no TCP port or host validation added. -1,
  65536 and empty host accepted by helper as Java does. Missing/invalid host command
  throws JavaCheckedException with No host specified / Must specify host:port.
  Direct other exceptions normalized; checked runner errors compare exact strings.
  Full help/metadata, fresh prototype, queue post content and injected post failure
  compared. No default no-op post binding. Null Main would fail in actual Java.

MANDATORY SOURCE STUDY now done for next NIOManager step:
- IMPORTANT correction: NIOManager.java is 1114 lines, NOT 815! Earlier notes
  claiming full read at815 were wrong. This turn read 1..280,280..560,560..840,
  840..1200 without truncation, covering its entire file. Lines printed twice at
  boundaries are output overlap, not duplicate source if statements.
- NIOMessage.java is1311 lines. FULLY read this turn through 1..190,190..290,
  285..625,625..970,970..1255,1250..1312. All active branches, not just imports.
- Pinned connect.java, Greeting.java full. Native Greeting, CommandRunner,
  Command/Message/MessageProcessor/TimerMessage, SendPollManager, NIOTraffic,
  NIOClientInfo, InetSocketAddressIO, JavaInteger/JavaExceptions, PeerHarness,
  IBDSupport/ChainServices interfaces, relevant oracle fixture methods inspected.
- Native/sibling searches found NO existing NIOManager or fixed-thread executor.
  Sibling src/system/MessageProcessor.hpp FULLY read, has condition-variable
  handler queue but one worker and discard-on-stop, unlike fixed executor drain.
  Current MessageProcessor.hpp/.cpp fully read, native create factory with shared
  ownership and explicit stop/join required; reuse lifecycle pattern for manager.
- Sibling src/objects/Greeting.hpp FULLY read; older version/default wire-only
  class has no createGreeting and cannot substitute live behavior.
- Sibling src/network/NIOMessage.hpp only first100 lines read this turn (enum,
  names and envelope beginning), NOT full class. Type-name mapping differs:
  pinned SINGLE_PING has no named convert branch -> UNKNOWN_11; MAXIMA_TXPOW name
  is MAXIMA, PONG is MSG_SINGLE_PONG, IBD/archive names have MSG_ prefixes.
- P2PFunctions.java only declarations/constants and invalid-peer methods/local
  address prefix helpers read this turn; full class NOT yet read. Its invalid-peer
  set is plain static HashSet<String>, duplicate add logs only on first insertion.
- Existing PeerMessagesOracle address/list helpers, NIOClientOracle Unsafe/field
  helpers, IBDOracle DB receiver/lock fixture and TreeOracle real chain patterns
  reused. No test placeholder counts used as evidence.

NIOManager translation requirements / next work:
- Derives MessageProcessor; constructor creates NIOTraffic, connecting concurrent
  map, NIOServer(GeneralParams.MINIMA_PORT,this), starts server thread. Reuse native
  factory after shared ownership; must resolve manager/server lifetime cycle.
- Has 4-thread fixed executor (Java lazy worker startup, unbounded queue). shutdown
  drains queued jobs; awaitTermination8000ms does not forcibly stop pending work.
  NIO_SHUTDOWN then wakes server, waits while isRunning in50ms, stops processor.
  hardShutDown only pool wait + server shutdown + processor stop, no server wait.
  Worker exception before NIOMessage's try can escape (null data/Main), so Java
  executor replaces worker. Do not invent successful processing/no-op handlers.
- NIO_SERVERSTARTED posts P2P_INIT, tokenizes CONNECT_LIST by commas dropping empty
  tokens, trims and uses newly ported connect helper. Valid auto-connect messages
  scheduled10000ms. Schedules last-read check120000ms; healthcheck schedule commented.
- NIO_CONNECT checks invalid peer, creates host-only NIOClient, mapinsert, posts
  NIO_CONNECTATTEMPT. connectAttempt starts separate blocking SocketChannel.open
  thread; success removes OLD client UID then registers socket (server creates NEW
  connected UID), failure posts NIO_RECONNECT. No cancellation check after successful
  blocking connect; preserve source behavior.
- RECONNECT increments attempts first (wrap Java int), limit default3. TXBLOCK_NODE
  or !P2P_ENABLED resets to1 afterlimit. Otherwise afterlimit abandon only if any
  connected or >1 connecting; post P2P_NOCONNECT and invalidate then remove map.
  If lone peer reset attempts1. Invalid peer cancels reconnect, ensures one no-connect
  message, else schedule connectAttempt30000ms. Preserve mutation/post error timing.
- disconnect/removeP2P uses GLOBAL Main manager to find UID even when invoked on
  other manager instance; invalidates, creates InetSocketAddress(host,port), posts
  P2P_REMOVE_PEER then local NIO_DISCONNECT. Actual disconnect removes connecting
  before server.disconnect. DISCONNECTALL posts each pending/connected UID.
- DISCONNECTED clears NIOMessage.mlastSyncReq/mLastChainSync, checks requested flag,
  valid greeting, incoming, invalid peer. TXBLOCK_NODE may FORCE outgoing reconnect
  if no other connected host, overriding earlier invalid/valid filtering. Creates
  new pending client+30000ms timer; posts P2P_DISCONNECTED with nioclient,uid,incoming,
  reconnect. NEWCONNECT removes by connectedUID then invalid-check; outgoing marks
  sentGreeting BEFORE createGreeting and sends type0. Greeting dependency ready.
- Incoming data builds NIOMessage(uid,data), trace/filter/fullhost then executor.
  NIOMessage/runtime wiring still absent. Do not stub discarded incoming messages.
- TXPOWREQ tests DB.exists then sends MiniData with type3. SYNCTXBLOCK requires root;
  last archive block else rootpow; stops atblock1; if !ARCHIVE stops before now minus
  SYNC_MAX_TIME (86400000*NUMBER_DAYS_ARCHIVE with long overflow). sends type13 TxPoW.
- CHECKLASTMSG compares signed wrapped now-last >600000, disconnects, schedules
  outgoing+valid host reconnect5000ms, schedules another120000ms. HEALTHCHECK body
  entirely commented; preserve as no-op. mSyncObject/mAwaitingConnect unused fields.
- Queries: connecting infos first thenconnected, valid/outgoing filters, countsJSON.
  checkConnected(host,false) connecting first; getNIOClientFromUID GLOBAL Mainserver.
  getMaximaUID uses client.getMaximaIdent().equals(input), nullable receiver canthrow.
- createNIOMessage = one MiniByte then object's stream bytes, no outer length yet;
  catches OutOfMemoryError -> IOException. sendNetworkMessage rejects null object,
  creates message BEFORE UID.equals, emptyUID broadcast else single through GLOBAL
  Mainserver. Logging optional; traffic breakdown type_single/all includes payload
  size (no outer4byteprefix), exceptions in breakdown swallowed.
- sendPingMessage sends type11 plus MiniData.ZERO_TXPOWID; socket connect10000ms,
  read timeout10000ms (misleading comment3s). Writes MiniData-framed packet; updates
  traffic breakdown sendPingMessage, reads bounded MiniData, updates breakdown,
  closes socket streams then reads response MiniByte but DOES NOT VALIDATE TYPE,
  parses Greeting. Any exception -> null, optionallylogs. Reuse owned POSIX socket,
  address resolver and framing code, not sibling protocol name/size substitutes.

Do not spawn subagents; no user authorization for delegation. Continue full goal.
All earlier no-generator /tmp restrictions and approved socket CTest prefixes
remain. No active sessions. Source map, Python syntax, explicit changed-file
whitespace and git diff checks pass. README/REVIEW/provenance updated.

---
Historical239 checkpoint follows; current facts above take precedence.

# Active full-port continuation — 239 counterparts

User explicitly requests continuing to 100% native 1:1 Java-to-C++ node port.
The UNBUDGETED GOAL IS ACTIVE, no blocker. Never mark complete based on mapped
file counts or subset tests. Full native node remains NOT implemented.
Pinned upstream 5564b9d78978efcaf23679190d05253961fc6e69, v1.1.2.8.
Authoritative map: 457 Java = 369 production + 88 tests; 239 counterparts =
220 production + 19 tests; 218 absent. All rows remain partial/not_verified.
Java source unchanged; cpp untracked. No commit/push requested. CLI 23 commands.

LATEST WORK (237 -> 239, supersedes historical notes below):
- src/org/minima/system/network/minima/NIOServer.hpp/.cpp, actual native server
  lifecycle and event loop built atop existing NIOClient/PosixNIOSocket/key,
  JavaConcurrentMap and MessageStack. mSelector uses mutex-protected shared_ptr
  (target libc++ lacks atomic<shared_ptr>). Poll + nonblocking pipe wakeup.
- support/NIOServerOracle.java, support/nio_server.cpp,
  tools/nio_server_differential.py; CMake #58.
- src/org/minima/system/network/p2p2/P2P2DB.hpp/.cpp, actual shared nullable-string
  peer list, JSON persistence and incremental load mutation. Reuses JsonDB,
  MiniFile and MiniUtil::StringList/convertArrayList. JsonDB loadDB/saveDB and
  destructor now virtual to preserve Java superclass dispatch.
- support/P2P2DBOracle.java, support/p2p2_db.cpp,
  tools/p2p2_db_differential.py; CMake #59.
- support/NIOSocketProfile.java standalone JDK wildcard profile fixture, compiled
  into build and executed outside sandbox. Actual default JDK 0.0.0.0 bind is
  AF_INET6 wildcard, accepts both 127.0.0.1 and ::1. No custom preferIPv properties
  were found in pinned source. Native binds dual-stack with IPv4 fallback on
  EAFNOSUPPORT/EPROTONOSUPPORT. Custom properties/other platforms still unverified.

Validation:
- nio_server_differential normal and ASAN/UBSAN: 14 lifecycle checkpoints match
  actual pinned Java. Incoming IPv4 and IPv6, outgoing registered channel,
  object identities/counts/metadata, 0/1/8193/65536-byte framed binary messages,
  direct send and valid-greeting broadcast filter, explicit reconnect=false,
  outgoing EOF reconnect=true, oversized incoming message disconnect, shutdown
  closes clients but RETAINS map and emits no disconnection messages. Exact
  traffic 73750 read / 73760 written. Freshly compiles actual 369 Java classes.
- Startup subprocesses: invalid ports -1 and 65536 return stopped; occupied
  dual-stack wildcard port logs fatal and halts(0); shutdown before run still
  opens/closes listener and posts started. Final rerun PASS in both builds
  (normal 3.13s, sanitized 3.68s). No running sessions at checkpoint.
- p2p2_db_differential: 211 programs / 5145 operations PASS normal AND ASAN/UBSAN.
  Shared peer-list identity, aliases, nulls/duplicates, startup flag, exact wire
  and disk bytes, virtual superclass dispatch, nonexistent/truncated/malformed
  files, wrong types and partial failure mutations; save-to-file-parent failure.
- Existing prefs1130 / peer_state798 (26118 ops) / P2PDBTest1000-peer save/load
  PASS normal and ASAN/UBSAN after virtual JsonDB change.
- Both libraries and minima_commands rebuilt. Full builds completed except an
  initial normal fixture compile failure fixed by dereferencing JSONArray pointer;
  the fixed target then built, sanitized full build succeeded. Source map check,
  Python syntax and git diff check pass. Explicit whitespace check covers all
  new implementation/fixture files; git diff alone does not inspect untracked files.
- No full node/wallet/miner/remote peer started. Local socket fixtures only.

Review / runtime boundaries:
- mNIOManager is strong shared<MessageStack>; actual NIOManager still absent.
  Resolve eventual manager/server ownership cycle at runtime shutdown; do not
  silently weak-reference and drop required receivers. NIORuntime::runtimeLookup
  must be installed before threads. Manager/startup replacement is ONLY fixture.
- NIOServer's misspelled regsiterNewSocket, pre-start queue append then exception,
  registration-before-disconnect-before-selected processing, greeting filter,
  explicit vs error reconnect flags, retained shutdown map follow pinned Java.
- Descriptor transfer uses shared_ptr(new PosixNIOSocket(guard.release())) so
  allocation happens before guard releases; constructor closes on its failures.
  Descriptor noncopyable. Cancelled key race in interest snapshot excludes fd.
- Installed JDK ServerSocketAdaptor.bind and ServerSocketChannelImpl.netBind read:
  default backlog50; netBind includes bind, listen and local-address lookup.
  Native fatal block now includes all three; reuseAddress set AFTER bind.
- Caller retains server and joins run thread before destruction. Selector pipes
  close under mutex; key callbacks retain closed wakeup safely. Java retains open
  selector after shutdown; native closes OS pipe but same public wake calls no-op.
  Concurrent ready-key iteration order is not claimed identical. Generic exception
  diagnostics, MinimaLogger prefixes, static demonstration main, every fault path
  and complete runtime integration still pending.
- Initial occupied-port fixture held an IPv4-specific address; this did NOT
  conflict with JDK dual-stack wildcard on this macOS and Java stayed listening
  until subprocess timeout killed it. Corrected fixture holds the SAME dual-stack
  wildcard address. No stray processes were intentionally left running.
- P2P2DB casts+appends each peer; replacing with MiniUtil::convertJSONArray would
  lose partial mutation on a later wrong type. save converts array before I/O;
  load first invokes MiniFile (swallows IO only), then clears shared list and
  uses getJSONArray (creates missing entry). Native logs text without prefix.

Source study completed this step:
- Pinned NIOServer.java full again; NIOClient, NIOClientInfo, NIOManager and
  NetworkManager full earlier (see history). Sibling src/network/NIOServer.hpp,
  NIOClient.hpp and tests/network/test_network.cpp fully read earlier.
- Pinned P2P2DB.java, P2P2Manager.java, JsonDB.java full; native JsonDB/MiniFile
  header/MiniUtil/P2PDB/JavaList and P2PDBTest fully inspected. MiniFile.java exact
  loadObjectSlow/saveObjectDirect bodies read, MinimaDB P2P2DB construction/getter
  and all relevant load/save callers read (not full MinimaDB this step).
- No P2P2DB counterpart or test found in sibling minima-core-cpp or pinned tests.
- P2PTestParams.java and P2PParams.java full, native P2PParams/JavaList full;
  P2PManager constructor/test-params caller, doDiscoveryPing and haveAnyPeers
  relevant default-list callers read. No P2PTestParams native counterpart yet.
  Default test peer minima_one:9001 resolves on Java class initialization; avoid
  eager DNS in every native executable just because test params were linked.
  setTestParams copies shared default list and 12 numeric fields + LOG_LEVEL,
  not VERSION/PEERS_LIST_SIZE; METRICS_URL belongs only to test params.

NEXT: continue remaining network manager/runtime dependencies and missing peer
classes. NIOServer and P2P2DB are implemented at the stated validated scope;
do not recreate them based on stale historical NEXT notes below. P2PTestParams
is a small already-read remaining leaf; P2P2Functions/Manager and P2PFunctions
need actual runtime bindings (pinned P2P2Manager includes intentionally empty
methods; preserve pinned behavior instead of inventing a new protocol).
NIOManager full source previously read; all Main/SQL/wallet/commands integration
remains substantial. Follow reuse gate and report exact sources before new code.

Permissions: loopback binds sandbox EPERM; approved ctest prefixes for build
and build-sanitized. Use require_escalated for actual socket CTests. Ordinary
P2P2DB/preferences tests work sandboxed. java -cp cpp/build NIOSocketProfile also
approved. No subagents allowed. Never rerun old /tmp/*write*/*harness_once* or
/tmp/minima_nio_native_once.py and /tmp/minima_nio_oracle_once.py generators.

---
Historical 237 checkpoint follows; current facts above take precedence.

# Active full-port continuation — 237 counterparts

User explicitly requests continuing until 100% of the native 1:1 Java-to-C++ node port is complete.
The unbudgeted goal is ACTIVE. Leave it active; no blocker, no completion claim.
Current authoritative map: 457 Java files, 237 counterparts (218 production + 19 tests), 220 absent.
All map rows remain partial/not_verified. Full native node is NOT implemented.
Pinned upstream: 5564b9d78978efcaf23679190d05253961fc6e69 (v1.1.2.8).
Java root and upstream-study unchanged. cpp/ is untracked; no commit/push requested.

Latest completed work (235 -> 237):
- cpp/src/org/minima/system/network/minima/NIOClient.hpp/.cpp
- cpp/src/org/minima/system/network/minima/NIOClientInfo.hpp/.cpp
- support/NIOSocket.hpp/.cpp (actual owned POSIX descriptor adapter + native key state)
- support/NIORuntime.hpp (required Main/NetworkManager/NIOManager binding)
- support/JavaDateString.hpp (Date.toString adapter, reuses prior native calendar code)
- support/NIOClientOracle.java, support/nio_client.cpp, support/nio_loopback.cpp,
  tools/nio_client_differential.py; CMake tests #56 and #57.

Validation at checkpoint, normal AND ASAN/UBSAN:
- nio_client_differential: 714 comparisons / 13972 operations PASS.
  495 base programs (73 UTC dates included) plus 219 additional date comparisons
  in Europe/London, America/New_York, Asia/Tokyo. Generic exceptions normalized.
  Native buffers and temporary/queued/posted data compare full SHA256 digests,
  positions, limits and counts, not just successful payloads.
  Includes actual 256MiB+1 outgoing allocation, drop and following-message recovery.
- nio_loopback: PASS. Actual local TCP sockets, port-zero binding, five payloads
  including empty and 1MiB, forced EAGAIN, complete echo, selector interest/wakeup
  counts, exact traffic, descriptor close and EOF. Does NOT start a Minima node.
- source_map --check PASS; git diff --check PASS (new files remain untracked).
Both libraries and minima_commands relinked at 237. CLI remains 23 commands.
No running sessions at checkpoint. README/REVIEW/provenance updated.
No wallet, miner, remote peer or real node started. Only loopback socket fixtures.

Socket permissions:
Initial sandboxed loopback bind failed EPERM. Correctly rerun with require_escalated.
User approved prefixes [ctest,--test-dir,cpp/build] and
[ctest,--test-dir,cpp/build-sanitized]. Use escalated CTest for actual local socket
checks; do not re-ask outside the tool when existing permission covers them.
Normal differential harness uses controlled channels and runs inside sandbox.

NIO implementation details / review:
- Both pinned constructors execute in oracle. Only Main/NetworkManager/NIOManager
  startup is bypassed via test-only Unsafe.allocateInstance; the actual NIOClient
  and NIOClientInfo classes are freshly compiled from all 369 pinned Java sources.
  Fake SocketChannel/SelectionKey/manager boundaries control partial I/O/faults.
- Host/port-only constructor intentionally leaves buffers, queue, socket and key
  absent. Connected constructor allocates buffers and timestamps. UID format is
  checked then replaced by fixture UID; clock effects normalized after actual Java
  wall-clock updates. No identical random values/timestamps are claimed.
- NIORuntime lookup absence throws (missing Main). A bound lookup may RETURN NULL:
  Java can store null NIOManager and only fail when I/O accesses it. Fixed late in
  review, tested via nullManager=true. Connected clock falls back to real System
  clock if manager is null. Native fixture normalizes both initial timestamps.
- NIO runtimeLookup must be configured before network threads; no default no-op
  services. Empty traffic/post callbacks fail on use. Full Main/NIOManager binding
  remains outstanding. NIORuntime currentTimeMillis defaults to TimerMessage clock.
- Metadata getters/setters and toJSON use mMetadataLock. Trace flag atomic. Buffer
  read/write state still has one event-loop owner, as in the Java NIOServer.
  PosixNIOSocket serializes fd read/write/close/getter with mIOLock to prevent races.
- NIOClient preserves 8192-byte buffer flip/compact states, signed header reads,
  256MiB limit, zero-length frames, invalid/null queued items and failure timing.
  sendData appends before interestOps and wakeup; failures retain appended data.
  disconnect wraps key.cancel and socket.close in one catch (cancel failure skips
  close). Message posts use NIO_NEWMSG, uid/fullhost/data and actual MessageObject.
- Native key state has atomic validity/interests and supplied wakeup; actual OS
  selector event loop is NEXT, not yet present. Posix socket ctor owns an already
  connected fd, sets O_NONBLOCK/SIGPIPE policy, retries EINTR, maps EAGAIN to 0 and
  read EOF to -1. Sibling blocking frame reader was incompatible (64MiB and no
  zero-size frames); only socket/ownership composition was reused.
- NIOClientInfo captures metadata but time/extra delegate to retained client.
  Explicit UID constructor has connected=true, minimaPort=0 and no underlying
  client, so time/extra methods throw. Shared extra objects retained via MessageObject.
- JavaDateString uses ICU current standard offset before 1900 (JDK has no LMT),
  exact integer calendar arithmetic, hybrid Julian cutover, unpadded years, ICU
  zone names independent of represented date, libc short-name fallback only for
  matching seasonal offsets, 400-year recurrence for extreme future offsets and
  signed-long transition overflow treatment near LONG_MAX. UTC/London/NY/Tokyo
  cases pass, including JDK London 1970 GMT label despite +1h standard offset.
  Other timezone name datasets/custom mutable TimeZone APIs and every historical
  transition remain unverified. Existing TxHeader/MiniUtil formatters unchanged.
- Native NIO log prefixes and generic exception diagnostics remain unported.

NEXT announced: NIOServer accept/register/readiness/disconnect/shutdown loop,
then NIOManager and full runtime integration. No native NIOServer code exists yet.
Before writing, report exact reused source paths/minimum changes (mandatory gate).

Sources read for this NIO step:
- Full pinned NIOClient.java, NIOClientInfo.java, NIOServer.java, NetworkManager.java.
- Full NIOManager.java by combined reads: initial cat was truncated in middle;
  follow-up sed 330..650 and 650..815 filled that gap. Keep full dependencies in mind.
- Main.java only the static singleton, mNetwork field and getNetworkManager /
  getNIOManager accessors read for binding; do NOT claim whole Main was read.
- Pinned MessageStack/MessageProcessor full (again); native Message.hpp,
  MessageStack.hpp, TimerMessage.hpp, ChainServices.hpp, ConsensusServices.hpp,
  MiniUtil.hpp/.cpp, FastByteArrayStream.hpp/.cpp, MiniFormat.hpp and relevant
  createRandomString implementation, NIOTraffic.hpp, Crypto.hpp, TxHeader.cpp.
- Full sibling src/network/NIOClient.hpp, tests/network/test_network.cpp and
  src/network/NIOServer.hpp. Other sibling test_live_node.cpp/test_p2p_sync.cpp were
  discovered previously but NOT fully read for this step. Their packet schemas
  differ, so do not substitute their whole node/NIOMsg code.
- Installed JDK Date.toString/getCalendarSystem, JulianCalendar era normalization,
  ZoneInfo.getOffsets and SimpleTimeZone getOffsets/getOffset/getStart/getEnd /
  getTransition methods. Behavior inspected; JDK implementation code not copied.

NIOServer facts / next inspection:
- Java constructor only stores port/MessageProcessor and creates channel queues.
- run sets running=true, opens default ServerSocketChannel, nonblocking, binds
  InetAddress.getByName("0.0.0.0").getHostName(), then sets reuseAddress AFTER bind.
  Bind IOException logs and Runtime.halt(0). Selector opened, ACCEPT registered,
  posts NIO_SERVERSTARTED, selects with 30s timeout.
- Processes registration queue BEFORE disconnect queue BEFORE selected keys.
  Registers outgoing channels via misspelled regsiterNewSocket; incoming accept.
  addChannel gets numeric peer host/remote port, sets nonblocking/TCP_NODELAY /
  KEEPALIVE, registers READ|WRITE then switches READ, constructs NIOClient,
  attaches it and inserts into ConcurrentHashMap, posts NIO_NEWCONNECT.
- Explicit disconnect posts NIO_DISCONNECTED with reconnect=false; read/write
  exception disconnect posts reconnect=!incoming. Valid-greeting-only broadcast.
- Shutdown wakes selector, closes listener and each client, calls selectNow, sets
  running=false; Java retains client map after shutdown. Need native ownership and
  selector wakeup/close behavior, not a blocking accept handler substitute.
- Sibling NIOServer.hpp uses AF_INET, SO_REUSEADDR-before-bind, blocking accept,
  backlog64, descriptor ownership and getPeerHost. Reuse smallest compatible POSIX
  setup/peer-address blocks; event loop must follow pinned source.
- IMPORTANT unanswered: Java ServerSocketChannel.open default may be dual-stack
  even when binding 0.0.0.0. No preferIPv4Stack/preferIPv6Addresses property setters
  found in pinned src. Verify actual JDK IPv4/IPv6 bind/accept behavior BEFORE
  choosing native AF_INET vs AF_INET6. No socket profile probe written/run yet.
- Possible native selector: poll plus a nonblocking wakeup pipe and atomic key
  interests, reusing NativeNIOSelectionKey. This was considered, not implemented.
  Need preserve source queue ordering and strong object lifetime; avoid manager/
  server ownership cycles without silently discarding required receivers.

Do not run /tmp/minima_nio_native_once.py or /tmp/minima_nio_oracle_once.py again;
they overwrite fixes. Older /tmp/minima_*write* and *harness_once* generators also
must not be rerun. No subagents allowed. Goal remains active and unblocked.

Earlier peer checkpoint follows for reusable source-study details; its counts,
not-yet-implemented NIO notes, CLI build revision and current-session claims are
superseded by the section above.

# Active full-port continuation

User explicitly requests continuing until 100% of the 1:1 native C++ port is complete.
An unbudgeted goal is ACTIVE (created 2026-09-08); do not mark it complete at a subset milestone.

Current map: 457 Java files, 235 counterparts (216 production + 19 tests), 222 unported.
All map rows remain partial/not_verified. Native full node is not implemented.
Pinned upstream: 5564b9d78978efcaf23679190d05253961fc6e69. Root Java checkout and upstream-study remain unchanged.

Latest changes (225 -> 235):
- InetSocketAddressIO, P2PDoSwap, P2PWalkLinks native counterparts plus InetSocketAddress support adapter.
- P2PState, P2PGreeting, P2PParams native counterparts; JavaConcurrentMap gains explicit supplied object hashes. SocketAddressSet reuses its HashMap bins and equality keys. JavaList models default Arrays.asList fixed-size checks. P2PLogLevel support enum matches P2PFunctions.Level without creating a placeholder P2PFunctions class.
- P2PDB thin JsonDB/MiniFile translation.
- Mapped InetSocketAddressIOTest, GreetingTest, P2PDBTest (all matching assertions).
- PeerHarness.hpp shared by peer_messages.cpp and peer_state.cpp; Java PeerStateOracle extends PeerMessagesOracle fixture helpers.

Latest validation, normal AND ASAN/UBSAN:
- peer_messages_differential: 1447 programs PASS. Swap/walk JSON, partial mutation, aliases, safe integer casts, numeric/resolved/unresolved address behavior, IPv6 scopes, address hash/equality.
- peer_state_differential: 798 programs / 26118 operations PASS. State defaults/getters/setters/maps/aliases/peer sets; greeting constructors/JSON/equality/shared lists; P2PParams fixed-list operations; P2PDB wire/default/type failures/detached lists/partial reads.
- InetSocketAddressIOTest + GreetingTest + P2PDBTest PASS, including exact JSON and 1000-peer persistence round trip.
- hashmap 135, ram 315, message 905 programs PASS after JavaConcurrentMap extension.
- source_map --check PASS. git diff --check PASS (new cpp tree is untracked).
Both libraries and minima_commands relinked at 235. CLI still has 23 commands.
No sessions running at checkpoint. README/REVIEW/provenance/source-map updated.
No live peer connection, wallet, miner or node was started. Address tests perform resolver calls and interface enumeration only.

Important semantics / review boundaries:
- SocketAddress equality: resolved raw bytes+port (scope ignored); unresolved Java case-insensitive Unicode equality, hash uses ROOT lowercase. Native address key retains UTF16 length and folded codepoints. Numeric IPv4 JDK 1..4-part decimal, IPv6 uncompressed canonical format. Wildcard port-only constructor hostname is 0.0.0.0, as in JDK.
- Three initial address defects fixed: LOCALHOST fallback carries lowercase localhost hostname; BSD literal guard must not reject numeric overflow 4294967296 (JDK delegates this to libc, which resolves 0.0.0.0 on this host); Deseret case folding requires codepoints, not UTF16 units. 1447 tests pass after fixes.
- DNS cache/TTL, custom JDK network properties/preferIPv6 and complete InetAddress API pending. JDK source inspected, not copied.
- Shared secret/path/list/map identity retained where implemented. P2PGreeting state constructor copies lists, appends own address if accepting, then OS-random Fisher-Yates shuffles. Tests compare shuffled contents as permutations, explicitly. Non-Comparable address HashSet collision-tree ordering uses JVM identityHashCode; declared collision cases compare membership. Ordinary set and string-map order compared exactly. Native tree tie break uses equality keys.
- Static Java String nullability, generic exception/log diagnostics, concurrent iterators/mutations and full runtime binding remain gaps. Counts are NOT verified parity percentages.
- JavaList currently small list API (get/set/add/remove/clear/iteration), not complete Java List interface.

Next work announced: NIOClient socket buffering and connection state, then server/manager runtime.
Read completely in this continuation (not implemented):
- upstream-study/src/org/minima/system/network/minima/{NIOClient,NIOClientInfo,NIOServer}.java
- NIOManager.java: initial full cat output was truncated; missing middle was then read with sed 330..650 and 650..815. Combined reads cover class.
- upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java
- upstream-study/src/org/minima/system/network/p2p/{P2PState,UtilFuncs}.java
- upstream-study/src/org/minima/system/network/p2p/params/{P2PParams,P2PTestParams}.java
- upstream-study/src/org/minima/system/network/p2p/messages/{InetSocketAddressIO,P2PDoSwap,P2PGreeting,P2PWalkLinks,P2PSwapLink}.java
- pinned peer tests InetSocketAddressIOTest, GreetingTest, P2PDBTest, SwapLinksFunctionsTest, WalkLinksFuncsTest and testingutils QuickState/QuickInetLists.
- local cpp/src/org/minima/utils/{JsonDB,MiniUtil,FastByteArrayStream}.hpp/.cpp, MiniFormat.hpp, NIOTraffic.hpp, JavaConcurrentMap.hpp, InetSocketAddress.hpp/.cpp and existing map/RAM/prefs/message/peer harnesses.
- sibling /Users/eurobuddha/Projects/minima/tools/minima-core-cpp/src/network/NIOClient.hpp (read again) and tests/network/test_network.cpp (read earlier this continuation). POSIX send/recv/descriptor ownership are candidates for minimal reuse. Whole sibling client is incompatible: blocking full-frame I/O, 64MiB limit, rejects zero-size messages; pin uses nonblocking 8KiB buffers, 256MiB limit and accepts zero-size frames.
- Sibling tests/network/test_live_node.cpp and test_p2p_sync.cpp discovered but NOT fully read for NIO task yet. No native NIO code written yet. Finish relevant dependencies/callers/test inspection and report exact reuse before writing.

NIOClient behavior to preserve:
- host/port-only constructor creates random UID, incoming=false, obtains Main network NIOManager, but does NOT allocate message list, buffers, socket/key or set connection clock. Five-arg constructor allocates 8192 input/output/temp buffers, message list and timestamps.
- Incoming frames: read once into available input buffer; -1 throws Socket Closed; traffic adds read bytes including zero; zero returns. flip then require >=4 for header, signed big-endian length; >256MiB IOException, negative fails FastByteArrayStream constructor; preserve unconsumed bytes via compact. Zero-length frames are posted. Completed frame posts Message NIO_NEWMSG with uid/fullhost/data then clears read stream and updates last-read clock.
- sendData synchronized list: only append if key valid, then READ|WRITE interest and selector wakeup. Null data can be enqueued and later fail on dequeue. handleWrite fills 8KiB buffer with length+payload, retaining partial header/payload state, drops oversized message and breaks fill; writes once, adds traffic, switches to READ only if buffer/queue/current data all drained; compact. Preserve error-side state before compact.
- disconnect wraps key.cancel and socket.close in ONE catch; failure cancel skips close, matching Java.
- Metadata includes nullable welcome/host/maxima strings, counters, greeting/P2P flags, shared extra Object; NIOClientInfo copies metadata at construction but time/extra delegate live. Its explicit UID constructor has connected=true, minimaPort=0, no welcome set.., no underlying client (time/extra methods throw).
- NIO date rendering is Date.toString (not MiniUtil's dd_MM_yyyy_HHmmss). Existing MiniUtil uses ICU + extreme date fallback and TxHeader uses host calendar; inspect/reuse minimal compatible calendar formatting, verify timezone/historical cases.
- Native NIOManager/Main/service binding design NOT chosen yet. Current class constructor directly needs Main.getInstance().getNetworkManager().getNIOManager(). Need inspect Main/NetworkManager and Java fixture strategy before choosing adapter. Do not fabricate no-op services.

Other pending leaf candidates:
- P2PTestParams setTestParams copies mutable numeric/log parameters and shares DEFAULT_NODE_LIST, excluding VERSION/PEERS_LIST_SIZE/METRICS_URL. Default list is fixed-size with minima_one:9001 resolved on Java class initialization. Avoid unconditional DNS at every native process startup; class-init/native API adaptation needs judgment and tests.
- P2P2DB extends JsonDB, mutable shared nullable-string peer list; overridden load clears/rebuilds list sequentially after loading, getJSONArray creates missing array. Save updates JSON array before inherited file operation; add checks duplicate then logs. Already-read MiniUtil converts same string lists, candidate reuse. Not implemented.
- P2PSwapLink.java is entirely commented out (no executable Java declaration). Eventual counterpart should represent that fact; do not invent behavior.

Earlier 225 validation (unchanged) is summarized in README/REVIEW; latest utility/command results remain in build directories. All full-port runtime/API gaps remain open.
Do not rerun old /tmp/write_*, /tmp/extend_* or /tmp/minima_*_write*.py / *_harness_once.py generators: they overwrite subsequent fixes or duplicate CMake.
No agents may be spawned under current instructions. Goal remains active; keep working.
