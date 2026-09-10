# cointrack — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/cointrack.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/cointrack.cpp)

## What this file does

Add or remove a coin’s MMR entry from the relevant-entry list of its containing tree node. Recalculate that node’s relevant coins when the list changes or tracking is disabled.

## Follow the logic

1. Read the required coinid string and parse it as MiniData before reading enable. Invalid ID input therefore wins over a later Boolean error.
2. Find the tree node containing the ID. A missing node produces the source coin-not-found error. Then search for the unspent coin; a node may exist even when this second search returns null because the coin is spent.
3. When enabling, ask whether the coin’s MMR entry is already relevant. If yes, return Coin already tracked without recalculating a potentially stale computed list.
4. Otherwise append the MMR entry and recalculate. Relevance is based on entry numbers, not unique coin IDs: several fixture coins sharing an entry can all become relevant.
5. When disabling, remove every occurrence of that entry and always recalculate, even if no occurrence was present. Return the source response only after recalculation succeeds.
6. Errors do not roll back earlier mutations. Observe the entry list and computed coins after SQL and decoding failures to identify how far execution got.

## How the translation is approached

The command reuses MiniData and Command getters, TxPoWSearcher, TxPoWTreeNode.removeRelevantCoin and calculateRelevantCoins. SQL rows flow through the real CoinDB and MinimaDB bridge. The comparison exposed eager dereferencing of nullable SQL coins and an empty-inner-loop mismatch; the shared implementations now retain the existing nullable Coin handle and source loop order. These are component fixes, not command-specific substitutes.

## What the tests tell us

[The lab written before these tests](../../../../../../../new-tests/node-state-commands.md) and [the executable corpus](../../../../../../../../tools/node_state_corpus.py) use the existing wallet fixture with actual command bodies, chain/MMR objects, RAM entries, preferences and recorded SQL. Both implementations receive the same source-generated wire. Exact reply text, current entry lists, relevant coin/MMR wire, preference wire and saved preference bytes are observed after each operation. SQL tree IDs are independently random: the comparison checks their 32-byte shape and consistent use, then labels them by encounter order. Other SQL bindings and effects remain exact. Independent assertions check counts, tracking results and generator preferences. See the lab for final run evidence and the mismatches that motivated shared fixes.

## Check your understanding

If the entry is already tracked but the computed list is empty, does enabling rebuild it? No. Why are lists inspected after a failed disable? Removal occurs before recalculation and is retained if recalculation fails.

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. Supplied SQL rows do not establish durable storage. Full Main startup, live chain/network integration, arbitrary retained collection aliases, concurrent mutation and exhaustive platform behavior remain unfinished. Java replaces some lists while C++ currently mutates vectors; held aliases need separate parity work.

Pinned Java SHA-256: `90ada3625f51d935c92b720b9a01894920d7b83084129f5029923e104abc2fb1`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
