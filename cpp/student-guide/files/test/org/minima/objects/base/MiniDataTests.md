# MiniDataTests — Byte arrays, hashes, and bounded stream reads

Java source: [test/org/minima/objects/base/MiniDataTests.java](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../source-map.json).

## Purpose and logic

Compare binary values and exercise ordinary data and fixed-hash serialization separately. The too-large cases test rejection paths. A length prefix controls how much memory a decoder may consume, so malformed lengths deserve their own assertions.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testValueBytes`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java#L19) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 27 |
| [`testReadFromStreamData`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java#L88) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 3 |
| [`testReadFromStreamDataTooBig`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java#L121) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 1 |
| [`testWriteHashStreamData`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java#L187) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |
| [`testReadHashFromStream`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java#L216) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |
| [`testReadHashStreamDataLarge`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java#L244) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 3 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 30](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java#L30):

```java
assertNotNull("should not be null", i.getLength());
```

[Source line 32](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniDataTests.java#L32):

```java
assertNotNull("should not be null", j.getLength());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

## C++ translation and validation

**The mapped counterpart `test/org/minima/objects/base/MiniDataTests.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `bfeab752e73e00e37b7dec560fb9e2cf3e4a3f2a3a48cc290ad3434935ec17d9`.
