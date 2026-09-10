# P2PTestParams — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/params/P2PTestParams.java](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PTestParams.java) | [src/org/minima/system/network/p2p/params/P2PTestParams.cpp](../../../../../../../../../src/org/minima/system/network/p2p/params/P2PTestParams.cpp) |

## What this file does

Provide a mutable test configuration and explicitly copy selected values into P2PParams when setTestParams is called.

## Follow the logic

1. Test defaults use DEBUG, two non-P2P links, a five-second loop, three-second variability, one-minute save/inbound/hash refresh, five-second walk expiry and thirty-second metrics.
2. The test default-node list contains minima_one:9001 and has fixed size. The metrics URL is a test field but is not copied into P2PParams.
3. setTestParams assigns the node-list reference first and then copies the listed scalar fields in source order. VERSION and PEERS_LIST_SIZE are not assigned.
4. Later edits to a copied scalar require another application. Element changes through the shared list are visible through both configurations, while replacing the test list does not change the active reference until application.

## How the translation is approached

A lazily initialized singleton settings object models Java static fields and defers host resolution until test configuration is accessed. The active list shares JavaList ownership, preserving fixed-size behavior and subsequent reference replacement.

## What the tests tell us

[Read the differential experiment](../../../../../../../../../tools/peer_state_differential.py). The testParams programs inspect defaults before application, modify each integer at signed boundaries, apply repeatedly and compare aliases. Explicit assertions require DEBUG/5000 defaults, the minima_one entry, errors on structural edits and visible element replacement through the active alias. Null list replacement and log-level changes are also exercised. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [P2PFunctions](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java).

Method navigation (declarations with bodies found in this file): [setTestParams](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PTestParams.java#L75).

## Check your understanding

Apply test settings, replace the test list, then append through the old active list. Which list will the next application install?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `68c965ef0efbc4b1bced431e9ba52e7197c8e8071e3a503871511299dd4ab3ca`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
