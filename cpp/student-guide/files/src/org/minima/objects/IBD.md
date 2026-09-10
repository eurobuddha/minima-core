# IBD — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/IBD.java](../../../../../../../upstream-study/src/org/minima/objects/IBD.java) | [src/org/minima/objects/IBD.cpp](../../../../../../src/org/minima/objects/IBD.cpp) |

## What this file does

Packages initial block-download data: an optional cascade followed by ordered TxBlocks, with logic for selecting the portion a peer needs.

## Follow the logic

1. A complete IBD deep-copies the current cascade but shares TxBlock references from the tree, arranged root-to-tip. Setters themselves simply retain supplied cascade/list references; their comments do not guarantee deep copies.
2. createIBD returns true immediately if the local tree is empty. Otherwise it locks reads. A peer with top -1 gets a complete IBD; overlapping chains get blocks after the first greeting-chain ID found locally.
3. For a peer behind the local root, search archive IDs at every twentieth greeting entry, then the final entry if still unmatched. An intersection at most 34,000 blocks behind prepends archive data and the current tree; missing/too-distant intersections fall back to a complete snapshot.
4. Overlapping ranges with no matching ID set the returned validity flag false, yet still create a complete IBD. Caught exceptions log and can leave partial state while returning the flag’s previous value. Several incremental paths append to existing lists rather than clearing them.
5. Sync mode clears the cascade and appends an archive range after a shutdown check. Archive mode can load a stored cascade when starting at zero, requests the open interval (start-1, start+256), and optionally appends the current main tree when its root matches the request.
6. checkValidData only adds checks when a nonempty cascade exists: blocks must exist, first height must equal cascade tip plus one, and cascade consistency must pass. It does not validate every transaction or block linkage.
7. Total weight sums block decimals and truncates that sum to an integer, separately truncates cascade weight, then adds. Chain comparison seeks crossover in tree/tree, tree/cascade, cascade/tree, cascade/cascade precedence, shortens both including the intersection and favors our chain on equal weight.
8. Wire format is a MiniByte presence flag, optional cascade, count and blocks. Reads replace the block list and retain any decoded prefix on later failure. Java catches OutOfMemoryError only inside the block-writing loop, after writing the declared count.

## How the translation is approached

Native IBD retains shared cascade/list/block ownership and reuses Cascade, TxBlock, MiniNumber and exact BigInt weight arithmetic. Required IBDServices callbacks represent database, archive, shutdown and lock access. checkChains is a native pure adapter used after the source-equivalent current snapshot. Standard exceptions/logging and allocation behavior still need separate fidelity review.

## What the tests tell us

The [Java IBD oracle](../../../../../../support/IBDOracle.java), [native harness](../../../../../../support/ibd.cpp) and [driver](../../../../../../tools/ibd_differential.py) compare complete/greeting/sync/archive paths, preloaded state, empty/bad cascades, shortening/crossover order, archive sampling positions, query parameters, shutdown/fault paths and lock depth. Derived wire cases inspect consumed offsets, cascade presence and partial block counts. The inspected [report](../../../../../../build/ibd-differential-results.json) has 1,057 matching cases. Java Main construction is bypassed; controlled archive and lock receivers make these offline behavioral tests.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [ArchiveManager](../../../../../../../upstream-study/src/org/minima/database/archive/ArchiveManager.java), [Cascade](../../../../../../../upstream-study/src/org/minima/database/cascade/Cascade.java), [CascadeNode](../../../../../../../upstream-study/src/org/minima/database/cascade/CascadeNode.java), [TxPoWTreeNode](../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [TxPowTree](../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java), [MiniByte](../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Main](../../../../../../../upstream-study/src/org/minima/system/Main.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [IBD](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L41), [getTreeRoot](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L46), [getTreeTip](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L50), [createIBD](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L58), [createCompleteIBD](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L248), [createSyncIBD](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L261), [createArchiveIBD](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L298), [createArchiveIBD](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L302), [setCascade](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L407), [getCascade](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L411), [hasCascade](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L415), [hasCascadeWithBlocks](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L419), [getTxBlocks](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L423), [setTxBlocks](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L427), [checkValidData](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L432), [getTotalWeight](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L471), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L490), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L514), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L532), [checkOurChainHeavier](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L542), [createShortenedIBD](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L662), [printIBD](../../../../../../../upstream-study/src/org/minima/objects/IBD.java#L714).

## Check your understanding

A peer claims an overlapping height range but shares no IDs. Why can createIBD return false while the object still contains a complete download? Which two observations must your test assert?

## Limits and traceability

No real network synchronization or database persistence is established. Generic errors/logs and out-of-memory stack printing are not compared; the native bad_alloc catch does not prove equivalent allocation timing. Null cascades can still fail comparison/printing APIs even though the wire permits absence. Partial counterpart, parity not_verified.

Pinned Java SHA-256: `7c02ec42010d47545f3ac5ef1abe701ac32c50e54d12f971b107d1880e4d3ece`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
