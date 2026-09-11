# Code Review — ItemList source semantics

## Summary

ItemList preserves its declared source API behavior using existing native list,
Unicode, trimming and shared raw-object components. Both focused builds and
selected regressions pass. The 457-file working native node remains unfinished.

## Findings

No unresolved defect was found in this scoped translation review. Null strings
return before append-list dereference; null indexed additions bypass bounds
checks. Exact UTF16 splitting keeps empty tokens and does not advance for an
empty separator. Multi-token splitting recognizes supplementary code points and
retains isolated surrogate units. The inspected JDK source supports this behavior.

Two source mistakes remain deliberately observable: the two-argument constructor
stores input as its future separator, and getArray's raw Object[] cast fails even
for an empty list. These are documented in the student guide and asserted against
Java. A bounded supplied append list makes the non-advancing loop test finite
without adding a guard or substituting different production behavior.

Raw List values retain native type and shared identity through MessageObject.
Self-add copies original elements before mutation. Joining checks size repeatedly
and retains the current item while calling its renderer; this protects the
executing object across callback-driven vector reallocation and includes newly
appended elements, matching Java. Existing JavaList storage and fixed-list checks
remain intact; the additions are indexed insertion and virtual append/destruction.

## Validation

Normal and ASAN/UBSAN focused comparisons pass 1,493 programs and 11,690 operations.
Every declared ItemList constructor and overload is called. UTF16 values, list
snapshots, callback counts, exceptions and self/retained aliases are compared.
Both full builds pass. Five affected checks pass each: peer state 874 cases /
26,348 operations; messages 905; JSON collections 67 programs / 606 operations;
nodecount 171 programs / 314 loopback pings; peer-state concurrency. All 457 student
guides pass structural audit. The map contains 376 counterparts, 81 absent files
and 217 registered CTest entries. No full current 217-suite pass is asserted.

## Limits

Enhanced NPE/CCE provider prose is normalized; string code units and other errors
remain exact. The raw object adapter uses its existing UTF8 rendering boundary;
full inherited Java List APIs, arbitrary object adapters, subclass/constructor
virtual dispatch, resource exhaustion and concurrent mutation remain unverified.
An ordinary list with an empty exact separator retains non-advancing source
behavior. No full native-node or durable-storage certification is implied.

## Verdict

Approve this scoped local checkpoint. Full-goal completion is not asserted.
Historical reviews follow.

# Code Review — incentivecash persistence and PUT

## Summary

The mapped command preserves the source optional-ID validation, immediate save,
current-ID read, raw URL construction, PUT and JSONObject cast. It reuses existing
production components and is registered, matching pinned CommandRunner. This is
scoped progress toward the unfinished 457-file native node.

## Findings

No unresolved defect was found in this scoped review. Supplied blank/null/wrong
type parameters fail before mutation; a missing/null stored value returns empty.
Save-disabled and actual save-failure cases preserve memory and continue through
the same request path. Request or JSON errors do not roll back a saved ID. JSON
null is accepted, while arrays/scalars fail the object cast. The historical URL
is unchanged and uses the base version value, not the micro-version suffix.

Two exposed shared gaps were corrected. RPCClient retains tabs in the raw request
target using its existing CR/NUL parsing approach. MiniFile direct-save checks
file opening before serializing, reports Java FileNotFoundException for open
failure, and logs caught I/O through existing NativeThrowable/MinimaLogger. It
still serializes in memory after opening; generic late iostream error wording
and partial buffered-stream writes remain outside this finite checkpoint.

The fixture records source URLs and pre-request files before routing to loopback.
Production defaults still call actual RPCClient. Java command and RPC source are
unmodified. Native requestPUT is an added protected test boundary. Snapshots copy
preference JSON to prevent later mutation from rewriting earlier observations.
Registry overrides and logger state are restored; temporary directory cleanup
is confined to the test-owned preference path. No public rewards requests occur.

## Validation

Both full builds and eight affected checks pass in each build. Focused normal
and ASAN/UBSAN comparisons pass106 programs/207 operations/79 HTTP requests per
runtime. RPC482 cases/597 requests plus actual notification integration;
utility1361; prefs1130; wallet675 programs/1907 ops/50275 SQL;
MinimaDB2367 programs/7519 ops/334711 driver calls; registry1265; DB lock and
ownership checks. All457 guides pass structural audit. The map has375 counterparts
and82 absent files, with216 registered CTest entries. Initial failures are saved.

## Limits

Exact preference bytes, original URLs, service data, request traces and selected
logs/errors are compared. Enhanced NPE/CCE/malformed-URL prose, Java stack frames
and substituted fixture origin in PUT diagnostics are treated explicitly. No
public-service access, complete TLS/provider/stream/reference/concurrency API,
durable SQL, complete node or full current216-suite claim. Counterparts remain
implemented_partial/not_verified.

## Verdict

Approve this scoped local checkpoint. Full-goal completion is not asserted.
Historical reviews follow.

# Code Review — node-count traversal

## Summary

The mapped nodecount class uses actual NIO ping transport, the existing socket
address set and peer decoder, with source scan/CSV/numeric behavior. It is not
registered because the pinned Java runner does not include it. The complete
457-file native node remains unfinished.

## Findings

No unresolved defect was found in the scoped traversal review. Defaults are
pinged first; their client counts are ignored. Discovered peers and pinged peers
retain source set semantics. Pending sets use the installed JDK collection-copy
capacity through the existing HashMap reserve helper, then the source removeAll
size branch. A local owner now retains the converted peer list during iteration;
the first loopback comparison exposed the earlier temporary-lifetime mistake.

Float32 rounding, double client accumulation, saturation of float-to-int log
values and nonfinite JSON nulls are preserved. Numeric parsing reuses the JSON
lexer's validated-strtod approach with Java's broader decimal/hex/suffix grammar.
NaN sign is canonicalized; typed exception storage preserves embedded NUL messages.
Finite parsing tests compare raw double bits and text, not approximate tolerances.

File opening precedes complete parsing. CSV text reaches disk only at the explicit
flush points; an empty scan creates an empty file. Later write errors are swallowed
as in PrintWriter. RAII closes the native descriptor; source leak/GC behavior is
not asserted equivalent. Fixed-size CSV lines avoid automatic-buffer overflow in
the covered execution path. Raw JSON preserves float rendering, not Java Float
object APIs.

Malformed greetings exposed older decoder categories: null/non-string string-array
entries and missing colon fields now throw Java null/cast/array-index exceptions.
Colon splitting, trailing-empty removal and port parsing remain source behavior.
Existing P2P discovery tests verify the other production caller. Initial registry,
numeric-message, discovery-lifetime and decoder failures are retained as evidence.

## Validation

Both full builds pass. Normal and ASAN/UBSAN focused comparisons pass 171 programs
and 314 loopback ping requests per runtime. Both builds pass four affected suites:
peer state (874 programs / 26,348 operations), NIO transport (640 serialization/send
cases plus actual ping cases), registry (1,265 cases), and P2P discovery
(31 programs / 84 operations / 39 TCP pings). All 457 student guides pass structural
audit. There are 374 counterparts and 215 registered CTest entries.

## Limits

No public-network enumeration, full native-node startup, durable database or full
current 215-suite claim. Test manager owners skip startup; all ping sockets and
frame handling are real. Java null/cast diagnostic prose is normalized; numeric
bits, parsed replies, log content, request order and CSV bytes are compared.
Other JDK collection versions, identity-hash tree ties, arbitrary reference or
concurrency APIs, all floating-rounding/locale cases, Java Float-object semantics
and leaked-writer lifetime remain unverified. The native file remains partial.

## Verdict

Approve this scoped progress checkpoint. Full-goal completion is not asserted.
Historical reviews follow.

# Code Review — multisig workflows

## Summary

Two mapped command counterparts share the source action logic and exact existing
contract. They reuse native wallet/search/proof/transaction/file/vault/miner
implementations. The 457-file native node is still incomplete.

## Findings

The scoped source review found no unresolved production defect in these translated
actions. Contract text and help metadata are unchanged. Parameter order, UTF-16 key
length checks, int32 threshold conversion and unescaped nested command strings
match Java. list preserves duplicate matches at different state ports; spend
selects the first match, computes scaled-token change, and distinguishes raw read
addresses from normalized full-command addresses. Parent replies preserve nested
false statuses rather than inventing rollback or error propagation.

Signing imports before key discovery, examines every state value of the first
input, and keeps repeated signing attempts. Unlock/relock surrounds child execution
with the source's lack of finally cleanup. Post uses mine:true/auto:true. Review
also checked fresh factories, registration and the common contract used by Wallet.

A malformed-file comparison exposed txnimport's older generic runtime exception.
It now uses JavaNullPointerException while retaining replacement-ID argument
parsing before a null receiver is rejected. The recording fixture independently
needed getMessage() to preserve embedded NUL text. Initial failures are retained.
New fixture compile/observation typos were corrected before the passing run.

The test-only tracing wrapper invokes actual child bodies and restores the
registry after each operation. Optional proof fixture configuration leaves old
fixtures unchanged. Whole exported TxnRow bytes are compared except complete
validated generated-ID fields; a separate reader verifies real signatures and
full file consumption. Subdirectory exports stay inside the observed temp root.

## Validation and limits

Normal comparison: 340 programs / 1,089 operations / 22,269 SQL observations.
The final sanitizer comparison passes the same corpus, including complete
subdirectory-file observations. Both full builds pass. Four affected regression
suites pass in both builds. All 457 teaching guides pass structural audit.

Recording SQL, fixed miner-entry PRNG and fixture chain/lifecycle services are
explicit boundaries. The normalizer excludes Java-specific null/class-cast prose,
shape-checked random IDs/timestamps and independently encrypted vault bytes. It
retains transaction/witness bytes, command text, SQL effects and decrypted state.
No full current 214-suite, durable-storage or live-node parity claim is made.
Empty-input imports, all threshold/root combinations and arbitrary reference or
concurrency APIs remain unverified. All mapped counterparts remain partial.

## Verdict

Approve this scoped progress checkpoint. Full-goal completion is not asserted. Historical reviews follow.

# Code Review — wallet token creation

## Summary

The mapped tokencreate command reuses native number/object/wallet/proof/signing,
generator and miner implementations. Source-specific control flow preserves raw
parameter casts, wallet search order and mutable token-name metadata. The full
native-node objective remains unfinished.

## Findings

No unresolved defect was found in the scoped translation review. Missing required
parameters precede parsing; mine/uselimits/state/name are read in source order.
Amount is a raw nullable string cast, decimals use Java integer parsing, and script
null remains deferred until construction. Supply is floored, optional upper limits
and multiplication order are retained, and no negative-burn rejection is invented.

The default address is obtained before chain checks. Mature simple native coins
are selected in search order, skipping miner/mempool reservations. Insufficient
funds return a false status/message; no eligible coins throw. MMR history is
clamped using the newest selected input. Proofs/scripts precede metadata signing;
required public keys are deduplicated. The token signature covers the future
first output coin ID. State parsing and transaction signing follow output/change
construction, then actual generation and synchronous/queued mining execute.
Name metadata is intentionally shared; late errors do not undo its mutations or
signature-use effects. No additional rollback was introduced.

The initial compile used calculateCoinID as static; it is an instance method and
was corrected before validation. The first corpus also used a legacy reservation
fixture whose Java miner lacked its message queue. Its replacement reserves through
the initialized miner in both runtimes. Initial evidence is retained. Production
behavior was not changed to emulate that incomplete fixture.

## Validation

Both configurations pass 335 programs / 1,349 operations / 26,719 SQL observations.
CTest allows 600 seconds for the sanitizer workload; its unchanged per-response
guard remains 30 seconds. Both registrations were inspected after configuration.
Exact replies/wire/SQL/queue effects and shared-name mutations match Java. Independent
decimal checks cover supply, coloring and conservation; cryptographic checks verify
transaction and token metadata signatures. Both full builds and three affected
regressions pass: registry, transaction mining and custom-address token wrappers.
All 457 guides pass structural audit. There are 371 counterparts and 86 absent
mapped files; all counterparts retain implemented_partial / not_verified status.
No full current 213-test suite or full native-node parity pass is claimed.

## Verdict

Approve this scoped checkpoint. Durable SQL, network acceptance and complete
inherited/reference/concurrency APIs remain outside this finite evidence.
Older scoped reviews follow for historical context.

# Code Review — historical burn metrics and nullable median

## Summary

The new mapped burn command composes existing TxPoW arithmetic, tree traversal,
TxPoWDB and numeric JSON. It preserves per-instance accumulators and source
window boundaries. The full native-node implementation remains incomplete.

## Findings

### MAJOR — throwing standard sort changed caller-visible values (resolved)

The original native median used std::stable_sort. With [3, 1, null], its throwing
comparator left [null, 1, null], while Java retained [3, 1, null]. The replacement
adapts OpenJDK run discovery, binary insertion and merge scheduling; sorted
non-null runs reuse std::inplace_merge. A final singleton null throws before
merge mutation. Source notices, license and input hash are retained. This is a
specialized nullable-number adapter, not a general throwing-comparator TimSort.

### MINOR — empty median result identity (resolved)

Repeated empty-list calls originally allocated different zeros. A shared helper
singleton now preserves repeated-result identity, with an explicit oracle check.
The native global MiniNumber constant/reference API is still a broader limitation.

## Source and fixture review

The database is captured once before action branching. Default list traverses
at most 50 ancestors, counts the block and each available reference, and keeps
duplicates. checkBurn rejects nontransactions and otherwise always updates the
50-block accumulator, even for direct counters >=50; negative counters update
the 10-block window. Empty minima become zero before median serialization;
unknown direct actions do not reset state. The runner's inherited empty parameter
allow-list rejects the hidden action parameter. Fresh factory instances reset.

The fixture retains actual command objects and decodes Java-created TxPoW bytes.
It compares exact numeric replies, list ordering/identity, database calls and
failure effects. Independent decimal calculations check totals, lower medians,
sentinel min/max and average truncation. Initial wrong metadata and token-filter
expectations were corrected against pinned Java and their failures were retained.
The initial fixture build also attempted an unavailable JSONObject.clear method;
it now assigns through the existing object, preserving its shared owner identity.

## Validation

Final normal and sanitizer runs each pass 364 programs, 4,361 operations and 30,113
SQL observations. Both complete builds pass after review.
Affected runner (1,265 cases), history (484 programs / 1,489 operations / 52,194 SQL), and
scanchain (320 programs / 2,027 operations) pass both builds. These regressions preceded
the final helper-only guards and empty-result identity correction; those changes
are covered by the final burn corpus. All 457 guides pass structural audit.
No full current 212-test suite or full-node parity pass is claimed.

## Verdict

Approve this scoped checkpoint. Arbitrary inherited
reference/subclass/concurrency behavior, durable SQL and complete native entrypoint
integration remain unfinished. Older reviews follow for historical context.

# Code Review — txnauto and shared automatic transaction builder

## Summary

The mapped command preserves the source's nested-command orchestration and
failure cleanup. Its static helper reuses the proven burn algorithm while
retaining the source's early amount check, unused address argument, default
link hash and mandatory signing. The full native-node goal is unfinished.

## Findings

No unresolved defect was found in this scoped implementation review. Parameter
order and duplicate checks precede insertion. False child statuses delete the
row before forwarding the typed error; direct proof failures leave it present.
Identifier concatenation is preserved, including space/separator behavior.
The helper resolves database/miner/wallet/tree before checking nullable amount,
filters mining/mempool coins, constructs proofs/change and signs once per key.
The existing burn API still validates late, supports exclusions/optional signing
and explicitly assigns its supplied link. No payment output was invented for
the unused address argument.

A test expectation assumed a 32-byte zero link. The pinned default is one byte,
0x00, and the implementation already preserved it. The corrected assertion and
initial failure are retained. The Java fixture's added type names also required
qualification before its initial compile could pass.

## Validation

Both comparisons pass 414 programs / 1655 operations / 29925 SQL observations.
Normal/sanitizer durations: 34.16s / 177.74s. Independent assertions cover cleanup,
retained rows, actual signature validity, change-only outputs and conservation.
Both builds pass the existing 671-program proof/burn corpus (2734 operations,
41653 SQL calls), 736-program funding corpus (3418 operations,39016 SQL calls),
and 1265 runner cases. Full normal and sanitizer builds/relinks pass.
All457 student guides pass the structural audit; this is not parity certification.

The older shared-JSON sanitizer baseline is terminal:202/204 initially, with
passing reruns for the stale MiniByte catch and unchanged NIO timeout case.
Full log and composite summary saved. No full-current-211-suite pass is claimed.

## Verdict

Approve this scoped checkpoint. Durable storage, network admission, native entry
point, arbitrary inherited reference/subclass/concurrency behavior and full-node
integration remain open. The source map keeps all counterparts partial.

## Previous scoped reviews

# Code Review — peers command and network fixture extension

## Summary

The native peers command follows the pinned Java branches and composes existing
Main, P2P, address parsing, shuffle, files and HTTP components. The command is
registered and compiled. Full-node parity remains unfinished.

## Findings

No unresolved defect was found in the scoped command review. Disabled/slave
short-circuiting precedes parameter reads. Forcecheck queues before parsing max.
Limits are inclusive, empty split behavior differs between list and forcecheck,
comma tokenization drops empties, and duplicates queue separately. Port parsing
and socket validation retain their separate failure points. Earlier messages
survive a later error. Fetch rejects file presence; publish uses MiniFile base
paths and writes actual address bytes. The helper retains partial text on caught
exceptions; Java Error/allocation failures escape.

Two fixture errors were found and fixed: a copied vector lost its temporary
shared owner before iteration, and the fixture omitted the database service
used by haveAnyPeers. Initial reports are retained. Production peer-list code
already retained ownership. Shared fixture regression checks pass after fixes.

## Validation

271 programs / 1355 operations / 48 HTTP requests per runtime pass normal and
ASAN/UBSAN. Independent assertions check shuffled membership/cardinality, queue
duplicates and partial effects, HTTP request attributes and actual file bytes.
Both builds pass 222 network programs / 794 operations, 124 status programs /
562 operations, and 1265 runner cases. The full normal build/relink passes.
The original older sanitizer baseline remains running; no full-current-210-suite
pass or native-node startup claim is made.

## Verdict

Approve this scoped checkpoint. Live peer admission, public DNS, arbitrary
subclass/reference behavior, concurrent owner replacement, platform encodings
and the full node remain open. Random streams are not compared for identical
choices; each selected set is validated before normalization. Missing-file and
partial status remain explicit in the source map.

## Previous scoped reviews

# Code Review — tokenvalidate and signature conversion

## Summary

The command preserves the pinned sequential validation flow and reuses existing
Token discovery, Signature/TreeKey, JSON parser and RPCClient implementations.
Negative signature/web checks remain response data with a successful command
status. The full native-node goal is unfinished.

## Findings

No unresolved defect was found in this scoped command review. Both signature
fields must exist; root comparison precedes verification over the coin ID.
Malformed signatures stop execution; false verification still allows the web
check. The URL cast remains outside the download catch, while null URL failure
occurs inside. ASCII trim and canonical token ID comparison preserve source
behavior. Java Error and allocation failures escape the Exception boundary.

The source converter's IOException-only catch/log behavior is restored. Tests
distinguish logged I/O failures from escaping NumberFormatException. Null imported
tokens now retain Java's exception category. Two initial fixture expectations
and the actual null-import difference are documented in retained evidence.

A regression found that restored logs polluted the command fixture's stdout
protocol. Reusing the existing stderr logger callback fixes it without changing
production logging. The sanitizer target-name mistake is recorded: its initial
batch used an older minima_command binary; the explicit correct-target rebuild
and passing rerun supersede that result.

## Validation

Both new comparisons pass: 126 programs, 254 operations, 33 actual HTTP requests
per runtime. All seven affected comparisons pass with current binaries in both
builds; reports, initial failures and final reruns are retained. Full normal
build/relink passed; the final fixture-only change was rebuilt separately.
The original older sanitizer baseline is still running. There is no full-current
209-test-suite pass claim.

## Verdict

Approve this scoped checkpoint. Finite metadata/signature/loopback cases and
selected provider-diagnostic normalization do not certify arbitrary reference
identity, concurrency, platform behavior, durable storage or full-node startup.
Existing broader Signature/verify/CHECKSIG API limitations remain separate port
work; the mapping explicitly retains partial status.

## Previous scoped reviews

# Code Review — scanchain command

## Summary

The new mapped scanchain implementation preserves the pinned sequential command
flow and reuses the existing parameter, tree, TxPoW, SQL and date components.
Its detail helper is virtual and nullable for the source's object-return API.
The full native-node goal remains unfinished.

## Findings

No unresolved defect was found in the scoped sequential command implementation.
The tip check precedes both parameter reads; offset precedes depth. Both loops
retain their inclusive conditions and Java's signed 32-bit counter increment.
Block, relative depth and timestamp remain JSON numbers; owned TxPoW references
survive detail calls. Referenced records retain duplicates and include empty
transactions. Each lookup resolves the current MinimaDB owner as Java does.

Two fixture setup errors were corrected: an unbracketed string state and a
MiniNumber timestamp outside its allowed range. Initial failures are saved.
The driver now closes both subprocesses even when one cleanup fails. It preserves
serialized reply data while normalizing only known exception diagnostic fields.

## Validation

Both native library/fixture builds pass without observed compiler warnings.
All 320 new programs / 2027 operations pass in normal and ASAN/UBSAN builds,
23.57 and 67.47 seconds respectively. Independent assertions check bounds,
field types, height subtraction, dates, ordering and transaction field presence.
All six affected comparisons pass: 1265 runner cases, 484 history programs /
1489 operations / 52194 SQL calls, and 680 TxPoW programs / 2099 operations /
41510 SQL calls per build. Reports and logs are retained with the new lab.
Source-map and all 457 student-guide structural checks pass.

## Verdict

Approve the scoped implementation and evidence. Finite tests do not certify
arbitrary subclass-induced collection mutation, concurrent tree replacement,
durable SQL, full inherited reference APIs or native-node startup. The full normal
build and relink pass; the older sanitizer baseline is still running, with its
known initial MiniByte and NIO failures already followed by passing reruns.
No complete current 208-test run or full-port parity is claimed.

---

# Code Review — saved coin-import integration evidence

## Summary

This follow-up changes documentation and retained validation reports only. The
coinimport implementation remains at 66f6b91. All 20 selected integration tests
pass against current rebuilt normal binaries. Existing node-state and command
runner comparisons also pass in the sanitizer configuration.

## Findings

No unresolved defect was found in this evidence update. Read all four affected
comparison reports, the 20-test CTest log and parsed summary, both sanitizer
logs, and the saved NIO rerun report. Node-state covers 363 programs / 1411
operations / 21009 SQL calls per build; runner covers 1265 cases per build.

The initial full sanitizer NIO failure is a subprocess timeout. Repeating the
unchanged driver and binary after builds finished passes 1918 programs / 4677
packets under the same 100-second child limit. No deadline or production change
was made. The initial timeout is retained; its precise timing cause is unproven.
Normal baseline coverage and current affected checks are reported separately.
The older 204-test sanitizer baseline remains running, so no complete 207-test
current-tree pass is asserted.

## Verdict

Approve this scoped evidence/documentation checkpoint. Full-port completeness,
durable SQL, full native-node startup and remaining sanitizer integration work
are still open. A local recovery bundle has been restored and verified; no
GitHub push or off-machine backup is claimed.

---

# Code Review — network status and control command

## Summary

network's mapped native body compiles and is registered. It preserves source
parameter validation, first matching UID, repeated owner lookups, actual detailed
status, traffic reset, host selection, queued restart and shared trace updates.
The full working-node goal remains unfinished.

## Findings

No unresolved defect was found in the scoped wrapper changes. The fixture's
NetworkManager UserDB callback is now bound to the real test database before
getStatus is called. Initial connection/traffic clocks are fixed only in the
opt-in status fixture; live reset times are bounded against operation clocks.

The earlier teaching page incorrectly described tracing as a separate state for
each subsystem. MessageProcessor stores static trace/filter fields. The corrected
lesson and independent assertions show the first logging call's shared effect
surviving a null second receiver. The production trace implementation is reused.

## Validation

Both full builds pass without observed compiler warnings. The initial 119-program
comparison passed; the expanded 124-program/562-operation suite passes in both
builds (4.53s/3.80s). All ten selected regressions pass in 19.15s/27.27s. All 18
result reports were read. Checks include empty lists, duplicate UID first match,
runner UID rejection, exact restart queue, partial trace effects, reset totals
and breakdown maps, configured-host bypass and read-only local host enumeration.

## Verdict

Scoped changes approved. Full asynchronous restart, exhaustive concurrent owner
replacement, enabled-P2P failure branches, durable storage and whole-node startup
remain unverified. Source-map status stays partial and parity not_verified.

---

# Code Review — nullable command replies and ping

## Summary

ping's mapped native body compiles and is registered. It reuses the existing
parameter parsing, Main ownership and NIO transport. The shared command API now
represents Java's nullable JSONObject return through the existing JSONValue.
The full working-node goal remains unfinished.

## Findings

### MAJOR — Object-only command return could not represent source null (resolved)

ping returns null when host has no colon. Every no-argument runCommand declaration
and definition now returns JSONValue. The migration manifest records 166 changes
in 165 files. Reverse-substitution hashes verify that 163 files changed only the
return type; Command.hpp also has an explanatory comment and the runner-error
fixture has explicitly added probes. String-taking helper overloads retain their
object result type. Existing concrete command algorithms were not rewritten.

### MAJOR — Result status must be read outside the catch block (resolved)

CommandRunner retains the nullable result, appends it, then reuses CommandResult's
status check. A null reply or missing/null status escapes as NullPointerException;
a wrong status type escapes as ClassCastException. Execution/logging exceptions
retain the existing handling. Direct assertions verify the escaped class, ordered
entry events and absence of later command execution.

### MINOR — Valid Greeting can contain null extra data (resolved for ping)

Greeting already stores parsed JSON null as an empty optional. Its new optional
accessor lets ping forward that value without dereferencing it. The loopback test
asserts extradata:null inside a valid response. Existing reference-taking getters
and their failure/aliasing boundaries remain separate work.

## Validation

Both full builds pass. The first new fixture build tried an unavailable JSONObject
remove method; the fixture now builds a filtered object using existing entries/put.
Only the rebuilt fixture's results are accepted. The 1,680 runner-error cases and
95 ping programs plus nine loopback cases pass in the normal build. All 35 selected
normal regressions pass in 181.54 seconds; all 35 sanitizer regressions pass in 762.10 seconds.

The ping driver reuses the established loopback peer and independently checks
request bytes, parsed endpoint, valid fields and invalid-field omissions. Comparison
keeps traffic and ordered log counts, while excluding socket/wire diagnostic prose
and enhanced NPE/CCE expressions. Other runner errors retain exact messages/logs.

## Verdict

Scoped changes approved after both full builds and all 35 selected regressions pass in each build.
Full startup, durable SQL/MySQL, remaining mapped files, complete registration,
NIO logger failures and exhaustive malformed Greeting/reference behavior remain
open. A finite comparison pass is not full behavioral parity.

---

# Code Review — direct-peer commands

## Summary

The mapped disconnect, message and p2pstate bodies compile and are registered.
They preserve raw parameter access, source validation order, Main ownership,
real NIO routing/encoding, detailed P2P status and their distinct reply fields.
The native working-node goal remains unfinished.

## Findings

### MINOR — Missing NIO owner had the wrong exception class (resolved)

The new comparison found Java NullPointerException versus a native generic
RuntimeException for an absent NIO manager. NIOManager and P2PFunctions now use
the existing typed null exception, including an unbound runtime lookup.

### MINOR — Local IPv6 hostname lost its interface suffix (resolved)

The P2P regression found a real unique-local IPv6 address with an interface suffix
in Java but none in C++. P2PEnvironment.numeric previously relied on the kernel's
sockaddr scope field. The cached OpenJDK NetworkInterface.c BSD enumeration assigns
the interface index to every IPv6 address before creating its Java representation.
The native helper now reuses if_nametoindex and its existing numeric formatter to
retain that suffix. Full real-interface sets match after this change.

### Test fixture setup (resolved)

NIOClient construction requires an already-published network owner. The new fixture
now publishes actual owners before constructing clients, then applies deliberate
missing-owner inputs. It changes the retained NetworkManager's NIO field in place
so a test-only replacement does not trigger extra native shutdown. Assertions
reject fixture setup failures and require successful detailed status for healthy
owners; this prevents two equally broken fixtures from being mistaken for parity.

## Validation

Both full builds pass without observed compiler warnings. All 222 programs and
794 operations match Java; final comparisons took 3.55s normal and 5.68s ASAN/UBSAN.
Independent assertions verify exact GENMESSAGE bytes and broadcast/targeted
recipient selection, disconnect type/UID/order, metadata and healthy P2P replies.
No peer command fixture starts networking workers or dispatches its queued actions.

All 12 distinct affected regressions pass across phases in both builds. Initial
batches passed eight and exposed three failures (37.16s/55.10s): two loopback suites
were blocked by the sandbox; the P2P real-interface test found the IPv6 discrepancy.
The normal loopback rerun passed in 26.60s with the required socket access. Final
batches passed in 55.91s/56.89s, including P2P interfaces/HTTP, network-manager status,
the new commands and remaining sanitizer loopback checks.

Inspected both sets of reports: command 12,289 cases; runner 1,265; runner-errors
1,120; Main 336 programs/897 operations; lifecycle commands 102/366; NIO state
1,449/3,879; NIO transport 640 cases and 17 ping cases; connector errors 12 plus
four native factory checks; P2P functions 672/3,670 and 39 localhost HTTP cases;
P2P manager 405/2,915; network manager 540. Native Main ownership also passes.
The final map has 355 counterparts (311 production, 44 original tests), with
102 absent files and 194 CTest entries. All 457 student lessons remain authored.

