# Pulse — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/Pulse.java](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java) | [src/org/minima/objects/Pulse.cpp](../../../../../../src/org/minima/objects/Pulse.cpp) |

## What this file does

A compact synchronization message containing a version and a list of recent block identifiers.

## Follow the logic

1. Initialize an empty block list. setBlockList replaces the Java list reference, and getBlockList exposes that mutable list.
2. Write mutable PULSE_VERSION, then a MiniNumber count, then each block ID using ordinary MiniData encoding. The unused mPulsePoW field contributes no wire bytes.
3. On read, replace the block list before decoding the version. Compare the incoming version with literal ONE, rather than the mutable PULSE_VERSION field.
4. For an unknown version, log and return immediately with an empty list. The rest of the input remains unread. For version one, decode the count and append each ID.
5. The Java createPulse factory holds the database read lock while obtaining TxPowTree.getPulseList, logs caught exceptions, unlocks and returns the assembled pulse.

## How the translation is approached

The native Pulse and TxPowTree now own their lists through shared pointers. The existing reference getters still work; getSharedBlockList and getSharedPulseList let runtime callers retain the same list. createPulse uses the actual global MinimaDB and its read lock. Tree recalculation builds a fresh vector, then replaces the shared pointer. Existing pulses keep the old vector alive. readDataStream similarly installs a new list before decoding, preserving old references and partial-read state.

The pinned wire codec is reused. The sibling Pulse.hpp cannot replace it: that file uses a one-byte count and imposes a 60-ID cap, while the pinned source writes version/count MiniNumbers and imposes no such Pulse cap. The native logger now receives the source unknown-version message. Java Error and allocation-error carriers escape the Exception catch in createPulse, preserving the source unlock boundary.

## What the tests tell us

[The lab written before testing](../../../../../new-tests/main-runtime.md) and [the pulse runtime corpus](../../../../../../tools/pulse_runtime_corpus.py) compare 152 programs and 946 operations through actual MinimaDB and TxPowTree objects. They check 0/1/2/8/511/512/513-node chains, list sharing from both directions, old lists after recalculation and database replacement, mutable versions, null lists, every prefix of a sample wire message, decoded prefixes and remaining-byte counts. Fault receivers record lock acquisition, tree lookup and unlock while forwarding successful calls to the real database locks. Exceptions, Java Errors, null trees and singleton clearing distinguish the source catch paths. Both full builds and comparisons pass: 10.18 seconds normal, 25.01 seconds under AddressSanitizer/UndefinedBehaviorSanitizer.

The comparison found one inherited MiniData diagnostic mismatch for an oversized declared ID. The correction reuses MiniFormat.formatSize and preserves the stale source phrase "256 MB" despite the actual 512 MB limit. Existing object wire tests remain separate; their generic ERROR comparisons do not prove exact exception fidelity.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [Pulse](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java#L28), [setBlockList](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java#L32), [getBlockList](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java#L36), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java#L41), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java#L52), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java#L69), [createPulse](../../../../../../../upstream-study/src/org/minima/objects/Pulse.java#L78).

## Check your understanding

If PULSE_VERSION is changed to 2 before writing, will the same class’s reader accept that message? Explain using the exact comparison in readDataStream.

## Limits and traceability

createPulse and instance reading now exist, and retained list replacement is tested. Null MiniData elements inside a list, direct aliases of each MiniData byte array, mutable global limits, all locale diagnostics and arbitrary concurrent mutation remain unverified. The unused mPulsePoW field is not represented. The full Main/native storage/node integration remains unfinished; this dependency increment does not establish a working node.

Pinned Java SHA-256: `c95ef0fa93551459eee0d66a13e90ae493f1b03f5224f5f78e71f6cf0fef1417`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
