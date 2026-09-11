# MiniNumberTests — Exact numeric values and their binary encoding

Java source: [test/org/minima/objects/base/MiniNumberTests.java](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../source-map.json).

## Purpose and logic

Compare zero and other values, perform the source numeric operations, and exercise instance and static stream readers. Oversized input tests check that malformed encodings are rejected rather than silently accepted.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testValueZero`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java#L21) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 8 |
| [`testValues`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java#L46) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 28 |
| [`testWriteDataStream`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java#L130) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |
| [`testReadDataStream`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java#L146) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 3 |
| [`testReadStreamTooLarge`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java#L167) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |
| [`testReadFromStream`](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java#L192) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 24](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java#L24):

```java
assertNotNull("should not be null", i);
```

[Source line 25](../../../../../../../../upstream-study/test/org/minima/objects/base/MiniNumberTests.java#L25):

```java
assertFalse("should be equal to constant zero", i.isEqual(MiniNumber.ONE));
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

## C++ translation and validation

Native counterpart: [MiniNumberTests.cpp](../../../../../../../test/org/minima/objects/base/MiniNumberTests.cpp).

The mapped file now executes all six original annotated methods, with 41 assertions on the exercised paths. It preserves zero arithmetic, constructors, numeric relations, significant-digit calls, stream writes/reads and the oversized input failure. The source compares BigDecimal equality using both coefficient and scale; the native test uses the existing unbounded decimal carrier for that observation, rather than numeric equality alone.

All original printed observations are compared in order against unchanged Java. Static ReadFromStream is still invoked and its return discarded where the source discards it. The Java methods run on fresh instances after checking the exact annotation set and options; assertions still come from the real JUnit library. Both normal and sanitizer builds pass. The separate API corpus and primitive regression are reported separately from these source assertions.

The [base test lab](../../../../../../new-tests/original-base-tests.md) explains the comparison defect, reused APIs, exact results and limits. Full parity remains not_verified.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `7a73c1941a02643a283736785d707ec7a7c2350e908166361faa99d9c441b9a8`.
