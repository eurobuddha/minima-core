# MMR — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/mmr/MMR.java](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java) | [src/org/minima/objects/mmr/MMR.cpp](../../../../../../../src/org/minima/objects/mmr/MMR.cpp) |

## What this file does

A Merkle Mountain Range stores a growing forest of perfect binary trees. Each node commits to both a hash and a sum; block-specific layers share older entries through parent links.

## Follow the logic

1. Index local entries by the string row:entryNumber. Track the largest entry in each of 256 rows and the next leaf number. A lookup searches the current layer and its parents until a requested oldest block time, optionally falling back once to a configured MegaMMR. Missing data is represented by an empty entry object.
2. A child MMR finalizes its parent if needed, advances block time by one and copies the parent’s peaks into its own local layer. A peak at row r represents 2^r leaves, so summing those powers reconstructs the next leaf number.
3. Appending puts a new leaf at row zero and increments the next index. While that entry is a right child, combine it with its left sibling to create a parent one row higher. Stop at a left child: it is the new peak. This is analogous to carrying through trailing one bits during binary increment.
4. Peaks are the maximum left-child entries in each row, visited from highest row downward. To form one root, repeatedly insert peak hash-and-sum values into a temporary MMR until only one peak remains. These values are reused directly; they are not rehashed as ordinary object leaves.
5. finalizeSet computes and caches peaks and root, then marks the set finalized. setEntry refuses writes when finalized. This is a procedural guard, not deep immutability: exposed entry/data references can still matter.
6. A proof to a peak records successive siblings until no sibling exists. A full proof appends the peak-to-root path created by repeatedly building MMRs from the peaks. checkProof accepts a result equal to either the full root or one of the peaks.
7. Time-aware validation first finds the parent layer matching the proof time and checks the proof there. It then looks for the leaf at or after that time; an empty result or identical data is accepted, while a newer differing leaf invalidates the old proof.
8. updateEntry assumes the old proof was already validated. It records a new leaf, rebuilds ancestors toward the current peaks, and obtains each sibling from sufficiently recent layers or, when absent, from the corresponding old proof chunk. It stores those siblings locally as it proceeds.
9. pruneTree recursively removes children below a zero-sum node while retaining their parent commitment. scanUnspendableTree additionally propagates unspendable metadata, prunes children that are unspendable or zero, and records removed row-zero entry numbers. The metadata is not serialized by MMRData.
10. Wire output contains block time, next leaf number, local entry count and local entries in Hashtable enumeration order. Parent links and cached metadata are omitted. Reading replaces the local table/maxima, loads entries through setEntry and finalizes. deepCopy is this serialization round trip, so it is not a clone of the complete parent graph.
11. The pinned reader does not reset the existing finalized flag before calling setEntry. Reading into a previously finalized receiver can therefore reject incoming entries while consuming them; fresh-object round trips do not reveal that situation.

## How the translation is approached

Native entries and MMRData use shared_ptr to preserve meaningful sharing between layers. JavaHashtable reproduces Java string hashing, bucket growth and enumeration order because that order reaches JSON and serialized bytes. The MegaMMR dependency is supplied through a callback. Native getAllEntries returns a vector snapshot of shared entries rather than Java’s mutable Hashtable, so direct container mutation is not equivalent.

## What the tests tell us

The object corpus builds sizes 0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 32 and 33 in normal, prune, scan and child modes. Leaves vary zero sums and unspendable flags. Normal mode gets and validates every leaf proof. Child mode adds a leaf, checks a historical proof, replaces its original leaf with zero data and checks that proof again. It compares local wire/JSON, roots and reported proof results, followed by Java-produced read/truncation/trailing cases. These small trees exercise peak transitions; they do not prove all memory, ownership or live MegaMMR behavior.

Read the actual [Java object oracle](../../../../../../../support/ObjectOracle.java), [native harness](../../../../../../../support/objects.cpp) and [fixture driver](../../../../../../../tools/object_differential.py). The inspected full-object report has 9,244 matching observations with zero failures. That is a corpus total; generic ERROR matching does not prove exact failure types.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [GeneralParams](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [MMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L78), [MMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L82), [calculateEntryNumberFromPeaks](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L129), [toJSON](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L146), [toJSON](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L150), [finalizeSet](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L185), [setFinalized](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L199), [isFinalized](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L203), [setBlockTime](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L207), [getBlockTime](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L211), [getEntryNumber](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L215), [clearParent](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L219), [setParent](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L226), [getParent](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L233), [getTotalEntries](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L237), [getHashTableEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L245), [addHashTableEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L249), [removeHashTableEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L254), [setEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L266), [getEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L294), [getEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L298), [addEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L349), [updateEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L378), [getProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L422), [getProofToPeak](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L444), [getPeakToRoot](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L467), [checkProof](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L520), [checkProofTimeValid](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L543), [getPeaks](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L572), [getRoot](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L601), [getParentAtTime](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L637), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L663), [deepCopy](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L685), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L714), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L733), [pruneTree](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L744), [prune](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L758), [getPrunedUnspendableCoins](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L793), [scanUnspendableTree](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L797), [scanUnspendable](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L815), [main](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L879), [printinfo](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L920), [printmmrtree](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java#L937).

## Check your understanding

Draw an MMR with five leaves: which peak rows exist? Add a sixth and identify the new hashes. Then explain how a valid proof from an older block can become invalid after its leaf is updated.

## Limits and traceability

The lesson follows upstream-study’s pinned source. The workspace’s separate src copy adds a heap-watermark check in readDataStream; that fork-only addition is not treated as an obligation of this pinned port. Native logging is missing in several paths, deepCopy propagates errors instead of Java’s logged null, and invalid rows are rejected before some Java partial mutations. Parent/cache aliasing and finalized-receiver reads need focused review beyond the ordinary object fixture. The source-map status remains partial and full parity not_verified.

Pinned Java SHA-256: `29f4316c48db75e2644b793e95b6e7b71fc599cecc88f69d199f094b352b03be`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## Added main and diagnostic evidence

The source demonstration now runs through the native tree methods. MMR.printinfo
and printmmrtree are also available. The printer preserves the source's 2,048-char
rows, partially initialized spaces and embedded NULs. MegaMMR keeps the source's
random coin/address draws and all six printing stages around pruning.

See the [MMR main lab](../../../../../../new-tests/mmr-main-methods.md) and
[batch results](../../../../../../../validation/mmr-mains-work-in-progress/README.md).
Normal and sanitizer runs each passed 30 comparisons and 112 assertions, including
complete output and unchanged serialized trees after direct printer calls. Full
class parity remains unverified.
