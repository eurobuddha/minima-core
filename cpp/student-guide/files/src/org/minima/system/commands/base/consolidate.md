# consolidate — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native counterpart exists; exhaustive API and behavior review remain open.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/consolidate.java) · [consolidate.cpp](../../../../../../../../src/org/minima/system/commands/base/consolidate.cpp)

## What this file does

Calculate a wallet consolidation amount and forward a send command back to a default wallet address. The selected coin list is not passed to send; send performs its own selection.

## Follow the logic

1. Read the raw tokenid String, burn, debug and dryrun. Dryrun forces debug. Fetch the tip, parse coinage, and reject an age below the configured confirmation depth.
2. Find relevant, simple, unspent coins for the token; keep sufficiently old coins. Reuse send.orderCoins, which stably sorts descending by amount then groups by address. The original help says smallest first, but the executed source differs.
3. Reject fewer than three eligible coins before reading maxsigs and maxcoins. Those limits default to five and twenty and have no explicit range checks.
4. Compute each coin’s token amount before checking whether a new address exceeds the signature limit. If accepted, add its amount and count, then check the coin limit. A nonpositive signature limit can select none; a nonpositive coin limit can still select the first coin.
5. Ask the wallet for a default address, even if the selected total is zero. Build send with coinage, split:2, dryrun, debug, burn, computed amount, address and raw tokenid.
6. When password exists, log a redacted placeholder before calling the password getter, then append its quoted value. Otherwise log the plain child command if debug is active.
7. Run the actual command runner and select its first reply. A true status contributes response. A false status sets the outer status false and chooses child message, then error, then the whole child reply. Null and wrongly typed status fail during Boolean unboxing/casting.

## How the translation is approached

The C++ implementation directly reuses send.orderCoins, TxPoWSearcher, Wallet.getDefaultAddress and CommandResult.first/status. Source help literals are retained exactly. Optional default-address text preserves null failure, and counter arithmetic explicitly preserves Java integer wraparound. No new coin-selection or transaction-construction implementation is introduced.

## What the tests tell us

[The wallet consolidation corpus](../../../../../../../../tools/wallet_consolidation_corpus.py), [the existing wallet driver](../../../../../../../../tools/wallet_commands_differential.py) and [the pre-test lab](../../../../../../../new-tests/wallet-consolidation.md) compare actual search, ordering, wallet SQL and command runner behavior. A scoped send prototype records the complete forwarded text and parsed parameters, then restores the real prototype. Controlled replies isolate failure precedence; real send cases use existing chain/MMR/miner fixtures. Independent assertions check selected ID order, forwarded totals, split count and password redaction. The expanded consolidation group passes all 247 programs, 742 operations and 23,185 recorded SQL calls in both normal and ASAN/UBSAN builds (17.06s/34.49s). It also exposed and fixed the shared runner status-unboxing error; direct and nested public-runner probes now agree with Java. Both full builds pass without warnings.

## Check your understanding

For amounts 6,5,4,3,2,1 at addresses A,B,C,A,B,C, trace the order and the total with maxsigs:1. Why can consolidation calculate one set of coins while the forwarded send chooses another?

## Limits and traceability

Counterpart implemented_partial and full parity not_verified. Recording SQL and fixture chain data do not establish durable storage or network acceptance. Real dry-run response normalization excludes only the checked 32-byte body PRNG; all other observed fields remain compared. Full native Main and complete API/reference/concurrency behavior remain unfinished.

Pinned Java SHA-256: `63ec0db90fb367114e8608d186879aee9fe91e793ca74ebca7158339339a87fe`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
