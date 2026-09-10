# UtilFuncsTest — Random peer selection at the empty-list boundary

Java source: [test/org/minima/system/network/p2p/UtilFuncsTest.java](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/UtilFuncsTest.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Build ten socket addresses and require selection to return an address. Then select from an empty list and require null. The older client-lookup scenario is commented out. These two tests do not measure the statistical uniformity of selection.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testSelectRandomAddress`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/UtilFuncsTest.java#L29) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |
| [`testSelectRandomAddressEmptyList`](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/UtilFuncsTest.java#L35) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 1 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 32](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/UtilFuncsTest.java#L32):

```java
assertNotNull(address);
```

[Source line 38](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/UtilFuncsTest.java#L38):

```java
assertNull(address);
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.system.network.p2p.testingutils.QuickInetLists](../../../../../../../../../upstream-study/test/org/minima/system/network/p2p/testingutils/QuickInetLists.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/system/network/p2p/UtilFuncsTest.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `31f01c86b5d63ee6ba87f0db6f4baf924905837a59f96f7cd910a07f9d285ee4`.