## Verdict and limits

Approve with suggestions for this scoped increment. Full network delivery,
startup/persistence, concurrent owner mutation and complete API parity remain
unverified. ping is still absent: its Java null command reply requires a faithful
native command-result API adaptation. All mapped rows remain implemented_partial
and parity not_verified; the complete-node goal remains active.

---

# Code Review — quit and rescue

## Summary

Both mapped command bodies compile and are registered. They reuse actual Main,
Command, CommandRunner and Message implementations. quit preserves parameter
validation before its two singleton lookups. rescue preserves explicit-host
precedence, configured fallback bytes and posting before its success response.

## Findings

No new production discrepancy was found in the comparison. The fixture groups
initially shared their javac output directory; separate class directories and
source listings now prevent parallel CTest invocations from overwriting classes
being read by another JVM. The Java fixture also resets rescue configuration
for every request, matching the native reset.

## Validation

Both full builds pass with no observed warnings. All 102 programs/366 operations
match Java. Initial comparisons passed in 2.59s/3.10s; final comparisons after
fixture isolation passed in 3.18s/2.43s. Independent assertions connect successful
rescue replies to actual queued message types, hosts and order. No rescue handler
was dispatched and no host was contacted.

All five selected regressions pass in both builds: 12,289 command cases, 1,265
runner cases, 1,120 runner-error cases, 336 Main programs/897 operations and native
Main ownership. The normal batch took 9.69s; the sanitizer batch including the
new command suite took 24.00s. Both final fixture groups also ran concurrently
successfully in the normal build (4.17s). All ten JSON reports were inspected.

## Verdict and limits

Approve with suggestions for this scoped increment. Full shutdown/compaction,
complete MegaMMR rescue, arbitrary concurrent singleton replacement and nullable
global configuration remain unverified. Reply text alone does not prove those
effects. The complete-node goal stays open. There are 352 mapped counterparts
(308 production and 44 tests), 105 absent files and 193 CTest entries. All 457
student lessons remain authored; every port row is partial/parity not_verified.

---

# Code Review — Main runtime integration

## Summary

Main now compiles and assembles existing native database, processor, miner,
network, notification and polling implementations. The source startup/lifecycle
and message bodies are translated, with explicit platform services and native
ownership cleanup. The complete node remains unfinished.

## Findings

### MAJOR — Reentrant owner destruction (resolved)

Main's singleton, listener and child-manager assignments initially retired owners
while holding their registry mutex. A destructor or joined worker can reenter
Main and deadlock. The assignments now swap under the mutex and release old
owners afterward. A native listener-destructor test exercises registry reentry.
Startup cleanup retains only the timer actually created by this Main; a library
failure before timer creation no longer assumes a global timer exists. Runtime
binding is included in the factory's failure-cleanup scope.

### MINOR — Wrong typed-getter exception classes (resolved)

Main tests exposed JsonDB Boolean/string getters leaking bad_variant_access.
Explicit type checks now preserve Java ClassCastException and null/default paths.
Message integer/float/Boolean/string getters translate wrong boxes and null
unboxing separately. The single-TxPoW MySQL path reads credentials in explicit
Java argument order. These are shared fixes, verified by affected regressions.

### MAJOR — Complete runtime acceptance remains open

MainServices still requires real SQL/MySQL drivers. Successful full startup,
complete backup/rescue commands, MegaMMR snapshot binding and the Minima entry
point remain unverified or absent. The fixtures use recording SQL and unstarted
workers; they cannot establish persistence or a working network node. Native
interrupted-sleep semantics, all aliases and all thread interleavings also remain
open. These limitations prevent approval of the full 1:1 goal as complete.

## Validation

Both full builds pass with no observed compiler warnings. All 336 Main programs
and 897 operations match Java (8.45s normal, 5.11s ASAN/UBSAN). Native checks pass
(0.69s/1.70s): failed library loading before a timer, failed first SQL connection,
weak-owner release, reentrant listener destruction and nested worker diagnostics.

All 14 distinct selected regressions pass across the validation phases. The
normal initial 13-test batch passed in 68.13s; preferences, utility and MinimaDB
checks after the final string-getter change passed in 40.70s. The sanitizer batch
passed 13/14 in 120.12s; NIO messages exceeded their internal 100-second process
limit. Its isolated rerun passed all 1,918 programs in 83.64s without code or time
limit changes. This timeout was not a behavioral comparison failure.

Inspected reports: Message 905 cases, preferences 1,130, utility 1,361, logger
2,016, uncaught handling 121, executor uncaught handling 10, MinimaDB 2,367
programs/7,519 operations, network manager 540 programs and NIO message dispatch
1,918 programs. Worker, thread-name and network-ownership checks also pass.
The mapped checkpoint is 350 counterparts (306 production and 44 original tests),
with 107 absent. All 457 student lessons remain authored. This is scoped evidence,
not a rerun of every CTest entry or a full-file parity certification.

## Verdict

Approve with suggestions for this scoped increment. The complete-node goal remains
open; all mapped counterparts stay implemented_partial and parity not_verified.

---

# Latest review — Pulse runtime prerequisite for Main

## Scope and findings

Reviewed Pulse, TxPowTree pulse-list publication, MiniData reader diagnostics,
the reused MinimaDB fixture, new pulse corpus and existing object-harness logger
boundary. Source review found that the native tree cleared its pulse vector in
place while Java replaces the list. Both classes now retain shared list ownership;
instance Pulse reads replace the list before consuming bytes. The new createPulse
method resolves the actual global DB and preserves the source catch/lock order.

The first comparison found an oversized MiniData diagnostic mismatch. The reader
now reuses MiniFormat.formatSize with the exact pinned message, including its stale
256 MB wording. The expected-size message is also source-exact and covered by 35
direct reader cases. No large payload allocation is used for oversized cases.

## Validation

Both final full builds pass without observed compiler warnings. All 152 programs
and 946 operations match Java in normal and sanitizer builds (10.18s/25.01s).
List identity and replacement have independent assertions; injected exceptions,
Errors, null trees and singleton clearing exercise real read-lock outcomes.
All eight affected regressions pass (63.66s/184.70s): 9,464 object cases, 1,027 tree
cases, 1,361 utility cases, 714 NIO-client programs, 2,367 MinimaDB programs, 324
chain-command programs, plus native lock and ownership checks. Both sets of seven
JSON reports were inspected. The map remains 349 counterparts/108 absent files.

## Verdict and limits

Accept this scoped dependency increment. Main itself remains absent. Null list
elements, direct MiniData byte-array aliases, mutable global limits, all diagnostic
locales and adversarial concurrent mutation are still unverified. An Error during
Pulse tree lookup leaves the source read lock held; native code intentionally
preserves that behavior, with fixture cleanup performed after observations.
The shared SQL fixture remains a recording driver. Full native storage, Main,
Minima and the working complete node remain required. All mapped rows stay
implemented_partial and parity not_verified.

---

# Latest review — MySQL connection wrapper

## Scope and result

Reviewed the pinned MySQLConnect body, native header/body, shared SQL interface,
recording-driver extensions, Java/C++ fixtures and comparison corpus. The class
preserves SQL text, method-specific catches, synchronization, partial assignments,
nullable decoded blocks and the 14 coin bindings. Fixed-decimal formatting reuses
the existing Java floating conversion and ICU symbols; the JDK26 formatting source
supplies the rounding rule. No production discrepancy was found in 1,598 programs.

Both full builds and both complete comparisons pass (40.98s/110.76s). All five
selected SQL regressions pass in normal and sanitizer builds (42.11s/118.87s):
SqlDB 464 programs, TxPoW SQL 2,592, CoinDB 1,350, ArchiveManager 3,333 and wallet
commands 675. All ten regression reports were inspected. This is a scoped check,
not a claim that all 189 CTest entries were rerun.

## Limits

The connection is a required native-driver boundary. The recording fixture does
not execute SQL or persist data, and an unconfigured driver throws explicitly.
Native MySQL integration, durable local SQL, driver discovery, the demonstration
main, arbitrary retained aliases, exhaustive locale/concurrency behavior and Main
startup remain open. The source's readOnly flag only skips DDL; wipe and write
methods preserve source behavior. Synthetic credentials and objects were used.

## Verdict

Accept this scoped increment with the above unfinished integration recorded.
The source map has 349 counterparts and 108 absent files. All mapped counterparts
remain implemented_partial, with parity not_verified. All 457 teaching pages are
authored; the structural audit checks 9,000 links, 2,634 line anchors and 457 source
hashes with no errors. Teaching coverage does not certify full port completion.

---

# Latest review — seed derivation and original casting tests

## Summary

seedrandom reuses actual wallet state and framed object hashing. Both builds
match Java on 309 programs, including independent SHA3-256 expectations and
SQL failure observations. Four original casting test files add 87 active
assertions per runtime without changing the existing cast implementations.
All selected command regressions and final casting/harness checks pass.

## Findings

### MINOR — Preserve explicit result-class casts in mapped tests (fixed)

