# RelayPolicy — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/minima/RelayPolicy.java](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/RelayPolicy.java) | [src/org/minima/system/network/minima/RelayPolicy.cpp](../../../../../../../../src/org/minima/system/network/minima/RelayPolicy.cpp) |

## What this file does

Apply local forwarding limits to output counts and the amount of state retained by output coins.

## Follow the logic

1. Check the main transaction’s number of outputs against MAX_RELAY_OUTPUTCOINS, then the burn transaction’s count. Equality with the limit is allowed; a failure logs which transaction exceeded it.
2. For each transaction, count only outputs whose storeState flag is true. Multiply that count by calculateStateSize; coins not retaining state contribute zero to this policy estimate.
3. Add main and burn costs using signed long behavior. Reject only when total exceeds the supplied maximum and log the detailed counts/sizes.
4. checkAllPolicies short-circuits output-count failure. A failed state-size check logs an additional forwarding-failure message. Passing these local relay policies is not complete consensus validation.

## How the translation is approached

C++ reuses Transaction state-size calculations and preserves tested signed multiplication/addition with explicit unsigned arithmetic. The current counterpart returns the Boolean policy results but omits the Java log calls. This is a concrete remaining behavioral gap, especially if logging itself fails.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/consensus_differential.py). The check fixture deserializes the same Java-created TxPoW/MMR bytes in both runtimes and calls checkAllPolicies with (output limit,state limit) pairs (64,65536), (2,128) and (-1,-1), alongside other checker calls. It compares returned outcomes; the driver strips Java logger lines. Consequently those comparisons cannot validate the missing native relay logs or their exception effects. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [checkMaxCoinOutputNumber](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/RelayPolicy.java#L15), [checkMaxStateStoreSize](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/RelayPolicy.java#L33), [checkAllPolicies](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/RelayPolicy.java#L86).

## Check your understanding

If two outputs store state and a third does not, why is the state charge twice the transaction’s state size rather than three times?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `6448ff26d649adccd9bf71a59a3762dd0a09d1d4b4c602b9faec2c7d5e1dcba0`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
