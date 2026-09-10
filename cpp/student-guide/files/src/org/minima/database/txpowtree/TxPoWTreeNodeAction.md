# TxPoWTreeNodeAction — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/txpowtree/TxPoWTreeNodeAction.java](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNodeAction.java) | [src/org/minima/database/txpowtree/TxPoWTreeNodeAction.cpp](../../../../../../../src/org/minima/database/txpowtree/TxPoWTreeNodeAction.cpp) |

## What this file does

Represents one operation applied during tree traversal, with an optional result that stops the walk.

## Follow the logic

1. A new action has no return node and optional extra string data.
2. The subclass implements runAction for one visited node. It can inspect that node, mutate external state or call setReturnObject.
3. isFinished is exactly returnNode != null. Setting a null result does not finish; setting a nonnull result makes the traversal stop immediately after that callback.
4. The tree checks completion after runAction, then pushes children only when unfinished. There is no independent done flag or automatic reset before a second traversal.

## How the translation is approached

The C++ abstract class uses a virtual method/destructor, shared node result and optional string. TxPowTree wraps lambdas in a derived Action to reproduce Java anonymous subclasses. The extra-data constructor takes a concrete string, so Java’s nullable argument surface is not fully reproduced.

## What the tests tell us

The [Java tree oracle](../../../../../../../support/TreeOracle.java), [native harness](../../../../../../../support/tree.cpp) and [driver](../../../../../../../tools/tree_differential.py) compare 450 seeded trees, unusual heights/weights, duplicate IDs, trims and print depths; a 513-node chain checks the 512-entry pulse cap. Another family builds block nodes, changes relevant-entry lists, writes/reads node bytes, appends trailing data and truncates selected positions. The inspected [report](../../../../../../../build/tree-differential-results.json) has 1,027/1,027 matching cases. The harness uses RAM coin mode and normalizes failures to ERROR; it does not certify SQL/wallet bindings or exact exception classes. The reset/printer callbacks are exercised indirectly. The inspected driver does not create a custom finishing action, so early-stop and action-reuse tests remain proposed validation, not direct coverage.

## Dependencies and reading map

Method navigation (declarations with bodies found in this file): [TxPoWTreeNodeAction](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNodeAction.java#L15), [TxPoWTreeNodeAction](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNodeAction.java#L17), [isFinished](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNodeAction.java#L22), [getReturnNode](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNodeAction.java#L26), [setReturnObject](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNodeAction.java#L30), [getExtraData](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNodeAction.java#L34).

## Check your understanding

If an action already holds a result from an earlier walk, does the next traversal skip its first callback? Trace the caller’s order carefully.

## Limits and traceability

This is a small partial counterpart. Direct null-extra-data, early-stop, reuse and subclass-error tests still need explicit linkage; compilation alone is not their evidence.

Pinned Java SHA-256: `f06abc48b7bb7efbe66a9c672e3a95d498482a5bb05df669b4b46c109b20988f`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
