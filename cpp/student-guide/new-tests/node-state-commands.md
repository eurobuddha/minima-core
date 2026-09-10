# Lab: mempool counts, coin tracking and desired network limits

Written before the new tests. Read base/mempool.java, cointrack.java and magic.java,
then the existing RamDB/map, TxPoWTreeNode, CoinDB, UserDB/JsonDB and Magic code.
No counterpart command was found in the current or sibling implementation.
Reuse those components; the commands are orchestration around existing state.

mempool enumerates RamData in Java ConcurrentHashMap order. Cascade entries are
counted but their TxPoWs are not read. Other entries contribute their ID, block
flag and transaction flag. The response named onchain actually counts all
non-cascade entries, regardless of RamData's on-main-chain flag. total includes
cascade entries. Finally query the real CoinDB size. Check empty, mixed, duplicate
and collision-key maps, null TxPoW inside a cascade/non-cascade RamData, SQL count
failures and exact ordering. Counters use Java int wrapping; finite fixtures do
not prove weakly consistent concurrent enumeration.

cointrack parses coinid before enable, finds the node containing that ID, then
searches for its unspent coin. Finding a node is not proof that searchCoin returns
a coin: a spent record may produce a later null error. Relevance uses MMR entry
numbers. Enabling an existing entry leaves the list alone; otherwise append the
entry and recalculate relevant coins. Disabling removes all matching entries and
recalculates. Compare actual entry lists, relevant Coin wire and MMR wire after
success and failure. Check repeated calls, equal entry numbers on different coins,
spent/newer records, empty nodes and SQL-backed coin lists. Do not replace the
searcher or relevant-coin calculation with a command-specific implementation.

magic writes optional preferences in order: kissvm, txpowsize, txnsperblock. A bad
later parameter leaves earlier writes present. Missing tip fails after writes.
lastblock is the tip's actual Magic JSON; desired contains numeric MiniNumber
objects, while JsonDB stores the same preferences as decimal strings. Compare
full preference wire after every operation, including errors and explicit save/
reload. Reuse the generator to observe desired values in a subsequent real TxPoW.
Test malformed stored preference types through the actual public JSON object.

The existing wallet fixture supplies real chain/MMR/coin and transaction objects.
Extend it only to create the actual CoinDB, seed/observe actual relevant entries,
mutate actual pool flags and observe preference/chain state. Source wire generators
and the recording SQL driver remain the test boundary. Exact serialized replies,
SQL effects and state bytes are compared with existing runtime exception-message
normalization. No live wallet, durable SQL, native Main startup or full node claim
follows from these fixtures. Retained Java list aliases and concurrency remain
separate parity work unless directly exercised.

First comparison finding: magic.java is not present in the pinned registry, even
though magic appears in the permission-name list. Remove its accidental native
registration. Instantiate the class explicitly for its direct metadata/body tests;
public runner tests must still report it as unavailable. A source file existing
is not evidence that its command is registered.

Second comparison finding: a stored Boolean under magic_kissvm made Java throw
ClassCastException while native JsonDB.getNumber exposed bad_variant_access.
Preserve the Java null/default branch, then use the existing JSONValue.isString
and JavaClassCastException before the existing MiniNumber constructor. The raw
command string helper cannot be reused whole because it renders null as text;
JsonDB must return its supplied default for null. The baseline reports are kept
as node-state-number-type-baseline.json in each build directory.

Third comparison finding: with a valid target coin followed by an undecodable
coin, Java performed two complete lookups but native performed only one before
throwing. MinimaDB's native bridge dereferenced all SQL Coin pointers eagerly.
Reuse Coin's existing private null handle (already used by deepCopy failures),
with friendship for MinimaDB, so consumers encounter null at the original point.
Keep node-state-null-coin-baseline.json in both builds as the failing evidence.

Review extension before its tests: call the actual relevance recalculation with
an empty relevant-entry list and malformed SQL coin rows. The source nested loop
never dereferences a coin when that inner list is empty. Compare this boundary
and explicit saved preference bytes. Validate SQL tree IDs as 32-byte hex values
and map each consistently to an encounter label; independently random IDs alone
are excluded from byte equality. Other bindings and SQL effects remain exact.

The empty-inner-loop case failed in both builds: Java completed the relevance refresh, while C++ threw a null-pointer exception after the same SQL read. The source nests the entry loop inside the coin loop; an empty entry list therefore never dereferences a coin. The counterpart now preserves that order, including the SQL read and first-match break. The failing reports are retained as `node-state-empty-loop-baseline.json` in each build directory. This corrects evaluation order; retained aliases of Java's replaced lists still require separate validation.

Final evidence: both full builds pass without warnings. All 363 programs, 1,411 operations and 21,009 SQL observations match Java (20.23s normal, 35.38s ASAN/UBSAN). All 13 selected regression checks pass (64.09s/145.86s): tree, preferences, command getters, runner, searcher, CoinDB, MinimaDB lifecycle/lock/ownership, wallet commands, balance, coin search and TxPoW search. Both builds’ 11 regression JSON reports were inspected; lock and ownership checks report through CTest. The source map contains 343 counterparts, with 114 still absent; these results do not establish full-file parity.
