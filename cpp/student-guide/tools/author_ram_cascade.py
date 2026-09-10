from lesson_writer import author,CPP

def w(rel,purpose,steps,adapt,detail,exercise,limits,suite):
 def tests(l):
  oracle='RamOracle.java' if suite=='ram' else 'CascadeOracle.java'
  return detail+' See '+l(CPP/'support'/oracle,'Java oracle')+', '+l(CPP/'support'/(suite+'.cpp'),'native harness')+' and '+l(CPP/'tools'/(suite+'_differential.py'),'differential driver')+'. The inspected existing report contains '+('315' if suite=='ram' else '2,892')+' matching cases with zero failures across this suite. Generic ERROR normalization does not prove exact exception classes.'
 author('src/org/minima/database/'+rel+'.md',purpose,steps,adapt,tests,exercise,limits+' Port status remains partial and parity not_verified.')
w('txpowdb/ram/RamData','One in-memory TxPoW cache row with access time and flags describing whether the transaction is on the main chain or in the cascade.',[
 'Keep the supplied TxPoW reference and initialize lastAccess from current wall-clock milliseconds. Both chain flags start false.',
 'getTxPoW simply returns the stored object; it does not refresh access time. RamDB explicitly calls updateLastAccess when it wants that behavior.',
 'updateLastAccess replaces the timestamp with the current clock. Main-chain and cascade flags can be changed independently; neither setter changes the other flag or timestamp.',
 'There is no wire serializer or persistence in this row class. Its state is retained only while callers keep the row alive.'
], 'The native row holds shared_ptr<TxPoW> and atomic timestamp/flag fields, preserving shared object ownership while avoiding native data races on those scalar fields. Java’s corresponding fields are ordinary fields, so this is not a proof of identical memory-model behavior.',
 'RamData is exercised through RamDB fixtures: repeated insertion/access, independent chain/cascade flags, cleanup and retained map handles. The fixture does not compare exact wall-clock timestamps or directly prove every possible concurrent observation.',
 'Which call refreshes access time: RamData.getTxPoW or RamDB.getTxPoW? Explain why that distinction matters to eviction.',
 'Atomic native fields improve defined access but do not prove Java concurrent semantics, clock rollback behavior or timestamp-boundary parity.','ram')
w('txpowdb/ram/RamDB','The in-memory TxPoW cache and mempool index. It tracks recent access, chain flags, candidate transactions and input coin references.',[
 'Index rows by TxPoW ID in a ConcurrentHashMap. addTxPoW refreshes an existing row’s timestamp and returns false; it inserts a new RamData only when the ID is absent. getTxPoW also refreshes access, while exists does not.',
 'cleanDB computes now - MAX_TIME and retains rows whose lastAccess is strictly greater than that cutoff. It creates a new map reusing surviving RamData objects, then replaces the map reference. Old map handles remain distinct.',
 'getAllUnusedTxns selects rows that are not on the main chain, are classified as transactions, and are not in the cascade. clearMainChainTxns resets only the main-chain flags.',
 'cleanUnusedTxn performs a stable descending sort by burn amount, then removes entries when counter > MAX_UNUSED_TXNS. Since counter starts at zero, a nonnegative limit N retains up to N+1 candidates, not N.',
 'setInCascade marks a matching row permanently for the purpose of ordinary unused-transaction selection unless another caller directly alters it. A missing ID is ignored by either flag setter.',
 'checkForCoinID scans transaction rows outside the cascade, including rows on the main chain. It checks coin IDs in both normal and burn witness coin proofs rather than inspecting only the Transaction input arrays.',
 'wipeRamDB clears the current map in place. This differs from cleanDB’s replacement and is visible to retained map references.'
], 'C++ uses the shared JavaConcurrentMap adapter to retain the tested quiescent enumeration order. A mutex protects replacement/access of the map pointer, row fields are atomic, and stable_sort preserves tie order. cleanDBAt is an additional native hook for controlled time; ordinary cleanDB still reads the system clock.',
 'The RAM fixture tests sizes 0 through 513, limits -1/0/1/7/500 and combinations of main-chain/cascade flags. It inserts each TxPoW twice, queries unused transactions and coin references, cleans the unused pool, clears flags, refreshes lookups, performs zero-age eviction and checks an old map handle before wiping. The same suite also drives many collision-heavy map insert/get/remove/clear programs. Its report explicitly limits claims to quiescent ordering and leaves concurrency/timing unverified.',
 'With MAX_UNUSED_TXNS=1 and four equally ranked candidates, how many survive? Why can an old map returned by getCompleteMemPool still contain rows after cleanDB replaces the current map?',
 'Compound get-then-put operations are not made atomically equivalent merely by using a concurrent map. Actual concurrent enumeration, replacement races, exact age boundaries and mutable TxPoW IDs need focused review.','ram')
