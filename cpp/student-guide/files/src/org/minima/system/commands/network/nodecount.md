# nodecount — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/nodecount.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java) | `src/org/minima/system/commands/network/nodecount.cpp` (planned) |

## What this file does

Crawls peer-advertised addresses and estimates server/client totals, optionally writing progress rows.

## Follow the logic

1. Optionally open a progress file and write a CSV header. The source does not close the PrintWriter. Read complete, then create sets for discovered addresses and already-pinged addresses.
2. Ping every default seed with the full-information flag. Add advertised peers from successful greetings to the discovered set, and mark seeds pinged even on failure. Seed client counts are not accumulated.
3. Scan discovered addresses not yet pinged. For each successful greeting, parse its clients field as a double and union its advertised peers into the discovered set. Mark every attempted address pinged.
4. Compute percentage as pinged/discovered/0.06 times 100. Unless complete is true, stop at that threshold. Complete mode uses pinged/discovered times 100 instead. A zero discovered count can yield floating NaN or infinity.
5. After each attempt, optionally write and flush a CSV row. Recompute the pending set after a batch so newly discovered addresses can be visited.
6. At termination, estimate servers as discovered count times 1.05 and clients as accumulated clients/pinged count times estimated servers. Complete mode uses discovered count and accumulated clients directly. The denominator includes seed attempts whose client counts were never added.

## How the translation is approached

No native nodecount counterpart exists. A faithful port needs controlled ping responses, address-set identity and Java float/double conversion semantics. The source’s statistical confidence comment is not a validated property of this algorithm.

## What the tests tell us

Proposed deterministic fixtures should supply a finite fake peer graph with duplicate addresses, failed seeds, absent clients, newly discovered peers and an empty graph. Compare query order only where set iteration is specified, plus final totals, early stopping and flushed CSV content. Test partial file output on parse failures and document resource lifetime. No external network crawl or native test has been run for this page.

## Dependencies and reading map

Direct Minima imports: [Greeting](../../../../../../../../../upstream-study/src/org/minima/objects/Greeting.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [InetSocketAddressIO](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java), [P2PParams](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [nodecount](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java#L24), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java#L29), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java#L34), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/nodecount.java#L150).

## Check your understanding

If two seed attempts are counted as pinged but only later peers contribute client totals, how can the estimated clients-per-server ratio change?

## Limits and traceability

This source algorithm is an estimator with observable rounding and sampling quirks; it is not evidence of the true size of a live network. The mapped native file is absent; all validation proposed here is future work.

Pinned Java SHA-256: `665bd3c4a90b2e21983fa4a6b4322ee044347e606948e79f7e86ab15f2177c95`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
