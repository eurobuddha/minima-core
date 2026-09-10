# TxPowTree — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowtree/TxPowTree.java](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java) | [src/org/minima/database/txpowtree/TxPowTree.cpp](../../../../../../../src/org/minima/database/txpowtree/TxPowTree.cpp) |

## What this file does

Maintains a tree of block candidates and selects a branch by recursively choosing the child with greatest subtree weight.

## Follow the logic

1. Recalculation traverses with a stack, resetting each node to its own TxPoW weight and rebuilding the ID lookup table. Children are pushed in list order, so the last child is visited first. Duplicate IDs overwrite lookup entries even though every node remains in traversal.
2. Stable-sort visited nodes by descending block number. Add each child’s current total weight to its parent. This produces bottom-up totals for ordinary increasing block heights; malformed height relationships can change that order, so the source is not a generic topological-sort implementation.
3. Starting at root, repeatedly choose the first child with maximum total weight. Replacement requires strictly greater weight, preserving the first child on ties. The selected branch need not contain the leaf with greatest individual weight.
4. Cache up to 512 IDs from tip backward. getHeaviestBranch also returns tip-to-root order; its length counts the root.
5. setLength walks backward to a new root and detaches its tree/MMR parent. It returns removed ancestors oldest-first. For nonpositive limits, the first visited tip becomes root; an empty tree fails rather than returning an empty trim result.
6. Serialize the cached unique-ID count, then traverse all nodes. Duplicate IDs can make that count differ from records written. Reading clears main state, inserts each decoded node into SQL/cache services before parent linking, and recalculates only after all records finish.

## How the translation is approached

Native code reuses JavaHashtable for ID behavior and stable_sort for equal-height ordering. Shared node ownership plus weak parent links avoids cycles; trimming retains removed ancestors before detaching. Decimal weights reuse the unbounded decimal representation. Main database effects are callbacks, not a full runtime.

## What the tests tell us

The [Java tree oracle](../../../../../../../support/TreeOracle.java), [native harness](../../../../../../../support/tree.cpp) and [driver](../../../../../../../tools/tree_differential.py) compare 450 seeded trees, unusual heights/weights, duplicate IDs, trims and print depths; a 513-node chain checks the 512-entry pulse cap. Another family builds block nodes, changes relevant-entry lists, writes/reads node bytes, appends trailing data and truncates selected positions. The inspected [report](../../../../../../../build/tree-differential-results.json) has 1,027/1,027 matching cases. The harness uses RAM coin mode and normalizes failures to ERROR; it does not certify SQL/wallet bindings or exact exception classes.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxBlockDB](../../../../../../../../upstream-study/src/org/minima/database/archive/TxBlockDB.java), [TxPoWDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java), [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniFile](../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Stack](../../../../../../../../upstream-study/src/org/minima/utils/Stack.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [TxPowTree](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L60), [findNode](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L67), [setRoot](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L85), [getRoot](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L92), [getTip](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L96), [getSize](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L100), [addFastLink](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L104), [recalculateTree](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L108), [runAction](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L118), [compare](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L142), [getPulseList](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L186), [calculatePulseList](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L193), [getHeaviestBranch](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L207), [getHeaviestBranchLength](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L219), [setLength](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L223), [printTree](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L260), [runAction](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L281), [traverseTree](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L318), [traverseTree](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L322), [loadDB](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L356), [saveDB](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L360), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L365), [runAction](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L372), [readDataStream](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPowTree.java#L387).

## Runtime integration update

Pulse integration now preserves the source list replacement: calculatePulseList builds a new shared vector and publishes it after traversal. getSharedPulseList permits Pulse.createPulse to retain the current vector. The 152-program pulse runtime corpus verifies shared mutation followed by separation after recalculation, including the 512-ID boundary and global database replacement. Both normal and sanitizer comparisons pass. Other tree/reference/concurrency limitations remain open. See [the runtime lab](../../../../../../new-tests/main-runtime.md).

## Check your understanding

Construct a root with child A of own weight 1 and two grandchildren of weight 5, versus child B of own weight 10. Which branch wins? Then explain how equal subtree weights depend on child insertion order.

## Limits and traceability

Java catches/logs an IOException for each node during tree writing and continues; native serialise currently propagates it. Java replaces the pulse list on recalculation, while native clears the existing vector, so retained-list aliases differ. Generic tree errors and fixture callbacks leave those parity gaps open. The [chain command lab](../../../../../../new-tests/chain-commands.md) adds four-zone rendering through actual commands and explains the signed-32-bit depth subtraction correction. Extreme populated-tree depths would execute billions of parent checks and remain outside the runtime corpus.

Pinned Java SHA-256: `9c53c16d47e6ed38abb0ef83125b5f73b4ab076a5349616fb4d69a2fc98ef0d9`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
