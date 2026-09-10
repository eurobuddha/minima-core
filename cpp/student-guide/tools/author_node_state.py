"""Teach the source command bodies and the failure-order findings."""
from lesson_writer import author,CPP
import re
for name in ['mempool','cointrack','magic']:
 p=CPP/f'student-guide/files/src/org/minima/system/commands/base/{name}.md'
 s=p.read_text().replace('**Port status:** `unported`','**Port status:** `implemented_partial`').replace(f'· `src/org/minima/system/commands/base/{name}.cpp` (absent)',f'· [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/{name}.cpp)')
 for old,new in [('Purpose','What this file does'),('Follow the command','Follow the logic'),('Translation and dependencies','How the translation is approached'),('Understanding the tests','What the tests tell us'),('Limits','Limits and traceability')]:s=s.replace('## '+old+'\n','## '+new+'\n')
 trace=re.search(r'Pinned Java SHA-256:[^\n]*',s)[0];s=re.sub(r'Pinned Java SHA-256:[^\n]*','',s)
 parts=re.split(r'^## (.+)\n',s,flags=re.M);sections=dict(zip(parts[1::2],parts[2::2]))
 order=['What this file does','Follow the logic','How the translation is approached','What the tests tell us','Check your understanding','Limits and traceability']
 p.write_text(parts[0].rstrip()+'\n\n'+'\n\n'.join('## '+h+'\n\n'+sections[h].strip() for h in order)+'\n\n'+trace+'\n')
def evidence(link):
 return link(CPP/'student-guide/new-tests/node-state-commands.md','The lab written before these tests')+' and '+link(CPP/'tools/node_state_corpus.py','the executable corpus')+' use the existing wallet fixture with actual command bodies, chain/MMR objects, RAM entries, preferences and recorded SQL. Both implementations receive the same source-generated wire. Exact reply text, current entry lists, relevant coin/MMR wire, preference wire and saved preference bytes are observed after each operation. SQL tree IDs are independently random: the comparison checks their 32-byte shape and consistent use, then labels them by encounter order. Other SQL bindings and effects remain exact. Independent assertions check counts, tracking results and generator preferences. See the lab for final run evidence and the mismatches that motivated shared fixes.'
limits='Counterpart implemented_partial; full parity not_verified. Supplied SQL rows do not establish durable storage. Full Main startup, live chain/network integration, arbitrary retained collection aliases, concurrent mutation and exhaustive platform behavior remain unfinished.'
author('src/org/minima/system/commands/base/mempool.md',
 'Summarize the TxPoW objects held in RAM and report the SQL coin-store size. This command reports classifications; it does not validate or remove transactions.',[
 'Obtain the actual TxPoWDB map and enumerate RamData values in ConcurrentHashMap order. No sorting step is present.',
 'For a cascade entry, increment only the cascade counter. The contained TxPoW is never read, so even a null TxPoW inside such an entry is harmless here.',
 'For every noncascade entry, increment size, read its TxPoW and append its ID, transaction flag and block flag. Test both flags independently when updating the counters: one object may count as both.',
 'Return the map size as total and the noncascade count as onchain. The variable name is misleading: the algorithm does not inspect the on-main-chain flag.',
 'Finally query the singleton CoinDB size. A SQL failure can occur after enumeration. Preserve Java integer wraparound for the counters and the source reply envelope.'
 ],'The translation reuses Command replies, TxPoWDB, RamData, TxPoW, JavaConcurrentMap traversal and CoinDB.getSize. Explicit unsigned addition followed by bit_cast preserves Java int wrapping without C++ signed-overflow undefined behavior. The command is registered in the native runner like the pinned source.',evidence,
 'Why can transactions + blocks exceed onchain? Because a single noncascade object may be both a transaction and a block. Why can a null cascade TxPoW succeed while a null noncascade TxPoW fails? Only the noncascade branch dereferences it.',limits+' Quiescent collision-map tests cover encounter order; they do not prove weakly consistent enumeration while other threads mutate the map.')
author('src/org/minima/system/commands/base/cointrack.md',
 'Add or remove a coin’s MMR entry from the relevant-entry list of its containing tree node. Recalculate that node’s relevant coins when the list changes or tracking is disabled.',[
 'Read the required coinid string and parse it as MiniData before reading enable. Invalid ID input therefore wins over a later Boolean error.',
 'Find the tree node containing the ID. A missing node produces the source coin-not-found error. Then search for the unspent coin; a node may exist even when this second search returns null because the coin is spent.',
 'When enabling, ask whether the coin’s MMR entry is already relevant. If yes, return Coin already tracked without recalculating a potentially stale computed list.',
 'Otherwise append the MMR entry and recalculate. Relevance is based on entry numbers, not unique coin IDs: several fixture coins sharing an entry can all become relevant.',
 'When disabling, remove every occurrence of that entry and always recalculate, even if no occurrence was present. Return the source response only after recalculation succeeds.',
 'Errors do not roll back earlier mutations. Observe the entry list and computed coins after SQL and decoding failures to identify how far execution got.'
 ],'The command reuses MiniData and Command getters, TxPoWSearcher, TxPoWTreeNode.removeRelevantCoin and calculateRelevantCoins. SQL rows flow through the real CoinDB and MinimaDB bridge. The comparison exposed eager dereferencing of nullable SQL coins and an empty-inner-loop mismatch; the shared implementations now retain the existing nullable Coin handle and source loop order. These are component fixes, not command-specific substitutes.',evidence,
 'If the entry is already tracked but the computed list is empty, does enabling rebuild it? No. Why are lists inspected after a failed disable? Removal occurs before recalculation and is retained if recalculation fails.',limits+' Java replaces some lists while C++ currently mutates vectors; held aliases need separate parity work.')
author('src/org/minima/system/commands/base/magic.md',
 'Read and optionally update the node’s desired network capacity parameters. Report the current tip’s Magic values alongside the node’s requested values.',[
 'Get UserDB and process optional kissvm, txpowsize and txnsperblock parameters in that order. Parse and write each before processing the next.',
 'A later parse failure preserves earlier writes. Likewise, a missing chain tip fails only after all supplied preference writes have happened.',
 'Obtain lastblock from the actual tip TxPoW’s Magic.toJSON. This is separate from desired: updating preferences does not rewrite an existing block.',
 'Read the desired values back through UserDB. JsonDB stores decimal strings, but the reply contains numeric MiniNumber JSON values. Stored non-string values must produce the source ClassCastException; null still selects the default.',
 'Return the reply without saving a file. Explicit save/load tests exercise inherited preference persistence separately. A subsequently generated TxPoW reads these desired values through the actual generator.',
 'The pinned source includes this class but does not register it in CommandRunner. Direct tests instantiate the class; public command dispatch must continue to report magic as unavailable.'
 ],'The translation reuses Command number parsing, UserDB getters/setters, JsonDB serialization, the real chain tip and Magic JSON. JSONValue.raw carries the MiniNumber decimal representation as a number. A type guard in JsonDB.getNumber retains Java’s cast behavior before the existing MiniNumber constructor. No extra command registration or automatic save is introduced.',evidence,
 'After kissvm:3000 succeeds and txpowsize:bad fails, is the first write undone? No. Does compiling magic.cpp make the command publicly available? No: source registry membership is a separate behavior.',limits)
