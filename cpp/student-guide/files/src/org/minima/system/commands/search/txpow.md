# txpow — teaching guide

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/search/txpow.java) · [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/search/txpow.cpp)

## What this file does

Choose one kind of TxPoW lookup. The result may come from RAM, the retained chain, stored TxPoW history, the archive, the on-chain index or a configured MySQL command. A stored object and a currently confirmed object answer different questions.

## Follow the logic

1. Select the first present parameter in this exact order: action, txpowid, relevant, inblock, onchain, block, address. Later parameters do not select another mode. action accepts only info and queries TxPoW SQL size, on-chain size, first row and last row, then reports the configured retention days.
2. For txpowid, normalize the identifier through getAddressParam and call the real TxPoWDB: it checks RAM before SQL. If missing, load an archived TxBlock and take its owned TxPoW. Only if that also misses, inspect MySQL autologin preferences and delegate mysql action:findtxpow through the normal command runner.
3. The MySQL fallback reads the first result’s response object and Boolean found field without first checking its status. found:true returns response.txpow as-is, even null or a non-object value. Wrong or missing fields raise the source’s typed errors; found:false produces the ordinary not-found command error.
4. Presence of relevant selects relevant SQL history even when its value is false, null or not Boolean. Convert max through MiniNumber to a Java int, including truncation and wrapping. Emit each returned TxPoW ID once in first-encounter order.
5. For inblock, only a lower-case 0x prefix selects the ID overload. Other text is parsed as a number and converted to a Java long. Query the existing on-chain SQL helper and wrap its rows under txns.
6. For onchain, first search the retained chain for the object or a block that references its ID. If absent, query the on-chain SQL index. A found result contains String block, blockid, tip and confirmations fields; confirmations equals tip minus containing height. With tip 12, a transaction in block 10 has 2 confirmations and one in block 12 has 0. A missing result contains only found:false.
7. For block, search the retained chain by height before loading from the archive. This mode does not use MySQL. For address, run the actual address search, including main and burn inputs/outputs, then deduplicate IDs in encounter order. No recognized search parameter produces the source error.

## How the translation is approached

The C++ command composes TxPoWDB, TxPoWOnChainDB, ArchiveManager, TxPoWSearcher, UserDB and the existing Command parameter helpers. Shared TxPoW owners preserve archive object lifetime. Reused CommandResult helpers perform real runner delegation and strict Boolean access; RawTxnCommands supplies the existing JSONObject check, and StateText supplies Java-style field text after an explicit null check. An unordered_set is used only for membership, never traversal, so output order remains source encounter order. Source help and the complete valid-parameter list are retained.

## What the tests tell us

[The lab written before the tests](../../../../../../../new-tests/txpow-search.md) and [the executable corpus](../../../../../../../../tools/txpow_search_corpus.py) compare actual command replies and component effects with Java. Source code generates Coin, TxPoW and TxBlock wire; both implementations process it. Optional rowsBySQL routing in the recording drivers represents a normal SQL miss followed by an archive hit without replacing either lookup implementation. Independent assertions verify returned IDs, deduplication order, confirmations, query order, numeric wrapping and no SQL after RAM/chain hits. Malformed wire, partial store initialization, nullable fields and SQL/runtime failures test error boundaries. A scoped MySQL prototype records calls through the actual runner; cases check raw response values, failed nested status and that other query modes never delegate. Exact reply strings preserve JSON key order and ordinary text resembling exception messages. All 680 programs, 2,099 operations and 41,510 SQL observations match Java in normal and ASAN/UBSAN builds (10.28s/26.97s). Both full builds and all 12 selected checks pass. The lab records the broader regression evidence and the corrected burn-input expectation.

## Check your understanding

If both txpowid and onchain are supplied, which mode runs? Answer: txpowid, so a successful result is the object itself and need not contain confirmations. Why can address search include a burn-only transaction while history details omit burn amounts? Answer: these commands intentionally use different source helpers and ask different questions.

## Limits and traceability

Counterpart implemented_partial; full parity not_verified. MySQL delegation tests use a controlled downstream body because the native mysql command remains absent. Supplied SQL rows do not establish durable storage. Full Main startup, live chain/network integration, all retained object mutations and exhaustive reference, concurrency and platform behavior remain unfinished.

Pinned Java SHA-256: `46fcbcb0a918c07717fa7d343a363224dec781fbe03d217eaaa2c2aa0f9a0f0d`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
