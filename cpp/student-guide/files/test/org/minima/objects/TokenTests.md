# TokenTests — Checking token metadata and its stream representation

Java source: [test/org/minima/objects/TokenTests.java](../../../../../../../upstream-study/test/org/minima/objects/TokenTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **unported**; behavioral parity: **not_verified**. [Source-map record](../../../../../../source-map.json).

## Purpose and logic

Build a token carrying its amount and scale, inspect those fields, and exercise stream writing and reading. Scale is part of the representation of token amounts; it must survive conversion alongside the raw number.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testTokenProof`](../../../../../../../upstream-study/test/org/minima/objects/TokenTests.java#L22) | Follow the fixture setup, call the operation, and inspect the assertions shown in this source method. | 4 |
| [`testStaticReadAndWriteDataStream`](../../../../../../../upstream-study/test/org/minima/objects/TokenTests.java#L47) | Write or read the source stream fixture and check the resulting values or expected decoding error. | 2 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 28](../../../../../../../upstream-study/test/org/minima/objects/TokenTests.java#L28):

```java
assertNotNull(t);
```

[Source line 29](../../../../../../../upstream-study/test/org/minima/objects/TokenTests.java#L29):

```java
assertEquals("Should be equal ", twelve, t.getAmount());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.objects.base.MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java)
- [org.minima.objects.base.MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java)
- [org.minima.objects.base.MiniString](../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java)

## C++ translation and validation

**The mapped counterpart `test/org/minima/objects/TokenTests.cpp` is absent.** This page is a teaching guide to the Java file, not a claim that its conversion is finished.

To port this file, preserve its active fixture inputs, operation order, expected values, and exception checks. Reuse the corresponding native Minima objects; adapt JUnit assertions to the existing native test convention. Keep commented examples inactive unless they are deliberately introduced and validated as new tests.

The eventual native case must be registered and run after a successful build. Independent differential suites may test related production behavior, but that does not create this missing 1:1 test counterpart. No new execution result is claimed here.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `6680a62703025cdef24fda123b6f615be134e0132fbc51a789377228f76c7252`.
