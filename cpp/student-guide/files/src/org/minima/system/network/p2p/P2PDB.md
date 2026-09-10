# P2PDB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/P2PDB.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PDB.java) | [src/org/minima/system/network/p2p/P2PDB.cpp](../../../../../../../../src/org/minima/system/network/p2p/P2PDB.cpp) |

## What this file does

Store versioned discovery peers in JsonDB using JSON address objects, with detached lists at the public API boundary.

## Follow the logic

1. getPeersList starts with a fresh empty result and defaults a missing peers key to an empty JSONArray. It decodes every stored address before returning.
2. setPeersList serializes the supplied list immediately and places the completed array in the database. Later changes to the caller’s list do not change stored peers.
3. Each get constructs new addresses and a new list. Mutating a returned list does not silently save it.
4. setVersion captures the current P2PParams.VERSION string. getVersion uses the current parameter only when no stored version exists; it does not upgrade older data automatically.

## How the translation is approached

The translation reuses JsonDB, MiniFile persistence and InetSocketAddressIO. It keeps conversion-before-assignment ordering, so malformed peer output cannot replace the old array with a partially constructed one. This is JSON storage, not a substitute SQL backend.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/peer_state_differential.py). The db programs change external and extracted lists independently, alter live JSON, round-trip exact wire bytes, truncate reads and change both parameter and stored versions. Each observation freezes the mutable JSON before the next operation. Type failures are normalized; complete filesystem integration is a separate responsibility. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [InetSocketAddressIO](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java), [P2PParams](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java), [JsonDB](../../../../../../../../../upstream-study/src/org/minima/utils/JsonDB.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java).

Method navigation (declarations with bodies found in this file): [P2PDB](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PDB.java#L14), [getPeersList](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PDB.java#L22), [setPeersList](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PDB.java#L35), [setVersion](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PDB.java#L42), [getVersion](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PDB.java#L53).

## Check your understanding

Set peers from list A, append to A, then append to getPeersList(). Which changes require another setPeersList call to persist?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `d946d075e2d5a7386b5f6f7da378943e2b879cb37f41c3360861a43c8eb74fa4`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
