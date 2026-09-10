# Lab: finding a TxPoW across chain, RAM and stored history

Written before the new tests. Read search/txpow.java, TxPoWDB, TxPoWOnChainDB,
ArchiveManager and TxPoWSearcher. A TxPoW is a proof-of-work object that can contain
transactions or represent a block. This command chooses one query by parameter
presence: action, txpowid, relevant, inblock, onchain, block, then address.

Reuse the real search, RAM, SQL and archive bodies. For txpowid, try RAM/SQL, then
an archived TxBlock, then the configured MySQL command via the real runner. For
block height, search the current chain before the archive. For onchain, locate
the containing chain block before consulting the on-chain SQL index. Report
confirmations as tip minus containing height, without adding one. Negative values
are possible with inconsistent supplied rows; this query does not validate them.

relevant is a presence switch: relevant:false still selects relevant SQL rows.
Both relevant and address results deduplicate TxPoW IDs while preserving first
encounter order. inblock treats only lower-case 0x as the ID-form prefix; otherwise
parse a number and apply Java long conversion. info executes size/first/last
queries in source order and includes the configured SQL retention days.

Use existing source-generated Coin/TxPoW and chain/MMR fixtures. Add a small
optional rowsBySQL selector to the existing recording drivers: a SQL miss and an
archive hit in one command must receive different result sets. The default row
behavior remains available. No candidate with this selector was found in current
or sibling test support. This is fixture routing, not an SQL engine. Compare the
exact prepared SQL identities, bindings, call order, partial effects and decoded
wire outputs. Exercise missing/nullable/malformed rows and SQL/runtime failures.

Reuse the existing scoped command-prototype recording pattern for the MySQL
fallback. The real runner parses the delegated command and handles replies; only
the mysql body is controlled in these tests. Check false/true/null/wrong-type
found fields, absent response, command failures and raw returned txpow values.
Do not claim native MySQL, durable storage or live node integration from this.

Independently assert branch priority, returned IDs/order, confirmations, omitted
fields and the absence of fallback SQL when RAM or the chain already answered.
Capture full serialized replies, retaining original JSON key order and data.
Only established Java-specific exception diagnostics are normalized. Explicit
stored timestamps are deterministic; no command wall-clock normalization is needed.

Review extension before its tests: the txpowid fallback reads response.found
without checking the nested command's status. Supply status:false with found:true
and assert that the raw txpow is still returned. Also enable MySQL preferences
while querying a missing height or missing on-chain ID and assert that neither
mode delegates to MySQL. The fallback belongs only to txpowid mode.

Final evidence: both full builds pass without warnings. All 680 command programs,
2,099 operations and 41,510 SQL observations match Java in normal and ASAN/UBSAN
builds (10.28s/26.97s). The initial 677-program corpus also passed both builds
(10.93s/25.58s), before the three fallback-scope review cases were added.

All 12 selected checks pass in each build, taking 58.91s normal and 156.18s with
sanitizers. Besides this command and the 340-counterpart map, they cover wallet
commands (675 programs), history (484), ArchiveManager (3,333; 10,831 operations;
125,655 SQL calls), TxPoWDB (1,355; 7,407 operations; 80,960 effects), OnChainDB
(855; 3,639 operations; 29,167 SQL calls), SqlDB (464; 1,260 operations; 8,352 calls),
searcher (189 programs; 6,468 queries), Command (12,289 cases), runner (1,265) and
runner errors (1,120). All 22 final comparison reports were inspected. The project
has 178 CTest entries; this was a selected regression run, not the entire suite.

Fixture corrections are also instructive. Java's command registry is a final
array: reuse the existing scoped element-replacement pattern, not array reassignment.
The initial address expectation omitted a burn input at address 0x01; both source
and native correctly returned that additional TxPoW. The MySQL recorder needed
its forwarded-call observations included in the output envelope. These fixes
changed test support and assertions; no production behavior discrepancy was found.

The native MySQL command, durable SQL and full Main/node integration are still
unfinished. These cases preserve and test delegation without claiming those
missing components have been implemented.
