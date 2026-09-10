# balance — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/balance.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/balance.cpp)

## What this file does

Report wallet coin totals by token. Confirmed means old enough; sendable further requires a simple wallet address and, for custom tokens, the exact token script RETURN TRUE. Neither label guarantees that a later send succeeds.

## Follow the logic

1. Read address, confirmations, debug, simple, coinlist, tokendetails and raw tokenid in source order. Require a chain tip before parsing megammr. debug is read by the body but omitted from the public valid-parameter list.
2. Without an address, search all relevant unspent coins. With an address, search all matching coins and include MegaMMR only when both the request and node enable it. The no-address branch ignores the MegaMMR flag.
3. Seed the Minima group, then traverse search results in order. Apply exact address and token-ID string filters. New custom token IDs require non-null token metadata immediately, even if the coin will be unconfirmed.
4. Compute depth as tip block minus creation block. A depth below confirmations is unconfirmed; equality is confirmed. Add the raw amount, increment the number of coins and retain each coin in its token list.
5. Debug logs describe every included coin and whether its address is simple. That SENDABLE log does not check confirmation depth or token script. The actual sendable total checks those additional conditions.
6. Emit Minima first unless filtered out, then token groups in first-encounter order. Convert custom-token raw totals through the existing scaling method. Optionally include coin lists and token details.
7. Store custom-token names as raw MiniString text, matching the source’s unconditional final assignment. This can produce invalid JSON for a plain name. simple mode instead parses the name, requires a JSONObject and returns its name field with the confirmed amount.

## How the translation is approached

The native command reuses TxPoWSearcher, Wallet.isAddressSimple, MiniNumber arithmetic, Token scaling, JSONValue.raw, JSONParser and the existing strict object-cast helper. It keeps a separate encounter-order vector beside lookup maps. The fixture serializes each actual reply into a string so malformed source JSON remains observable. Coin-list entries are serialized native values; arbitrary Java reference mutation and serialization timing are not certified.

## What the tests tell us

[The pre-test balance lab](../../../../../../../new-tests/wallet-balance.md) and [the comparison corpus](../../../../../../../../tools/wallet_balance_corpus.py) test real search, spent/duplicate filtering, wallet cache, confirmation boundaries, address versus relevance, MegaMMR, token scaling, raw names and public dispatch. Hand-calculated checks include confirmed 6/unconfirmed 15 for six fixture coins at tip 11 and depth 3, plus custom-token totals scaled by 100. The first comparison exposed an existing MiniNumber overflow-message discrepancy. Direct constructor probes distinguish BigDecimal/plain conversion from String/exponent input. All 275 programs, 776 operations and 21,515 SQL calls now match Java in both normal and ASAN/UBSAN builds (11.57s/37.07s), including 24 direct number-constructor probes. The lab records the broader regressions separately.

## Check your understanding

At tip 11, a simple-address coin created at 9 has amount 2. With confirmations:3, which totals include it, and what does debug log? Answer: unconfirmed includes 2, confirmed and sendable do not, yet debug logs SENDABLE because that label checks only address simplicity. Why can a plain custom-token name break simple mode?

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. The comparison uses source-generated fixture coins and supplied SQL rows. Full Main, durable SQL, live balances, mutable Java object identity, allocator/platform behavior and concurrency remain unfinished or unverified.

Pinned Java SHA-256: `02f70532559ced2800ace362544e93efe2699ea35e816dc6621393741b3164ae`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