**Files:** test/org/minima/tests/kissvm/functions/cast/*Tests.cpp.
The first translation read result text through the base Value interface after
checking its type tag. Java explicitly casts the result class. Native tests now
retain that check with dynamic_cast. All ten final checks pass after rebuilding.

## Verdict

Approve this scoped increment. No seed derivation discrepancy was found in the
309-program corpus; it uses synthetic seeds only. UTF-8 fixture output replaces
isolated surrogate code units, so its response observations have that transport
limit. Original assertions are finite examples, not complete casting parity.
The source map has 348 counterparts and 109 absent files, all parity not_verified.
Full Main startup, durable native SQL and complete runtime integration remain open.

---

# Latest review — node-state commands

## Summary

The three bodies reuse actual state components, source branch order and reply
formats. Both full builds, the 363-program comparison and all 13 selected
regressions pass in normal and ASAN/UBSAN configurations.

## Findings

### MAJOR — Extra magic registration (fixed)

**File:** src/org/minima/system/commands/CommandRunner.cpp.
The Java class is absent from the pinned registry. Native registration exposed
an extra command. Removed it; direct tests instantiate the real class explicitly,
while public dispatch remains unavailable.

### MAJOR — Wrong stored-number exception category (fixed)

**File:** src/org/minima/utils/JsonDB.cpp.
A stored Boolean reached native variant access instead of Java’s String cast.
The existing type predicate and JavaClassCastException now precede MiniNumber
construction; null still returns the supplied default.

### MAJOR — Eager null-coin dereference skipped a query (fixed)

**File:** src/org/minima/database/MinimaDB.cpp.
The SQL-to-chain bridge dereferenced every result during copying. Reusing Coin’s
private null handle preserves the source’s later consumer failure and two lookups.
Friendship is limited to MinimaDB; the public Coin API is unchanged.

### MAJOR — Empty relevance loop dereferenced null (fixed)

**File:** src/org/minima/database/txpowtree/TxPoWTreeNode.cpp.
The simplified loop accessed coin fields with no relevant entries. The source
nested loop now controls dereferencing and retains the SQL read and first-match
break. The comparison’s empty-inner-loop example passes after this change.

## Verdict

Approve this scoped increment with limits. The lab retains failing evidence and
explains the fixes. Java replaced-list aliases, concurrent map iteration and
complete runtime/durable SQL parity remain open. The full 1:1 port is incomplete.

---

# Latest review — TxPoW search command

## Summary

txpow composes real RAM/SQL, on-chain, archive and chain searches, retaining source
branch priority and result shapes. It uses the existing command runner for MySQL
delegation. Both full builds and all 12 selected checks pass. No source/native
production discrepancy was found in the 680-program command comparison.

## Findings

### MINOR — Burn input missing from independent address expectation (fixed)

**File:** tools/txpow_search_corpus.py.

The source address helper checks burn inputs as well as main transaction coins.
One fixture coin belongs to address 0x01, so its TxPoW also appears in the result.
Both implementations included it. The explicit expected ID/order assertion now
includes that object; no search implementation was changed.

### MINOR — MySQL recorder needed the existing registry replacement pattern (fixed)

**Files:** support/WalletCommandsOracle.java and support/wallet_commands.cpp.

Java's registry array is final. The fixture now reuses scoped element replacement
and restoration, as the existing send recorder does. Native test support appends
a scoped mysql prototype because the real native command is still absent, restoring
the original registry on every exit. The actual command runner remains in use.
Forwarded calls are included in the test envelope for these programs.

### MINOR — Fallback scope needed independent assertions (addressed)

**File:** tools/txpow_search_corpus.py.

Review added status:false/found:true and missing block/onchain cases with MySQL
preferences enabled. The txpowid branch returns the nested txpow without checking
status, and the other two branches do not delegate. Assertions verify both rules.

## Validation

Both full builds93598/38830 pass without warnings. Initial677 programs2090ops41337SQL
passed10.93s/25.58s. Final680/2099ops41510SQL passed10.28s/26.97s.
All12checks39590/55967 passnormal58.91s/sanitizer156.18s; all22reportsread.
Other groups: wallet675,history484,ArchiveManager3333/10831ops/125655SQL,
TxPoWDB1355/7407ops/80960effects,OnChainDB855/3639ops/29167SQL,SqlDB464/1260ops/
8352SQL,searcher189/6468queries,command12289,runner1265,runnererrors1120,map340.
Not all178 CTest entries. No native production or compiled fixture edits after final full builds; later
corpus additions were rerun.

Optional prepared-query rowsBySQL routing extends the existing recording drivers;
ordinary rows and all recorded effect identities remain available. Actual SQL
bodies and decoders run. Exact serialized reply strings preserve JSON order and
ordinary exception-like text. Existing runtime diagnostic normalization remains.

## Verdict

✅ Approve with suggestions for this scoped implementation. The340 counterparts
remain partial and117 are absent. Native mysql, Main/Minima and durable SQL are
unfinished. Recording SQL and a controlled downstream command do not prove a
working MySQL integration, live chain, full node, retained-reference mutation or
exhaustive concurrency/platform parity.

---

# Latest review — transaction history

## Summary

history reuses actual SQL, wallet relevance, Coin/Token scaling and Java collection
adapters. The source’s action precedence, parameter conversion, default time,
chain counting and serialized result order are preserved in the exercised cases.
Both final full builds pass without warnings. All ten selected checks pass across
validation phases. This is a scoped port review, not a complete-node approval.

## Findings

### MAJOR — Public helper argument was unnecessarily const (fixed)

**Files:** src/org/minima/system/commands/search/history.hpp and history.cpp.

Java permits an override to modify its TxPoW argument. The initial native signature
prevented ordinary mutation. It now accepts TxPoW*. A source/native override sets
height to 99; the observed object changes while the already-serialized reply keeps
height 20. The final 484-program history suite passes after this correction.

### MINOR — Independent total checks needed one-sided and repeated coins (fixed)

**File:** tools/transaction_history_corpus.py.

Review added input-only/output-only tokens, repeated coins, negative and zero
amounts, a disjoint token union and independently computed totals across Hashtable
resize. Exact serialized comparisons additionally observe Java collection order.

## Validation

Final full builds and history: 484 programs, 1,489 operations, 52,194 SQL calls;
5.46s normal/10.04s ASAN+UBSAN. Earlier 483-program suite passed5.59s/10.10s.
Nine additional selected checks: wallet675, balance275, coins287, command12289,
runner1265, runnererrors1120, HashMap135, TxPoWSqlDB2592/8729ops/86333SQL, map339.
Normal nine-check batch passed158.81s. Sanitizer batch passed8/9 in160.85s, with
SQL timing out at its existing120-second limit. A separate full SQL driver run
passed; it is recorded separately, not as a successful original CTest batch.
All18 final comparison reports read. Not all177 CTest entries. Source Java
compilation still prints its pre-existing deprecation/unchecked warnings.

Default wall-clock values are checked against real before/after bounds and SQL
bindings before normalization. Explicit times and other reply bytes remain exact;
existing runtime exception prose normalization is retained. Source-generated wire
and recording SQL boundaries remain explicit. No database engine was substituted.

## Verdict

✅ Approve with suggestions for this scoped implementation. The 339 counterparts
remain partial and118 are absent. Main/Minima/durable SQL remain unfinished.
Retained JSON aliases, arbitrary nullable Coin collections and exhaustive Java
reference, concurrency and platform equivalence remain unverified.

---

# Latest review — coin-search command

## Summary

coins composes the actual searcher, wallet cache, miner/RAM reservations and
send selector. It preserves the source's presence-based relevance and ordering
rules. Both full builds pass without warnings; all nine selected checks pass
per build in61.55s/193.68s. All16 comparison reports were inspected. No production
body discrepancy was found in the 287-program command group.

## Findings

### MINOR — Blank state was incorrectly expected to yield a result (test fixed)

**File:** tools/coin_search_corpus.py.

Both languages rejected supplied state:"" through Command.getParam. The initial
assertion expected a JSON result. It now asserts the exact blank-parameter error,
while independent matching cases check nonempty state searches. The production
getter and command were unchanged.

### MINOR — Shape-only state checks could pass on an empty result (addressed)

**File:** tools/coin_search_corpus.py:validate.

Review added exact expected IDs for state0, state1, hex-state and missing matches.
Two additional RETURN FALSE token cases also establish that coins sendable:true
checks simple addresses without the token-script restriction used by balance.
These additions make the documented distinctions independently reviewable.

## Validation

Initial285/1136 operations/24992SQL passed11.30s/27.70s. Final287/1144/25168SQL
passed20.89s/41.27s. Other selected groups pass balance275, wallet675, funding736,
searcher189/6468queries, command12289, runner1265, runnererrors1120 and mapping338.
All9 checks pass in each build, not all176 entries. Full builds88906/43551 pass
without warnings, with no later production edits. Existing normalization and
wallet fixtures are unchanged; only the driver adds this corpus group.

## Verdict

✅ Approve with suggestions for this scoped implementation. Full release parity
remains unverified:119 counterparts are absent, and native Main/durable SQL are
unfinished. Fixture reservations and SQL rows do not establish live node behavior,
atomic snapshots or complete Java reference and concurrency equivalence.

---

# Latest review — balance and arithmetic overflow text

## Summary

balance reuses real search, wallet cache, token arithmetic and JSON parsing. Its
275-program comparison includes exact serialized replies, even malformed source
JSON, and 24 direct number-constructor probes. Both full builds pass without
warnings. Thirteen distinct selected checks pass per build across phases, and
all 24 comparison reports were inspected. The full node remains incomplete.

## Findings

### MAJOR — Arithmetic constructor checked range before plain conversion (fixed)

**File:** src/org/minima/objects/base/MiniNumber.cpp:317.

The scale-44 balance case reported Java's overflowing amount as a plain integer
but native code as scientific notation. The coefficient/scale constructor called
checkLimits before the negative-scale conversion corresponding to BigDecimal's
toPlainString. It now expands only zeros retained by the existing 64-digit
precision before context/range checks. This preserves the representation without
an exponent-sized allocation. The String constructor remains distinct. Twenty-four
probes cover signs, overflow, zero, fractional zeros and precision boundaries.
The failing baseline is build/wallet-balance-range-baseline.json.

### MINOR — A supplied blank address is not an omitted address (test fixed)

**File:** tools/wallet_balance_corpus.py.

The first MegaMMR expectation supplied address:"" for the no-address branch. Both
implementations correctly rejected it through the existing Command getter. The
fixture now omits that key when testing wallet-wide search and separately keeps
explicit blank rejection. No production behavior was changed for this finding.

### MINOR — Generic exception normalization could erase token data (prevented)

**File:** tools/wallet_balance_corpus.py:preserve_text.

The reused SQL normalizer strips runtime NPE/cast prose from strings. Serialized
replies and coin debug logs can contain these words as token names. Those exact
raw observations are restored before comparison; a deliberately exception-like
name tests this boundary. Escaped exception fields retain the established prose
normalization. Raw names and reply structure are not repaired or normalized.

## Validation

Balance275/776 operations/21515 SQL passes in11.57s/37.07s. The normal11-check
regression batch passes in60.64s; sanitizer12-check batch including balance passes
in193.38s. Separate primitive runs pass3802 cases in1.48s/2.54s. Other groups:
consolidation247, wallet675, funding736, command12289, runner1265, errors1120,
objects9464, originalNumberValue99 assertions plus11 errors, searcher189/6468queries,
KISSVM4471 and mapping337. Thirteen distinct checks per build, not all175 entries.
The final full builds65913/95723 pass without warnings; no subsequent production edits.

## Verdict

✅ Approve with suggestions for this scoped implementation. No unresolved blocking
defect was found in the changed paths. Full release parity remains unverified:
120 counterparts are absent; Main and durable SQL are unfinished. Native result
coin-list values do not certify arbitrary Java aliasing or serialization timing,
and bounded number conversion does not certify all resource-exhaustion behavior.

---

# Latest review — wallet consolidation and runner status

## Summary

The command reuses send ordering, real coin search, wallet address lookup and
CommandResult. All247 programs/742 operations/23185 SQL calls match the pinned
Java in both builds. Both full builds and all8 selected checks pass. All14
comparison reports were inspected. Main and durable SQL remain absent.

## Findings

### MAJOR — Returned status used a generic C++ variant cast (fixed)

**File:** src/org/minima/system/commands/CommandRunner.cpp.

The first consolidation comparison stopped at program167: a missing child status
raised Java NullPointerException but native RuntimeException: bad_variant_access.
The post-execution status check now reuses CommandResult.status outside the
existing catch region. Four direct missing/null/String/integer status cases and
nested public dispatch distinguish unboxing/cast failures from command errors.
The original failing report is build/wallet-consolidation-status-baseline.json.

### MINOR — Real dry-run wrapper drops the outer dryrun marker (addressed)

**File:** tools/wallet_consolidation_corpus.py.

The scoped normalizer checks the successful, real consolidate dry-run path and
32-byte PRNG shape before replacing this one random field. Controlled replies
are not normalized. Other fields, SQL calls, logs and stored bytes are compared.
Recording send tests use a scoped prototype restored after every operation;
separate real-send cases exercise the existing proof/mining fixture. They do
not establish durable persistence or network acceptance.

## Validation and verdict

Final normal/sanitizer comparison times: consolidation17.06s/34.49s,
wallet675programs1907operations50275SQL, funding736/3418/39016SQL,
wrappers659/2060calls, command12289, runner1265, runnererrors1120 and mapping336.
All8 selected checks pass in58.53s/191.87s; not all174 CTest entries.
Both final full builds90631/77313 pass without warnings. The pre-test lab and
file lessons cover the discovered discrepancy and public permission checks.

✅ Approve with suggestions for this scoped implementation. Full release parity
remains unverified;121 counterparts are absent and all336 remain classified partial.

---

# Latest review — custom-address consolidation

## Summary

consolidatefrom reuses real coin search, tree storage, token arithmetic and RAM
mempool lookup. Its public helper remains a separate algorithm, preserving input
order, one output and nullable JSONObject return. Both full builds pass without
warnings. All eight selected checks pass per build (22.25s/65.92s), including
331 consolidation programs and 3,815 child calls. All fourteen comparison reports
were inspected after completion.

## Findings

### MINOR — Boundary coverage needed more than three coins (addressed)

**File:** tools/consolidate_address_corpus.py.

A three-coin fixture cannot distinguish the default 50 limit from no limit.
Review added 55-coin inputs for default50, explicit51 and the unlimited helper,
plus fixed-precision division of the smallest unit into ten zero outputs.
Independent assertions check selected IDs, order, counts and output text.
The public helper is virtual, matching Java's overridable method.

### MINOR — Orchestration evidence has explicit integration limits

**File:** support/WalletWrappersOracle.java and support/wallet_wrappers.cpp.

Actual chain/MegaMMR search, age and mempool filtering run in both languages.
The fixture uses source-supported test tree nodes and controlled proof/sign/post
replies. Selected real input/output/script commands produce compared row bytes.
This proves those scoped observations, not a funded or accepted network transfer.
Native Main, durable SQL, arbitrary collection mutation and complete runtime
integration remain unfinished or unverified.

## Validation

Initial327 programs/3603 calls passed both builds. Final331/3815 pass in
12.81s/22.38s. The other selected groups pass custom492, wallet659, raw656,
command12289, runner1265 and runnererrors1120; mapping confirms335 counterparts.
All eight checks pass in each build, not all173 CTest entries. No normalization
was broadened in this batch. The pre-test lab and updated file lesson explain
source behavior, including retained rows, ignored statuses and the distinct
helper response cast after deletion.

## Verdict

✅ Approve with suggestions for this scoped implementation. No unresolved
blocking defect was found in the changed body. Full release approval remains
withheld: 122 counterparts are absent and full behavioral parity is unverified.

---

# Latest review — custom-address sending and token creation

## Summary

sendfrom and createtokenfrom preserve the pinned source's parameter order,
child-command sequence, typed status/error checks and cleanup timing. They reuse
existing number, state, transaction, token, database and command implementations.
Both full builds pass without warnings. All ten selected checks pass per build
(40.23s normal, 124.63s ASAN/UBSAN), including 492 custom-wrapper programs and
2,052 observed child calls. All eighteen comparison reports were inspected.

## Findings

### MAJOR — Wrong exception for out-of-range powers (fixed)

**File:** src/org/minima/objects/base/MiniNumber.cpp:498.

Negative decimal counts expose the existing exponent guard. Java throws
ArithmeticException with Invalid operation; native code previously threw a
standard out_of_range error classified as RuntimeException. The preserved
custom-send-error-baseline.json shows both failing inputs. The guard now reuses
JavaArithmeticException, leaving the arithmetic algorithm unchanged. Four direct
signed boundary probes join seven prior arithmetic observations; all eleven match
exactly alongside the 99 original NumberValue assertions in both builds.

### MINOR — Runtime prose carried inside command errors (comparison scope)

**File:** tools/wallet_wrappers_differential.py.

Real offline posting has no chain tip. Java and native runtimes describe the
resulting null-pointer dereference differently, and the wrapper propagates that
text in CommandException. The normalizer now retains the two exception class
names while excluding their runtime-specific prose in this exact exception-field
shape. It does not establish byte-identical diagnostic wording across runtimes.
Nullable errors and embedded-NUL errors still have independent exact assertions.

## Validation and scope

The fixture supplies explicit ordinary outputs through real txnoutput commands;
it observes actual stored output replacement, token metadata/hash, row wire bytes
and preserved change. Controlled funding and posting replies establish wrapper
behavior only. Real-child cases preserve source spacing after fromaddress: and
observe an empty output list or offline chain-tip failure. A real signing reply is
controlled in the new sendfrom offline case; earlier wrapper signing tests still
run and verify actual signatures. No live wallet or network posting occurred.

Selected checks: source mapping 334, custom wrappers 492, wallet wrappers 659,
raw wrappers 656, command 12,289, runner 1,265, runner errors 1,120, object 9,464,
primitive 3,802, and original NumberValue 99 plus eleven supplemental observations.
These are ten selected checks, not all 172 CTest entries. Only known random IDs and
specified runtime diagnostics are normalized. The pre-test lab and affected
student lessons describe observed behavior and remaining limits.

## Verdict

✅ Approve with suggestions for this scoped batch. No unresolved blocking defect
was found in the changed wrapper bodies. Full release approval remains withheld:
123 counterparts are absent, existing counterparts are partial, and native
Main/Minima, durable SQL and complete node integration remain unfinished.

---

# Latest review — raw transaction wrappers and Unicode state values

## Summary

rawfrom and rawtxnfrom reuse the existing command runner, transaction commands and
codecs. Shared helpers retain identical output/state logic while wrapper-specific
input/script order stays explicit. Both final full builds pass without warnings.
Final raw comparison: 656 programs, 3,991 child calls per build, 4.06s/4.55s.
Final object regression: 9,464 cases per build, 25.10s/91.34s.

Before the focused state correction, all seven extraction checks passed per
build: raw 632, wallet wrappers 659, funding 736 (3,418 operations/39,016 SQL calls),
command 12,289, runner 1,265, runner errors 1,120 and source mapping 332. Those
batches took 88.75s/271.44s. The state correction was checked by the expanded raw
and object groups. This is eight distinct selected checks across phases, not all
171 tests. All fourteen distinct comparison reports were read.

## Findings

### MAJOR — ASCII-only state Boolean comparison disagreed with Java (fixed)

**File:** src/org/minima/objects/StateVariable.cpp.

The source uses equalsIgnoreCase. It recognizes the long-s spelling falſe as
FALSE; the native implementation instead attempted numeric parsing and failed.
The preserved source/native baseline reproduces the discrepancy. The constructor
now reuses JavaStringIgnoreCase.hpp, already used by vault and InetSocketAddress.
Twenty-four added cases compare Boolean normalization and real exported state.
The object regression checks existing wire/JSON behavior after the change.

### MAJOR — Full native runtime and broader API parity remain incomplete

**Files:** source-map.json; src/org/minima/system/commands/send/wallet/rawfrom.hpp;
src/org/minima/system/commands/send/wallet/rawtxnfrom.hpp.

There are 332 counterparts and 125 absent files, all partial/not_verified. Main,
Minima and durable SQL are absent. The native string/object value APIs do not
represent every nullable Java argument, return or Map/reference identity. The
fixture proves scoped construction/cleanup and bytes, not funded-chain acceptance.

**Fix:** complete remaining APIs, runtime ownership, storage and node integration
before claiming full-port parity or release readiness.

## Verdict

Approve with suggestions for the scoped wrappers, extraction and Unicode fix.
Full release remains unapproved. Review checked all new wrapper/header files,
RawTxnCommands, StateText, CommandResult extraction, StateVariable, registry/build
entries, both fixture extensions, driver/corpus and teaching updates. Null/type
failure timing, insertion order, Java string concatenation, port-before-value
conversion, ignored child status and cleanup match the stated source experiments.
No algorithm was substituted for the child commands; no live wallet/node or
external deployment was used.

--- Previous reviews (historical counts follow) ---

# Latest review — wallet orchestration and complete error messages

## Summary

Four wrappers now preserve the pinned child-command sequences and cleanup rules.
Both full builds pass without warnings. The final comparison passes 659 programs
and 2,060 child calls per build (normal 14.29s, ASAN/UBSAN 58.31s). All five selected
checks pass per build: wrappers, command 12,289, runner 1,265, runner errors 1,120,
and source mapping 330. All eight comparison reports were read. This is not all
170 registered tests. Student lessons and the pre-test lab explain the evidence.

## Findings

### MAJOR — Receiver checks changed Java argument evaluation (fixed)

**Files:** src/org/minima/system/commands/send/wallet/postfrom.cpp;
src/org/minima/system/commands/send/wallet/signfrom.cpp.

When decoding returned null and id was numeric, Java failed casting the argument
before invoking setID; the initial native expression failed on the receiver first.
Both wrappers now parse id into a local before checking/dereferencing the row.
The preserved baseline reproduces the discrepancy, and 14 crossing cases pass.

### MAJOR — Embedded NUL and null exception messages were lost (fixed)

**Files:** src/org/minima/system/commands/CommandException.hpp;
src/org/minima/system/commands/CommandException.cpp;
src/org/minima/system/commands/CommandRunner.cpp.

A child failure can supply no error message. It must produce a CommandException
with a null message and runner error:null, without generic exception logging.
Also, constructing the returned message from what() truncated embedded NUL text.
The exception now stores the complete optional string; the runner uses getMessage.
Direct and public-runner cases compare null, empty, Unicode and embedded-NUL text.

### MINOR — Child result type errors needed explicit Java categories (fixed)

**File:** src/support/CommandResult.hpp.

Empty arrays initially produced a vector error; missing/wrong status values
produced bad_variant_access. The shared helper now preserves IndexOutOfBounds,
NullPointer and ClassCast categories, and rejects null/wrong child error values
in the source order after cleanup. Explicit errors remain compared; NPE/CCE
runtime prose is normalized as in the existing comparison infrastructure.

### MAJOR — Complete runtime and broader APIs remain unfinished

**Files:** source-map.json; src/org/minima/system/commands/send/wallet/*.hpp.

There are 330 counterparts and 127 absent files, all still partial/not_verified.
Main/Minima and durable SQL remain absent. The fixture executes actual commands,
rows, codecs and signing, but does not prove funded-chain posting or network
confirmation. Native value returns do not represent every nullable Java String/
JSONObject or reference-identity API. Source createfrom’s tested empty export is
preserved and explicitly not presented as a valid funded transfer.

**Fix:** finish lifecycle/storage and complete API/runtime integration before release.

## Verdict

Approve with suggestions for the four wrappers and scoped exception fixes; full
port release remains unapproved. Review inspected every new wrapper/header,
shared helper, exception changes, registry/build entries, complete Java/native
fixtures, corpus/normalizer/assertions and lesson updates. Prototype substitution
is restored by finally/RAII, and real export readers verify full consumption,
roundtrip bytes and a cryptographically valid decoded signature. No live node,
wallet, commit, push or deployment was performed.

--- Previous reviews (historical counts follow) ---

# Latest review — sendpoll command and queue integration

## Summary

The new wrapper preserves the pinned branch and failure order while reusing the
existing manager unchanged. Both full builds pass without warnings. The new Java
comparison passes 367 programs and 3,216 operations in normal and ASAN/UBSAN builds
(6.75s/8.54s). All six selected checks pass per build, including actual native
worker/timer and command/runner/error regressions. All eight reports were read.
The regression batches took 13.69s/25.31s. This is not all 169 registered tests.

## Findings

### MAJOR — Full native runtime ownership remains unfinished

**File:** src/org/minima/system/commands/send/sendpoll.hpp:9.

The command exposes a Main-manager receiver callback, matching the existing
service approach. Its default reports a missing Main. Tests bind an actual dormant
manager; this is not evidence of a complete native node. Main/Minima and durable
SQL remain absent (326 mapped counterparts; 131 absent).

**Fix:** complete runtime ownership and lifecycle before claiming full-port parity.

### MINOR — Exception prose and broader worker failures remain partial

**Files:** src/org/minima/system/commands/Command.cpp;
src/org/minima/system/commands/sendpoll/SendPollManager.cpp.

All 88 initial differential mismatches were generic native parameter-cast prose
versus Java class/loader details. The comparison now explicitly checks NPE/CCE
types while excluding their prose. Existing manager null-message/null-stored-UID
and malformed-status native exceptions also remain broader API gaps; this batch
does not change those previously scoped worker behaviors. Timer delivery, logs,
live wallets and exhaustive thread schedules are outside the new comparison.

**Fix:** resolve these API differences in the broader parity audit.

## Verdict

Approve with suggestions for the scoped command addition. The full-port release
remains unapproved. Review covered both command files, registration, complete Java
and native fixtures, corpus/normalizer/assertions, source mapping and teaching
updates. The fixtures use actual command/worker bodies and restore native bindings
on exit. No wallet, node, commit, push or deployment was performed.

--- Previous reviews (historical counts follow) ---

# Latest review — unsigned transaction creation

## Summary

sendnosign reuses the working selector, recipient parser, wallet, proof, generator
and file components while retaining its own pinned control flow. Normal comparison
passes 309 programs, 1,248 operations and 20,081 recorded SQL calls per build
(normal 28.74s, ASAN/UBSAN 94.27s). Both full builds pass without compiler warnings.
All seven selected checks pass per build, after rerunning only the corrected
zero-total assertion group. All twelve comparison reports were inspected.

## Findings

### MAJOR — Complete native startup and storage remain absent

**Files:** source-map.json; src/org/minima/system/commands/send/sendnosign.cpp.

The new command works through existing native services and test fixtures. The
project still lacks native Main/Minima startup and a durable SQL engine. There
are 325 mapped counterparts and 132 absent files. Recorded SQL and a queued
transaction cannot establish durable signing counters or network acceptance.

**Fix:** finish remaining lifecycle, storage and integration before release.

### MINOR — Broader source APIs remain unverified

**File:** src/org/minima/system/commands/send/sendnosign.hpp.

AddressAmount stores native values; arbitrary nullable constructor arguments and
Java reference identity are not represented by those value fields. Tests exercise
ordinary command construction and outputs, not every public Java API, concurrent
global mutation or platform filesystem case.

**Fix:** complete those adapters and dedicated checks before full parity approval.

### Fixed — Independent zero-amount expectation was incorrect

**File:** tools/unsigned_send_corpus.py.

The first extra assertion expected output for total zero. Both implementations
actually reject that total before output creation. The corrected check asserts
the specific rejection and separately exercises a zero-valued individual recipient
in a positive-total multi-send. No production behaviour was changed to satisfy
the mistaken expectation. The observation is saved with both source/native results.

### Verified design — Randomness does not hide transaction bytes

**Files:** support/WalletCommandsOracle.java; support/wallet_commands.cpp;
tools/unsigned_send_corpus.py.

Both observers use actual header/TxPoW readers to identify the PRNG slice. The
normalizer checks its length and exact match to decoded JSON before replacing
those 32 bytes. Remaining bytes are compared, including proofs and signatures.
The complete create/view/sign/verify/post flow uses these actual saved files.

## Verdict

Approve with suggestions for this scoped translation and test batch. Complete
node release remains unapproved until the missing lifecycle/storage and remaining
API work are finished. No merge, push or deployment was performed.

--- Previous verified review follows ---

# Latest review — offline transaction files

## Summary

sendview, sendpost and sendsign reuse the existing TxPoW codecs, wallet, script,
witness, vault and asynchronous miner. The normal comparison passes 201 programs,
558 operations and 9,773 recorded SQL calls, including the demonstration main.
Both full builds pass without compiler warnings. All twelve selected checks pass in each build (normal 133.68s, sanitizer 515.77s).
All twenty-two comparison reports were inspected; source mapping also passes.

## Findings

### MAJOR — Native startup and durable storage are still missing

**Files:** source-map.json; src/org/minima/system/commands/send/sendpost.cpp.

The explicit miner receiver permits isolated execution but does not supply Main
startup or a durable native wallet database. 324 counterparts exist; 133 remain
absent. The recording SQL fixture checks requests and effects, not persisted
one-time-key counters or an operating network node.

**Fix:** complete the remaining native lifecycle, storage and integration work
before calling this a working full Minima implementation.

### MINOR — Full stream and reference behaviour remains outside this batch

**Files:** src/org/minima/utils/MiniFile.cpp; src/org/minima/utils/MiniFile.hpp;
src/org/minima/objects/Witness.hpp.

The new comparisons cover selected regular files, malformed paths, directories,
append/replace and creation/open failures. They do not certify file races,
symlinks, all permissions or large-file/stream behaviour. Older whole-file
loadObjectSlow/saveObjectDirect and std::clog wrappers retain documented
source differences. Witness vectors still do not supply all Java object aliases.

**Fix:** finish those adapters and dedicated stream/ownership checks before
certifying complete per-file parity.

### Fixed — File errors must preserve operation order

**File:** src/org/minima/utils/MiniFile.cpp.

Missing-file reads and writing below a regular-file parent initially produced
generic C++ stream exceptions. The corrected code reuses JavaUnixFile helpers,
checks narrowed file length before opening, and follows exclusive creation then
output opening. Typed IOException/FileNotFoundException preserve observed
messages. The output-parent fixture independently requires a signing write to
have occurred before the file error. Failed baseline reports are retained.

### Fixed — Test expectations confused signing with witness insertion

**File:** tools/offline_send_corpus.py.

The source leaves reqsigs empty, so it signs every input. Witness separately
rejects duplicate roots. The corrected fixture asserts key-use write counts and
verifies retained signatures independently. It does not change source behaviour
to match an intended optimisation. The lesson records the mistaken assumption.

## Verdict

Approve with suggestions for this scoped translation and test batch. Complete
node release remains unapproved until the missing lifecycle/storage and remaining
API work are finished. No merge, push or deployment was performed.

--- Previous verified review follows ---

# Latest review — coin selection, transaction funding and send

## Summary

Three native command counterparts reuse the existing wallet, proof, generator,
key and mining components. The source's address grouping, reserved-coin fallback,
mutation before failure, token scaling and dry-run burn signing are preserved.
The command comparison passes 736 programs in both normal and ASAN/UBSAN builds.
All twelve selected checks pass per build, including the original REV/LEN mappings
and shared parser/generator regressions. The map was regenerated after the two
test files were added; its corrected check passes. All reports were inspected.

## Findings

### MAJOR — Complete native startup and durable storage remain absent

**Files:** source-map.json, src/org/minima/system/commands/send/send.cpp.

The commands call the existing explicit miner receiver. They require native Main
startup and the native SQL engine before the project can operate as a complete
node. Synthetic SQL rows and dormant miner fixtures do not supply those systems.
321 counterparts exist and 136 remain absent; full parity stays not_verified.

**Fix:** complete the remaining node/lifecycle/storage code and integration.

### MINOR — Broader Java API surfaces remain unverified

**Files:** src/org/minima/system/commands/send/send.hpp,
src/org/minima/system/commands/txn/txnaddamount.hpp.

The native helper interface uses vector references and string IDs; arbitrary null
list/amount arguments, nullable lock IDs and replacing the public list object are
not covered. AddressAmount uses native values; the standalone Java demo main is
not yet exposed. JSON byte-size reports use string keys, so generic MiniData-key
lookup/identity is not supplied. The tests establish the tested command paths,
not every possible Java API call or concurrent invocation.

**Fix:** finish these API adapters and dedicated edge/ownership tests before
certifying complete per-file parity.

### Fixed — Shared array casts and mempool rejection logs

**Files:** src/org/minima/system/commands/Command.cpp,
src/org/minima/system/brains/TxPoWGenerator.cpp.

The new corpus reproduced a wrong exception category for non-array multi and
missing generator work/size rejection logs. Both use existing native helper
patterns now. Failed reports are retained in cpp/build. The source generator's
capacity-removal and caught-exception logging still require separate review.

## Verdict

Approve with suggestions for this scoped translation and test batch. The code
reuses the reviewed components and preserves the tested source observations.
Full node release and complete per-file parity remain unapproved until the
major integration work and remaining API validation are finished.

--- Previous verified review follows ---

# Latest review — vault/signing, wallet keys and original bit tests

## Summary

The new vault and txnsign commands reuse the actual wallet, encryption, key,
witness, transaction and posting code. Source operation order is preserved,
including effects retained after failures. Normal comparison passes 247 programs,
800 operations and 16,275 recording SQL calls. The new original BITGET/BITSET
mappings pass all four registered tests in normal and ASAN/UBSAN builds. Both full
builds pass without compiler warnings. The sanitizer vault retry also passes all 247 programs. All 18 selected regressions pass in both builds.

## Findings

### MAJOR — Full native lifecycle and persistence remain unfinished

**Files:** src/org/minima/system/commands/backup/vault.hpp, source-map.json.

The explicit Main receiver must be bound by native startup. The test receiver
records lifecycle calls but does not execute Main shutdown/restoration. The SQL
fixture records real wallet SQL calls and returns supplied rows; it is not a
native H2/MVStore engine. Full Main/Minima and durable storage remain absent.
Current mapping has 316 counterparts and 141 absent files, all parity unverified.

**Fix:** finish the native lifecycle/storage implementations and node integration
before treating reset, restore or signing as a complete operational native node.

### MINOR — Reference lifetime and interruption gaps remain

**Files:** src/org/minima/objects/Witness.hpp,
src/org/minima/system/commands/backup/vault.cpp.

Nullable signature insertion now rejects null correctly, but signatures are held
as values and do not reproduce every Java alias/list mutation. stopAllKeysCreated
performs the source two-second delay without a Java interruption model. Direct
TreeKeyNode constructor edge categories still need full parity review.

**Fix:** extend owner/list and interruption APIs with source comparisons during
full-file completion; the present tests do not certify these behaviors.

## Corrections checked

- Negative custom key use exposed RuntimeException: vector; actual array accesses
  now use the existing Java bounds exception with the source index/length message.
- TreeKey default logs now use MinimaLogger. The existing object harness captures
  signing logs consistently with ObjectOracle rather than changing production logs.
- A supposed successful automatic-post fixture failed in both languages because
  its chain was too short. That rejection is retained; successful cases reuse the
  existing twelve-block chain. The command implementation did not change for it.
- The sanitizer vault run exceeded the shared harness's 30-second response limit
  during full-size custom signing. The runner now accepts an optional timeout;
  vault selects 180 seconds, with no relaxed behavioral assertions.
- BITGET/BITSET native tests preserve every active source input/expectation. The
  original BitSet loops run 63 positions, and the mapped test uses independent
  byte patterns. No production bit algorithm changed.

## Wallet-key follow-up

The newly mapped keys command reuses actual key/seed checking and generation.
Its checkKey helper is kept distinct from checkkeys. The initial comparison found
a null String receiver mismatch in filtered lists; an explicit null check now
preserves the Java exception before field comparison. The 132-program corpus also
uses a source-derived consistent seed/modifier/private/public record, a mismatched
modifier, mixed public-key checks and exact MAIN_LOAD_ALL_KEYS payloads. Both
corrected full builds pass. The final normal and ASAN/UBSAN comparisons both pass 132 programs, 262 operations and 7,895 SQL calls.
No source algorithm was replaced. The untested random genkey branch and absent
Main/H2 integration remain limitations.

## Verdict

Approve the scoped translation and tested behavior. Both full builds and all 24 selected distinct CTest entries pass in both builds; all 42 comparison reports were inspected. The full 161-entry suite was not rerun.
This is not approval of a complete C++ node. No merge, commit or deployment was made.

--- Earlier reviews follow ---

# Latest review — transaction input and witness construction

## Summary

Three additional mapped production files compile: txninput, txnbasics and txnutils.
The two commands use actual custom transaction ownership and preserve source
selection/mutation order. The utility reuses search, MMR, wallet, signing and ID
code, including all burn-construction branches. The numeric syntax repair lives
in the shared MiniNumber path. The normal and sanitizer input comparison both
pass 671 programs. Both builds pass all 13 selected regressions. This is a scoped review, not full-port approval.

## Findings

### MAJOR — Native startup and durable database execution remain absent

**Files:** src/org/minima/system/commands/txn/txnutils.hpp, source-map.json.

The burn helper's getMiner callback must be bound by native Main. Tests use the
actual miner membership check with a dormant native miner and an allocated Java
receiver; neither starts the node. SQL recording receivers cannot establish H2
persistence or restart behavior. There are 155 absent counterparts, and all
existing counterparts still have unverified full parity.

**Fix:** complete Main/Minima startup, bind the actual runtime owner, implement the
native storage backend, finish remaining files, and verify node-level behavior.

### MINOR — Reference/null and extreme numeric APIs remain outside this check

**Files:** src/org/minima/system/commands/txn/txnutils.hpp,
src/support/JavaBigDecimalSyntax.hpp.

The public native helper accepts non-null transaction/witness references. The
experiments do not exhaust null object arguments, all aliasing or concurrent
mutation. The new decimal scanner fixes syntax diagnostics, but the existing
numeric conversion still needs complete extreme-exponent/precision review.

**Fix:** extend source comparisons to these APIs before marking parity verified;
do not infer their coverage from the new command fixtures.

## Verdict

✅ Approve this scoped increment. Both full builds and all 14 selected checks
pass in each build; report bodies contain zero mismatches.
No unresolved discrepancy in the 671 input/witness/burn programs. Full goal remains
incomplete. Reviewed new production bodies and headers, shared parser change,
command registration/CMake changes, both harnesses, corpus/driver and teaching
updates. Earlier review records below describe their original checkpoints and
must not override newer verified evidence.

---

# Code Review

## Summary

This is an incomplete port with an exhaustive destination map, compiled
primitive and protocol object implementations and a separately compiled legacy C++ node.
The map distinguishes absent files from real implementations. Selected
cross-language differential tests pass; they are not evidence of full node parity.

## Findings

### CRITICAL — Existing node consensus differs from pinned upstream

**Files:** existing `src/validation/TxPoWValidator.cpp` versus upstream
`src/org/minima/system/brains/TxPoWChecker.java`.

The existing C++ proof-of-work check compares only the first hash byte. Its
signature check hashes the TxPoW ID; Java verifies the transaction ID with
TreeKey. The C++ code also permits empty signature-proof slots. These are
concrete incompatibilities, despite the existing project's parity claims.
Do not mark this candidate as a completed port of TxPoWChecker.

**Fix:** port the complete upstream validation sequence and dependencies;
exercise valid and invalid Java-generated transactions, signatures, proofs,
burn transactions, and block contexts in differential tests.

### MAJOR — Mapping is complete; implementation is not

**Files:** `source-map.json`, `src/`, `test/`.

163 of 457 mapped destinations do not exist. Existing implementations still
require complete API/behavioral review. JSON generic-array/container overloads,
Java UTF-16 string behavior, Unicode numeric parsing, extreme decimal scales,
reference aliasing and exact exception/logging behavior are not fully covered.
Selected successful fixtures do not resolve these gaps. MegaMMR/Cascade/TxPowTree file I/O now delegates to MiniFile; failure-side-effect
and persistence round-trip coverage still require completion. MMR database lookup
integration now retains ownership through MinimaDB fallback; full node integration
remains incomplete. Header date rendering uses the
host calendar and has not been verified against Java for historical dates.
Greeting creation now uses required runtime services; Pulse live creation remains absent. All KISS production classes now
compile; complete execution/API parity still requires review. Generic runtime
exception names/messages, Java reference aliasing, mutable charset configuration and all public
MiniString stream overloads, locale-sensitive case conversion and pathological resource
limits remain unresolved. The differential VM runner normalizes generic
runtime errors while comparing KISS errors and ordinary traces exactly. JSON stream overloads currently buffer
the input; memory use and incremental I/O semantics are not equivalent.
KISS case-insensitive tokenization still needs locale-sensitive Unicode review.

**Fix:** implement and verify the outstanding mapped files. Keep the full-port
check failing until the evidence establishes equivalence.

### MAJOR — Existing tests alone overstate coverage

**Files:** existing `tests/objects/test_txpow.cpp`; upstream
`test/org/minima/system/brains/TxPoWProcessorTest.java`.

The C++ TxPoW test is a `CHECK(true)` placeholder; both Java processor test
bodies are commented out. Test-suite pass counts do not establish coverage
of these behaviors. The added primitive cross-language tests are independent
of those placeholders.

**Fix:** use Java-generated fixtures and meaningful behavioral tests as
each mapped implementation is completed.

### CRITICAL — Existing WOTS algorithm is incompatible

**File:** sibling `src/crypto/Winternitz.hpp` and `tests/crypto/test_wots.cpp`.

The sibling treats w=8 as base 8, emits 2,880-byte signatures and public keys,
and derives chains from SHA3(seed || index). The pinned Java wrapper uses
Bouncy Castle's 8-bit parameter, 1,088-byte signatures, 32-byte public keys,
and GMSSRandom seed updates. Its self-consistency tests assert the wrong
constants. The new implementation follows the inspected BC source and passes
byte comparisons plus cross-verification against the pinned Java runtime.

## Corrected in the new primitive port

- Preserve zero's decimal scale on the wire and render numeric zero as `0`.
- Preserve tested remainder preferred scales through integral-quotient arithmetic.
- Enforce Java MiniNumber's range; preserve unsigned division magnitudes.
- Correct tested floor/ceiling results and exact-division scale.
- Treat significant-digits zero as unlimited precision.
- Preserve malformed UTF-8 replacement behavior in tested byte sequences.
- Preserve division precision for large unscaled operands.
- Compute square roots with unrestricted integer intermediates and preserve
  preferred scale for exact roots.
- Use guarded decimal intermediates for negative and positive powers.
- Compare MiniData numerically after removing leading zeroes for ordering;
  preserve byte-exact equality and Java's empty-data rendering.
- Use bounded spans for native decoding; reject truncated/negative lengths.
- Replace the inherited PRNG with OS randomness on this POSIX target.

## Newly translated boundaries still under review

- ChainServices and ConsensusServices require real runtime database, wallet and
  notification connections. The library is not a functioning replacement node.
- Native object values and weak tree parents do not yet reproduce every Java
  reference-aliasing and garbage-collection behavior.
- TimedChecker uses owned asynchronous inputs. The full 120-second timeout was
  exercised under sanitizers; process-exit coordination remains incomplete. Runtime callers must synchronize
  access to inputs while a timed-out worker continues.
- JavaConcurrentMap preserves quiescent iteration rules; its snapshot enumeration
  differs from concurrent Java traversal and must be resolved before full parity.
- MiniFile gzip helpers buffer whole files. Malformed-stream/IO-failure partial
  destination writes, directory lengths, and all Java exception/log semantics
  are not equivalent yet.
- RSA private-encrypt/public-decrypt Cipher modes and all JDK-dependent cipher
  state/lifetime behavior remain outside the existing interoperability coverage.

- MessageProcessor uses a native factory/start step after derived-object construction;
  its worker holds shared ownership through completion. C++ cannot asynchronously
  interrupt arbitrary handler code; a timed shutdown flag is separate from the join
  barrier. Timer targets use weak references to avoid ownership cycles, which can
  differ from Java getProcessor() after a stopped receiver loses all owners. Logging
  and real MinimaDB lock cleanup remain integration boundaries.
- Message HashMap adapter shares tested red-black internals with ConcurrentHashMap;
  null/string keys, ordinary/tree bins and ordering are compared against the JDK.
  Complete Java Map views, iterator fail-fast semantics and all Object boxes are pending.
- UserDB name normalization combines ICU with inspected JDK final-sigma word rules.
  Locale initialization, Thai boundaries and all mutable DateFormat operations are not
  fully matched. Extreme dates have UTC fixtures; other timezone extremes need review.
- TxnDB/TxnRow preserve row/list ownership and bind an actual UserDB instance. Runtime
  MinimaDB binding and full stream API/error behavior are under review. Typed wire I/O
  failures now distinguish malformed numeric values and preserve consumed input bytes.

- IBD requires IBDServices bindings for the live tree, cascade, archive and database
  locks. Its offline comparisons use in-memory Java fixtures with no node startup.
- MegaMMR backup/sync wrappers preserve shared lists and partial read replacement.
  Underlying MegaMMR/MMR value ownership and the full output-stream API remain gaps.
- SendPollManager requires a native CommandRunner callback and explicit shared
  construction before worker startup. Worker logic is tested with offline commands;
  no actual wallet sends or full node integration have been exercised.

- RawArchiveInput buffers one object plus gzip chunks and replays a static decoder
  when more bytes are required. Very large objects can repeat allocation/hashing;
  full Java stream overloads and all failure/reconnect resource states remain pending.
- Wallet rows use SqlRow to request typed columns. The H2 driver, its data conversion,
  persistent format and full Wallet are not implemented by this adapter.
- The sibling BIP39 seed implementation is incompatible with this pin: Java hashes
  phrase UTF-8 once with SHA3-256; the sibling chains hashes into a 64-byte result.
  The new translation follows Java. Locale initialization and all Java String
  reference/isolated-surrogate semantics remain wider API review items.

- FastByteArrayStream covers the pinned writeData behavior and common inherited
  byte-stream operations. Named/custom charset overloads and allocation-failure
  logging/GC behavior remain incomplete; writeData requires a single owning reader.
- NIOTraffic uses the tested Hashtable ordering extended to UTF-16 label hashes.
  Its reset clock has an explicit fixture adapter; non-English formatSize locale
  behavior remains unverified. RPC Authorizer requires a live UserDB callback.
- Command replies retain the actual parameter object; null/default/type behavior
  passes 12,289 command comparisons. Address conversion now exposes typed numeric errors
  and Java Mx error text. Full Java String/null-key API coverage is still pending.

- CommandRunner preserves the tested parser, duplicate parameter, permission and
  stop-on-error rules. The registry has 24 concrete command counterparts; it is not complete.
  Generic C++ exception messages do not yet reproduce every JDK exception class,
  null diagnostic or index error. The direct command oracle normalizes generic
  exceptions explicitly and does not establish that missing parity.
- `convert` uses checked JDK-style Base64 input rules and the existing byte/UTF-8/Mx
  representations. `random` uses OS randomness; tests verify byte counts, hashes
  and keycode construction independently. Rare 16-byte keycode substring failures
  retain a generic native diagnostic instead of Java's exact index message.
- `maths`, `mmrcreate`, `mmrproof`, `runscript` and `verify` reuse the tested KISS,
  MMR and TreeKey implementations. Underlying reference/stream/diagnostic gaps
  remain; command-level tests do not resolve those complete API gaps.
- Nine custom transaction commands use a required TxnDB runtime lookup. The
  standalone console binds a single in-memory database for its session. MinimaDB
  lifecycle, wallet, automatic saving and full node integration remain incomplete.
  Java/native file and HEX round trips and mutation ordering pass 1,129 sequences
  (6,904 operations). Tests use synthetic rows and dedicated build fixture files.
- Base32 encoding now adapts the existing BigInt decimal conversion loop to a
  byte-wise radix-32 divisor. It preserves the alphabet and numeric leading-zero
  handling while avoiding repeated general division; command and object suites
  pass in normal and sanitized builds after this change.

- TxPoWSearcher uses explicit tip, MegaMMR, wallet and transaction database bindings.
  In-memory fixtures exercise 6,468 queries in normal and sanitized builds. Chain
  token lookups currently copy Token values; imported token references are shared.
  Java's successful MegaMMR token lookup returns with its read lock still held;
  the translation preserves this observable behavior. Fixtures record and release
  the held locks after each query. Live runtime lock integration remains pending.
- tokens/txnoutput/coincheck/coinexport reuse the existing Token, Transaction and
  MMR proofs. Token import catches wire IO failures separately from runtime numeric
  errors, preserving Java's different import-list mutation. Coin proof history
  fixtures bind MMR lookup to the existing in-memory MegaMMR; this is test setup,
  not a replacement for live MinimaDB integration.

- Maths logarithms reuse the permissively licensed fdlibm sources from Netlib,
  with safe native bit access and floating-point contraction disabled. The
  3,809-case Maths/Stack/TimedRequest suite compares raw floating-point bits. Other JDK
  versions or JIT intrinsics may choose different Math approximations; this
  evidence applies to the tested pinned-source runtime. Stack shares the
  existing MessageObject API and its remaining general Java Object limitations.

- txnlock preserves polling in 100ms increments, the strict timeout comparison,
  and the delay before unlocking. Contention, timeout and delayed release checks
  pass in normal and sanitized builds. Its public native flag is atomic to avoid
  undefined behavior when the command reads it concurrently. Java thread
  interruption and shared runtime cancellation are not yet integrated.
  debugflag retains global mutable settings; runtime callers must synchronize
  access to the pre-existing GeneralParams fields.

- InetSocketAddressIO, P2PDoSwap and P2PWalkLinks preserve tested JSON shapes,
  numeric casts, partial read mutations and shared path/secret ownership. The
  address adapter uses POSIX resolution with the default JDK IPv4-first policy.
  DNS caching/TTL, custom network properties and complete InetAddress APIs are
  pending. Tests cover local numeric/invalid hosts, IPv6 scopes, unresolved-name
  equality and hashing; no live peer connection was made.
- P2PState and P2PGreeting reuse the existing HashMap bins and shared address
  collections. Address HashSet membership, first-object retention, ordinary-bin
  order and resizing are compared. Non-Comparable collision-tree order depends
  on JDK identityHashCode; declared collision cases compare membership. Native
  tie breaks use stable equality keys. State-generated greeting shuffles use
  OS-backed Fisher-Yates; their contents are compared as permutations, without
  claiming an identical Java Random sequence. Concurrent map/list mutation and
  iterator views remain open API items. P2PParams uses the actual pinned values;
  the P2PFunctions enum is held in a support header pending that class's port.
- P2PDB reuses JsonDB/MiniFile and preserves detached peer lists, failed-set
  behavior, version defaults, serialized bytes and tested partial reads. Its
  upstream 1,000-peer save/load test passes normal and ASAN/UBSAN. Native MinimaDB
  lifecycle binding remains pending. Java null static String parameters and all
  inherited stream/error/logging APIs remain wider review items.

- NIOClient/NIOClientInfo now translate the pinned 8KiB buffers, 256MiB limit,
  queued partial writes, read compaction, empty/null frames, traffic and message
  posting. 714 comparisons pass normal and ASAN/UBSAN, including an actual
  256MiB+1 outgoing allocation and failure-side buffer snapshots. The Java harness
  bypasses Main/manager startup with test-only Unsafe allocation; it runs the
  actual NIOClient constructors and methods. Missing Main and a null NIOManager
  are distinguished; the latter is stored and fails only when I/O uses it.
- PosixNIOSocket reuses sibling send/recv/descriptor ownership with nonblocking
  EAGAIN/EOF handling, EINTR retry and SIGPIPE suppression. Real loopback tests
  verify backpressure, a 1MiB transfer, wakeups/interests, exact payloads and
  traffic, closure and EOF. Metadata access and trace flags are synchronized;
  read/write buffer state still requires one event-loop owner, as in the Java
  server. NIOServer now drives these channels, with native NIOManager integration covered by a loopback fixture;
  full Main binding and complete selector failure behavior are not complete. Configure runtimeLookup before starting network threads.
- JavaDateString adapts existing MiniUtil/TxHeader calendar formatting, ICU zone
  metadata, the inspected JDK pre-1900 offset rule and extreme transition
  overflow behavior. Four timezone suites pass. Other timezone name datasets,
  custom mutable TimeZone APIs and every historical transition remain unverified.
  Existing TxHeader/MiniUtil formatters were not replaced by this new adapter.
  Native NIO log prefixes and generic JDK exception diagnostics remain gaps.


- NIOServer reuses native clients, concurrent maps and MessageStack with a poll
  selector and a nonblocking pipe. Actual Java/native fixtures compare IPv4/IPv6
  accept, outgoing registration, payloads, direct send, broadcast filtering,
  explicit and failure disconnects, reconnect flags, retained shutdown entries,
  and traffic. Startup fixtures check invalid ports, shutdown before run, and
  the pinned halt(0) behavior for an occupied wildcard port in subprocesses.
  Review corrected descriptor transfer on allocation failure, cancellation races
  during interest snapshots, and the fatal bind/listen/local-address lookup scope.
  NIOManager uses a weak receiver callback and running-thread ownership;
  the lifecycle fixture verifies release after shutdown. Exceptional resource
  failure cleanup remains under review;
  full runtime binding, static demonstration main, custom network properties and
  precise exception/log formatting remain pending. Selector key iteration order
  across independent ready channels is not asserted to match JDK implementation
  order. Callers must retain the server and join its run thread before destruction.
- P2P2DB reuses JsonDB/MiniFile with a shared nullable-string peer list. Its load
  clears the original list and appends after each cast; a later failure retains
  the earlier peers. save updates JSON before attempting I/O. 211 programs/5,145
  operations pass normal and sanitized builds, including file bytes, alias identity,
  missing/truncated files, wrong JSON types and failed writes. JsonDB now provides
  virtual load/save and destruction so superclass calls dispatch correctly.
  Existing prefs (1,130 programs), peer state (798 programs), and the mapped
  1,000-peer database test pass in both builds. Full MinimaDB integration, generic
  exception diagnostics and logger prefixes remain unresolved.

- Greeting.createGreeting preserves lock outside the caught body, unlock after
  the catch, partial JSON mutation and appending the chain on repeated calls.
  532 network-startup programs (1,764 greeting calls) pass normal and ASAN/UBSAN;
  actual Java classes, node chains, service calls and wire bytes are compared.
  GreetingServices follows existing required-service boundaries. Full Main/DB/P2P
  wiring, nullable GeneralParams.MINIMA_HOST, general stream APIs and all Java
  reference aliasing remain pending; a fixture pass does not close those gaps.
- The connect command preserves first-colon parsing, Java trim, Unicode decimal
  parsing and signed 32-bit limits. It deliberately does not add address or TCP
  port-range validation that the source lacks. Checked command errors now render
  as java.lang.Exception through a dedicated JavaCheckedException, with 1,233
  command-runner regressions and 9,244 object regressions passing in both builds.
  Message posting requires a runtime binding. The startup test captures posts
  and injected failure; the new manager fixture binds the command to its actual
  native queue and connects to an owned loopback peer.


- NIOManager translates the pinned message branches and static transport helpers,
  reusing MessageProcessor, NIOServer, NIOClient, Greeting, MiniData and NIOTraffic.
  NIOManagerServices contains required external receivers; Main/NetworkManager
  and P2P runtime bindings remain absent. NIOMessage now has an actual native
  dispatcher and a TCP integration check; the original 32-frame fixture separately
  tests callback concurrency and ownership.
  640 transport comparisons cover all byte type names, exact payloads, null and
  failing serializers, allocation-error conversion and single/broadcast accounting.
  17 loopback ping comparisons cover arbitrary response type acceptance, fragmented
  input, malformed/truncated frames, length limits and read timeout. These pass
  normal and ASAN/UBSAN builds. The fixture originally assumed a 32-byte zero ID;
  inspected MiniData.java confirms ZERO_TXPOWID is one byte. The fixture is fixed.
- The native manager loopback test exercises its real processor/server threads,
  connect command, outgoing greeting, incoming metadata, four-worker dispatch of
  32 frames, disconnect cleanup and shutdown ownership. A separate state-machine
  differential suite now passes 1,449 programs / 3,879 operations in both builds,
  using actual pinned processMessage/query/disconnect methods. It covers reconnect
  policy, attempts and limits including overflow, forced slave reconnect, invalid
  peers, timer delays, sync cache removal, greeting-send mutation order, startup
  list parsing, archive/TxPoW requests, stale reads, nulls and receiver failures.
  Stale-read cases avoid ambiguous live-clock boundaries; multiple global manager
  instances, concurrent interleavings and full API/exception/log parity are still
  unverified. The fixture checks successful packet/timer/P2P outcomes so matching
  empty or error-only implementations cannot satisfy the suite.
- Manager state comparisons found and corrected a null-ID call-order mismatch:
  native NIO_TXPOWREQ dereferenced the optional transaction ID before invoking
  the DB lookup, while pinned Java passed the null reference to that lookup.
  NIOManagerServices.txPoWExists now accepts optional<string>, and the manager
  passes it unchanged before logging/construction. Cases cover a returning
  receiver and an injected receiver failure with retained state/call order.
- Manager teardown now catches selector-wakeup failure separately, then always
  stops the processor and performs both thread ownership barriers. The prior
  broad catch could skip these later operations if waking the server threw.
  Normal/sanitized loopback lifetime regressions pass; exhaustive resource-failure
  injection is still pending. Fixed worker/process-exit differences below remain.

- FixedTaskExecutor adapts the existing CV queue to the inspected JDK fixed pool:
  lazy workers, unbounded queue, replacement after an escaping task exception,
  shutdown drain, rejection and non-canceling await. Native concurrency tests pass
  under sanitizers. Thread creation failure and exact JDK resource-error semantics
  are not claimed equivalent.
- NIOConnections owns descriptors and uses poll for timed connect/read. Review of
  installed NioSocketImpl.timedRead found that interrupted waits must retain a
  monotonic deadline; the adapter now preserves it across EINTR and resets after
  positive reads. Signal-interruption, progress, zero-timeout and EOF checks pass
  both builds. Descriptor copying is disabled. Wildcard destinations still
  need review: JDK Socket.connect replaces any-local addresses with getLocalHost,
  while SocketChannel.open and the current shared connector use the supplied
  resolved address. Custom network properties and other platforms remain pending.

## NIOMessage review

The dispatcher reuses the pinned native object codecs, consensus checks, IBD
selection, tree searches and NIOManager transport. Offline fixtures call the
unmodified Java run method and compare packet bytes, lookup order, client state,
P2P/timer events, IBD processing and heavier-chain rescue notifications.
The full node remains incomplete; these receivers are captured by the fixture.

### Corrected — null MMR was rejected too early

**Files:** `src/org/minima/system/network/minima/NIOMessage.cpp`,
`src/org/minima/system/brains/TxPoWChecker.cpp`.

The dispatcher initially dereferenced the tip MMR before checking scripts. Pinned
Java does not use that argument in its script checker, and its MMR checker only
uses it when a witness contains a proof. Pointer overloads now preserve that order;
existing reference overloads delegate to them. Empty witnesses can pass with a
null MMR, while proof validation fails at the first actual MMR receiver use.
The differential corpus includes valid, null and empty MMRs with monotonic and
non-monotonic scripts, invalid proofs, signatures, burns and mempool conditions.
NIOMessage also checks nullable server/traffic receivers before native dereference.

### MAJOR — full runtime and log notification parity remain pending

**Files:** `src/support/NIOMessageServices.hpp`,
`src/org/minima/system/network/minima/NIOMessage.cpp`.

Main/MinimaDB/P2P/processor/miner startup bindings remain required. MegaMMR sync
uses a required getCurrentMegaMMRIBD receiver whose native command implementation
is still absent. Logging currently uses the native stream rather than the pinned
MinimaLogger MINIMALOG notifications, prefix, memory accounting and exception stack
traces. The Java fixture explicitly excludes MINIMALOG; it retains the separate
MDS_HEAVIER_CHAIN event. Exact clock thresholds, all injected receiver/resource
failures and concurrent mutable global state remain outside this validation.

**Fix:** connect the remaining native runtime components and logger, then validate
those event and lifecycle paths before claiming complete behavior equivalence.

### Validation

Dispatcher comparisons pass 1,918 programs / 4,677 packets in normal and
ASAN/UBSAN builds. Existing consensus (2,890 comparisons), message-worker and
manager-state (1,449 programs / 3,879 operations) regressions also pass both builds. Coverage guards require real packet types 0/1/2/3/4/12/14/16/19/20,
peer notifications, mining/processing callbacks, disconnect timers, both sides of
the difficulty-ratio threshold, heavier-chain notification and rescue timer.
A guard caught invalid cascade fixture parents that otherwise matched only the
rejection path; the fixture now uses the proven ArchiveCacheOracle zero-parent rule.
The TCP manager fixture dispatches actual transaction-ID and single-ping frames in
both builds, in addition to its previous 32-frame/four-worker and lifetime checks.

## Verdict

**Request changes / incomplete** for the requested full 1:1 node.
The existing node compiles; the new subset compiles and has independent
Java/C++ comparisons. Neither result establishes full upstream equivalence.


## P2PFunctions / UtilFuncs review

### Summary

The pinned peer helpers reuse the native manager/client queues, JSON/MiniString,
peer maps and address types. All 672 programs / 3,670 operations and 39 real HTTP
cases pass in normal and ASAN/UBSAN builds. This verifies the scoped helper
behavior, not a complete P2PManager/Main runtime.

### Resolved — HTTP retry and proxy redirect differences

**File:** `src/support/P2PEnvironment.cpp`.

Initial comparison caught Java retrying a dropped GET response once. The adapter
now retries receive failures/empty responses once using a fresh connection.
Inspection and real request traces also showed that JDK HTTP 305 retains its
cached original request path while connecting through the proxy. The adapter
preserves this path, the original resource URL and the 20-response redirect limit.
The tests include both a successful separate proxy and a self-redirecting proxy.

### MAJOR — Complete environment and runtime parity remains open

**Files:** `src/support/P2PEnvironment.cpp`,
`src/org/minima/system/network/p2p/P2PFunctions.cpp`.

The native default availability call uses libcurl HTTPS. The checked local
transport fixtures use HTTP; TLS trust stores, Java proxy/authenticator/cache
settings, default connection timeout (curl's default is 300 seconds), response
body close/drain errors and all malformed responses are not equivalent or fully
validated. The logger currently emits body text through a receiver or clog;
MinimaLogger prefixes and MINIMALOG notifications still require native binding.
Local-interface hostname sets match on this host; other platforms/interfaces and
failure fallback paths require broader verification. The Java helper's diagnostic
main entry point has no standalone mapped executable yet.

**Fix:** complete these bindings and platform semantics before promoting the
source-map rows to verified. Preserve the HTTP fixture trace checks.

### MINOR — Default random constructor is nondeterministic across platforms

**File:** `src/support/JavaRandom48.hpp`.

Seeded bounded selection and rejection match java.util.Random, including six
extreme seeds and large rejection bounds. Default initialization uses existing
OS entropy; Java uses its seed uniquifier and nanotime. No equality of unseeded
sequences is claimed. Full concurrent interleavings/resource behavior remains open.

### Verdict

Request changes for a claim of full parity. The scoped implementations compile
and pass their tests; the outstanding runtime and environment differences remain
explicitly recorded.


## SwapLinksFunctions review

### Summary

All pinned methods are translated using existing peer state/maps, greeting/walk
objects, MiniData and actual native NIO manager/client queues. The differential
suite passes 651 programs / 1,959 operations in normal and ASAN/UBSAN builds,
including state remaining after
exceptions and shared wrapped-JSON identity. Full node integration remains open.

### Resolved — Fixture known-peer ownership

**File:** `support/p2p_functions.cpp`.

The initial test setup dereferenced a temporary shared list in a range loop. Its
owner expired before iteration. The first Java/C++ comparison detected missing
known peers. The fixture now retains the list owner throughout the loop. This was
a fixture bug, not a change to the pinned peer algorithm.

### MAJOR — Required Main-presence and logger integration

**File:** `src/org/minima/system/network/p2p/SwapLinksFunctions.cpp`.

processGreeting distinguishes an absent Main singleton from a present Main with
an absent manager. It uses an explicit required mainPresent receiver and the
existing manager lookup. Client greeting flags and disconnect queues are actual
native implementations; the receiver is not a native Main implementation. The
existing P2PFunctions logger still needs MinimaLogger/MINIMALOG integration.

**Fix:** bind these receivers from actual Main/P2PManager lifecycle code, then
validate live lifecycle and error propagation before marking full API parity.

### MINOR — Nondeterminism and generic failures limit evidence

**File:** `tools/swap_links_differential.py`.

Generated secret bytes are compared through stable tokens retaining their lengths
and equality relationships. Independently shuffled greeting knownPeers are compared
as multisets. Deterministic secrets remain exact in this corpus. Generic exception
text and class names, full allocation/receiver failures and concurrent map mutation
are outside the present oracle. Mapped upstream SwapLinksFunctionsTest and its
QuickState/QuickInetLists fixtures still require their own native counterparts.

### Verdict

Request changes for full-port approval. The translated helper methods and scoped
comparisons are concrete progress; Main/P2PManager and complete runtime semantics
are still required.


## WalkLinksFuncs review

### Summary

The walk-routing and swap-generation methods reuse the validated peer state,
messages, address equality, NIO connect effects and Java Random algorithm.
PeerOperations extracts the existing SwapLinksFunctions message/map helpers for
shared use. P2PFunctions exposes the existing native log-body adapter so both
helpers use one receiver; a complete MinimaLogger remains pending.

### Resolved — Nullable path entry after failed connection

**File:** `src/org/minima/system/network/p2p/WalkLinksFuncs.cpp`.

A failed attempt removes the selected address from the walk. A null next entry
must raise the Java-style failure before reading host or port. Combining a checked
host expression and an unchecked port expression in one C++ call left their order
unspecified. The code now checks the receiver, reads host and port in explicit
order, and then calls checkConnect. New cases exercise rejection followed by null.
Own-address comparisons also handle null arguments without dereferencing them.

### Resolved — Swap guard assumed UID order

**File:** `tools/walk_links_differential.py`.

Initial outputs agreed, but a coverage assertion assumed the numerically final
UID was the final HashMap key. Actual Java bucket order wraps across these fixture
keys. The guard now uses the independently captured map order and verifies that
swap generation excludes its last key and throws with only one key, as pinned.

### MAJOR — Node lifecycle and complete exception semantics remain open

**Files:** `src/org/minima/system/network/p2p/WalkLinksFuncs.cpp`,
`src/support/PeerOperations.hpp`.

P2PManager/Main still need to bind the real lifecycle and logging receivers. The
current differential corpus uses actual method implementations with an offline
Main notification fixture. Generic exception text/types and allocation failures,
concurrent map/path mutation, and extreme collection capacities are not fully
verified. Generated load-swap secrets are canonicalized by length and identity;
fixed walk secrets remain byte-exact. Source-map rows remain partial/not_verified.

**Fix:** finish the native lifecycle and outstanding API/platform behavior before
claiming full parity. Retain path mutation, source quirks and failure-order checks.

### Verdict

Request changes for full-port approval. The new component is implemented and under
scoped differential validation (883 programs / 2,153 operations pass normal and
ASAN/UBSAN); it does not establish a completed native node.


## P2PPeersChecker review — Java/TCP validation

### Summary

The checker now passes 339 programs / 758 operations / 329 real loopback pings
against freshly compiled pinned Java, in normal and ASAN/UBSAN builds. The new
fixtures exercise actual native/Java checker methods and ping transports, using
Java-generated block identities and actual TxPoWTree lookups. Exact snapshots cover
request bytes, traffic, peer sets, parent messages, logs and timer delays. Separate
native tests cover worker start/stop. Shared fixture extraction passed the existing
P2PFunctions, SwapLinks and WalkLinks regression suites in both configurations.

### MINOR — Early-return trace attribution race (fixed)

**File:** `tools/peers_checker_differential.py`.

With Main absent, ping writes its request then fails while accessing traffic,
returning before the local responder necessarily reads that request. The first
sanitizer run matched all state but attributed one Java request to the subsequent
native trace. A fixture-only control connection now drains earlier connections
before the trace is captured and the next program's replies are configured. The
corrected suite passes normally and under ASAN/UBSAN with the same 329 pings.

### MAJOR — Full parent/timer/runtime integration remains open

**Files:** `src/org/minima/system/network/p2p/P2PPeersChecker.hpp`,
`support/peers_checker.cpp`, `support/PeersCheckerOracle.java`.

Full MinimaDB/Main binding and checker retry dispatch are still pending;
fixtures capture timer/parent requests. The native checker weakly references its
parent MessageStack to avoid a shared ownership cycle, whereas Java retains a strong
reference. Owner-managed lifecycle and arbitrary retained references need full
runtime verification. Shared set enumeration retains earlier concurrent-iteration
limitations, and generic exception identity and full logger output are not compared.
Random admission uses actual fresh generators with 500 trials and state invariants;
this does not prove exact matching nondeterministic multi-peer eviction choices.

**Fix:** complete P2PManager Java validation and bind real runtime receivers, then verify parent and
checker ownership, timer dispatch and shutdown before claiming full parity.

### Verdict

Request changes for full-port approval. Scoped Java/TCP behavior is verified;
full runtime integration remains required.

## P2PTestParams and nullable logging review

### Summary

Pinned defaults and every setTestParams assignment are translated. Existing
JavaList preserves the fixed-size default list, element replacement, nulls and
shared list identity. One function-local settings object defers address resolution
until first access, following the existing Crypto singleton pattern. The Java
static fields are accessed through `P2PTestParams::values()` in C++.

The expanded peer-state suite passes 874 programs / 26,348 operations normally and
under ASAN/UBSAN, including 76 test-settings programs / 230 operations. It compares
all numeric defaults and assignments, repeated application, before/after scalar
changes, fixed-size list failures, list replacement, nulls and shared mutation.
The extracted parameter snapshot now checks null active lists before iteration.

### MINOR — Null log level could not be represented (fixed)

**Files:** `src/org/minima/system/network/p2p/params/P2PParams.hpp`,
`src/org/minima/system/network/p2p/params/P2PTestParams.hpp`.

Java permits a null enum field; comparing it to any configured level returns false.
The prior native nonnullable enum could not represent that state. Both native
settings now use optional<P2PLogLevel>; existing P2PFunctions comparisons retain the
same logic. Actual Java/native tests now assign/apply null, compare its snapshots,
and call all three logging methods with a null level.

### MAJOR — Complete runtime and concurrent initialization remain open

**File:** `src/org/minima/system/network/p2p/params/P2PTestParams.cpp`.

Native P2PManager construction in test mode is now tested. Concurrent mutable
parameter access, environmental DNS caching and Java erroneous-class behavior after
initialization/resource failure are not covered by these sequential comparisons.

**Fix:** exercise test settings through the real manager and complete runtime
startup/ownership; preserve initialization behavior when adding those bindings.

### Verdict

Approve scoped sequential settings behavior; request changes for full-port approval
until manager/runtime integration and remaining source-map gaps are complete.

## P2PManager review — native implementation and lifecycle

### Summary

All pinned method bodies are translated with existing peer components, actual NIO
helpers and database/timer receivers. Initial native tests verify state transitions,
peer limits, saved-peer admission, persistence thresholds, greeting serialization and
processing, connectivity assessment, disconnect/reclassification, seeded loop delays
and float client counts. Test-mode ordering matches Java: state is constructed before
global settings change. The actual manager factory, checker worker and timer thread
execute, stop/join and release ownership normally and under ASAN/UBSAN.

PeerShuffle extracts the already tested P2PGreeting permutation without changing its
algorithm. Existing peer-state and swap-link Java comparisons pass in both builds.

### Verified manager behavior and shared-state fixes

The manager differential now executes the pinned class and native method bodies in
405 programs / 2,915 operations, with 46 emitted packets. It compares all five peer
maps, scalar flags, known/verified/unverified sets, checker admission queues, NIO
queues, timers, database contents, save calls, log bodies and ordered partial errors.
The initial corpus accidentally used string peers in several success cases; review
corrected these to the source's address objects and added successful greeting/walk
assertions. Final normal and ASAN/UBSAN runs pass with these corrections.

Discovery adds 31 programs / 84 operations / 39 real TCP pings per build. It reuses
the checker responder and Java greeting encoder, including the responder barrier.
Exact request frames, traffic, peer state, malformed replies, zero-attempt guards,
three-attempt cap and actual five-second failed-ping pauses match both builds.
Native and Java binaries run separately; the native implementation does not use Java.

P2PState now protects scalar and shared-pointer reads/replacements with short mutex
sections. Serialization retains local strong references; address resolution occurs
outside the lock. Existing map/set synchronization remains independent. Public
manager counters/timer use atomics. The new concurrent test performs 2,000 state
replacements, 2,000 actual manager updates and 16,000 reads; normal, ASAN/UBSAN and
ThreadSanitizer runs pass. It explicitly joins manager, checker and timer before
clearing receiver callbacks. No race-detector finding occurred in the exercised paths.

Fixture reuse: P2PFunctionsFixture.hpp and P2PManagerFixture.hpp extract existing
support implementations; the original helper, swap/walk and manager tests pass in
both builds after extraction. The new native fixture retains temporary collection
owners throughout iteration. Initial compilation required dereferencing JSON object
pointers for the existing JSONValue constructors; no production workaround was added.

### MAJOR — Complete runtime binding and lifecycle semantics remain open

**Files:** `src/org/minima/system/network/p2p/P2PManager.cpp`,
`src/org/minima/system/network/p2p/params/P2PParams.hpp`.

Main/MinimaDB/NetworkManager bindings are still required. GeneralParams/P2PParams
configuration and static runtime callbacks are frozen during concurrent tests;
arbitrary concurrent replacement is not verified. Actual checker-to-manager promotion
with both workers running over TCP and complete node startup/shutdown remain open.
The manager differential uses unstarted fixtures, captures timer requests, compares
exception outcomes rather than every Java exception type, and normalizes generated
secret identities and shuffled greeting order. Passing these suites is not full node
or full concurrency equivalence.

**Fix:** bind actual native owners, exercise worker promotion and shutdown, and audit
runtime initialization/public mutation before full-port approval.

### Verdict

Request changes for full-port approval. Scoped manager parity, real discovery and
shared-state concurrency are verified progress. Full native runtime remains incomplete.

## P2P2 helpers and manager review

### Summary

P2P2Functions reuses tested P2PFunctions IP predicates, NIO queries/actions and native
availability behavior. Its invalid-peer set and hostname cache remain independent;
P2P2-specific connection messages use the source's unconditional logging. The new
manager reuses MessageProcessor, TimerMessage, P2P2DB/P2PDB and JavaRandom48, preserving
all active method statements and the pinned source's inactive branches.

837 programs / 3,374 operations match Java in normal and ASAN/UBSAN builds: admission,
queries, cache identity/separation, startup mutations and receiver call order, timer
requests, failure points, nullable peers, random membership/distribution and shutdown.
A real worker/timer test verifies initialization, fast-loop dispatch, the source's
ignored shutdown message and explicit shutdown/join with released ownership. Existing
P2P2 database tests pass in both builds. All native targets compile in both builds.
The initial support compile required dereferencing MiniUtil's JSON-array pointer;
production compiled without correction.

### MAJOR — P2P2 database/global concurrency and full owners remain unverified

**Files:** `src/org/minima/system/network/p2p2/P2P2Manager.hpp`,
`src/org/minima/system/network/p2p2/P2P2DB.hpp`.

The DB exposes its existing shared vector; concurrent external list mutation is not
safe merely because the new worker has atomic numeric controls. Mutable static message
strings do not yet expose Java null values or concurrent replacement semantics. The
runtime still needs native MinimaDB/NetworkManager owners and complete logger binding.
Database log bodies are excluded in the new oracle because the current native DB
writes them directly to clog; this is explicit, not evidence of full log equivalence.

**Fix:** finish owner integration and synchronize external database access, retain
Java nullable static field behavior, and bind the full logger before full approval.

### Verdict

Request changes for full-port approval. New method translations and scoped tests are
verified; complete runtime and remaining semantic gaps still require work.

## RPC listener review

### Summary

Server/HTTPServer translate the pinned listener using existing PosixNIOSocket,
NIOServer dual-stack bind/backlog and shared-ownership worker conventions. The
nonblocking wakeup pipe was extracted from NIOServer into SocketWakeup.hpp. Source
HTTPServer has no NIOServer fatal-exit/reuse-address behavior, so neither is applied.
Shutdown wakes polling workers even when socket close reports an error. Native
factory startup waits until derived ownership exists; run remains virtual.

Three Java/C++ programs and ten actual handlers per implementation pass normal and
ASAN/UBSAN builds: manual/automatic startup, simultaneous independent handlers, exact
binary output, bind failure log and shutdown completion. The collision fixture was
corrected to occupy the IPv6 wildcard used by Java on macOS; no production workaround
was added. NIO listener and manager loopback regressions pass after extraction. All
native targets compile in both builds.

### MAJOR — Complete RPC ownership and exception equivalence remain open

**Files:** `src/org/minima/system/network/rpc/HTTPServer.cpp`,
`src/support/SocketWakeup.hpp`.

CMDHandler and full NetworkManager ownership are not yet connected. Repeated starts,
pre-start shutdown, all Java socket/IO exception distinctions, daemon failure logging
and general concurrent configuration changes require further audit. Passing listener
tests does not establish complete RPC/node equivalence.

**Fix:** finish actual command handling and owners, and compare the remaining lifecycle
and resource-failure paths before full-port approval.

### Verdict

Request changes for full-port approval. Listener behavior in the stated scope passes.

## CMDHandler review

### Summary

CMDHandler translates the pinned method using actual Authorizer/CommandRunner,
GeneralParams, Java date/integer/case helpers and native sockets. SocketCharacterReader
adds buffered Reader/readLine semantics with JavaUnicode/MiniString decoding and
UTF-16 body counts, including CRLF skipping, EOF padding and malformed prefixes.
nioReadSome reuses existing poll/deadline/socket code. JavaURLDecoder retains the
installed JDK26 strict hex alphabet and MiniString replacement decoding.

2,106 requests across six configurations pass in normal and ASAN/UBSAN builds.
The real HTTPServer launches actual CMDHandler instances; requests cover successful
and failed commands, multi-command dispatch, case-sensitive and duplicate headers,
authentication, CRLF settings, malformed requests/UTF-8, URL escapes, body truncation,
zero padding, fragmented scalars and the buffered reader boundary. Response bytes,
content lengths and body newline match, with only the wall-clock Date value replaced.
Handler failure sites/order and exact short-read logs match.

One initial normal run failed at fixture status; subsequent repeat runs passed. The
oracle now reuses SearcherOracle's separate stdout protocol/stderr log stream to
remove log interference. Both RPC suites pass in both builds after that change.
Existing nio_connections also passes both. Production compiled without corrections.

### MAJOR — Full logger, character API and node shutdown remain incomplete

**Files:** `src/org/minima/system/network/rpc/CMDHandler.cpp`,
`src/support/SocketCharacterReader.hpp`, `src/org/minima/system/commands/CommandRunner.cpp`.

Full MinimaDB/Main bindings and the complete command registry are still needed.
The handler implements exact quit-triggered process halt, but the quit command's
Main shutdown is not yet ported or verified. UTF-16 bodies become the existing native
UTF-8 command interface, so arbitrary unpaired-surrogate command parameter semantics
remain under review. The current logger adapter does not establish Java stack traces
or all exception class/text parity; frozen globals are required during RPC tests.

**Fix:** complete actual runtime owners and quit command, audit remaining character
API boundaries and bind full logger before full-port approval.

### Verdict

Request changes for the full port. Actual RPC handling in the stated scope is verified.

## NetworkManager review

### Summary

NetworkManager now owns the existing native NIOManager, P2PManager, P2P2Manager and
HTTPServer/CMDHandler. Disabled peer managers use actual MessageProcessor workers
with the pinned shutdown-message branch. Native shared ownership is established
before startup; weak NIO-to-network callbacks avoid a reference cycle. Owner getters
use the existing short-mutex pattern. The initial atomic<shared_ptr> implementation
did not compile with this Apple libc++; replacing it with short locks fixed the build.
RPC startup/stop is serialized; destruction also cleans up partial initialization.

Host discovery reuses P2PEnvironment getifaddrs ownership and numeric rendering,
preserving the inspected OpenJDK BSD reverse insertion order for IPv4 groups and
addresses, IFF_UP plus IFF_RUNNING, loopback exclusion and the pinned wl-prefix rule.
Status uses actual NIO maps/NIOTraffic/MiniFormat/JavaDateString and preserves negative
Duration floor-seconds semantics and Java MIN_LONG/-1 division without C++ overflow.

Review found that an initial NIO service binding incorrectly substituted the network
shutdown flag for Main's flag. The pinned NIOManager checks Main; the implementation
now retains that required external receiver. A real worker regression queues a
connect while Main is shutting down and confirms no pending client is inserted,
while NetworkManager's own flag remains false.

Eight native lifecycle combinations cover both peer enablement flags, soft/hard
shutdown, actual P2P/P2P2 initialization, RPC hash commands and restart, NIOMessage
ping dispatch, traffic and owner release. 540 Java/C++ programs cover actual host
selection, configured-host bypass, real NIO maps, status and traffic rates, signed
bounds, configuration fields and unused UserDB receiver/error order. Only the
traffic start date is normalized; elapsed ages avoid wall-clock minute boundaries.

All native targets compile in normal and ASAN/UBSAN builds. Seven selected suites
pass both: NIO manager loopback/connections, P2P functions, HTTP listener, CMDHandler,
native network lifecycle and network status differential.

### MAJOR — Full node ownership and lifecycle equivalence are still incomplete

**Files:** `src/org/minima/system/network/NetworkManager.cpp`,
`src/org/minima/system/network/p2p/P2PManager.cpp`, `src/support/P2PEnvironment.cpp`.

Main/MinimaDB still need to supply actual database, message-dispatch and static
runtime lookups; current integration tests install explicit receivers. The native
lifecycle suite is not a Java constructor/shutdown differential. Full exception/log
identity, repeated concurrent startup/shutdown, interface reconfiguration/aliases
and non-BSD host enumeration need further audit. Mutable global configuration and
external database-list access are not generally thread-safe; the tests freeze them
while workers run. SQL/wallet/consensus processors and complete command shutdown
remain incomplete, so this is not a complete working node.

**Fix:** complete actual runtime owners and database services, exercise remaining
lifecycle failures/concurrency and platform variants, and finish all mapped classes.

### Verdict

Request changes for full-port approval. Network ownership, real worker integration
and the stated Java comparisons are verified progress.

## SqlDB review

### Summary

SqlDB translates the pinned lifecycle and generic SQL result handling through an
explicit native connection boundary that extends the existing SqlRow interface.
Existing MiniData/MiniString/JSONParser/JSONObject/JSONArray are reused. The sibling
SQLite Database.hpp and its persistence tests were inspected; its storage format,
SQL dialect and encryption/backup behavior cannot replace the pinned H2 driver.
No fallback driver or successful no-op connection is installed in production.

Review corrected Java Unix File normalization, root/bare parent handling and NUL
path guards. Embedded NULs cannot reach native filesystem deletion with a truncated
path. Repeated/trailing slashes are normalized without resolving dot segments.
Virtual methods and forwarding overloads preserve Java subclass dispatch. The
reentrant mutex is shared with future synchronized subclass operations. Sticky
encryption state after a later plain load, connection assignment before auto-commit,
exception-side partial state, dropped SQL NULL columns, metadata-class-sensitive
CLOB conversion, signed length narrowing and failed statement-close result reset
all follow the pin. Native ownership replaces the JVM GC hint; no explicit source
statement/result-set close is invented where Java omits it.

All native targets build normally and with ASAN/UBSAN. 464 programs execute the
actual pinned SqlDB against Java JDBC proxies and native recording connections,
covering 1,260 operations and 8,352 driver calls. Return values, events, file effects,
credentials and log bodies match; Java NPE/class-cast diagnostic text and logger
prefixes/stack traces are explicitly excluded. A separate eight-thread test passes
1,024 serialized calls and two reentrant schema calls in both builds.

### MAJOR — H2-compatible persistence and runtime owners remain absent

**Files:** `src/support/SqlConnection.hpp`, `src/org/minima/utils/SqlDB.cpp`.

The interfaces do not execute SQL, read/write H2 MVStore files, encrypt a database,
or run backup/restore scripts. Only test fixtures implement the connection boundary.
Additional driver APIs and native Wallet/archive/coin database
subclasses remain outstanding. TxPoWSqlDB now has a counterpart with recording-driver validation. Main startup-debug and full MinimaLogger notification/exception
bindings are not connected. Generic driver object conversions, nullable metadata
names, arbitrary Java UTF-16 SQL strings, concurrent unsynchronized lifecycle calls,
platform filesystem differences and full public API parity still need review.
Passing wrapper comparisons cannot establish a working native database or node.

**Fix:** supply and verify a real native H2-compatible backend, port the database
subclasses and connect actual Main/MinimaDB services; validate persistent round trips,
encryption, backup recovery and lifecycle concurrency against the pinned engine.

### Verdict

Request changes for full-port approval. The compiled wrapper and stated behavioral
checks are verified progress; the 457-file working-node goal remains incomplete.

## TxPoWOnChainDB and range-exception review

### Summary

TxPoWOnChainDB directly translates the pinned schema, eight prepared statements,
wipe, insert, single transaction lookup, both in-block query overloads, first/last
records, count and cleanup. It reuses SqlDB's recursive monitor, lifecycle and logger
boundary, MiniNumber conversion, TimerMessage clock and JavaDateString. The sibling
BlockStore and persistence tests were inspected; its serialized-block schema and
replacement semantics are not compatible with the on-chain membership table.

The original retention expression overflows as Java int before widening; the native
default is 500654080, checked against the actual Java field initialization. Cleanup
preserves signed-long wrap and the second clock read in hard mode. Source SQL-only
catches retain partial JSON/list results and propagate runtime failures. wipeDB keeps
the existing prepared statements. Helpers receive references to the prepared fields
so a reopen can replace them before use. Prepared interfaces and getLong were added
to the existing native driver boundary; SqlDB's null-receiver helper was extracted
unchanged, and logging is now available to subclasses.

The existing recording fixture was extracted to SqlRecordingFixture.hpp and its
Java proxy implementation reused through SqlDBOracle. 855 programs, 3,639 operations
and 29,167 driver calls pass normal and ASAN/UBSAN. Statements, parameter order,
prepared identities across partial creation failures, typed/null row values, partial
results, runtime versus SQL failures, date output and cleanup are compared. Live
timestamp parameters are validated against before/after clock windows with the exact
retention arithmetic before normalization. Java-specific NPE diagnostic expressions
and logger prefixes/stack traces are excluded; native clock call counts are checked.

The first run exposed an existing MiniNumber range-error mismatch: input 1e40 threw
a generic native range exception instead of Java NumberFormatException. The fix uses
the existing JavaNumberFormatException and shares MMREntryNumber's tested decimal
renderer through JavaBigDecimalText.hpp. Positive/negative bounds, retained fractional
zeros, scientific notation, precision rounding and decimal-place truncation are
checked by additional cases. All native targets compile; eight selected primitive,
VM, cascade, command and SQL suites pass both builds after the correction.

### MAJOR — Native database execution and integration are still incomplete

**Files:** `src/org/minima/database/txpowdb/onchain/TxPoWOnChainDB.cpp`,
`src/support/SqlConnection.hpp`.

Only test fixtures currently implement the connection interfaces. These comparisons
do not execute SQL storage, constraints, ordering or prepared-statement invalidation
in a native H2 engine. Full Main/MinimaDB/TxPoWDB ownership, transaction processing,
remaining database classes and the source demonstration main are not complete.
Arbitrary concurrent unsynchronized lifecycle/wipe calls, complete Java string/null
metadata semantics and logger notification/exception parity remain under review.
MiniNumber's other constructor/arithmetic/malformed-input exception details also
remain part of the full parity audit.

**Fix:** implement the actual native backend and runtime owners, validate persistent
round trips and engine failure behavior, and finish the remaining mapped sources.

### Verdict

Request changes for the full port. The on-chain database wrapper, shared formatter
correction and stated normal/sanitized checks are verified progress.


## TxPoWSqlDB and malformed-data review

### Summary

TxPoWSqlDB translates the pinned schema, ten prepared statements, insert, first-row
lookup, child IDs, relevant/latest lists, counts, cleanup and close/reopen. It reuses
SqlDB lifecycle/recursive locking, native TxPoW serialization/conversion, TimerMessage
and JavaLowerCase. SQL interfaces and the existing recording fixtures now support
setInt/setBytes/getBytes. Nullable decoded rows remain null list entries; SQL failures
return the source partial result and runtime failures propagate. Reopening updates
prepared fields before use. wipeDB preserves the pinned omission of index/statement
recreation. SQL filter removal is a single pass over each original intermediate
string, including occurrences formed by a previous deletion. No alternate SQL engine
or persistence implementation is substituted.

Review corrected TxPoW.convertMiniDataVersion's broad exception catch: only I/O errors
produce null, while runtime failures propagate. Its logger callback lets TxPoWSqlDB
reuse its existing log boundary. addTxPoW follows MiniData.getMiniDataVersion's I/O
failure handling: log, continue binding the first five parameters, then throw for the
null serialized value before setBytes. An oversized-hash fixture exposed the old
native message mismatch. MiniData hash read/write messages and negative ordinary
array-length exception types now match Java; MiniNumber invalid wire lengths and
zero-length BigInteger exception details were also corrected using existing Java
exception types. Numerical arithmetic and successful wire encodings are unchanged.

2,592 programs execute the actual pinned Java methods and the native counterpart,
covering 8,729 operations and 86,333 recording-driver calls. Tests compare byte payloads,
IDs/block/transaction metadata, logs, prepared identities, all query parameters,
partial initialization and checked/runtime failures. They include all truncated
prefixes of four TxPoWs, a non-empty transaction, malformed primitive fields, long
wraparound cleanup, both relevant-list overloads and failure-side binding order.
Clock parameters are checked against actual call windows before normalization;
Java NPE expression details and logger prefixes/stack traces are excluded.
All native targets compile normally and with ASAN/UBSAN. Twelve selected suites
pass in both builds: malformed streams, source mapping, primitives, objects, archive
cache/list, NIO transport/loopback/message, SqlDB, SQL concurrency, OnChainDB and
TxPoWSqlDB. Normal total 106.38 seconds; sanitized total 255.35 seconds. No new
ThreadSanitizer run was performed.

### MAJOR — Backend, runtime ownership and complete API parity remain absent

**Files:** `src/org/minima/database/txpowdb/sql/TxPoWSqlDB.hpp`,
`src/org/minima/database/txpowdb/sql/TxPoWSqlDB.cpp`, `src/support/SqlConnection.hpp`.

Only recording fixtures implement the driver interface. Actual H2 SQL execution,
constraints, ordering, durable writes, encryption, backups and engine-side prepared
statement invalidation are not validated by these tests. Main/MinimaDB/TxPoWDB and
wallet/processor bindings are still incomplete. The required connection-provider
constructor and lazy maxSQLMilli accessor are native API adaptations; the source
static demonstration main, nullable mutable MiniNumber static reference, unrestricted
UTF-16 strings/default-locale behavior and full logger notifications remain under
review. Other primitive reader exception/partial-state cases and unsynchronized
lifecycle concurrency are still part of the full audit.

**Fix:** implement and verify the native backend and owning runtime, finish the
remaining source/API translations, and test real persistence and recovery against Java.

### Verdict

Request changes for full-port approval. The new counterpart and regression fixes are
verified component progress; a functioning 457-file native node remains incomplete.


## TxPoWDB review

The owning class composes the actual RamDB, TxPoWSqlDB and TxPoWOnChainDB, preserving
RAM-first reads without caching SQL results, duplicate RAM timestamps, autobackup
messages on each insertion, SQL existence/relevance order and shared object identity.
Main message and wallet resolution are required callbacks. SqlDB File normalization
was extracted unchanged into JavaUnixFile. Header-only transaction access exposed
an existing bad_optional_access mismatch; four transaction getter overloads now
throw the shared typed JavaNullPointerException.

1,355 programs / 7,407 operations / 80,960 recorded effects pass normally and with
ASAN/UBSAN. Tests run actual pinned RAM, SQL, searcher and wallet membership bodies.
They compare driver/owner call order, partial failures, queued TxPoW references,
RamData and map identity, state-variable relevance, duplicate insertion, both-store
lifecycle and malformed/null inputs. RAM and SQL clocks are validated before
normalization; Java NPE expression diagnostics and logger prefixes/stacks are excluded.
All targets compile; ten selected map/object/RAM/searcher/NIO/SQL regressions pass
normal (149.23s) and sanitized (399.91s). No new ThreadSanitizer run.

### MAJOR — Native storage and runtime integration remain incomplete

Only fixtures implement SqlConnection. Main/MinimaDB/Wallet owners are not supplied
by these callbacks. Nullable checkMempoolCoins input, other TxPoW body accessor
exceptions, arbitrary concurrent map traversal, complete Java UTF-16/reference APIs
and full logging remain open. These tests establish component progress, not a
working node. Request changes for full-port approval.


## CoinDB review

CoinDB translates the pinned tree-node cache schema, four prepared statements, insert,
list, cleanup, size and compact close/reopen. Its private constructor removes the old
.mv.db path before loading; SQL failures are logged and allow static replacement,
while runtime failures leave the old instance unchanged. Retained shared instances
remain usable after replacement. A short native mutex protects shared-pointer
publication; method monitors reuse SqlDB's recursive lock.

Coin conversion now catches only I/O errors, preserving runtime failures. The
TxPoWSqlDB I/O error formatter was extracted unchanged into JavaIOExceptionText and
is shared by both SQL codecs. Insertion retains the source sequence after a
serialization I/O failure: log, clear/bind ID and block, then fail on the null bytes.
Reading retains null decoded entries and partial lists after checked SQL failures.
The native Coin fixture is extracted unchanged from objects.cpp and Java reuses
ObjectOracle.coin. Filesystem tests operate only under generated build temp paths.

The first test failure corrected a fixture connector: JDBC DriverManager converts
a driver's null connection into SQLException, rather than returning null. The second
corrected the new CoinDB logger to use the existing I/O formatter. Neither mismatch
was hidden by result normalization. Normal comparison passes 1,350 programs, 4,318
operations and 28,803 driver effects. Every truncated prefix of three coins, token
and state data, malformed wire numbers/hashes, oversized-hash serialization,
constructor publication/deletion, retained instances, partial rows and reopen
failures are checked. Normal seven object/map/SQL regressions pass in 68.28 seconds;
all targets compile both builds. All eight selected suites also pass ASAN/UBSAN
in 201.07 seconds, including the same 1,350 CoinDB programs. No new ThreadSanitizer
run was performed.

### MAJOR — Storage and node coin-cache binding remain unfinished

The recording driver does not execute SQL or provide H2 durability, encryption or
recovery. ChainServices currently returns value coins and cannot yet retain a null
decoded CoinDB list entry. Runtime binding must resolve this before use. The static
publication mutex, other concurrent lifecycle behavior, complete Java nullable
object/UTF-16 APIs, filesystem platform differences and full logger behavior remain
review items. No full node was started. Request changes for full-port approval.


## Wallet review

Wallet translates three schemas, eighteen prepared statements, seed initialization,
replacement and wiping, deterministic TreeKey creation, script storage, cache
relevance, default-key gates, signing and persisted-use update order. It reuses the
existing SQL monitor, row classes, BIP39, Address and crypto implementations. Source
behavior is retained where it is surprising: removeScript leaves cache membership;
new KeyRow returns modifier 0x00 even when another modifier was stored; generated
words are requested before processing a supplied seed phrase; failed use updates
prevent returning a signature. The test constructor's default-key count is sticky.

The exact multisig contract is a support constant, without claiming a port of the
command. BIP39's trim function is extracted unchanged. TreeKey now exposes its
source logger through a callback, retains exhaustion logging before counter reset,
and accepts nullable signing data at the actual signing point. Its maximum-level
exception text now matches the source. Successful hashing/signature algorithms are
unchanged. Source unsynchronized operations were not silently made atomic transactions.

1,433 programs / 6,521 operations / 85,452 recorded JDBC effects pass normally.
Tests run the actual pinned Wallet, rows, BIP39, Address, TreeKey and Winternitz.
They compare schema/query/parameter order, failures around seed/cache replacement,
retained mutable SeedRow identity, private-key derivation, signature bytes, failed
key inserts and signing-use updates, default-key creation and constructor behavior.
Private cache sets are sorted for membership comparison. Enhanced Java NPE details
and logger prefixes/stacks are excluded. Default-address randomness is checked only
with zero or one row; the initialization corpus supplies a fixture seed phrase.
All targets compile both builds; eight selected normal suites pass in 67.49 seconds.
The same eight suites pass ASAN/UBSAN in 272.07 seconds, including all 1,433 wallet
programs (144.49 seconds for that suite). No new ThreadSanitizer run.

### MAJOR — Durable signing state and runtime ownership remain absent

**Files:** Wallet.cpp, support/SqlConnection.hpp. Only the recording fixture supplies
connections. These tests cannot establish durable key-use updates, SQL constraints,
crash recovery or wallet encryption. Main/MinimaDB/TxPoWDB callbacks still require
actual runtime binding. Implement the native backend and validate key-use persistence
across close/reopen and interrupted writes against Java before using a live wallet.

### MAJOR — Complete concurrency and Java API parity remain open

**Files:** Wallet.hpp/Wallet.cpp, objects/keys/TreeKey.cpp. Shared seed/cache updates
from unsynchronized lifecycle methods need a native concurrency/lifetime audit with
the runtime owners; synchronized getters alone do not make those writes safe. Native
set references and visibility differ from Java HashSet replacement, and nullable
mutable GeneralParams strings, arbitrary UTF-16, pathological TreeKey parameters and
full logger notifications remain incomplete. Finish these APIs and test the actual
owner call paths. No new ThreadSanitizer run or full node startup was performed.

### Verdict

Request changes for full-port approval. The compiled wallet and specified comparisons
are component progress; the full native node remains unfinished.


## ArchiveManager and codec review

### Summary

ArchiveManager translates all active non-demo methods, its two tables, index and
fifteen prepared statements. It reuses SqlDB's recursive monitor and lifecycle,
TxBlock/Cascade codecs and the existing clock and logger boundaries. The source's
high/low two-query reads, strict range bounds, stable ordering, 256-block sync lower
bound, shutdown check after rows.next, one retry for saveBlock exceptions, partial
SQL-error results and unchecked cascade-replacement path are retained. Retention is
Java long arithmetic: NUMBER_DAYS_ARCHIVE is declared long, despite an earlier
continuation note saying int. Cascades are not revalidated on the replacement path;
a failed generic DELETE result does not prevent the subsequent INSERT in Java.

TxBlock conversion now catches only I/O exceptions. TxBlock and Cascade converters
accept the existing SQL logging callback; Cascade validation emits its source
level/parent diagnostics. Successful wire formats, pruning and validation decisions
are unchanged. All modified production and fixture files were read against source.

3,333 programs / 10,831 operations / 125,655 SQL effects pass in the normal build.
These execute actual pinned ArchiveManager, SqlDB, TxBlock and Cascade methods,
with a recording JDBC provider. Tests include SQL/runtime/null/partial initialization
failures, retry and reopen identities, cleanup overflow, stable equal-height blocks,
shutdown during iteration, cascade validation/replacement, failed serialization and
every truncated prefix of three block and three cascade encodings. Clock bindings
are validated against actual operation windows before normalization. Enhanced NPE
expressions and logger prefixes/stacks are excluded. The first corpus timed out
because it constructed every unused fixture for every case; constructing only
referenced inputs retained the comparisons and reduced runtime to 30.70 seconds.
All targets compile normally and with ASAN/UBSAN. Nine selected normal suites pass
in 171.61 seconds. The same eight supporting suites pass ASAN/UBSAN in 422.25
seconds; the final 3,333-program archive suite passes sanitized in 92.71 seconds.

### MAJOR — Native database and full runtime remain incomplete

**Files:** ArchiveManager.cpp, support/SqlConnection.hpp. Recording connections do
not execute SQL constraints, queries or durable writes. Main/MinimaDB ownership,
crash recovery, concurrent unsynchronized lifecycle operations and the destructive
source demonstration main remain incomplete. Complete the native backend and actual
runtime bindings, then compare persisted data and failures across restarts. The
sibling in-memory BlockStore and SQLite persistence class are incompatible with the
pinned archive schema and semantics; neither was substituted.

### MAJOR — Complete Java API and logging parity remain unverified

**Files:** ArchiveManager.hpp, Cascade.cpp, TxBlock.cpp. Source reference aliasing,
nullable/UTF-16 APIs, allocation-error versus Java Error behavior, concurrent object
mutation and full MinimaLogger notification/stack semantics still need review.
Only the stated component behavior is verified; no new ThreadSanitizer test or
production node startup was performed.

### Verdict

Request changes for full-port approval. The compiled archive and normal comparisons
are verified component progress; the full native node remains unfinished.


## MinimaDB coordination and ownership review

### Summary

MinimaDB follows the pinned constructor, singleton publication, component getters,
replacement, lifecycle/recovery/restart and save ordering. The implementation reuses
actual native database classes and file serializers. Reentrant read/write locks
preserve nested holds, writer-to-reader downgrade, wrong-unlock behavior and the
source's one-hold safeRelease and diagnostic-state semantics. The native lock
adapter follows the inspected JDK contract; no JDK implementation was copied.

Review found and corrected reference-lifetime hazards in new TxnDB and MegaMMR
search bindings. Their callbacks now return shared ownership; eight transaction
commands and the two MegaMMR search loops keep the returned owner while using it.
The standalone command and search fixtures use the same ownership contract.
A standalone-header compile also exposed a missing string include in JavaExceptions;
it is now explicit. Every changed production/fixture file and relevant callers
were inspected. General source object aliasing remains a separate unresolved issue.

2,253 Java/native programs compare 6,696 operations and 316,180 recording-driver
calls. They cover construction/identity replacement, path composition, nullable
notification keys, nested locks/ignore flag, file bytes, save gates, normal/encrypted
SQL URLs, reload/restore/restart, checked/runtime failure injection, source recovery,
rescue and startup-error callbacks. Timer and hard-cleanup timestamps are checked
against actual operation windows. Five selected normal suites pass in 58.47s;
MinimaDB itself passes in 35.99s. The native lock test exercises 4,096 operations
across eight threads, simultaneous readers, downgrade, wrong-thread unlock and
terminated-owner identity. Both all-target builds pass. The same five suites pass ASAN/UBSAN in 90.26s;
MinimaDB itself passes sanitized in 45.48s. No new ThreadSanitizer run.

### MAJOR — Native storage and complete runtime remain absent

**Files:** MinimaDB.cpp, support/SqlConnection.hpp, support/ChainServices.hpp.
Recording SQL validates coordination and driver calls, not H2 execution or durable
storage. Main, miner/processors and remaining runtime bindings are not complete.
Chain coin lists and block insertion callbacks still copy some source objects and
cannot preserve every null/alias case. MMR owner lookup is now connected. Complete
these connections and the native H2 engine before treating this as a working node.

### MAJOR — Full lifecycle and concurrency parity are not established

**Files:** MinimaDB.cpp, support/ReentrantReadWriteLock.hpp.
The fixture now includes nonempty chain/cascade objects and truncated state files.
Transaction-rich chain restoration, real halt-process behavior and concurrent
singleton/service rebinding require further validation. Component pointer replacement
is exercised separately; concurrent mutation of one MegaMMR remains unverified. The adapter's extreme-count failure
is a C++ overflow_error rather than Java Error; resource-error catches and exact
nonfair scheduling are not established. Complete those cases and run relevant
concurrency instrumentation before claiming full parity.

### Verdict

Request changes for full-port approval. This is compiled and differentially tested
component progress; 194 mapped files remain absent and no working full node is claimed.

## MMR runtime ownership and restoration review

### Summary

MMR fallback now obtains shared ownership from the current MinimaDB singleton and
retains it through lookup. MegaMMR owns its MMR through a shared pointer, preserving
held-object identity across clear and load. Existing algorithms and wire formats are
reused. MegaMMR load now emits the source file-size diagnostic. Truncated cascade
restoration exposed an EOF logging mismatch; MiniFile reuses JavaIOExceptionText to
report typed I/O errors. No comparison normalization was added to hide this mismatch.

Actual changed production and fixture files were inspected. All-target normal and
ASAN/UBSAN builds pass. Seven targeted suites pass in 94.39s and 229.34s respectively;
MinimaDB compares 2,367 programs, 7,519 operations and 334,711 recorded SQL calls
(34.83s / 53.50s). The added owner fixture checks last-owner transfer, held MMR reset
and 7,000 concurrent reads during 1,000 fully constructed component replacements.
File tests cover actual nonempty tree/cascade objects, MegaMMR time boundaries,
48 truncated file cases, partial state and file-size getters.

### MAJOR — Full mutation and object-alias parity remain incomplete

**Files:** objects/mmr/MegaMMR.hpp, objects/mmr/MMR.cpp, database/MinimaDB.cpp.
The replacement stress test publishes constructed MegaMMRs; it does not establish
safety for concurrent clear/load/mutation of one MegaMMR. Coin-table aliases and
value-copy APIs still differ from Java references. Arbitrary null fallback providers
and singleton/service rebinding are not fully verified. Preserve these limitations
until the actual runtime paths and their synchronization are implemented and tested.

### Verdict

Request changes for full-port approval. Scoped restoration and ownership changes
are compiled and verified; the full node and native H2 engine remain incomplete.

## TxPoWMiner review

### Summary

The pinned miner is translated using existing native Crypto, BigInt, TxHeader,
TxPoW and MessageProcessor implementations. BigInteger nonce bytes overwrite the
header at offset four; mining compares strict transaction difficulty. Shared
TxPoW/message identity is retained, including mutation of the source start-message
reference and the absent automine field on its end notification. Timeout/exception
reservations and stopped-loop ZERO-nonce finalization follow the pinned source.
Coin-list operations use a short mutex to avoid native container data races.

The sibling miner was rejected after full source/test inspection because it
reserializes integer nonces and directly updates a different chain model. The
new factory starts the existing owned worker after construction. Required Main
callbacks remain explicit; default generation, tip lookup and broadcast use the
actual native TxPoWGenerator, MinimaDB and NIOManager implementations.

Every new production/fixture file was inspected. Both all-target builds pass.
357 programs/1,352 operations compare real SHA3 output, full TxPoW bytes and IDs,
message snapshots and retained aliases, duplicate/shared input reservations,
post failures, null inputs, timeout recovery, mutation before dequeue, stopped
mining and signed automine timer boundaries. Log times and timer deadlines are
validated against operation windows before normalization. NPE expressions and
logger prefixes/stacks are excluded. The first fixture compile exposed unsupported
JSON convenience accessors; it now reuses the existing recording-fixture adapters.
Six selected normal suites pass in 38.54s; miner comparison itself takes 9.17s.
The same six suites pass ASAN/UBSAN in 126.45s; miner comparison takes 28.78s
and its native worker checks take 6.03s. No ThreadSanitizer run.

Native tests additionally mine 16 actual TxPoWs, stop during the start callback,
release the outside owner before worker completion and recover after a callback
exception. An explicit clock checks strict timeout equality and hash-rate arithmetic;
generator/broadcast routing is checked with supplied fixture callbacks. These
routing callbacks do not establish full generator/network runtime integration.

### MAJOR — Full shared-object synchronization and runtime remain incomplete

**Files:** system/brains/TxPoWMiner.cpp, objects/TxPoW.hpp.
Main start messages retain the mutable TxPoW while the miner updates it. Arbitrary
concurrent consumers can race on its fields; shared ownership alone does not
synchronize mutation. Complete the runtime's object access/synchronization contract
and validate it before starting a full node. GeneralParams mutation and simultaneous
mining of the same TxPoW are also outside current concurrency coverage.

### MAJOR — Complete Java API and failure semantics remain unverified

**Files:** system/brains/TxPoWMiner.cpp, objects/TxHeader.cpp, objects/TxPoW.cpp.
Generic wrong message-payload casts, header-only process messages, malformed-header
converters and resource failures retain existing native exception differences.
Specifically, pinned Crypto.hashObject catches Exception, logs and returns null;
its native value-returning template throws instead. TxHeader hash fields do not yet
represent that null result, so correcting this requires the shared object API rather
than changing only the miner.
Benchmark random/timing effects have native adapter coverage, not exact Java runtime
comparison. Complete these cases and default generator/network/Main bindings;
do not treat this component milestone as full-node parity.

### Verdict

Request changes for full-port approval. 264 counterparts exist, 193 mapped files
remain absent, and all are still classified partial/not_verified.


## TxBlock and checker ownership review

Reviewed actual TxBlock, TxPoWTreeNode, TxPoWChecker and TimedChecker source changes,
BlockFixture extraction, SharedCheckerFixture, both ownership oracles, the Python
comparison driver, deadline fixture and CMake registration. Shared TxPoWs now retain
identity through block construction and validation. Synchronous value adapters
borrow only for the duration of the call; timed adapters retain their original
vector owner. Review caught an empty-vector lifetime gap in that adapter; its
owned list now retains the vector even without element aliases.

TxBlock.readDataStream replaces its main TxPoW only after successful parsing,
preserves removed states and exposes partial collection restoration. Tests compare
every truncated prefix of a populated block, source read offsets, old/new aliases,
nullable constructor lists and first-match transaction lookup. Additional checker
programs compare repeated aliases, validation cache mutation and worker completion
on nullable inputs. Exact missing-transaction text is restored. NPE expressions
are normalized; checker log diagnostics are outside this comparison.

1,558 programs/6,219 operations pass normal and ASAN/UBSAN. Ten selected regression
suites pass in 264.38s normal and 589.95s sanitized, including both actual 120-second
worker deadlines. Those checks prove inputs survive the caller's timeout and are
released after worker completion. The shared-pointer list and value-vector adapter
are both covered. Both full builds passed before the subsequent processor addition.

Remaining: collection/state/Coin aliases are not fully represented; value overloads
copy inputs; parent peak-list identity and exact checker error/log order remain
incomplete. Shared ownership does not synchronize simultaneous object mutations.
Timed value-vector callers must not resize or mutate inputs while a worker uses
them. Full node integration, complete API parity and native H2 remain outstanding.

Verdict: scoped ownership changes verified; full-port approval remains withheld.


## TxPoWProcessor review

### Summary

All active bodies in the pinned processor are translated: posting gates, owned
message payloads, LIFO orphan/child processing, full-block timed validation,
TxBlock-node validation, synchronization, GHOST recalculation, cascading/archive
updates, on-chain flags, notifications and missing-transaction requests. Existing
MinimaDB, TxPoWDB, TxBlockDB, tree/MMR/checker, Stack and MessageProcessor code is
reused. Source cascade creation intentionally deep-copies headers. Main callbacks
are required; native singleton and NIOManager defaults resolve at each source call.
The owned factory starts its worker after construction. Atomic first-IBD/finished
fields provide native visibility for those two flags. Java finally ordering is
preserved with explicit cleanup, including exceptions from cleanup itself.

Reviewed actual production, support, oracle, Python and CMake changes. Native and
pinned Java tests execute the real processor/checker/tree/MMR using eight mined
Java block fixtures, a fork and a referenced empty transaction. RAM flags are real;
SQL indexing, wallet relevance, archive writes and external receivers are injected.
384 programs/1,434 operations pass in normal (24.05s) and ASAN/UBSAN (45.93s).
Coverage includes missing-parent/transaction retry, duplicate blocks, shared TxPoW
identity, normal and TxBlock-node paths, shutdown/restoring/override gates, main/
archive/backward IBD, nulls, invalid cascades, configured cascade depths, 257/1,001
batch boundaries, mismatched archive heights, notification order, and injected
receiver failures. A baseline guard requires an actual six-block chain to attach.
NPE expressions, logger prefixes/stacks and elapsed-time values are normalized.
IBD elapsed values are checked against operation windows before normalization.

Comparisons caught an existing TxBlockDB null-argument exception mismatch, now a
JavaNullPointerException while holding its source-method lock. Header validation
now checks chain ID before parent dereference and emits the pinned chain/height/
parents/time/difficulty/Magic rejection messages. Invalid-header cases cover both
processor modes and the source's 999/1,000-height range transition. Other checker
exception/duplicate-coin logging paths retain previously documented limitations.

Native worker checks exercise 18 FIFO messages, recovery from malformed IBD,
inherited cleanup, original block identity, and release of the processor after
its outside owner is dropped during a blocked callback. Already-running IBD
finishes after stop, matching the source. Twenty-four native explicit-clock cases
cover strict first-IBD age and recent-tip bounds; these are not injected-Java-clock
comparisons. Both all-target builds pass. Normal consensus/archive-cache/worker
regressions pass in148.36s; additional normal map/ownership/checker/processor tests
pass in52.33s. The same five map/ownership/checker/processor suites pass sanitized in135.70s;
the final sanitizer consensus/archive-cache regression also passed (462.05 s; archive cache 27.04 s).

### 🟠 MAJOR — Full runtime and mutable-object synchronization remain incomplete

**Files:** system/brains/TxPoWProcessor.cpp, database/txpowtree/TxPowTree.hpp,
database/txpowtree/TxPoWTreeNode.hpp, objects/TxPoW.hpp.
Main and native H2 are not integrated. Tree fields/children, parent links and
mutable TxPoWs do not yet have a complete cross-worker access contract. Shared
ownership prevents dangling objects but does not prevent concurrent data races.
Do not start a full node until its readers/writers are synchronized and the
actual persistence/network/runtime path has been validated. Current worker tests
serialize fixture database access; they are not a stress test of network readers
against a changing chain. No new ThreadSanitizer run.

### 🟠 MAJOR — Complete API and failure parity remain unverified

**Files:** system/brains/TxPoWProcessor.cpp and existing object/checker dependencies.
Transaction-rich chain processing, real storage recovery, nullable collection
identity and mutation, full header/body error semantics, Java identity-string
rendering and generic payload-cast diagnostics remain incomplete. The synthetic
archive confirms call ordering and block selection, not successful disk storage.
Native collection/resource/exception differences are not resolved by these tests.

### Verdict

Request changes for full-port approval. The processor subset is compiled and
compared; 265 counterparts exist and192 remain absent, all partial/not_verified.


## Notification manager and webhook command review — 267 counterparts

### Summary

The pinned NotifyManager and webhooks command now have compiled counterparts.
They reuse MessageProcessor, MessageObject/JSONObject ownership, UserDB/JsonDB,
MiniUtil and Command/CommandRunner. The notification constructor loads the actual
UserDB hooks. Its factory follows the existing native owned-worker convention.
The database receiver defaults to current MinimaDB; listener, HTTP and logging
receivers are required. This is a translated component, not a completed node.

Reviewed the actual new production headers/implementations, fixture/oracle,
worker, differential driver, changed UTF-16 adapters, exception type, registry
and CMake entries. Java fixture runs the unchanged pinned NotifyManager,
webhooks, CommandRunner and RPCClient. Only Main and HttpURLConnection receivers
are controlled. No remote webhook, wallet, miner or production node is started.

527 programs/2,667 operations match in normal (3.86s) and ASAN/UBSAN (4.21s).
Tests cover retained queued JSON, listener edits and message-payload replacement,
event filtering using the pre-listener event string, listener exceptions/logging,
first malformed hook abort, snapshot iteration, per-hook failures, stopping,
nullable/duplicate hooks, unconditional array replacement on every save,
DB rebinding/failure order, independent returned lists, Unicode/isolated surrogate
filters, exact command metadata, administrator and MiniDapp dispatch, typed/null/
blank parameters, exact remove strings and nullable manager lookup order.
JVM-generated null/cast expressions and logger prefix metadata are excluded.
HTTP fixtures use valid local fixture URLs; network/URL/TLS parity is not claimed.

A comparison exposed isolated-surrogate loss through the existing UTF-8 string-list
adapter. New lossless UTF-16 paths in MiniUtil/UserDB/Command retain those code
units through webhook storage, filtering and direct command parameters. Existing
UTF-8 adapters remain available for their current callers. Hook locking uses a
recursive mutex to preserve Java monitor reentrancy; error logging is atomic.
JavaClassCastException now represents invalid notification/event types explicitly.

Native worker tests cover 400 concurrent posts with concurrent hook edits, an
initial malformed message, continuation after delivery errors, finally cleanup,
retained original JSON, stopping in a blocked listener, and reentrant persistence
lookup. The first sanitizer lifetime assertion raced the interval between strong
owner expiration and member destruction; the test now waits for both manager and
JSON expiration. Ten repeated sanitizer worker runs pass (1.92s total).
Eight selected notification/preferences/message/command/registry suites pass in
normal16.05s and sanitized26.05s; the final added malformed-command cases pass in
the later 527-program runs above. Both final all-target builds pass. No ThreadSanitizer coverage.

### 🟠 MAJOR — Runtime receivers and shared mutable data still need integration

**Files:** system/network/webhooks/NotifyManager.hpp/.cpp,
system/commands/network/webhooks.hpp/.cpp, utils/json/JSONObject.hpp,
database/userprefs/UserDB.hpp.
The full Main/RPCClient/MinimaLogger runtime does not exist. The command receiver
must be bound at runtime startup before dispatch. NotifyManager protects its hook
list, but external concurrent UserDB writes and shared JSONObject mutation need
the node-wide synchronization contract. Worker tests do not authorize concurrent
unsynchronized mutation of queued JSON. Complete this contract and actual HTTP
transport validation before declaring a working node.

### 🟡 MINOR — Complete platform and API diagnostics remain incomplete

**Files:** utils/MiniUtil.cpp, system/commands/Command.cpp parameter adapters and HTTP/log receivers.
Existing UTF-8-only callers still cannot retain isolated Java surrogate code units.
The notification path has lossless list and parameter adapters, but complete
Java collection identity, wrong-type initial UserDB arrays, resource failures,
JVM constructor-thread timing, URL/TLS failure classes and logger formatting are
not fully validated by this component fixture.

### Verdict

Request changes for full-port approval. 267 counterparts exist (248 production,
19 mapped tests);190 remain absent and all counterparts remain partial/not_verified.


## RPC client and live notification transport review — 268 counterparts

### Summary

RPCClient's ten active public method forms compile and execute natively through
libcurl. Its comparison runs the unchanged pinned Java client against controlled
HTTP/HTTPS loopback servers and compares return values, stdout, request methods,
paths, header values and bodies. The notification manager now defaults to that
native POST implementation. Full native-node approval remains blocked by missing
implementations and the unresolved compatibility surfaces below.

Read the complete pinned RPCClient, MinimaRPCClient, MinimaLogger and NotifyManager;
relevant production callers; native RPCClient, CurlTransport, P2PEnvironment,
ByteIO/JavaIOExceptionText, NotifyManager, both RPC fixture implementations,
notify_http_worker, the Python comparison and changed CMake entries. Reused the
existing P2P curl transport, Base64, MiniString decoder, worker ownership and global
reference snapshot pattern. Inspected installed JDK connection, redirect, request
printing, URI/proxy selection and MeteredStream source. No JVM in production and
no JDK implementation source copied. Only loopback fixtures open connections.

### Findings resolved during implementation

#### MAJOR — Binary exception text was truncated

**Files:** support/ByteIO.hpp, support/JavaIOExceptionText.hpp.

The first POST-201 comparison retained the correct binary request body but lost
`NUL + end` from Java's error message through `what()`. IOException now retains
its complete string and the shared renderer reads that value. EOF rendering
remains unchanged. The NUL case stays in the comparison.

#### MAJOR — URL credentials changed authorization

**File:** src/org/minima/utils/RPCClient.cpp.

curl generated Basic authentication from URL user-info, unlike the pinned Java
client. Automatic HTTP authentication is disabled; explicit source API headers
still work. Username-only, username/password and percent-escaped user-info are
compared with all five relevant method forms. Cross-port redirects remove explicit
authentication and POST redirects reset headers according to inspected Java code.

#### MAJOR — Native mutable USER_AGENT could race with worker reads

**File:** src/org/minima/utils/RPCClient.hpp.

A public mutable optional string was unsafe to replace concurrently. The native
field adapter now uses the existing MinimaDB mutex/snapshot pattern, preserving
assignment and nullable reads. The native integration fixture stresses 4,000
assignments/readbacks on four threads before delivery. This is not TSAN evidence.

#### MAJOR — Source protocol failure behavior differed

**Files:** RPCClient.cpp, support/CurlTransport.hpp.

Added source-class transport exceptions, exact premature-EOF text, retry before
unexpected-101 rejection and the 20-redirect protocol error. A 305 proxy request
now removes curl's extra Proxy-Connection header. Raw initial request targets
retain empty queries, dot paths, CR and UTF-8. NUL URLs fail at Java's proxy-selection
phase instead of connecting to a truncated URL. The fixture preserves these cases.

### Remaining findings

#### MAJOR — Full Java networking and TLS behavior remains unverified

**Files:** src/org/minima/utils/RPCClient.hpp/.cpp, support/CurlTransport.hpp.

The native SSLContext is a connection-configuration callback, not a complete
javax.net.ssl provider/key/trust-manager implementation. Verified cases cover
rejection of an untrusted certificate and a caller-supplied CA for localhost/IP;
platform trust stores, cipher policies, mutual TLS, session reuse and all failure
orders remain unresolved. JDK-global proxy/authenticator/cookie/cache properties,
non-UTF8 default charsets, isolated UTF16 strings, rare URL forms and malformed
response parser behavior are not complete. NUL-containing custom header values
still meet curl C-string APIs and require a byte-preserving transport solution;
this is distinct from the fixed binary POST body and source error message.
Header order and selected VM/provider diagnostic text are deliberately normalized.
**Fix:** complete those source/runtime adapters and retain separate strict fixtures
before accepting full RPC parity. Do not infer complete behavior from case counts.

#### MAJOR — Native Main and persistent full-node integration are absent

**Files:** src/org/minima/system/network/webhooks/NotifyManager.cpp and node runtime.

NotifyManager's production HTTP default now delivers real requests and recovers
from source POST failures. Its Main/listener/log receivers still require full
runtime integration, as do native H2/MVStore persistence and other missing files.
Existing shared JSON/UserDB concurrency limits remain. Nothing in this milestone
starts a production node, wallet, miner or remote webhook.
**Fix:** continue the complete source mapping and runtime/persistence integration.

### Validation and verdict

477 Java/C++ cases compare 597 HTTP requests per runtime. Native queued delivery
adds two events, seven requests and two logged failures, checking listener mutation,
filtering, 307 redirects and continued delivery after HTTP 404. Ten related suites
pass normal62.58s and ASAN/UBSAN72.02s; the final USER_AGENT change is followed by
another focused build/test run, recorded in CONTINUATION.md and provenance.json.
All production and fixture targets compile in both configurations.

**Verdict: Request changes for full-port approval.** 268 counterparts exist
(249 production and19 mapped tests);189 are absent. Every counterpart remains
partial/not_verified. The full node is not implemented.


## Logger and native notification logging review — 269 counterparts

### Summary

MinimaLogger now translates the source string, exception and uncaught-exception
logging sequence. Native heap/clock/stdout defaults work without Main, and
NotifyManager now uses it by default. The complete pinned logger, RPC CLI, relevant
Main notification methods, existing MiniFormat/MiniUtil/date/socket readers and
the new production/fixture files were read. The calendar computation was extracted
from the tested JavaDateString implementation; existing NIO date behavior remains
covered by its unchanged comparison. Full-port approval is still unavailable.

### Findings

#### MAJOR — Releasing a runtime receiver under the registry lock could deadlock (fixed)

**File:** src/org/minima/utils/MinimaLogger.cpp.

Replacing std::function callbacks could destroy their captured receiver while
holding the runtime mutex. A destructor that logs would reacquire that mutex.
The registry now swaps immutable shared Runtime snapshots under the lock, then
releases retired owners and copies callbacks outside it. A native fixture removes
the last owner of a receiver whose destructor logs, and verifies reentry succeeds.

#### MAJOR — Throwable capture and full mutable DateFormat are incomplete

**File:** src/org/minima/utils/MinimaLogger.hpp.

The logger consumes explicit Throwable text and recorded frame providers. It
preserves toString/getStackTrace/log ordering, null array/element failures, five
spaces before each frame and per-line notification. It does not yet provide
throw-site frame capture for every native standard/custom exception. The DateFormat
adapter implements the source fixed pattern and captures the initial default zone;
it is not a complete mutable java.text.SimpleDateFormat API. The clock and heap
are native measurements, so JVM heap-dependent prefix text is normalized in the
comparison. macOS allocator statistics were read from the installed SDK declarations;
the glibc mallinfo2 branch is not compiled/tested on this machine.
**Fix:** finish the native throwable/runtime and required formatter APIs before
claiming full class/API parity. Do not fabricate catch-site frames as throw-site frames.

#### MAJOR — Main startup and full-node receiver wiring are absent

**Files:** MinimaLogger.hpp and system/network/webhooks/NotifyManager.cpp.

The logger correctly has no Main receiver before binding. Its real notification
worker integration uses a controlled Main receiver; it is not proof of a complete
Main lifecycle. The full native H2/MVStore backend and remaining mapped files are
also absent. Native runtime bindings must be cleared as part of orderly shutdown
before destroying captured receiver state.
**Fix:** complete actual Main/runtime/persistence integration and full-node tests.

### Validation

2016 cases run the unchanged pinned logger with controlled DateFormat/PrintStream/
Main receivers and explicit Exception frame state. Heap-dependent prefix and
automatic NPE details are normalized. Four process default zones cover UTC, London,
New York and Tokyo; 384 cases compare the original pinned DATEFORMAT on selected
and seeded signed-long timestamps. Rebinding before print/post, nested logs,
callback errors, null text, unpaired UTF16 and shared JSON mutations are covered.

Native checks verify 400 concurrent print-before-notify pairs, exactly800Mainlookups,
second-lookup disappearance, heap subtraction wrap/order, real native heap/clock/
stdout defaults, receiver retirement/reentry, and a listener failure producing a
queued MINIMALOG through actual NotifyManager/default logger. No TSAN was run.

Seven suites pass normal57.40s and ASAN/UBSAN130.06s, including NIO714cases/13972ops,
notification, RPC and logger checks. Both final all-target builds pass. After the
receiver-lifetime correction, final normal logger tests pass3.20s; the sanitized
seven-suite run reaches its logger/worker tests after the corrected binary build
and passes them3.47s/0.79s. The source map validates269counterparts/188absent.

### Verdict

Request changes for full-port approval. This milestone provides native logger
behavior and tested notification wiring through the available receiver boundary;
it does not establish a working complete node or full exception/formatter parity.


## RPC command-line client and native exception capture review

### Summary

The pinned MinimaRPCClient active algorithm now has a native counterpart and
`minima-rpc` executable. It directly reuses the port's RPC, JSON, formatter,
logger and character decoder. All new production and fixture files and the
modified common exception/reader boundaries were inspected. The native client
runs without Java; Java is used only as the comparison oracle.

### Findings

#### MAJOR — Sticky socket EOF prevented terminal recovery (fixed)

**Files:** src/support/SocketCharacterReader.hpp and utils/MinimaRPCClient.cpp.

The shared reader remembered EOF permanently, appropriate for a closed socket.
Java StreamDecoder uses a per-read EOF flag and can read subsequent terminal
input after Ctrl-D. The stdin caller now explicitly retries EOF on each readLine;
existing socket callers preserve their prior behavior. A real pseudo-terminal
sends Ctrl-D followed by exit and both original Java and native executables exit
successfully. Pipe checks also prove requests are processed before EOF, and
permanent EOF retains the source loop rather than silently exiting.

#### MINOR — Comparison normalization could hide deeply nested JSON (fixed)

**File:** tools/minima_rpc_cli_differential.py.

An initial normalizer treated any five-space prefix as an exception frame. It
now only removes frames from logger-prefixed lines or stderr stack output, and
preserves JSON whitespace and control characters. A nested-object case exercises
six/eight-space indentation. Native/JVM frame symbols and frame counts remain
explicitly normalized, while frame presence is compared.

#### MAJOR — Full throwable, runtime and node integration remains incomplete

**Files:** support/NativeStackTrace.hpp, support/NativeThrowable.hpp and the port.

Construction-time native frames now survive copies/rethrows for common typed
exceptions, wire errors and ParseException. Uncaptured standard exception types
still have no original frames; causes/suppressed exceptions, complete JVM error
classification/resource failures and formatter/PrintStream/global-state behavior
are not implemented. Native compiler/unwinder frame names/counts cannot be treated
as literal JVM frame equivalence. Default stdin/output are POSIX/native streams;
full node Main startup and native H2/MVStore remain absent. No node or wallet was
started to claim this client milestone.

### Validation

89 cases invoke the unchanged pinned Java main and native main against loopback
HTTP: 70requests/runtime, nullable arguments, UTF16 surrogate replacement, form
encoding, authentication defaults, JSON/status/error paths, exact banner content,
interactive trim/quit, recoverable read errors and closing errors. Ten executable
checks cover help/unknown/missing-argument exit status, actual stdin, EOF and Ctrl-D.
All pass normal and ASAN/UBSAN. A native test checks 160+recorded frames and an
original throw-function symbol, copy/throw retention, NUL-containing IO messages,
parser and typed-error capture.

Both all-target builds pass. Ten affected normal suites pass53.79s and twelve
sanitized suites pass128.36s. After the terminal EOF correction, four reader/CLI
suites pass13.32s normal and14.11s sanitized, including finalCLI5.47s/5.75s. Source
mapping now has270counterparts/187absent, all partial or unported.

### Verdict

Request changes for full-port approval. The native RPC executable is usable within
this tested scope; this does not establish a complete native Minima node.

## Startup configuration and test parameters review

### Summary

The complete active ParamConfigurer consumer table and TestParams assignments now
compile against the existing native parameter fields, logger and RPC client.
The new parser preserves the source's argument lookahead, staged collection,
precedence and failure-side effects. All changed production/fixture files, the
original five tests and relevant pinned/JDK source dependencies were inspected.
The port remains incomplete; this is startup configuration, not a running node.

### Findings

#### MAJOR — Numeric argument construction rejected Java digits (fixed)

**Files:** objects/base/MiniNumber.cpp and support/JavaInteger.hpp.

The existing decimal regex accepted only ASCII digits and raised a generic
invalid_argument for invalid text. The constructor now reuses the existing BMP
Unicode digit recognition, then applies its existing decimal grammar and numeric
logic. Invalid decimal/exponent/scale inputs use NumberFormatException. The same
checked digit accumulator now supports signed 64-bit startup settings, including
Long.MIN_VALUE without signed overflow. Full numeric resource/extreme-scale
behavior remains a broader port limitation.

#### MAJOR — Bulk map loading could change consumer order (fixed)

**Files:** support/JavaConcurrentMap.hpp and params/ParamConfigurer.cpp.

A fresh Java HashMap loaded by putAll can start below sixteen buckets. The shared
map now implements that pre-sizing while preserving previously allocated table
capacity. Enum keys use stable native identity hashes and ordinal tie ordering;
the comparison binds actual Java enum hashes to assess the exact map traversal
and resulting global changes. Native and JVM identity hash generation itself is
not claimed to produce identical values across processes.

#### MAJOR — Directory creation had additional side effects (fixed)

**File:** support/JavaUnixFile.hpp.

std::filesystem::create_directories created an extra intermediate directory for
`dot/../inside`. The Java-style helper tries mkdir first, then resolves a canonical
path before creating missing parents. Stored parameter paths retain their original
absolute spelling. Separate Java/native directory snapshots cover missing parents,
repeated separators, dot segments, symlinks, a file used as a parent and NUL paths.

#### MAJOR — Configuration reads reported errors in the wrong order (fixed)

**Files:** support/StrictUTF8CharacterReader.hpp and params/ParamConfigurer.cpp.

Decoding one line at a time reported a duplicate-key error before malformed UTF-8
that Java discovers while filling its character buffer. The strict reader now
validates the fetched 8192-byte block before publishing its decoded characters,
retains incomplete prefixes for the next read and reuses the existing line reader.
Read-boundary tests compare duplicate versus malformed-input precedence. POSIX
open/fstat/read also preserve a directory-read IOException that the original
std::ifstream path incorrectly treated as empty input. Opening errors retain the
source print/exit behavior; traversal errors remain UncheckedIOException.

#### MAJOR — Native node integration and full runtime parity are outstanding

**Files:** system/params and the remaining port.

Actual global settings are changed, but Main startup and native H2/MVStore are
absent. GeneralParams/GlobalParams still lack complete nullable/isolated-UTF16,
reference and concurrent-update semantics. Reentrant map traversal, all resource
failures, JVM-global stream/locale behavior and huge/special/changing configuration
files are not fully established. Native configuration descriptors close through
RAII; source Files.lines streams are not explicitly closed and collection timing
is unspecified. These limitations preclude full-node or full-API approval.

### Validation

719 programs exercise all67handlers, map state/order, global/TestParams settings,
repeat configuration and failure-side effects. Three executable checks compare
help output/status, unreadable-file exit and first-use TestParams version capture.
Four actual loopback HTTP requests per runtime cover peer-list success, status
failure and socket retry/error logging. The original five test methods execute
unchanged with fresh GeneralParams state between methods, and the mapped C++ five
methods also pass. NPE/index and numeric diagnostic text is normalized; classes,
duplicate/decoding diagnostics, resulting state and source content are compared.

Eighteen affected suites pass149.99s normal and259.21s ASAN/UBSAN. Both final
all-target builds pass. Final comparison after switching the fixture to the
production exception renderer passes719cases in5.07s/7.88s, including NUL-bearing
unknown-argument messages.

### Verdict

Request changes for full-port approval. Startup parameter behavior now has native
implementations and meaningful comparisons, but273counterparts/184absent is not
completion or a behavioral-parity percentage.


## Uncaught exception handler and worker boundaries — 274 counterparts

### Summary

The pinned MinimaUncaughtException implementation now has a compiled native
counterpart. It reuses the existing logger, recorded native exception frames and
worker ownership. The shared captured default changes on each constructor, while
last-error suppression remains per instance. Nullable inputs and first/second
toString calls retain source ordering. Fatal memory, MVStore and timeout types
invoke immediate native _Exit(0), bypassing graceful shutdown and exit handlers.

### Findings

#### MAJOR — Worker failures were incorrectly swallowed (fixed)

**Files:** src/org/minima/utils/messages/MessageProcessor.cpp and TimerProcessor.cpp.

The old outer catches printed to clog and set completed/stopped flags even when
Java would terminate the thread through its uncaught handler. The native thread
boundary now dispatches those failures. MessageProcessor uses the actual logger
for serious errors and ordinary exceptions, runs cleanup even if logging fails,
and allows a cleanup exception to replace the pending exception. Startup and
shutdown logging failures preserve the source running/completion state. An orphan
timer now raises NullPointerException before removing its pending entry.

#### MINOR — Empty native carrier messages lost the trailing separator (fixed)

**File:** src/support/JavaExceptions.hpp.

The new fatal-type carriers preserve a supplied empty message as an empty string,
not a null message. Existing two-argument JavaTypedException formatting is retained
for prior callers. Dynamic native type checks include subclasses and std::bad_alloc;
a throwable merely displaying an OutOfMemoryError class name is not classified
as a memory error.

#### MAJOR — Full runtime integration remains incomplete

**Files:** support/NativeThread.*, Minima startup and remaining worker launch sites.

MessageProcessor and TimerProcessor factories use the new boundary. The following
HTTP/executor milestone adds their worker integration; NIOManager server/connect
threads and TimedChecker remain outstanding. Native frames differ from JVM frames;
standard exceptions without recorded frames and foreign C++ throws do not acquire
fabricated catch-site frames. Allocation failure while materializing/logging an
exception, JVM thread groups/per-thread overrides, causes/suppressed exceptions,
exact VM stderr diagnostics, interruption semantics and the Java memory model are
not fully implemented. Native thread metadata is borrowed during handler calls.
Timer receiver ownership remains a weak native reference, and callback reentry
through nonrecursive timer locks still requires review. H2/MVStore and full Main
startup remain absent. No production node or wallet was started.

### Validation

121 pinned Java/C++ handler programs pass, with exact callback/log ordering,
shared-default replacement, duplicate suppression and nullable/failing inputs.
Six isolated fatal-process checks (three types, directly and on a native thread)
verify exit status 0 and skipped exit hooks. Thirteen real worker scenarios compare
exception/error handling, cleanup precedence and termination state. Native checks
cover late default lookup, actual thread identity, subtype classification,
reentrant registry retirement, empty messages, foreign throws and stderr fallback.

Both final all-target builds pass with no compiler warnings or errors. All 28
affected regression suites pass: 145.07s normal and 281.49s ASAN/UBSAN. The new
uncaught differential takes 4.81s/6.10s; native thread checks take 0.31s/0.48s.
Native/JVM frames, automatic NPE diagnostic details, allocator prefixes and
elapsed worker trace time are not asserted identical. Actual source classes are
compiled unchanged; Main, database release, logger inputs and explicit throwable
frames are controlled only in test fixtures.

### Verdict

Request changes for full-port approval. This is scoped, tested progress toward
the active goal. 274 counterparts and 183 absent files do not establish full parity.


## HTTP and executor thread integration — 274 counterparts

### Summary

HTTPServer listener and daemon request workers now use NativeThread. Logging uses
the actual native MinimaLogger, and escaping runtime/Error failures reach the
registered default handler. FixedTaskExecutor retains its queue, worker ownership
and drain behavior while dispatching escaped task failures after worker exit and
replacement. There are no new mapped source counterparts in this milestone.

### Findings

#### MAJOR — HTTP runtime failures were caught where Java leaves them uncaught (fixed)

**File:** src/org/minima/system/network/rpc/HTTPServer.cpp.

The generic outer catch and detached-handler catches hid failures from the default
handler. They are replaced by the source BindException, SocketException and
IOException branches plus the native thread boundary. Bind failures always log;
socket failures log only while running; other I/O failures also log after shutdown.
Logger failures escape the corresponding catch. Invalid port diagnostics use the
source IllegalArgumentException text. Native SocketException/BindException carrier
subtypes reuse TypedIOException and the existing exception renderer/frame capture.

#### MAJOR — Executor task failures bypassed the default handler (fixed)

**File:** src/support/FixedTaskExecutor.hpp.

The old worker caught failures, logged them and silently exited. It now clears the
task, updates worker ownership, starts a replacement when required, wakes termination
waiters and rethrows through NativeThread. A replacement-start failure replaces the
original task failure, matching finally behavior. DefaultThreadFactory-style names
use a global pool counter and a per-pool worker counter with signed Java int wrap.
An executor may be terminated while an uncaught handler remains blocked. The new
comparison explicitly verifies that source behavior.

#### MAJOR — Other runtime boundaries and full node integration remain unfinished

**Files:** NIOManager.cpp, NIOServer.cpp, TimedChecker.cpp, TxPoWChecker.cpp and Main.

At this milestone NIO server/connect threads and the timed checker still needed
direct boundary wiring and correct Error-versus-Exception catches in dependencies.
The following NIO milestone resolves the tested server/connect paths. HTTP close-error
callback reentry, unusual OS resource failures, concurrent repeated starts and
complete JDK thread groups/priority/interruption behavior remain unverified.
Native socket errors retain platform-specific diagnostics outside the explicit
carrier cases. Main, H2/MVStore and the full node remain absent.

### Validation

Thirteen affected normal suites pass across three runs (13.77s,29.58s,43.11s).
These include the existing binary HTTP listener/concurrency and RPC command tests,
NIO transport/state/message comparisons, network ownership, source map and native
thread checks. Thirty new HTTP cases compare actual pinned listener/daemon threads,
factory/handler/runtime/Error/OOM/I/O/socket/bind failures, logger failures, invalid
ports, direct-run propagation and running/bound/completion state. Ten executor
cases compare actual JDK fixed pools, late default replacement, replacement-thread
identity, queue drain and termination while the default handler is still running.
Source event order and names are compared; generated Thread counters and automatic
native/JVM frame lines are excluded from the HTTP comparison.

Both final all-target builds pass without compiler warnings/errors. All sixteen
sanitizer suites pass in 173.36s, including HTTP failure cases in 6.76s and executor
cases in 3.19s. Normal comparisons take 4.53s and 2.83s respectively. All new result
files report zero failures; source-map and whitespace checks pass.

### Verdict

Request changes for full-port approval. The HTTP and executor boundaries have
compiled implementations and targeted evidence; the overall goal remains active
with 274 counterparts and 183 absent files, all still classified partial/unverified.

## NIO server and connector thread integration — 274 counterparts

### Summary

NIOManager's actual server and connector factories now use NativeThread. The
existing transport, connection ownership and queues are retained. NIOServer uses
MinimaLogger and distinguishes ordinary exceptions from JavaError/out-of-memory
failures in its source catch boundaries. NIOClient.disconnect preserves the same
Error distinction. No mapped counterpart was added in this milestone.

### Findings

#### MAJOR — Server catches incorrectly swallowed Error failures (fixed)

**Files:** src/org/minima/system/network/minima/NIOServer.cpp and NIOClient.cpp.

The old std::exception catches hid JavaError and native allocation failures.
Server send/broadcast, accept, read/write, outer-run and client cancellation now
rethrow those failures. Ordinary exceptions retain the source catch behavior.
Server logs use NativeThrowable rather than untyped what() output. Trace messages
use notify=false; catch and fatal-bind logs use the default notification behavior.
A failed catch logger escapes before the source's final running=false assignment.

#### MAJOR — Connector and server launchers could bypass default dispatch (fixed)

**File:** src/org/minima/system/network/minima/NIOManager.cpp.

Both actual launchers now wrap their work with NativeThread and retain the owner
through dispatch. The connector preserves successful socket registration/pending
removal before logging, ordinary success-log failures entering the reconnect
catch, and Error failures bypassing that catch. A failure in the catch logger
prevents reconnect posting and reaches the default handler. Unnamed worker names
are allocated before launch, using the existing native thread-name adapter.

#### MINOR — Fixture observations needed synchronization (fixed)

**File:** support/nio_manager_errors.cpp.

The detached connector's owner count is used only as its completion observation.
Pending registration inspection also acquires the production registration mutex;
message draining and event recording retain their existing locks. Server fixtures
join the actual worker before inspecting pending lists. Controlled client/runtime
ownership is released without a shared-reference cycle.

#### MAJOR — Full runtime and remaining catch semantics are incomplete

**Files:** NIOManager.cpp, NIOClient.cpp, NIOServer.cpp, TimedChecker.cpp and Main.

Other NIOManager methods still have legacy logging and broad catches, including
sendPingMessage and sendSerialized's traffic callback. createNIOMessage currently
catches std::bad_alloc but still needs explicit JavaOutOfMemoryError coverage.
NIOClient read/write trace and oversize logs remain legacy output. NIOServer's
native selector closes on normal shutdown while the Java selector remains open;
resource retention, exceptional cleanup, callback reentry and cancellation races
are not fully equivalent. Platform socket diagnostics are not all JDK diagnostics.
TimedChecker and its checker dependency still need Error-boundary work. Main,
native H2/MVStore and the requested complete node remain absent.

### Validation

Both all-target builds pass. Ten distinct normal affected suites pass in two runs
(80.67s and 47.98s). Sixty-three new cases compare actual pinned NIOServer/NIOClient
with native selector workers and loopback sockets: startup, accepted/outgoing
registration, decoded messages, explicit disconnection, direct/broadcast send,
key cancellation, runtime/Error/OOM/I/O failures, logger failures, trace notification
flags, event order, running state, retained clients and pending queues. Automatic
native/JVM stack lines and generated client identifiers are excluded.

Twelve cases compare the actual private Java connectAttempt runnable against the
native connector, using invalid ports and loopback success plus success/catch
logger failures. Four additional native checks exercise the actual manager server
factory with invalid-port catch logger failures. The explicit fixture handlers
prevent fatal OOM halt. Java Main is controlled, not started; the connector uses a
pre-shutdown server solely for its selector and registration queue. Generated
thread counters and ephemeral ports are excluded. New normal reports contain no
failures. All thirteen sanitizer suites pass in 285.25s, including the 63 server
cases in 11.93s and the 12 connector/four factory cases in 4.66s. Both new sanitizer
reports have passed=true and zero failures. No compiler warnings/errors appear in
the final build logs. Source-map and whitespace checks pass.

### Verdict

Request changes for full-port approval. The tested NIO boundary paths now have
compiled implementations and parity evidence; the goal remains active and
incomplete at 274 counterparts and 183 absent mapped files.

## Timed checker and block validation catches — 274 counterparts

### Summary

TimedChecker now uses NativeThread for its actual validation worker, preserving
asynchronous input ownership and the source's persistent finished/valid flags.
TxPoWChecker restores catch and rejection logging and visits all transactions
before testing duplicate coin IDs. No new mapped counterpart was added.

### Findings

#### MAJOR — Error failures changed finished/valid state incorrectly (fixed)

**File:** src/org/minima/system/brains/TimedChecker.cpp.

The previous std::exception worker catch swallowed JavaError and allocation
failures, then set valid=false and finished=true. Those failures now escape to
NativeThread before either assignment. Ordinary exceptions log before valid=false,
and a failure in that log also escapes before assignments. The outer catch follows
the same Exception/Error distinction. The final BLOCK_LOGS call remains outside
the catch and dereferences a null block as Java does. Shared state and original
parent/block/list/ancestor ownership remain in place after the caller returns.

#### MAJOR — Block validation skipped later failures after duplicate IDs (fixed)

**File:** src/org/minima/system/brains/TxPoWChecker.cpp.

The old loop returned on the first duplicate, skipping subsequent transaction
accesses. Java gathers all main/burn proof IDs before constructing its uniqueness
set. The native block checker now preserves that order; a duplicate followed by a
null transaction reaches the source exception path. Header, maximum transaction,
duplicate and MMR mismatch logs now use MinimaLogger. Block and header-only catches
log the source prefix and then the exception; Error and catch-log failures escape.
The separate basic-check helper is unchanged.

#### MINOR — Reused-checker comparisons depended on relative worker speed (fixed)

**Files:** support/TimedCheckerErrorsOracle.java and timed_checker_errors.cpp.

Seven initial cases had matching logs/final flags but different return values:
Java completed within the 50ms poll, while the native worker completed later.
Resetting production flags would have violated the source. Fixtures instead use
known matching initial/final results for ordinary rejection, prime true for
escaping failures, and hold a worker until the checking call has returned to prove
the retained-result behavior. That last case uses an explicit release signal,
replacing an initial fixed delay. Fresh valid/invalid and the real 120000ms Error
deadline separately verify unprimed state. Final signal-based reruns pass in both
builds, with all 91 cases and zero failures.

#### MINOR — Processor fixture missed actual logger output (fixed)

**File:** support/txpow_processor.cpp.

The fixture captured only std::clog. After the checker switched to MinimaLogger,
its chain-rejection message was absent from the captured list, although processor
state and events matched Java. The native console receiver now forwards stripped
logger bodies into the existing capture, excluding automatic frame lines as the
Java fixture already does. Java expectations and production processor code remain
unchanged. The corrected comparison passes 384 programs and 1434 operations.

#### MAJOR — Complete checker and node equivalence remain unproven

**Files:** TimedChecker.cpp, TxPoWChecker.cpp and Main.

Native interruption semantics and Java's stored checker Thread field remain
unimplemented. Thread creation/resource failures, concurrent mutation and complete
reference APIs are not established. Other basic/MMR/script logging paths remain
legacy output. A concrete remaining diagnostic mismatch exists for an empty MMR:
Java MMR.getRoot returns null and block validation dereferences it, while native
validation currently throws invalid_argument("Empty block MMR"). This needs an
explicit source comparison and correction. The new comparison excludes generated
NPE expressions while retaining exception category and log order. Main, native
H2/MVStore and the full node remain absent.

### Validation

The targeted suite passes 91 Java/C++ cases; a separate fresh-Error case runs the
actual two-minute deadline in both implementations. Source-generated serialized
blocks and a duplicate-proof transaction drive actual validation. Normal consensus
passes 2890 cases; block ownership passes 1558 programs/6219 operations; corrected
processor validation passes 384 programs/1434 operations. Existing value and shared
list ownership deadlines both pass in 120.78s. Eleven distinct affected normal
suites pass after the fixture correction. All thirteen sanitizer suites pass in
557.30s, including 2890 consensus cases (557.29s), processor comparison (55.37s),
ownership comparison (182.99s), value/shared ownership deadlines (122.94s/121.32s)
and fresh-Error deadline (125.72s). The final release-signal fixture passes all
91 cases in both builds (normal 20.19s; sanitizer direct runner also passes).
New targeted, deadline, consensus, processor and ownership reports were inspected;
all have zero failures. All production and final fixture builds pass without
compiler warnings/errors. Source-map and whitespace checks pass.

### Verdict

Request changes for full-port approval. These paths have targeted implementation
and evidence; the goal remains incomplete at 274 counterparts and 183 absent files.


## Checker rejection behavior and student documentation — 274 counterparts

### Summary

The remaining basic/simple/script/MMR/signature rejection paths now use the real
MinimaLogger and preserve logger-failure propagation. The differential harness
executes the pinned Java checker on source-generated wire fixtures, comparing
ordered logs, exception categories, cache flags and transaction IDs. This is
scoped parity evidence; no new counterpart or full-node startup was added.

### Findings

#### MAJOR — Missing transaction logs changed failure ordering (fixed)

**File:** src/org/minima/objects/Transaction.cpp.

The first checker comparison exposed missing amount/token rejection messages
inside Transaction.checkValid. Omitting them shifted the first observable logger
failure into TxPoWChecker's subsequent transaction-JSON log. The native method now
calculates input totals before output totals, emits each source rejection message,
and preserves output-token collection and Hashtable enumeration order using the
existing JavaHashtable helper. The ObjectOracle native fixture suppresses this
scoped logger output, matching the Java fixture's existing behavior.

#### MAJOR — Native container exceptions replaced source categories (fixed on tested paths)

**Files:** TxPoW.hpp, keys/Signature.hpp and kissvm/Contract.cpp.

Body-dependent getters returned bad_optional_access for header-only TxPoW objects;
missing token data failed similarly during Contract.setGlobals. Empty signature
root lookup produced a native vector-range diagnostic. These paths now throw
native typed exceptions representing the corresponding Java null-pointer or
index-out-of-bounds category. The new comparison retains exception categories and
normalizes only Java-generated NPE expression diagnostics.

#### MAJOR — Checker rejection events and nullable MMR order differed (fixed on tested paths)

**File:** src/org/minima/system/brains/TxPoWChecker.cpp.

The checker restores the exact ordinary/token script failure labels and selected
complete trace logs, proof and signature failures, and duplicate-ID diagnostics.
The duplicate set uses the existing JavaHashMap capacity/traversal implementation;
fixtures cross set-allocation boundaries and collect IDs from both witnesses.
Simple-check forwarding now accepts an MMR pointer; block callers avoid forming a
reference to null before earlier difficulty/size checks can reject. Empty
reconstructed MMR roots now reach the source null-pointer catch path.

#### MAJOR — Complete file behavior remains unverified

**Files:** Transaction.cpp, TxPoW.cpp/.hpp, Contract.cpp, TimedChecker.cpp.

This change does not establish every API, alias or exception path. For example,
Transaction.calculateStateSize still has a broad native catch without the source's
logging behavior. TxPoW still lacks the full in-place stream-read API, and several
body setters retain native optional-access behavior. Contract's trace-enabled
logging and broad catches remain separate parity gaps. Native interruption and
full Java Thread semantics remain incomplete. Main and native H2/MVStore are absent.
The student guide must distinguish these limitations from passing scoped tests.

### Validation

Normal full build passes. The first checker corpus failed78 of1185 comparisons;
production corrections and expanded fixtures now pass1343 comparisons from100
source fixtures. Timed failures pass105 comparisons, including empty MMR roots and
nested logger failures. Twelve additional normal suites pass in152.58s, including
9244 object comparisons,4471 VM comparisons,2890 consensus comparisons,135 HashMap
cases,1558 ownership programs/6219 operations and384 processor programs/1434
operations. All six broader JSON reports and both targeted reports were inspected
with zero failures. Fourteen distinct affected normal suites pass. The full
ASAN/UBSAN build passes; all 14 selected suites pass in 492.87s. Its 1,343 checker
comparisons and 105 timed failure cases have zero failures, as do the six broader
JSON reports inspected after completion. Final normal
and sanitizer build logs contain no compiler warning/error lines.

### Verdict

Request changes for full-port approval. The corrected rejection paths have
meaningful source comparisons, while complete file parity and a working native
node remain outstanding. The accompanying student lessons are being backfilled;
page count is not implementation completion.


## ScriptValueTests mapped counterpart — 275 counterparts

### Summary

Added the missing native test file with all ten assertions from the original
ScriptValueTests.testConstructors, in source order. Existing Value and StringValue
production code is reused. Direct original-JUnit invocation follows the proven
ParamConfigurerOriginalTests approach with the pinned bundled JUnit dependency.

### Findings

No additional defect was found in the inspected cases. Bracket parsing through
Value.getValue differs deliberately from literal StringValue construction, and
both concatenation orders and all four type checks are preserved. This is a
mapped-test addition, not a new runtime subsystem or full StringValue certification.

### Validation

Full normal and ASAN/UBSAN builds pass. The mapped executable, unchanged original
Java test comparison, and source mapping pass in both builds. Both JSON reports
were read: one original method and ten assertions per runtime pass. No production
code changed after the preceding 14-suite normal/sanitizer verification, so those
suites were not repeated for this test-only addition. The student lesson now
explains the native mapping, original Java execution and observed results.

### Verdict

Approve the scoped mapped test. Request changes for full-port approval: 275
counterparts exist, 182 remain absent, and the full native node is incomplete.


## HexValueTests and numeric constructor exception type — 276 counterparts

### Summary

The mapped HexValueTests preserves all active constructor/comparison cases. Source
inspection found and corrected generic native argument exceptions where Java
requires NumberFormatException for negative and fractional numeric construction.

### Findings

#### MAJOR — Numeric HEX constructor exposed the wrong category (fixed)

**File:** src/org/minima/kissvm/values/HexValue.cpp.

Both numeric validation failures now use the existing JavaNumberFormatException
with their original messages. The mapped test checks that exact native exception
type. Malformed MiniData text already used it. No numeric conversion algorithm or
wire format was replaced.

The comparison fixture preserves six source construction routes, including the
literal hexadecimal10 versus decimal10 difference, and the repeated same-direction
inequality assertion. Commented ordering assertions remain inactive. The original
JUnit methods run unchanged; the shared driver separates class-specific build and
report paths. These tests do not certify all public API/reference/error semantics.

### Validation

Full normal and ASAN/UBSAN builds pass without compiler warning/error lines.
Eight affected suites pass in23.81s/85.21s. Both original-value reports pass (Hex:
2methods/35assertions per runtime; Script:1/10), and both builds' value1451, VM4471
and object9244 comparison reports have zero failures. The final SAN object report
was read after its process completed. Source mapping passes at276counterparts,
181absent. Student production/test lessons record the exact change and evidence.

### Verdict

Approve the scoped correction and mapped test. Request changes for full-port
approval: full node startup, native storage and remaining mapped files are unfinished.


## NumberValueTests and zero-division errors — verified

### Summary

The mapped test preserves the source's 99 executed assertions and the exact
ArithmeticException expectation. A shared decimal helper correction restores
Java's exception category and zero-versus-nonzero dividend message distinction.

### Findings

#### MAJOR — Zero division exposed generic runtime errors (fixed)

**File:** src/org/minima/objects/base/MiniNumber.cpp, divideDecimal.
The helper now uses JavaArithmeticException, built on the existing typed exception
and stack-capture machinery. The installed JDK BigDecimal.divide(MathContext)
branch and pinned MiniNumber caller were read. Successful arithmetic is unchanged.
Seven extra source/native observations cover zero, positive and negative dividends,
scaled zero divisors and a negative power of zero. Full output matches exactly.

The mapped comparisons deliberately stop after the original routes1and2: source
case3 is commented out and triggers the default termination. Expected values above
machine ranges use the source MiniNumber arithmetic; no native signed overflow is
introduced. A count check protects the99executed assertions. Full Java original
methods execute unchanged; no broadened exception catch substitutes for them.

### Validation

Full normal and ASAN/UBSAN builds pass without warning/error lines. All16selected
regression suites pass193.70s/528.87s. Both original reports4methods99assertions plus
7supplementalcomparisons pass. Primitive3802,value1451,VM4471,object9244,
checker1343,logger2016,consensus2890 reports all have zero failures. The finalSAN
consensus report was read after the terminal successful test result.

### Verdict

Approve this scoped correction and mapped test. Full-port approval still requires
the absent native owner/storage and remaining file/API behavior; this review does
not certify the complete node. Student lessons record both the observations and
these limits.


## DUMMYFUNCTION, MinimaFunctionTests and ABSTests — 280 counterparts

### Summary

Three source test files now have compiled native counterparts. Existing production
function code is reused. The helper stays outside the production registry; native
parameter retrieval checks preserve source expression identity.

### Findings

No new blocking defect found in the scoped fixtures. All11MinimaFunction and
23ABS assertions on the original success paths are retained. Catch-block fail()
sites are not counted as successful assertions. Wrong count/type cases require
ExecutionException, while unknown function lookup requires MinimaParseException.
ABS's Long.MIN_VALUE case uses native MiniNumber, avoiding machine-int abs overflow.

The DUMMYFUNCTION body advertises a minimum but deliberately does not enforce it
inside runFunction, matching source. The mapped original test does not execute
that body or its minimum metadata; the guide states that coverage limit. Unicode
name handling, negative indexes and full function API parity remain open.

### Validation

Full normal/SAN builds pass with no warning/error lines. Nine selected suites pass
10.55s/10.83s, including both new original-JUnit comparisons and all three existing
value original-test comparisons. All ten report files were read after completion.
Source mapping passes280counterparts177absent;126CTest cases are registered.

### Verdict

Approve the scoped test translations. Full-node and complete1:1 approval remain
open; neither a counterpart count nor these finite assertions proves completion.

## CommandRunner exception boundary — verified correction

**Review outcome:** approve the scoped correction; full-port verdict remains incomplete.
The generic command catch now uses MinimaLogger/NativeThrowable and nativeExceptionText,
with JavaError and bad_alloc rethrows preserving the source Exception boundary.
The new actual-runner/logging comparison improves 450/896 baseline mismatches to
zero in normal and ASAN/UBSAN builds. All nine selected affected suites pass:
eight directly in each initial selection, and cmd_handler_differential after
rerunning with local socket permission (the original sandbox attempts failed to bind).
Full registry, Main ownership and arbitrary native/JVM diagnostic parity remain open.

## FLOORTests and CEILTests — verified mapped tests

**Review outcome:** approve the scoped test translations. The original three Java
methods per class and the native counterparts pass30FLOOR/26CEIL assertions in
both builds; all10selected tests pass13.21s/13.07s. Exact decimal input strings,
fresh functions/shared method contexts, typed ExecutionException and active
assertion counts are preserved. Production arithmetic unchanged. Empty domain
blocks and missing two-argument assertions remain explicit source coverage gaps.
Current map282counterparts=254production+28testfiles;175absent.131CTestcases.

## Wallet command integration — verified scoped port

**Review outcome:** approve these four scoped command translations and three shared
corrections; the overall native node remains incomplete.

scripts/newscript/removescript/sign now resolve the actual MinimaDB wallet and
are registered with source metadata and factories. Tests preserve source lookup,
cleaning, removal-before-insertion, checked-failure swallowing, cache and signing-use
order. The new675programs/1907operations/50275SQLcalls pass normal and ASAN/UBSAN.
All13affected suites pass36.37s/151.68s. The1120case command-error comparison and
existing1433programWallet/4471VM/12289command/2106HTTP regressions also pass.

New failing examples exposed native SQL carrier names rendered as RuntimeException,
cleanScript bypassing MinimaLogger, and getParam producing bad_variant_access on
null. Corrections reuse sqlExceptionText, actual MinimaLogger and getParamUTF16.
Full builds have no compiler warning/error lines. Initial fixture member-call and
logger-include compile errors were corrected before running the rebuilt tests.

Remaining: the recording SQL driver proves requested call order and supplied-row
processing, not durability or crash-safe signing-use persistence. Full Main/H2,
arbitrary malformed row fields, unpaired-surrogate command values, concurrent
wallet lifecycle and complete exception/API parity remain open. Map286counterparts
(258production+28tests),171absent.132CTestcases; all rows remain partial/unverified.


## Address commands — verified scoped port

**Review outcome:** approve the scoped newaddress/getaddress/coinnotify/slavenode
translations; full node remains incomplete. Actual Wallet, MinimaDB and UserDB
operations are reused. Creation precedes key counting and row serialization;
getaddress preserves its empty legacy createall branch and unused type read;
notification address parsing precedes action validation; slavenode saves even on
queries and ignores malformed host values when disabling.

Full normal and ASAN/UBSAN builds pass with no warning/error lines. All six
selected suites pass in 28.41s/156.12s. Reports read after terminal success in both:
134 address programs, 276 operations, 3,693 SQL calls; 675 existing wallet programs,
1,907 operations, 50,275 SQL calls; 1,233 runner, 1,120 runner-error and 1,130
preference cases. Explicit guards establish successful nondefault tracked simple
address creation and Java-produced Mx/hex notification identity. Source mapping passes.

The recording driver does not persist wallet writes. Actual preference bytes,
save-disabled memory changes, reload and owner recreation are compared, but full
startup, crash/filesystem failures and concurrent lifecycle remain unverified.
Random selection is restricted to empty, single or identical duplicate rows.
No new blocking issue found within this reviewed scope. Map290counterparts
(262production+28testfiles),167absent.133CTestcases; all rows remain partial/unverified.


## MINTests and MAXTests — scoped mapped-test review

### Summary

Existing native MIN/MAX, ConstantExpression, NumberValue, Contract and JavaRandom48
are reused. The original three JUnit methods are freshly compiled and executed
unchanged. Native tests preserve every successful-path assertion and the six
10,000-trial loops, in source order. No production arithmetic changes.

### Findings and validation limits

No blocking defect found in this scoped translation. Native counts are guarded at
120,023 per class:4construction+2zero-result+120,000random-result+17rejection checks.
The valid method keeps one raised-limit Contract; invalid cases use a separate
ordinary-limit Contract. Boundary positions first/last/middle and int/long widths
remain distinct. Invalid ordered type pairs exclude only number/number and require
ExecutionException. Unsigned sample assembly plus bit_cast avoids signed overflow.

Java uses its original unseeded Random. Native draws reuse bounded JavaRandom48
with reproducible seeds and assemble signed 32/64-bit samples. The tested property,
ranges, loop counts and boundary positions match; sampled inputs and generator
sequences do not. This explicit test adaptation does not establish PRNG parity.
Source domain blocks are empty; decimal fractions and tie identity are not proved
by these mapped original tests. Existing production code retains its prior limits.

Both full builds pass with no warning/error lines. All12selected suites pass
36.91s normal /69.38s ASAN/UBSAN. Both MIN/MAX reports record3methods120023assertions
and60000trials per runtime. All18original reports were read; seven earlier shared
runner classes still pass. Final sanitizer MAX report read after terminal success.

### Verdict

Approve the scoped mapped tests. Full node approval remains open. Current map:
292counterparts=262production+30testfiles;165absent=107production+58tests.
137CTestcases; complete public API, native H2 and Main startup remain unfinished.


## Witness commands — scoped implementation and parameter-cast review

### Summary

txnmmr and txnscript now use the existing runtime transaction database, actual
MinimaDB chain/Wallet, MMRProof/ScriptProof and Witness methods. Source metadata,
parameter order, positional proof skipping, raw script hashing and immediate
partial insertions are retained. Command.getJSONObjectParam now throws the existing
JavaClassCastException for wrong nonnull types; missing/default/null and shared
object behavior remain unchanged. The fixture's direct exception observation now
uses nativeExceptionText, which already handles EOFException correctly.

### Findings

**Major remaining fidelity gap — CoinProof coin ownership.** CoinProof.hpp stores
Coin by value; pinned CoinProof.java retains its constructor's Coin reference.
Later mutation through the transaction's coin reference is therefore not established
by these fixtures and remains work for the full port. The command passes current
value/byte comparisons; this is not complete identity parity. Resolving ownership
must preserve reference stability across transaction input-vector growth/removal.

No new blocking defect found in the scoped comparisons. Manual scripts retain
LinkedHashMap order and raw text. Existing MMR proofs are skipped by count, even
when they identify a different input. Absent historical nodes are dereferenced
only when a new proof is needed. Checked SQL failures may leave earlier scripts
inserted while later runtime failures escape. The unused added-script list stays
local, matching source. Source coin/proof validation is not invented here.

### Validation

Full normal56443/SAN64846 builds pass with no compiler warning/error lines. Nine
selected suites pass44.49s/182.20s. All16reports read after terminal completion:
418witness programs1128ops8637SQLcalls;675wallet1907ops50275SQL;134address276ops3693SQL;
12289command;1233runner;1120runnererrors;1129txncommands6904ops;2503TxnDB, each runtime.
Source mapping passes294counterparts=264production+30testfiles,163absent;138CTestcases.

Retained failing baselines: witness-io case123/392 (harness EOF rendering), then
object case345/394 (production JSONObject cast). Twenty-four additional cast-order
cases expand to418. Initial fixture attempts found uninitialized custom databases
in both runtimes, including a sanitizer snapshot null dereference; setup now reuses
the existing Java field/native runtime-lookup convention before tests run. Initial
missing GlobalParams includes were corrected before verification.

### Verdict

Approve the scoped implementation and correction. Complete1:1 approval remains
open for coin/proof identity, arbitrary malformed graphs/Unicode/exception cases,
concurrency, native H2 and full Main lifecycle. SQL recording is not durability.


## Code Review — Coin shared ownership correction

### Summary

Coin now stores its existing fields in one shared allocation. Ordinary native handle copies preserve mutation visibility through Transaction and CoinProof/Witness without borrowing vector-element addresses. Constructors and decoders create independent storage; the existing wire-based deepCopy keeps explicit copies independent. No codec, JSON, leaf-hash or consensus algorithm was replaced.

### Findings

The earlier MAJOR Coin value-copy finding is corrected for the exercised public setters and retained Coin handles. All 220 new ownership programs failed before the correction while the previous 9,244 object cases passed. Both corrected runs pass 9,464 cases. Review traced all native coin setters and inspected Transaction, Witness, TxBlock, BlockMMR, tree and searcher callers. Explicit source deep-copy boundaries remain intact. The test acquires its short-lived mutation target pointer only after vector growth/removal and does not use it after clearing containers.

#### 🟠 MAJOR — Broader Java reference and stream fidelity remains incomplete

**Files:** `src/org/minima/objects/Coin.hpp`, `src/org/minima/objects/CoinProof.hpp`, `src/org/minima/objects/CoinProof.cpp`.

**Problem:** Sharing one Coin does not share caller-owned state lists between different coins or reproduce retained primitive/token field objects. MMRProof is still a value. CoinProof's converter catches every std::exception silently rather than only logging IOException. Coin and CoinProof lack their source in-place read APIs; Coin.deepCopy also differs on I/O failure. These are existing remaining full-port gaps, not claimed resolved by the new experiment.

**Fix:** Continue source-derived ownership and partial-read/error experiments before certifying complete parity. Retain explicit deep copies and use owned handles for references that outlive vector changes; do not introduce borrowed vector pointers.

### Validation

Full normal and sanitizer builds pass without compiler warning/error lines. The normal run passes all 21 selected checks in 303.51 seconds. The first sanitizer run passes 20 of 21 in 703.82 seconds, with block ownership timing out at 240.03 seconds. The sandbox also denied process inspection during cleanup; no orphan test process remained in the subsequent process check. The isolated rerun passes all 1,558 programs and 6,219 operations in 141.61 seconds, with the unchanged 240-second limit and corpus, using process-management access. Concurrency and permission conditions both changed; the initial timeout remains recorded in the log.

All 36 JSON reports were read after final completion. The following counts pass in each build:

| Comparison | Cases or programs | Operations where recorded |
|---|---:|---:|
| Objects, including 220 ownership programs | 9,464 | — |
| Consensus | 2,890 | — |
| Tree / cascade | 1,027 / 2,892 | — |
| RAM / hash map / TxnDB | 315 / 135 / 2,503 | — |
| Searcher / KISS VM | 189 / 4,471 | — |
| Transaction commands | 1,129 | 6,904 |
| Witness commands | 418 | 1,128 |
| CoinDB | 1,350 | 4,318 |
| Block ownership | 1,558 | 6,219 |
| Miner / processor | 357 / 384 | 1,352 / 1,434 |
| Checker logs / MMR sync / IBD | 1,343 / 3,600 / 1,057 | — |

TransactionTests, TxHeaderTests and source_mapping also pass. This is 21 selected tests of 138 registered. Mapping remains 294 counterparts and 163 absent files. No mapped CoinTests was added: its five successful-path assertions, identity-sensitive amount check and discarded static decode result are now described accurately in its lesson.

### Verdict

✅ Approve the scoped ownership correction and its teaching material. Complete 1:1 approval remains open for the listed fidelity gaps and unfinished native H2/Main.


## Code Review — coin/proof readers and MiniString source test

### Summary

The reader changes retain the existing byte layout and algorithms while making
partial mutation and outer shared ownership observable. Converters now distinguish
I/O errors from runtime/Error failures and use the actual logger. The MiniString
addition reuses production code unchanged and documents its weak source assertions.
All changed production files, harnesses, drivers and registration were read directly;
these files are untracked, so an empty git diff is not review evidence.

### Findings

#### MAJOR — full Java object identity and full node remain incomplete

**Files:** `src/org/minima/objects/Coin.hpp`, `src/org/minima/objects/mmr/MMRProof.hpp`, source map.
**Problem:** The owned state-list bridge is explicit; legacy vector setters still
copy. Tokens, primitive fields, proof chunks and nested MMRData retain value
semantics. Native Main/Minima and H2/MVStore remain absent.
**Fix:** Continue source-derived ownership and runtime work; keep map partial and
parity not_verified. Do not represent this scoped change as a completed native node.

#### MINOR — StateVariable writer and in-place API remain incomplete

**File:** `src/org/minima/objects/StateVariable.cpp`.
**Problem:** Its reader now reports the exact invalid type. The source writer has
no invalid-type else branch, while the native writer throws; the source in-place
reader can expose a rejected type on a retained object and the native API cannot yet.
**Fix:** Add a dedicated retained-state reader/writer experiment before translating
that behavior. The current coin/proof corpus only validates the nested decoder error.

### Validation and verdict

Normal and sanitizer full builds pass. New stream comparisons pass 3,045 programs
each, zero differences; baseline 2,716/3,029 differences is retained. An intermediate
eight snapshot differences were corrected in the harness by matching source JSON-
before-wire evaluation, not by altering production to fit an invalid experiment.
Both MiniString target builds and all four selected checks pass (normal8.63s,
SAN10.72s): five ordinary plus one helper assertions, FLOOR30, primitive3802/3802.
Normal broad regressions finish20/21 in242.66s. Searcher harness failed its
one-reply-per-line protocol because new converter logging reached stdout. Its
Java oracle already routes diagnostics to stderr; the native harness now uses
the logger println callback to do the same. Normal search rerun passes189programs6468queries in9.58s. Sanitizer broad
passes20/20 in711.13s; all35 completed broad reports have zero differences.
Isolated sanitizer ownership passes1558programs6219operations in146.76s under
the unchanged240-second limit. All36broadreports inspected with zero differences.
Verdict: approve this scoped correction and mapped test addition. Full-port approval
remains blocked by the explicit incomplete scope above.


### Added mapped command — automine

Reviewed the complete pinned automine body, shared parameter helpers and registry,
then the new native class, registry/CMake edits and both updated runner harnesses.
The source's mining branches are comments. Native code preserves its active
parameter read and standard reply, including inherited empty valid parameters.
The existing tested framework is reused without a new mining/settings dependency.
Baseline passes1233oldcases and fails32new; corrected1265cases pass in both builds
(normal2.88s/SAN3.65s). Direct/meta/runner cases are distinct. Direct malformed
parameter errors normalize to ERROR, so this harness does not establish exact
exception text for those inputs. Full registry and node integration remain open.
Final normal full build passes without compiler warnings/errors. Additional
command/shared-error/mapping checks pass3/3 in8.72s. The final sanitizer full build also passes without compiler warning/error lines.
Its final four sequential checks pass in171.05s, including ownership146.76s and
command12289/error1120/mapping checks. Across this checkpoint all29selectedchecks
pass in both builds after the normal search-harness correction. Approve the scoped
automine addition; this does not approve an incomplete full node.


## Code Review — chain-inspection commands

### Summary

block, printmmr and printtree are mapped and registered using the existing native
Command framework and actual MinimaDB owners. Their exact help, factory freshness,
JSON types, leading newlines, parameter parsing and distinct empty-tip behavior
are compared with Java. Tree depth subtraction now preserves signed-32-bit wrap.
The four-zone test caught an older independent TxHeader strftime formatter; it now
reuses JavaDateString unchanged after a direct source/native helper comparison.
Changed production files, both database harnesses and driver were read in context.

### Findings

#### MAJOR — full runtime and per-file parity remain incomplete

**Files:** source map, native MinimaDB and absent Main/Minima counterparts.
**Problem:** The command fixture records JDBC/Main effects; no native H2 durability
or full node startup is established. Existing object ownership/reader gaps persist.
**Fix:** Continue actual storage/runtime work and complete remaining counterparts;
keep all current per-file parity classifications unverified.

#### MINOR — extreme-depth runtime scope is limited

**File:** `src/org/minima/database/txpowtree/TxPowTree.cpp`.
**Problem:** At minimum int depth the source subtraction wraps to maximum int and
can execute billions of parent checks. Corrected native arithmetic preserves that
bound; tests use empty trees for extreme depths and populated trees for bounded
values. Arbitrary concurrent parent changes and the billion-iteration path are
not certified by these tests.
**Fix:** Keep this explicit in the lab and API review; do not introduce a silent
command-level clamp or pretend the empty-tree case exercises the long traversal.

### Validation and verdict

Both full builds pass. The original missing-registry baseline fails metadata in its
first fail-fast program. Initial UTC78programs pass; the first multi-zone corpus
stops at Londoncase2 because cascade dates usedBST where Java usesGMT. That report
is retained. The corrected expanded324program4096operation corpus passes normal
9.69s and ASAN/UBSAN20.22s. Normal nine regressions pass50.23s and sanitizer nine regressions pass177.75s.
All14comparisonreports were inspected: objects9464,tree1027,cascade2892,
MinimaDB2367programs7519operations334711SQLcalls,command12289,runner1265,error1120
perruntime. TxHeaderTests and mapping also pass. Both final full builds have no
compiler warning/error lines. Allten selectedchecks perbuild pass.
Verdict: approve this scoped command addition and rendering corrections. None of
these checks certifies a completed native node.

## Code Review — transaction checking, owner retention and INC/DEC source tests

### Summary

The command reuses actual generator/checker/signature and database behavior.
Shared owners now preserve the transaction-cache interaction required by the
pinned txncheck sequence. TxBody's in-place reader preserves the four field
owners and observed partial mutation. New mapped INC/DEC tests preserve the
original active assertions and constructor overloads. The finite comparisons
pass; full native-node completion remains an open project requirement.

### Findings resolved in this change

- **MAJOR — generated transaction copied away from the stored row.**
  TxBody/TxPoW/TxPoWGenerator now retain shared owners when supplied through the
  shared API. The command's clearIsMonotonic and checker result affect the same
  stored transaction. The corpus checks stale flags, mutation visibility and
  lifetime after row deletion. Legacy value convenience callers still allocate
  owners and are explicitly outside a complete reference-graph certification.
- **MINOR — wrong exception for a missing token creator.** At
  `src/org/minima/objects/TxBlock.cpp:49`, a source-derived invalid creation output
  exposed bad_optional_access. The access now throws JavaNullPointerException
  before reading the creator's fields, matching the source category.
- **MINOR — generator hash-rate error differences.**
  `src/org/minima/system/brains/TxPoWGenerator.cpp:12` checks the truncated integer
  divisor and throws the source ArithmeticException for zero. The nonnegative
  BigInteger constructor in `src/org/minima/objects/base/MiniData.cpp:184` retains
  the original negative-input message, including its spelling. No arithmetic
  algorithm or global BigInt error policy was replaced.
- **MINOR — original test constructor overloads.** The INC/DEC native tables use
  a variant so -1, 0 and 1 still enter the integer constructor and fractional
  values enter the string constructor. Source inputs and active expectations
  remain unchanged. The comparison driver executes the original Java methods.

### Remaining findings

#### MAJOR — full runtime and API equivalence remain unfinished

**Files:** `source-map.json`, absent Main/Minima counterparts, SQL service boundary,
TxPoW/TxBody/TxPoWGenerator APIs.
**Problem:** 152 destinations remain absent. Existing native value call sites,
nullable/list-reference surfaces, complete in-place TxPoW reading, service
replacement under concurrency, Main startup and durable native H2/MVStore are
not certified by these tests. Recording JDBC is not a native storage engine.
**Fix:** Keep all counterparts partial/parity-not-verified and continue actual
runtime, storage and remaining source work. Do not substitute the txncheck report
for full node validation: its omission of allsignaturesvalid from its final
conjunction is intentional pinned-source behavior.

### Validation and verdict

Final normal and sanitizer full builds pass, including the two additional mapped
test targets. The new txncheck corpus passes
396 programs / 1,049 operations in each build, including typed failures, body
aliases/null fields/truncated reads and owner lifetime. Both original INC/DEC
comparisons pass 30 active assertions per class per runtime, and their four CTest
entries pass in each build after preserving integer-constructor inputs.
All 21 selected regressions pass in each build (353.96s normal, 898.47s sanitizer).
All 36 regression comparison reports were inspected with zero failures. Including
the new command comparison and four original-test entries, 26 checks pass per
build; the entire 148-entry suite was not rerun.
No native compiler warning/error lines were observed in the completed builds.
Verdict: approve this scoped component progress; request continued work before
claiming a complete working native node. The remaining goal is not blocked.


## Code Review — transaction mining/posting and POW/SIGDIG mapped tests

### Summary

Three command counterparts reuse the actual generator, shared row owners, wallet
burn builder and miner. The port preserves posting/deletion order and the source's
null-payload side effect. Source POW/SIGDIG tests now have native counterparts
that execute the same active assertion sets. Full native node integration remains
the headline gap.

### Findings resolved during validation

#### MAJOR — Convenience decoder failures used the wrong logging path (fixed)

**Files:** `src/org/minima/database/userprefs/txndb/TxnRow.cpp:16`,
`src/org/minima/objects/TxPoW.cpp:25`.
The prior default converters returned null for IOException but emitted an ad hoc
stderr message instead of passing the throwable through MinimaLogger. The first
mining comparison retained the source EOF log and exposed the mismatch. Both
converters now reuse the existing Coin NativeThrowable/MinimaLogger pattern.
The catch remains IOException-only. Arbitrary runtime exceptions are not silently
converted to null.

#### MINOR — Old transaction fixture mixed logger text with JSON (fixed)

**File:** `support/transaction_commands.cpp:33`.
Once decoder logging reached the actual logger, its stdout output added lines to
the older native request/reply protocol. The pinned Java fixture already redirects
diagnostics to stderr. The native fixture now routes MinimaLogger println to
stderr through the existing runtime hook; the actual production log remains.
The previously failing 1,129-program regression passes in both builds after this
harness correction. This fixture does not compare logger text; the mining fixture
captures and compares normalized log bodies separately.

### Remaining findings

#### MAJOR — Full startup, durable storage and object concurrency are unfinished

**Files:** `src/org/minima/system/commands/txn/txnminepost.hpp:13`,
`src/org/minima/system/commands/txn/txnutils.hpp`, and absent native Main/Minima.
The new Main-message callback and existing getMiner callback expose the required
runtime owner. Defaults fail on a missing receiver. Recorded fixture messages
cannot establish full-node acceptance, network propagation or durable wallet key
usage. Native H2/MVStore remains absent. Shared transaction/witness owners do not
by themselves prove all list/coin aliases or synchronized concurrent mutation.
Complete Main binding, persistence and full runtime validation are still needed.

#### MINOR — Source test scope is narrower than arithmetic parity

**Files:** `test/org/minima/tests/kissvm/functions/maths/POWTests.cpp`,
`test/org/minima/tests/kissvm/functions/maths/SIGDIGTests.cpp`.
The mapped POW positive example tests only zero to power zero. SIGDIG compares
the function with the same underlying decimal helper for precisions 0..32. This
faithfully ports the source tests but is not an independent proof of rounding.
The lessons explicitly separate these assertions from broader differential tests.

### Validation and verdict

Both full builds pass. Mining comparisons pass 207 programs / 699 operations /
1,893 recording-SQL calls in each build (54.49 seconds normal, 62.54 sanitizer).
All 16 selected affected regressions have passing final results in each build,
including one targeted rerun per build after the harness correction. The initial
groups retain that old protocol failure in their logs; it is not a current failure. POW/SIGDIG's four
CTest entries pass in both builds (25/89 active assertions per runtime).

Manual review covered all three commands against the pinned Java, changed
converter/miner entry declarations, fixture and driver paths, assertion mappings,
registration and teaching explanations. The PRNG seam changes only generated body
randomness at miner entry; real queue/worker-body and synchronous hashing execute.
Injected false/throwing miner results are identified separately from real mining.

**Verdict: Request changes for the overall complete-port goal.** These scoped
counterparts and tests are useful progress; none establishes a complete native
node or full per-file behavioral parity.


## Code Review — BITCOUNT table and mapped source test

### Summary

The original BITCOUNTTests references a public table that the previous native
implementation did not expose. The native function now follows the source table
lookup, and the mapped test preserves the complete source assertion set.

### Findings

#### MAJOR — Direct popcount ignored source table mutations (fixed)

**Files:** `src/org/minima/kissvm/functions/hex/BITCOUNT.hpp:8`,
`src/org/minima/kissvm/functions/hex/BITCOUNT.cpp:18`.
Java's final array reference still permits element edits, and totalBits reads
those elements. The previous native popcount always computed the mathematical bit
count. The native class now exposes the source's 256-entry table and totalBits
reads it. An unsigned 32-bit accumulator and explicit bit conversion preserve
Java wrapping when edited entries overflow the signed total.

The mapped source test independently computes every table entry by repeated
integer division, checks thirteen hex inputs, constructors and exact rejection
classes. Supplemental separate-process tests edit an entry, call both helper and
function, and force signed overflow. Java and native results match: 84, 42,
-2147483648. The public nullable-array API and concurrent mutation still need
review; this correction does not establish full Java API equivalence.

### Validation and verdict

Full normal and sanitizer builds pass. Three selected CTest entries pass in each
build: native test, unchanged original Java methods plus supplemental observations,
and the 4,471-program KISS VM differential suite. The source executes 34 successful
JUnit assertions and checks 256 conditional predicates per runtime. Results:
12.90 seconds normal, 21.74 seconds sanitizer. No rounding/other arithmetic change.

**Verdict: Approve the scoped table correction and mapped test; the complete-port
goal remains unfinished.** Manual review checked the source table literal,
accumulator conversion, source inputs, typed failures, wrapper and report counts.


## Code Review — checkpoint 359: trace and clock commands

### Summary

Reviewed both native command bodies and the changed Main fixture/oracle, driver,
corpus and CMake registration. Existing parameter/state/clock helpers are reused.
Ordered side effects and Java integer wrap are preserved. Both full builds pass;
209 programs / 652 operations match in both builds, and eleven affected regression
tests pass (36.66s / 61.15s). Twenty reports were read without failures.

### Findings

No new blocking defect found in this scoped review. Clock normalization follows
independent timestamp-bound and exact UTC date checks. Partial trace updates are
observed through real shared state after failures. Metadata and runner gates are
compared. Enhanced null/cast diagnostic wording remains excluded by the existing
comparison rules; full-node scheduling and exhaustive locale parity remain open.

### Verdict

Approve this checkpoint for local preservation. This is not certification of the
complete 457-file port. There are 359 counterparts and 98 absent mapped files;
all existing entries remain implemented_partial / parity not_verified.


## Code Review — checkpoint 360: healthcheck

### Summary

Reviewed the full pinned Java command, native header/body, registration, new Main
fixture setup, oracle, driver group and corpus. The port preserves repeated owner
lookups, typed missing receivers, the explicit missing-tip exception, MiniNumber
height arithmetic, exact response types, and the source's nocacade spelling. It
reuses actual native database/tree/cascade components. No Maxima behavior is added
from help text alone.

### Findings

No blocking defect found in the scoped command review. The first test run exposed
an invalid setup expectation for an out-of-range height. The corpus now preserves
that rejection explicitly and asserts that every intended successful setup really
succeeded. Both builds pass and all 176 programs / 635 operations match Java.
Independent expectations cover height arithmetic, branch choice, empty/null
cascade behavior and no SQL/timer/listener/queue effects from direct commands.

Shared pointers retain receivers without merging the source's repeated calls.
Concurrent singleton/root replacement remains unverified; the wrapper is a height
consistency report, not a cryptographic chain-validation routine. Full-node
startup, persistence and complete port parity remain outside this evidence.

### Verdict

Approve the command implementation with these documented limits. All fourteen affected
regressions pass in both builds (115.26s / 247.61s); twenty-six reports were read
without failures and saved under validation/checkpoint-360. No full-port completion
is claimed.


## Code Review — checkpoint 361: logs and atomic logging flags

### Summary

Reviewed the pinned logs command, native wrapper and registry, seven GeneralParams
field declarations/definitions, both fixture adaptations, Main oracle/driver and
new corpus. Parsing and updates remain interleaved in source order. txpowdb still
performs two separate writes and reports the block flag only. Existing worker
consumers read the actual globals; atomic bool storage replaces unsafe concurrent
plain-bool access without changing their logging branches.

### Findings

No blocking defect found in this scoped review. The parameter snapshots explicitly
load bool values, and processor fixture assignments retain the original order.
Independent expected state checks include all nine flags after success or failure,
so the hidden transaction flag cannot be masked by a correct-looking response.
Main.setSyncIBD confirms one real logging consumer. Both full builds and the new
327-program / 982-operation comparison pass in normal and ASAN/UBSAN builds.

The source uses plain Java booleans. Native atomics define individual access;
this does not certify all possible Java schedules, make multiple assignments one
transaction, or synchronize unrelated GeneralParams settings. No thread-sanitizer
run or complete native node is claimed by the current ASAN/UBSAN evidence.

### Verdict

Approve this scoped checkpoint. All 25 affected consumer regressions pass in both
builds (194.27s / 457.64s), and all 42 reports were read and saved under
validation/checkpoint-361. Reversing only the documented atomic substitutions
reproduces the prior GeneralParams header/body and ParamSnapshots hashes exactly.
No complete 457-file port or full-node readiness is claimed.

## Code Review — checkpoint 362: status

### Summary

Reviewed the full pinned command, new native header/body and registration, existing
runtime callback changes, Main fixtures, oracle/driver, corpus and student lessons.
The wrapper calls actual database, chain, cascade, wallet, filesystem, decimal and
network components. Source ordering is preserved, including clean-key presence,
early complete-mode tip access, repeated Main mode reads, sparse no-root fields
and debug messages after each completed phase.

### Findings

No blocking defect found in this scoped review. Zero difficulty throws the Java
arithmetic category before reading user hash rate. Existing HALF_EVEN division
and MiniNumber arithmetic are reused. The byte adapter pads difficulty without
truncation. Unsigned intermediate arithmetic preserves Java signed wraparound.
The initial compiler error was an incorrect native API spelling for the byte
adapter; the existing MiniData.withMinLength method is now called directly.

Two independent test expectations were corrected: large device counts use plain
integer text, and tied timestamps 0,0,1000,2000 select median block 2, yielding
speed 2 against block 0. Production arithmetic was not changed for these errors.
Nonzero cascade weights, malformed preference types, empty/null receivers, exact
file totals, debug order and optional network output have explicit comparisons.

The new collection-hint callback extends the existing runtime registry and binds
Main's existing service. Its native default is a no-op. Heap bytes are validated
separately for JVM and native allocators; clock normalization follows bounded
validation. These adaptations do not assert JVM collection, equal heap sizes,
full-node startup, durable SQL or every concurrent owner-replacement schedule.

### Validation and verdict

Both full builds and all 187 new programs / 634 operations pass in normal and
ASAN/UBSAN builds. All 25 affected regressions pass in both builds (301.18s /
952.17s). All 44 comparison reports were read and saved with both CTest
summaries under validation/checkpoint-362. Approve this scoped checkpoint.
Full-port completion remains unproven and is not claimed.

## Code Review — RPC work in progress

### Summary

The command body, registry and Main pause binding are implemented. Source
parameter order, unused ssl, duplicate valid parameter, user-array aliasing and
replacement, locale-sensitive modes and typed errors are exercised. Both full
builds and eleven selected regressions pass; 258 programs / 590 operations match
Java. The initial blank-action expectation was corrected without changing the
production command. JsonDB's bad_variant_access was corrected to ClassCastException.

### MAJOR — Concurrent authentication configuration needs safe native storage

GeneralParams.hpp still declares RPC_ENABLED/RPC_AUTHENTICATE as plain bool and
RPC_PASSWORD as std::string. The new command writes them, while CMDHandler and
Authorizer can read them on request threads. Source Java reference assignment does
not justify concurrent mutation of a native std::string. Reuse the existing atomic
flag and immutable-string snapshot patterns before certifying live configuration.

### MAJOR — Shared RPC-user arrays need concurrency review

UserDB returns the actual mutable JSONArray. rpc adds in place and removal replaces
the stored array; Authorizer and response serialization may read it concurrently.
The current native JSON containers do not define safe overlapping access. Preserve
the source's tested aliasing and replacement behavior while addressing native
container lifetime/access safety. The sequential comparison cannot establish this.

### Verdict

Request changes before calling RPC ready. Save the current implementation as WIP;
complete live enable/authenticated loopback integration and shared-state work next.
No completed RPC checkpoint, full-node startup or complete 457-file parity is claimed.


## Code Review — RPC configuration follow-up

### Summary

The plain flag/password finding from the initial RPC WIP review is addressed.
JavaStringReference extracts the exact mutex/optional-string storage previously
inside RPCClient::UserAgent, retains its public alias and adds an explicit load.
RPC_PASSWORD uses that adapter; RPC_ENABLED and RPC_AUTHENTICATE use the existing
GeneralParams atomic-boolean pattern. Parameter snapshots copy values explicitly.
Authorizer preserves the source short circuit and null inequality. The command's
stop/sleep/assignment/start order is unchanged; no atomic multi-field update is
claimed. No additional defect was found in this scoped change.

### Findings

#### MAJOR — Shared RPC-user JSON state remains unresolved

**Files:** src/org/minima/utils/JsonDB.cpp, src/org/minima/utils/json/JSONArray.hpp,
src/org/minima/utils/json/JSONObject.hpp.

User records still use unsynchronized native containers; append/replacement can
race with authorization and serialization. Returning a borrowed vector/value
reference after briefly locking its getter would not fix the lifetime hazard.
The pinned JSONArray extends ArrayList and JSONObject extends LinkedHashMap;
serialization uses iterators. Safe storage must preserve tested object aliasing,
replacement and relevant iterator behavior. JavaConcurrentMap is a proven locked
snapshot candidate, but its hash traversal order and enumeration semantics cannot
replace these containers unchanged. JavaList has no synchronization and is not a
solution to this finding. No JSON-container code was changed in this follow-up.

### Verdict

Approve the scoped configuration storage change, subject to the saved validation
results. RPC as a whole remains work in progress: shared JSON access and actual
command-driven listener enable/authenticated requests still need completion.
Native snapshot stress and ASAN/UBSAN do not certify every scheduling interleaving;
no ThreadSanitizer run or full native-node parity is claimed.


## Code Review — shared JSON storage, full regression pending

### Summary

JSONSequence preserves the existing ordered vector representation, uses the
repository's locked-copy access discipline, and retains state ownership in its
iterators. Accessors return copied JSONValue wrappers with shared nested owners.
Structural changes increment an unsigned counter; map value replacement does
not. Array hasNext uses current size; map traversal retains its initial end.
Locks are released before traversing children, and copy assignment snapshots the
source before locking the destination. JsonDB root reads/replacements retain
owners under a separate mutex. Source multi-operation ordering remains separate.

### Findings

No new blocking defect found in the implemented access/iteration surface. The
67-program Java comparison and actual UserDB concurrent stress pass both builds.
The initial three index-category mismatches were corrected in the production
array getter, with their failing report retained. Full regression found a stale
MiniByteTests catch; the production byte reader was already correct. That test
now catches wire::EOFException and passes direct runs in both builds.

The generic inherited Java collection API, arbitrary keys, every reentrant
callback and every concurrent schedule remain incomplete. Individual safe
accesses do not guarantee transactional snapshots across multiple source reads.
Global runtime callback registries and main_runtime's stream-buffer swapping
still require care in the future live-command integration fixture.

### Verdict

Save as WIP while the full 204-test suites run. Inspect their outcomes before
claiming this shared-container step fully validated. No completed RPC checkpoint
or 457-file parity certification is claimed.


## Code Review — live RPC command integration

### Summary

The fixture reuses the existing Main and network setup, production command,
listener, handler, preferences, authorizer and client. Java runs the pinned
implementations with the same unstarted node-worker boundary. HTTP bytes match
after validating and normalizing Date; ephemeral ports are checked separately.
The extracted MainFixture keeps existing operations unchanged. Logging uses a
stable sink for the lifetime of background RPC workers.

### Findings

MAJOR, fixed: Authorizer had an empty default database callback. Isolated server
fixtures supplied it, but real command-driven authentication failed. Its default
now resolves the existing current MinimaDB UserDB, matching the Java source. The
live database-replacement case verifies that it does not capture a stale owner.

MINOR, fixed: the new driver's failure report indexed an empty observations list
if the Java runtime failed first. The reporting expression now guards that case;
the passing test path is unchanged. Runtime/test exceptions remain failures.

No additional blocker found in this scoped change. Global callback mutation,
complete concurrency semantics, durable SQL and full-node startup remain wider
port limitations. The live fixture neither starts those workers nor certifies them.

### Verdict

Approve the scoped live integration fix with its normal/sanitizer comparison
evidence. Save as WIP while the prior 204-test full suites finish. Relink and run
affected existing regressions before declaring this RPC step fully validated.


## Code Review — hashtest command and benchmark comparison

### Summary

The command wrapper reuses the existing miner benchmark, numeric operations,
parameter helpers, logger and command registry. It preserves default amount,
measurement/logging order and Java long subtraction without native signed
overflow. Metadata strings come from the pinned body. The new test group reuses
the Main fixture and independently validates timing and rate before normalization.
Its separate native target keeps already-running full-suite executables intact.

### Findings

MAJOR, fixed: hashes was initially serialized as a string. The pinned writer
emits MiniNumber through its raw object-toString fallback. Existing RawJSON
numeric composition now preserves the numeric token without a floating conversion.
The failing report is retained and the normal rerun passes all 59 programs.

MINOR, corrected test deadline: the sanitizer's real million-hash case exceeded
the generic runner's 30-second response limit. This group now has a bounded
120-second response deadline and 240-second CTest limit. Other groups retain
their prior limits. No production workload reduction or fake clock was added.

The finite corpus compares ordinary numeric values after JSON parsing; it does
not certify arbitrary-precision JSON consumers, every clock discontinuity or
performance equality. Existing miner tests cover controlled elapsed-time clamps.

### Verdict

Approve the scoped wrapper and comparison: both builds pass all 59 programs.
Both relinked command-runner and network-utility regressions pass in both builds
(1,265 and 5,668 cases). The earlier full suites and remaining RPC integration
regressions must finish before a completed checkpoint claim.
The wider full-node and all-file parity goal remains incomplete.


## Code Review — coinimport and historical MMR restoration

### Summary

The wrapper translates the pinned two-path algorithm using existing proof, tree,
MMR and database implementations. Source null dereferences have checked native
failures. The first path preserves its ignored track/supplied-proof behavior;
the historical path preserves finalization, coin insertion, late track parsing,
relevance mutation and final root comparison in source order. The unused first
getMMRData calculation is retained because the source evaluates it before update.

### Findings

No new blocking defect found in the implemented sequential body. Both builds
pass 142 programs and 390 operations. Initial independent expectations were
corrected against Java: repeat historical imports can duplicate records, and an
artificially pruned newer spent leaf changes the time-valid proof check result.
Neither finding justified replacing the source policy.

Two fixture differences were corrected: INSERT tree IDs now use the existing
checked identity mapping alongside SELECT IDs; Java captures diagnostics from
the explicit post-command SQL observations to match native recording services.
Other node-state groups retain their prior projection and capture defaults.
The existing wallet driver accepts an optional native target, like the Main
driver, to test the current fixture without replacing live full-suite binaries.

### Verdict

Approve the scoped translation and comparison evidence. Existing node-state and
registry regressions remain pending. Recording SQL does not establish durable
storage; arbitrary pruning is a synthetic missing-information experiment, and
shared tree mutation/full native-node behavior remain wider incomplete work.
