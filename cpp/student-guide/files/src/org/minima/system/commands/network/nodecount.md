# nodecount — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native class compiles and has direct workflow coverage. Like Java, it is absent from the command registry.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/nodecount.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java) | `src/org/minima/system/commands/network/nodecount.cpp` |

## What this file does

Crawls peer-advertised addresses and estimates server/client totals, optionally writing progress rows.

## Follow the logic

1. Optionally open/truncate a progress file, then read complete and create the peer sets. Ping defaults before buffering the CSV header. The source does not close the PrintWriter; without a later scan iteration, the header never reaches the file.
2. Ping every default seed with the full-information flag. Add advertised peers from successful greetings to the discovered set, and mark seeds pinged even on failure. Seed client counts are not accumulated.
3. Scan discovered addresses not yet pinged. For each successful greeting, parse its clients field as a double and union its advertised peers into the discovered set. Mark every attempted address pinged.
4. Compute percentage as pinged/discovered/0.06 times 100. Unless complete is true, stop at that threshold. Complete mode uses pinged/discovered times 100 instead. A zero discovered count can yield floating NaN or infinity.
5. After each attempt, optionally write and flush a CSV row. Recompute the pending set after a batch so newly discovered addresses can be visited.
6. At termination, estimate servers as discovered count times 1.05 and clients as accumulated clients/pinged count times estimated servers. Complete mode uses discovered count and accumulated clients directly. The denominator includes seed attempts whose client counts were never added.

## How the translation is approached

The mapped class compiles directly, with the original constructor, accepted parameters, inherited help and fresh-instance factory. It is deliberately absent from CommandRunner because the pinned Java registry does not include it. Compiling every source file is different from making every class a terminal command.

The scan uses existing NIOManager ping transport, SocketAddressSet, JavaHashMap and InetSocketAddressIO. Copying the pending set uses the installed JDK collection constructor capacity, then removes already-pinged peers. Keeping the returned peer list in a local shared owner avoids invalid iterators after a temporary smart pointer is destroyed.

CSV output reuses the native file-open/write primitives and explicitly buffers until the source's flush point. The descriptor closes by RAII while unwritten header text stays unwritten. Float32 operations preserve the source's rounding stages; double parsing accepts Java decimal/hex/suffix syntax. Serialized float results use the existing raw numeric JSON representation; arbitrary Java Float-object APIs are not implemented by this command.

## What the tests tell us

The [node-count lab](../../../../../../../new-tests/node-count.md) documents the experiment. `tools/node_count_differential.py` starts isolated loopback servers and executes actual Java/C++ ping transport. Both runtimes use the same bound ports, so the comparison can check address hash ordering and exact visited-node sequences. Every request must contain the real SINGLE_PING plus ZERO_TXPOWID wire envelope.

Both normal and ASAN/UBSAN comparisons pass 171 programs and 314 pings per runtime. Complete graphs independently check measured totals and visit counts. Other cases compare cycles, discoveries over multiple passes, duplicate defaults, sampled traversal, broken frames, malformed peer types and numeric edge cases. Replies, traffic breakdowns, logs and complete CSV bytes are compared. A one-node scan checks that an opened CSV stays empty when no later peer is visited.

Tests exposed three implementation issues: mistaken registration of this source-only class, a temporary-list lifetime bug, and older peer-decoder exception categories. Numeric tests also required retaining embedded NUL in error messages. Failed comparisons remain in the validation directory. This is finite local evidence, not proof of a live network's size.

## Dependencies and reading map

Direct Minima imports: [Greeting](../../../../../../../../../upstream-study/src/org/minima/objects/Greeting.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [InetSocketAddressIO](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java), [P2PParams](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [nodecount](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java#L24), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java#L29), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java#L34), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java#L150).

## Check your understanding

If two seed attempts are counted as pinged but only later peers contribute client totals, how can the estimated clients-per-server ratio change?

## Limits and traceability

This source algorithm is an estimator with observable rounding and sampling quirks; it is not evidence of the true size of a live network. The native counterpart exists with finite local validation. Full node integration, every inherited/reference/concurrency API, other JDK collection versions, identity-hash tree collisions and all writer-lifetime/platform behaviors remain unverified.

Pinned Java SHA-256: `665bd3c4a90b2e21983fa4a6b4322ee044347e606948e79f7e86ab15f2177c95`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
