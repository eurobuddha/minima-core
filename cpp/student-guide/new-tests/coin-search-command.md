# Lab: coin search, age, reservations and selection

Written before adding the new tests. Read search/coins.java and trace its stages:
parameter parsing, search, age filtering, optional reservations, ordering,
optional amount selection, then JSON formatting.

The source sets relevant from parameter presence, not its Boolean value. Supplying
relevant:false still restricts the search to relevant coins. With none of relevant,
coinid, address, state or tokenid supplied, relevance also defaults on; amount
alone does not disable it. Keep the original help text and executed logic distinct.

Parse simple-state output, sendable, coin ID, amount, address, token ID, state,
coinage and depth before reading the tip. A missing tip returns an empty array
before parsing megammr, checkmempool, order or totalamount. A real tip enables
actual search, with state substring matching, optional simple-address filtering,
wrapped integer depth and MegaMMR only when globally enabled.

Keep coins created at or before tip minus coinage. With checkmempool:true, fetch
the Main miner even for an empty list, then check mining reservations before RAM
mempool membership for each coin. An absent Main differs from a present Main whose
miner is null: the latter is dereferenced only when a coin is visited.

Only order:asc stably sorts by creation block. Other order text preserves search
order. totalamount converts through the first coin's token when the parsed token
ID is not Minima, then calls the existing send.selectCoins. This selector can
reorder by amount/address and may return all coins if no single candidate fits;
it is not a prefix-sum selector. Selection itself also consults the miner/pool.

Reuse the wallet chain fixture, source-generated Coin/Token bytes, real miner
reservation state and actual RAM TxPoW/witness membership. Compare metadata,
replies, errors, logs, SQL/cache observations and stored transaction bytes.
Independently assert coin IDs, ages, ascending stable order, relevant versus full
search, state matching, reservation filtering and selector results. Exercise null
Main versus null miner, missing tip and parameter-error order, malformed parameters,
raw coin/token IDs, Mx addresses, token scaling and public dispatch.

These are offline fixture observations. Supplied SQL rows do not prove durable
storage, and synthetic coins or queued miner objects do not establish live network
acceptance, full Main startup or exhaustive Java reference/concurrency parity.

## Review experiments

The initial 285 programs passed in both builds after correcting one fixture
expectation: a supplied blank state is rejected by Command.getParam. Keep that
rejection alongside the omitted-state path.

Add independent exact coin IDs for state0, state1, hex-state and missing matches,
so state-shape assertions cannot pass vacuously. Also compare a RETURN FALSE token
with sendable:true: this command's search checks address simplicity, whereas
balance's sendable total additionally checks the token script. Preserve the
source distinction rather than imposing a shared meaning on the same word.

## Verified results

All 287 programs, 1,144 operations and 25,168 SQL observations match Java in both
builds (20.89s normal, 41.27s ASAN/UBSAN). Both full builds pass without warnings.
All nine selected checks pass per build (61.55s/193.68s); all 16 comparison reports
were inspected. The other comparisons cover 275 balance programs, 675 wallet
programs, 736 funding programs, 189 searcher programs with 6,468 queries, 12,289
command cases, 1,265 runner cases and 1,120 runner-error requests. Source mapping
confirms 338 counterparts. This selected set is not all 176 CTest entries.

For six fixture coins at tip 11, the default search excludes the one created at
12. The returned ages are 3, 2, 0, 11 and 1 in search order. order:asc changes that
order; totalamount can change it again by invoking the existing selector.
The omitted token ID is 0x00, because that is ZERO_TXPOWID's actual value. It does
not force a non-Minima conversion. No source/native body discrepancy was found
in this batch; the blank-state failure was an incorrect test expectation.
