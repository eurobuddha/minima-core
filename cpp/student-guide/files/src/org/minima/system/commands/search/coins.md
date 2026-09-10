# coins — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/search/coins.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/search/coins.cpp)

## What this file does

Search for coins, filter their age and optional reservations, optionally select an amount, then report each coin with its age. This composes existing components; it does not construct a transaction.

## Follow the logic

1. Enable relevance when relevant is present, regardless of its value, or when none of relevant/coinid/address/state/tokenid is present. Thus relevant:false still means a relevant search, and amount alone does not disable the default.
2. Read simple-state formatting, sendable, coin ID, amount, address, token ID, state, coinage and depth. Use the existing address getter for 0x/Mx normalization; coin and token IDs use raw MiniData parsing. Depth uses Java int conversion, including wrapping.
3. Return an empty response for a missing tip before reading megammr, checkmempool, order or totalamount. Otherwise run the real search with substring state matching, simple-address filtering and optional globally enabled MegaMMR.
4. Keep coins created at or before tip minus coinage. If checkmempool is true, retrieve the Main miner even for an empty list, then visit each coin: skip mining reservations before checking RAM witness membership.
5. Only the exact order text asc triggers a stable creation-height sort. The default desc and other strings retain search order.
6. When totalamount exists, use the first coin’s token scaling only for a non-Minima selected token ID, then call send.selectCoins. That helper sorts amounts and groups addresses, looks for a single qualifying coin and otherwise returns its whole ordered list. It is not a sum-until-target algorithm.
7. Serialize each selected coin with the requested state format, add age as a String, and place the array in the normal command reply.

## How the translation is approached

The C++ command directly reuses TxPoWSearcher.searchCoins, txnutils.getMiner, TxPoWMiner.checkForMiningCoin, TxPoWDB.checkMempoolCoins, Token scaling, send.selectCoins and Coin.toJSON. A stable sort implements the source asc branch. Existing parameter helpers preserve typed errors and parsing order. No alternate search, miner or mempool is introduced. MiniData.ZERO_TXPOWID is 0x00, so the omitted token filter has the same stored value as Minima.

## What the tests tell us

[The pre-test coin-search lab](../../../../../../../new-tests/coin-search-command.md) and [the corpus](../../../../../../../../tools/coin_search_corpus.py) compare the actual command using existing wallet, chain, Coin/Token, miner-reservation and RAM-witness fixtures. Independently calculated IDs, ages and order check the important boundaries. Cases distinguish absent Main from a present Main with null miner, age from depth, relevance from explicit filters, and checkmempool from the selector’s own reservation checks. A supplied blank state is rejected; it is not an omitted search filter. The final 287 programs, 1,144 operations and 25,168 SQL calls match Java in normal and ASAN/UBSAN builds (20.89s/41.27s). Exact state-match IDs and a RETURN FALSE token confirm that coins sendable:true checks address simplicity without evaluating the token script. The lab records the broader regressions.

## Check your understanding

Why can totalamount:7 return all five candidates rather than the first few that sum to 7? Answer: the reused selector tries to find one qualifying coin; if it cannot, it returns the whole ordered list. Also explain why relevant:false does not broaden this command’s search.

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. Recording SQL and synthetic chains do not establish durable storage, Main startup, live network balances, atomic snapshots, arbitrary Java reference mutation or concurrency/platform parity.

Pinned Java SHA-256: `8638c3281e2ec27cf3e747abbfd243893332d2602357fb3dc59b521ac328c5cf`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
