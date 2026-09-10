# NIOTraffic — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/minima/NIOTraffic.java](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java) | [src/org/minima/system/network/minima/NIOTraffic.cpp](../../../../../../../../src/org/minima/system/network/minima/NIOTraffic.cpp) |

## What this file does

Keep synchronized byte totals and independently accumulated per-category traffic statistics.

## Follow the logic

1. reset captures the current time, clears both totals and replaces both breakdown tables. It does not preserve old table entries.
2. addToTotalRead and addToTotalWrite change only overall counters. addReadBytes and addWriteBytes change only a named breakdown entry: the apparent total updates in those methods are commented out.
3. A category starts at zero, then adds the signed amount. Java long arithmetic wraps on overflow and accepts negative increments.
4. getBreakdown enumerates each Hashtable and formats values with MiniFormat.formatSize into reads/writes JSON. Null category keys fail through Hashtable rules.

## How the translation is approached

C++ holds a mutex around each operation, uses JavaHashtable for tested UTF-16 key enumeration and performs addition through unsigned arithmetic plus bit reinterpretation to obtain Java wraparound without C++ signed-overflow undefined behavior. resetAt is a native fixture aid for deterministic timestamps.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/network_utilities_differential.py). Traffic programs interleave resets, total and category additions with zero/negative/extreme signed values, collision-heavy keys and Unicode. After each step the fixture compares result, totals and formatted ordered JSON, and finally the chosen start time. This sequential experiment does not establish concurrent interleaving equivalence. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MiniFormat](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [NIOTraffic](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L18), [reset](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L22), [getStartTime](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L31), [getTotalRead](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L35), [getTotalWrite](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L39), [getBreakdown](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L43), [addToTotalRead](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L73), [addToTotalWrite](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L77), [addReadBytes](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L81), [addWriteBytes](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java#L92).

## Check your understanding

After addReadBytes("greeting",100), what is totalRead? Explain why automatically adding to the total here would double-count some callers.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `d4386b863c6b1f72538dd34d479a42849e78e38f2a9d5b1aa6b8dd45ef8b5853`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
