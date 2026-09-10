# TxPowTreeTest — Choosing and trimming a weighted block tree

Java source: [test/org/minima/database/txpowtree/TxPowTreeTest.java](../../../../../../../../upstream-study/test/org/minima/database/txpowtree/TxPowTreeTest.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../source-map.json).

## Purpose and logic

Build small branching trees with explicit IDs, heights, and weights. Change a branch weight while keeping much of the shape fixed, then inspect the selected tip and total weight. Check lookup failure and the order of the tip-to-root branch.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testTxPowTreeTip`](../../../../../../../../upstream-study/test/org/minima/database/txpowtree/TxPowTreeTest.java#L15) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 14 |
| [`testFindNode`](../../../../../../../../upstream-study/test/org/minima/database/txpowtree/TxPowTreeTest.java#L96) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 12 |
| [`testGetHeaviestBranch`](../../../../../../../../upstream-study/test/org/minima/database/txpowtree/TxPowTreeTest.java#L139) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 4 |
| [`testTxPowTrimTree`](../../../../../../../../upstream-study/test/org/minima/database/txpowtree/TxPowTreeTest.java#L197) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 0 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 30](../../../../../../../../upstream-study/test/org/minima/database/txpowtree/TxPowTreeTest.java#L30):

```java
assertEquals(tip.getTxPoW().getTxPoWID(), "0x04");
```

[Source line 31](../../../../../../../../upstream-study/test/org/minima/database/txpowtree/TxPowTreeTest.java#L31):

```java
assertEquals(root.getTotalWeight().intValue(), 5);
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java)

## C++ translation and validation

The mapped native test is [test/org/minima/database/txpowtree/TxPowTreeTest.cpp](../../../../../../../test/org/minima/database/txpowtree/TxPowTreeTest.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native test turns repeated Java fixtures into a table of weights and expected tips. It also asserts trimming and parent detachment. The original trim routine prints a tree for inspection; printed diagnostics alone are not an assertion.

[CMake registration](../../../../../../../CMakeLists.txt) registers `TxPowTreeTest` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^TxPowTreeTest$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `8a86b98c76ec90b3d418a2d76e179577ab1850aa155a89a2ffd283bf077f3cf5`.
