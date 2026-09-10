# history — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/search/history.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/search/history.cpp)

## What this file does

Answer four history queries and calculate how a transaction changes the amounts held at tracked wallet addresses. Stored history and current chain membership answer different questions; neither response alone certifies a transaction as confirmed or valid.

## Follow the logic

1. Read action (default list), then relevant (default true). size counts relevant SQL rows. With relevant false it counts rows newer than startmilli, which defaults to the real current time minus one day. customsize sends the required where text to the existing SQL method, including that method’s filtering behavior.
2. transactions parses depth as MiniNumber and converts it to a Java int. Read the tip height before the loop, so even depth zero fails when there is no tip. Walk parents until depth is reached or the chain ends. Add one for a transaction-bearing TxPoW and add the length of its transaction-ID list. Duplicate IDs count repeatedly; they are not loaded or validated here.
3. list parses max using the mutable SQL default and offset using zero. Retrieve relevant rows or latest rows through the real SQL methods. For each decoded TxPoW, call toJSON before the public getTxnDetails helper. Keep parallel txpows and details arrays and report their length.
4. getTxnDetails reads only the main transaction. Visit inputs first, then outputs. A coin contributes exactly when Wallet.isAddressRelevant accepts its address. A tracked address need not be simple. Burn coins and state relevance do not contribute to these totals.
5. Keep a separate total per token on each side. Minima uses the raw coin amount. Other tokens use Coin.getTokenAmount, including its raw-amount fallback when token metadata is absent. For scale 2, raw inputs 1+2+3 become 600 tokens and outputs 4+5+6 become 1500.
6. Enumerate the input and output Hashtables to build decimal-string JSON fields and the union of token IDs. Iterate the HashSet union to calculate output minus input, treating a missing side as zero. For the example above the difference is 900. Preserve these collection traversal orders in serialized JSON.
7. Put the selected response into the normal command envelope. Unknown actions throw. Preserve the source’s typed parameter errors and SQL/runtime failure boundaries, including partial row retrieval and a nullable overridden helper result.

## How the translation is approached

The native command reuses Command getters, TxPoWSqlDB, Wallet, Coin/Token scaling, MiniNumber, TimerMessage.currentTimeMillis, JavaHashtable and JavaHashMap as the HashSet backing behavior. It introduces no alternate database or amount arithmetic. A virtual shared-pointer helper takes a mutable TxPoW pointer, preserving Java override mutation and a null return. Explicit unsigned arithmetic plus bit_cast preserves Java integer wrapping for chain counters and clock subtraction. The source’s raw numeric startblock differs from String token totals.

## What the tests tell us

[The lab written before these tests](../../../../../../../new-tests/transaction-history.md) and [the history corpus](../../../../../../../../tools/transaction_history_corpus.py) explain 484 programs, 1,489 operations and 52,194 SQL observations, all matching Java in normal and ASAN/UBSAN builds (5.46s/10.04s). Source code generates Coin/Token/TxPoW bytes; both implementations decode and process them. Independent assertions cover scaling, missing metadata, tracked-but-not-simple addresses, burn omission, one-sided totals, duplicate coins, negative and zero amounts, many-token totals, mutable pagination defaults and duplicate transaction-ID counts. Exact reply strings make collection order observable. Default timestamps must lie in the measured clock window and agree with SQL bindings before normalization; explicit timestamps remain exact. A review test changes the helper argument’s height to 99 while the command reply keeps the original height 20, demonstrating toJSON-before-helper timing. Tests also observe public helper overrides, nulls, malformed wire, partial database setup, SQL/runtime failures and registry dispatch.

## Check your understanding

A tracked input carries 12 tokens and change returns 5. Explain why difference is -7, why burn inputs do not alter this value, and why a duplicate ID in a block still increases the transactions action’s count. Answers: output minus input; the helper examines only the main transaction; that action counts list entries without deduplication.

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. Recording SQL rows do not establish durable native storage or a live history database. Full Main startup, retained JSON-object mutation, arbitrary nullable Coin collections, exhaustive reference API, concurrency and platform behavior remain unverified.

Pinned Java SHA-256: `e5f200b0110e53664b0dbf1b838a40208dc0e9acfbf23877c5bc4766ca056ca4`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
