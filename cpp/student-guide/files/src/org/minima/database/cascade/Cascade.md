# Cascade — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/cascade/Cascade.java](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java) | [src/org/minima/database/cascade/Cascade.cpp](../../../../../../../src/org/minima/database/cascade/Cascade.cpp) |

## What this file does

A linked list of historical header-only nodes retained at progressively stronger superblock levels, with a recalculated cumulative weight.

## Follow the logic

1. addToTip creates a CascadeNode and points it to the previous tip. It does not update mTotalWeight immediately; cascadeChain is responsible for recalculation.
2. cascadeChain leaves an empty cascade unchanged. For a nonempty list, retain the tip, initialize total weight from it, begin required level zero and count the tip as the first retained node.
3. Walk older nodes. Retain one only when its superlevel is at least the required level, assign that level, relink it into the retained list and add its rounded node weight to the exact cumulative sum.
4. When the retained count reaches MINIMA_CASCADE_LEVEL_NODES, advance the required level if another level exists and reset that level’s count. Finally sever the last retained node’s parent link, dropping skipped older nodes from this list.
5. The tip’s existing current level is not reset by cascadeChain. checkCascadeCorrect separately requires a nonempty tip to start at zero and requires level changes to increase by exactly one.
6. Validation examines superparent IDs from each node’s current level upward. After encountering 0x00, all higher IDs must also be zero. Required nonzero IDs must be found at the current node or farther back; the final node in a level has a special exemption for its same-level pointer.
7. Write a MiniNumber list length followed by nodes from newest to oldest. Reading replaces the list, links decoded nodes and calls cascadeChain, so deserialization can change levels, prune nodes and recalculate weight rather than merely restoring a byte-for-byte object graph.
8. deepCopy uses that wire round trip and therefore includes its recalculation behavior. printCascade reverses list order for oldest-first display. loadDB/saveDB delegate to MiniFile’s slow-read/direct-write helpers.

## How the translation is approached

Native shared_ptr nodes preserve list sharing; unbounded decimal adapters accumulate node weights. The code exposes injectable logging for validity/conversion paths and reuses MiniFile persistence helpers. Native counters use explicit wraparound where Java int behavior is observable.

## What the tests tell us

The fixture crosses quotas 1/2/3/256, lengths around small and default boundaries, incremental/final cascading and mutations of tip level and superparent pointers. It compares bytes, total weight, length, correctness, per-node levels/body state and printed display. Additional 257/513-node cases reach the default quota. Java-produced reads, truncations, trailing bytes and deep copies exercise reconstruction. The report explicitly leaves file persistence pending; these cases do not run loadDB/saveDB. See [Java oracle](../../../../../../../support/CascadeOracle.java), [native harness](../../../../../../../support/cascade.cpp) and [differential driver](../../../../../../../tools/cascade_differential.py). The inspected existing report contains 2,892 matching cases with zero failures across this suite. Generic ERROR normalization does not prove exact exception classes.

## Dependencies and reading map

Direct Minima imports: [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [GlobalParams](../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [MiniFile](../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [Cascade](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L26), [addToTip](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L28), [getTip](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L41), [getTotalWeight](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L45), [getLength](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L49), [cascadeChain](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L60), [deepCopy](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L114), [printCascade](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L136), [loadDB](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L172), [saveDB](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L176), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L182), [readDataStream](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L203), [convertMiniDataVersion](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L230), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L250), [checkCascadeCorrect](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L256), [checkPastNodeExists](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L349), [main](../../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java#L362).

## Check your understanding

Why can getTotalWeight be stale immediately after addToTip? Explain how deepCopy may do more than simply duplicate the list currently in memory.

## Limits and traceability

This test corpus establishes cascade algorithms and format observations, not durable file replacement, full chain consensus, arbitrary shared-list mutations or exact diagnostics. Port status remains partial and parity not_verified.

Pinned Java SHA-256: `23e3d5da3465cfafec3415816b88de2648ceb443b749245f090e2f89c882572f`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
