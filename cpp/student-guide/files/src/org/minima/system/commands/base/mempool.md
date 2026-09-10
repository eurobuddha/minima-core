# mempool — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/mempool.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/mempool.cpp)

## What this file does

Summarize the TxPoW objects held in RAM and report the SQL coin-store size. This command reports classifications; it does not validate or remove transactions.

## Follow the logic

1. Obtain the actual TxPoWDB map and enumerate RamData values in ConcurrentHashMap order. No sorting step is present.
2. For a cascade entry, increment only the cascade counter. The contained TxPoW is never read, so even a null TxPoW inside such an entry is harmless here.
3. For every noncascade entry, increment size, read its TxPoW and append its ID, transaction flag and block flag. Test both flags independently when updating the counters: one object may count as both.
4. Return the map size as total and the noncascade count as onchain. The variable name is misleading: the algorithm does not inspect the on-main-chain flag.
5. Finally query the singleton CoinDB size. A SQL failure can occur after enumeration. Preserve Java integer wraparound for the counters and the source reply envelope.

## How the translation is approached

The translation reuses Command replies, TxPoWDB, RamData, TxPoW, JavaConcurrentMap traversal and CoinDB.getSize. Explicit unsigned addition followed by bit_cast preserves Java int wrapping without C++ signed-overflow undefined behavior. The command is registered in the native runner like the pinned source.

## What the tests tell us

[The lab written before these tests](../../../../../../../new-tests/node-state-commands.md) and [the executable corpus](../../../../../../../../tools/node_state_corpus.py) use the existing wallet fixture with actual command bodies, chain/MMR objects, RAM entries, preferences and recorded SQL. Both implementations receive the same source-generated wire. Exact reply text, current entry lists, relevant coin/MMR wire, preference wire and saved preference bytes are observed after each operation. SQL tree IDs are independently random: the comparison checks their 32-byte shape and consistent use, then labels them by encounter order. Other SQL bindings and effects remain exact. Independent assertions check counts, tracking results and generator preferences. See the lab for final run evidence and the mismatches that motivated shared fixes.

## Check your understanding

Why can transactions + blocks exceed onchain? Because a single noncascade object may be both a transaction and a block. Why can a null cascade TxPoW succeed while a null noncascade TxPoW fails? Only the noncascade branch dereferences it.

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. Supplied SQL rows do not establish durable storage. Full Main startup, live chain/network integration, arbitrary retained collection aliases, concurrent mutation and exhaustive platform behavior remain unfinished. Quiescent collision-map tests cover encounter order; they do not prove weakly consistent enumeration while other threads mutate the map.

Pinned Java SHA-256: `4a50a1d83c4b69fabbf6f8679a23595c82950e013e648cab7d7d6dc90e5072ee`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