w('cascade/CascadeNode','A compact chain node that retains a copied TxPoW header, its cascade level and a link to the next older retained node.',[
 'Deep-copy the supplied TxPoW, then clear its body. Start current level at zero and record the supplied TxPoW’s superlevel.',
 'Parent links connect retained nodes but are not serialized within one CascadeNode. The surrounding Cascade reconstructs the list.',
 'Compute current weight as the TxPoW weight multiplied by 2^currentLevel using DECIMAL32 rounding. This uses seven significant digits with HALF_EVEN behavior, distinct from MiniNumber’s 64-digit DOWN context.',
 'Serialize the copied TxPoW followed by current level encoded as MiniNumber. Reading restores those values and derives superlevel from the decoded TxPoW; it does not explicitly clear a body supplied on the wire.',
 'toString includes current/superlevel, TxPoW ID, block number, decimal weight and local date.'
], 'The native node stores its copied TxPoW by value and parent by shared_ptr. MMREntryNumber’s decimal adapter represents unbounded weight; multiplyDecimal32 reproduces the seven-digit rounding. Exponentiation uses repeated squaring rather than floating-point powers.',
 'The node fixture varies twelve block inputs, difficulty divisors 1/2/3/7 and levels -1/0/1/7/23/31/64, comparing bytes, weight unscaled value/scale/text and display text. Derived read cases also check superlevel, body presence and trailing-byte count. Dedicated weight vectors target HALF_EVEN ties and large values. TZ is fixed to UTC.',
 'Why is multiplying a double weight by pow(2,level) insufficient for a 1:1 port? What does body presence after readnode tell us that the normal constructor alone does not?',
 'Parent identity, exact negative-level errors and every date/locale are outside these observations. Deep-copy failure behavior follows TxPoW and still needs full parity review.','cascade')
w('cascade/Cascade','A linked list of historical header-only nodes retained at progressively stronger superblock levels, with a recalculated cumulative weight.',[
 'addToTip creates a CascadeNode and points it to the previous tip. It does not update mTotalWeight immediately; cascadeChain is responsible for recalculation.',
 'cascadeChain leaves an empty cascade unchanged. For a nonempty list, retain the tip, initialize total weight from it, begin required level zero and count the tip as the first retained node.',
 'Walk older nodes. Retain one only when its superlevel is at least the required level, assign that level, relink it into the retained list and add its rounded node weight to the exact cumulative sum.',
 'When the retained count reaches MINIMA_CASCADE_LEVEL_NODES, advance the required level if another level exists and reset that level’s count. Finally sever the last retained node’s parent link, dropping skipped older nodes from this list.',
 'The tip’s existing current level is not reset by cascadeChain. checkCascadeCorrect separately requires a nonempty tip to start at zero and requires level changes to increase by exactly one.',
 'Validation examines superparent IDs from each node’s current level upward. After encountering 0x00, all higher IDs must also be zero. Required nonzero IDs must be found at the current node or farther back; the final node in a level has a special exemption for its same-level pointer.',
 'Write a MiniNumber list length followed by nodes from newest to oldest. Reading replaces the list, links decoded nodes and calls cascadeChain, so deserialization can change levels, prune nodes and recalculate weight rather than merely restoring a byte-for-byte object graph.',
 'deepCopy uses that wire round trip and therefore includes its recalculation behavior. printCascade reverses list order for oldest-first display. loadDB/saveDB delegate to MiniFile’s slow-read/direct-write helpers.'
], 'Native shared_ptr nodes preserve list sharing; unbounded decimal adapters accumulate node weights. The code exposes injectable logging for validity/conversion paths and reuses MiniFile persistence helpers. Native counters use explicit wraparound where Java int behavior is observable.',
 'The fixture crosses quotas 1/2/3/256, lengths around small and default boundaries, incremental/final cascading and mutations of tip level and superparent pointers. It compares bytes, total weight, length, correctness, per-node levels/body state and printed display. Additional 257/513-node cases reach the default quota. Java-produced reads, truncations, trailing bytes and deep copies exercise reconstruction. The report explicitly leaves file persistence pending; these cases do not run loadDB/saveDB.',
 'Why can getTotalWeight be stale immediately after addToTip? Explain how deepCopy may do more than simply duplicate the list currently in memory.',
 'This test corpus establishes cascade algorithms and format observations, not durable file replacement, full chain consensus, arbitrary shared-list mutations or exact diagnostics.','cascade')
