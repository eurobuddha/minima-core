# TxPoWSearcher — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/brains/TxPoWSearcher.java](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java) | [src/org/minima/system/brains/TxPoWSearcher.cpp](../../../../../../../src/org/minima/system/brains/TxPoWSearcher.cpp) |

## What this file does

Queries chain coins, blocks, transaction addresses and token definitions. Its central problem is choosing the newest qualifying version of a coin while preserving source traversal order.

## Follow the logic

1. General coin search walks from a supplied tree node toward its parents. The depth comparison is depth++ > limit: zero visits the starting node, one also visits its parent, and a negative limit visits none. An absent starting node performs no search, including no MegaMMR fallback.
2. Read either cached relevant coins or all coins from each node. Apply token, coin ID, amount, address and state filters before examining spent status. Then mark a spent matching ID as seen; otherwise deep-copy the first unseen unspent coin. Consequently a newer spent record that fails a filter does not suppress an older record that passes it.
3. If enabled and reached within the depth limit, take one final MegaMMR pass after chain ancestors. Acquire its read lock, copy the table values, process them and unlock. The final pass consumes a depth step. Simple-address filtering happens after the combined search using wallet membership.
4. Block-height and coin-location searches return the first matching ancestor. searchChainForTxPoW returns the containing block when the requested ID is either that block or a transaction referenced by it; the block-only variant checks only block IDs.
5. Address-history search checks block main/burn inputs and outputs, then fetches referenced transactions. This work occurs for every matching coin in a node without deduplication: two matching coins can produce repeated results and database calls.
6. Relevance checks examine main and burn input addresses and HEX state, then output addresses, then complete transaction state. Address and public-key wallet checks short-circuit, so ordering also controls which service calls occur.
7. Token enumeration scans all chain coins, including spent ones, deduplicates non-Minima token IDs, and appends imported tokens afterward. It does not search MegaMMR. Token lookup searches chain, then the optional MegaMMR branch, then imports. A successful early return inside that MegaMMR branch leaves the Java read lock held; the native adapter deliberately exposes this behavior.
8. Imported tokens are shared mutable references under synchronization. Null token references can enter the collections and cause later failures. Coin results are deep copies, while returned tree/block references can remain connected to their containing objects; these are distinct ownership contracts.

## How the translation is approached

The counterpart reuses tree, MegaMMR, wallet and transaction services, with recursive locking for imported tokens and explicit service hooks for database access and lock observations. Native token lookup copies chain/MegaMMR token values in places where Java retains a token object; full reference-identity parity remains open.

## What the tests tell us

The [Java oracle](../../../../../../../support/SearcherOracle.java), [native harness](../../../../../../../support/searcher.cpp) and [driver](../../../../../../../tools/searcher_differential.py) compare coin wire values, block IDs, node identity indices and lock counts. Inputs include repeated coin IDs, spent variants, null token references, all five filters, negative/zero/bounded depth, relevant/simple flags and MegaMMR. Seeded query sequences exercise imports and repeated state changes. Real MMR histories also support token/coincheck/coinexport command cases. The inspected report passes 189 programs and 6,468 queries across this whole family, not 6,468 searcher-only assertions. Both harnesses clean remaining locks after each query: that records the early-return lock leak without proving a long-running process survives it.

The [coin and proof reader lab](../../../../../../new-tests/coin-streams.md) also calls real floating-coin search. If a matching coin’s deep copy logs an I/O failure and returns null, the native single-result helpers now return an empty optional. Keeping a present optional containing a null Coin handle would falsely signal a result. Logger exceptions still escape.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java), [TxPoWTreeNode](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [Wallet](../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [StateVariable](../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [Token](../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [GeneralParams](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java).

Method navigation (declarations with bodies found in this file): [getAllRelevantUnspentCoins](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L25), [getRelevantUnspentCoins](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L36), [searchCoin](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L48), [searchCoin](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L52), [getFloatingCoin](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L70), [searchCoins](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L88), [searchCoins](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L100), [searchCoins](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L113), [getTreeNodeForCoin](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L248), [getTxPoWBlock](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L275), [searchChainForTxPoW](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L295), [searchChainForTxPoWBlock](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L329), [searchTxPoWviaAddress](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L352), [checkTxPoWForAddress](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L404), [checkTxPoWRelevant](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L437), [getAllTokens](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L529), [importToken](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L595), [getToken](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java#L601).

## Check your understanding

A newer spent coin has amount 2 and an older unspent version has amount 1. Search for amount 1. Explain why the filter-before-spent ordering can return the older version, then predict an unfiltered search.

## Limits and traceability

Depth limits, partial-state effects and lock counts have concrete fixtures, but general concurrent access, arbitrary service exceptions, retained token identity and full runtime integration remain open. Counterpart existence and passing selected queries do not establish complete API parity.

Pinned Java SHA-256: `63ed77f401b2ad4c8246a179f11742e3b931b4d12fe3693ff3016e430e76fbe5`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
