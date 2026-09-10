# RamData — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowdb/ram/RamData.java](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java) | [src/org/minima/database/txpowdb/ram/RamData.cpp](../../../../../../../../src/org/minima/database/txpowdb/ram/RamData.cpp) |

## What this file does

One in-memory TxPoW cache row with access time and flags describing whether the transaction is on the main chain or in the cascade.

## Follow the logic

1. Keep the supplied TxPoW reference and initialize lastAccess from current wall-clock milliseconds. Both chain flags start false.
2. getTxPoW simply returns the stored object; it does not refresh access time. RamDB explicitly calls updateLastAccess when it wants that behavior.
3. updateLastAccess replaces the timestamp with the current clock. Main-chain and cascade flags can be changed independently; neither setter changes the other flag or timestamp.
4. There is no wire serializer or persistence in this row class. Its state is retained only while callers keep the row alive.

## How the translation is approached

The native row holds shared_ptr<TxPoW> and atomic timestamp/flag fields, preserving shared object ownership while avoiding native data races on those scalar fields. Java’s corresponding fields are ordinary fields, so this is not a proof of identical memory-model behavior.

## What the tests tell us

RamData is exercised through RamDB fixtures: repeated insertion/access, independent chain/cascade flags, cleanup and retained map handles. The fixture does not compare exact wall-clock timestamps or directly prove every possible concurrent observation. See [Java oracle](../../../../../../../../support/RamOracle.java), [native harness](../../../../../../../../support/ram.cpp) and [differential driver](../../../../../../../../tools/ram_differential.py). The inspected existing report contains 315 matching cases with zero failures across this suite. Generic ERROR normalization does not prove exact exception classes.

## Dependencies and reading map

Direct Minima imports: [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java).

Method navigation (declarations with bodies found in this file): [RamData](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java#L15), [getTxPoW](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java#L20), [updateLastAccess](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java#L24), [getLastAccess](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java#L28), [setOnMainChain](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java#L32), [isOnMainChain](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java#L36), [setInCascade](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java#L40), [isInCascade](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/ram/RamData.java#L44).

## Check your understanding

Which call refreshes access time: RamData.getTxPoW or RamDB.getTxPoW? Explain why that distinction matters to eviction.

## Limits and traceability

Atomic native fields improve defined access but do not prove Java concurrent semantics, clock rollback behavior or timestamp-boundary parity. Port status remains partial and parity not_verified.

Pinned Java SHA-256: `edf7070ea7d0d413c7b9fbe11e49e176adbb75fe38c03c2dc9238a5e31153416`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
