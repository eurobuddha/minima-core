# UtilFuncs — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/UtilFuncs.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/UtilFuncs.java) | [src/org/minima/system/network/p2p/UtilFuncs.cpp](../../../../../../../../src/org/minima/system/network/p2p/UtilFuncs.cpp) |

## What this file does

Look up discovery links by address and select a random address without changing the input list.

## Follow the logic

1. searchLinksMapForAddress iterates map keys in their collection order. On the first equal address it immediately returns P2PFunctions.getNIOCLientInfo for that UID, even if the client lookup returns null.
2. getClientFromInetAddress searches inLinks, outLinks, not-accepting P2P links, then non-P2P links, proceeding to the next category when the preceding result is null.
3. selectRandomAddress returns null for an empty list. A one-element list uses index zero without consuming a random number; larger lists use nextInt(size).
4. A null map value fails when equality is invoked. A null selected list element can be returned normally; no address validation is added here.

## How the translation is approached

C++ reuses peer map/address equality and the existing JavaRandom48 generator. Shared state and client-info objects preserve lifetimes; seeded fixtures can compare both selections and subsequent random draws, exposing accidental extra consumption.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/p2p_functions_differential.py). The map-precedence/search groups include missing client UIDs, null keys/values and multiple categories. Random-selection programs use six seeds, empty/single/multiple lists and explicit bounded draws afterward. Invalid bounds and null lists produce normalized ERROR. This observes generator consumption, not just whether a result belongs to the list. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java).

Method navigation (declarations with bodies found in this file): [UtilFuncs](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/UtilFuncs.java#L13), [searchLinksMapForAddress](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/UtilFuncs.java#L15), [getClientFromInetAddress](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/UtilFuncs.java#L24), [selectRandomAddress](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/UtilFuncs.java#L39).

## Check your understanding

If the first matching UID has no active client but a second matching UID does, does searchLinksMapForAddress inspect the second?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `56dd6ba5718ac3bb5dee59e6e2e4af8830c015a307c1c33ef19c3856f7e78862`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
