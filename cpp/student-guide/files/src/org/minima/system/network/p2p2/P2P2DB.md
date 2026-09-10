# P2P2DB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p2/P2P2DB.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java) | [src/org/minima/system/network/p2p2/P2P2DB.cpp](../../../../../../../../src/org/minima/system/network/p2p2/P2P2DB.cpp) |

## What this file does

Maintain the second discovery system’s live peer-string list alongside JsonDB persistence and a first-startup flag.

## Follow the logic

1. getAllKnownPeers returns the same mutable ArrayList. addPeerToAllKnown checks list membership, appends only a new value, logs either outcome and returns whether it added.
2. saveDB first converts the live list to all_known_peers in JSON, then calls saveObjectDirect. A failed disk write can therefore leave changed in-memory JSON.
3. loadDB first invokes loadObjectSlow on this object, clears the existing live list, then casts and appends JSON entries one at a time. A late non-string leaves earlier entries already appended.
4. The list object survives loading, so old callers observe its cleared/repopulated contents. Direct readDataStream affects JsonDB data without automatically rebuilding this separate list.
5. first_startup defaults to true and is changed only by its setter; adding a peer does not implicitly clear it.

## How the translation is approached

C++ reuses JsonDB virtual load/save dispatch, MiniFile and MiniUtil string-list conversion. A shared vector of optional strings preserves list identity and null values. UTF-16-compatible string comparison supports duplicate detection; native log text lacks Java logger prefixes.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/p2p2_db_differential.py). P2P2DBOracle and p2p2_db call load/save through a JsonDB reference, retain an alias, compare wire and file bytes, and inject absent/truncated/malformed files and a blocked output path. A JSON list with valid strings, null and a later integer tests partial append before failure. Errors and log prefixes are normalized. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [JsonDB](../../../../../../../../../upstream-study/src/org/minima/utils/JsonDB.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java).

Method navigation (declarations with bodies found in this file): [P2P2DB](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java#L15), [loadDB](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java#L20), [saveDB](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java#L35), [isFirstStartUp](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java#L50), [setFirstStartUp](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java#L54), [getAllKnownPeers](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java#L58), [addPeerToAllKnown](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2DB.java#L62).

## Check your understanding

After loading ["first",null,7,"last"], what should the retained list alias show when the integer cast fails?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `179ebeee728891cba277f6a5bd56c009fc0b69af7ff035d6a2bd738564ef7163`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
