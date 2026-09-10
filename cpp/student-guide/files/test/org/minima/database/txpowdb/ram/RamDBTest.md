# RamDBTest — An in-memory catalogue of transaction proof-of-work objects

Java source: [test/org/minima/database/txpowdb/ram/RamDBTest.java](../../../../../../../../../upstream-study/test/org/minima/database/txpowdb/ram/RamDBTest.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Insert the same ID three times and check that only one entry is retained. Insert two different IDs, then look up present and absent keys. This teaches the difference between object creation and unique-key storage.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testAddTxPoW`](../../../../../../../../../upstream-study/test/org/minima/database/txpowdb/ram/RamDBTest.java#L14) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 6 |
| [`testFindTxPoW`](../../../../../../../../../upstream-study/test/org/minima/database/txpowdb/ram/RamDBTest.java#L42) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 4 |
| [`testCleanDB`](../../../../../../../../../upstream-study/test/org/minima/database/txpowdb/ram/RamDBTest.java#L64) | Empty active body: the method establishes no behavior, even if comments describe a scenario. | 0 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 18](../../../../../../../../../upstream-study/test/org/minima/database/txpowdb/ram/RamDBTest.java#L18):

```java
assertEquals(0, size);
```

[Source line 22](../../../../../../../../../upstream-study/test/org/minima/database/txpowdb/ram/RamDBTest.java#L22):

```java
assertEquals(1, size);
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java)
- [org.minima.utils.TestUtils](../../../../../../../../../upstream-study/test/org/minima/utils/TestUtils.java)

## C++ translation and validation

The mapped native test is [test/org/minima/database/txpowdb/ram/RamDBTest.cpp](../../../../../../../../test/org/minima/database/txpowdb/ram/RamDBTest.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The Java cleanup test is empty. The C++ adaptation adds an actual cleanup case and retains a previously returned mempool list, checking that clearing the database does not erase that snapshot.

[CMake registration](../../../../../../../../CMakeLists.txt) registers `RamDBTest` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^RamDBTest$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `fe5626290664c7b4845beca6c0deed39b911f0ec0514317cf0f894395b13e661`.
