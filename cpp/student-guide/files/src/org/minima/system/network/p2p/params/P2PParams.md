# P2PParams — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/params/P2PParams.java](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java) | [src/org/minima/system/network/p2p/params/P2PParams.cpp](../../../../../../../../../src/org/minima/system/network/p2p/params/P2PParams.cpp) |

## What this file does

Define mutable process-wide discovery defaults: capacity targets, delays, version, logging and the initially empty default-node list.

## Follow the logic

1. The pinned version is 1.1. Defaults include 50 stored peers, five target links, 100 non-P2P links and two minimum client connections.
2. Delays are integer milliseconds: main loop/save/metrics 600000, loop variability 30000, inbound check 3600000, walk expiry 10000 and authorization expiry 300000. Hash-rate refresh is 43200000.
3. DEFAULT_NODE_LIST is an empty Arrays.asList list. Its reference is replaceable, but the initial list has fixed size; an append fails rather than supplying a seed peer.
4. Consumers may copy scalars at construction or consult them later. Changing these fields is not a broadcast update to every existing P2PState.

## How the translation is approached

C++ uses static signed integers and a nullable log-level value. JavaList models fixed-size structural restrictions, while shared ownership allows replacement and aliasing. No fallback discovery node has been invented.

## What the tests tell us

[Read the differential experiment](../../../../../../../../../tools/peer_state_differential.py). The params branch snapshots each field and tries append, clear and element replacement on the original empty list. Other branches change LOOP_DELAY and inspect existing versus reset state. Test-mode application has additional explicit scalar-copy and list-alias assertions. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [P2PFunctions](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java).

## Check your understanding

Why does replacing DEFAULT_NODE_LIST with a growable list differ from appending to its initial value?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `9c7e2dc4c0732d4184bac4ea3adea0fed45b20e10b81ce6d00ad122d7e92d345`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
