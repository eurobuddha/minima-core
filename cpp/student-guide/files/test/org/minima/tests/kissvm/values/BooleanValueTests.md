# BooleanValueTests — Boolean values, constants, and equality

Java source: [test/org/minima/tests/kissvm/values/BooleanValueTests.java](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/BooleanValueTests.java). Pinned revision: `5564b9d78978efcaf23679190d05253961fc6e69`.

Mapping status: **implemented_partial**; behavioral parity: **not_verified**. [Source-map record](../../../../../../../../source-map.json).

## Purpose and logic

Compare newly created true/false objects with their shared constants. Check the boolean type tag, uppercase text, true/false predicates, and equality for matching and opposite values.

Read each case as an experiment: the fixture supplies input, the operation produces an observation, and an assertion states what must be true.

## Walking through the Java file

The following is a source-derived navigation map of active public methods. Commented-out code is excluded. It is not an exhaustive behavioral review.

| Method | What to follow | Assertion call sites |
| --- | --- | --- |
| [`testConstructors`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/BooleanValueTests.java#L13) | Construct objects or fresh function instances, then inspect their initial state, type, name, or displayed representation. | 8 |
| [`testGettersAndSetters`](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/BooleanValueTests.java#L32) | Read/set fixture state and compare values or enforce the source type checks. | 9 |

Counts are assertion call sites in the source, not executed cases: a loop can run one assertion many times, and an exception can prevent later assertions from running.

## Concrete test evidence

[Source line 16](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/BooleanValueTests.java#L16):

```java
assertEquals("should be equal ", "TRUE", bvt.toString());
```

[Source line 20](../../../../../../../../../upstream-study/test/org/minima/tests/kissvm/values/BooleanValueTests.java#L20):

```java
assertEquals("should be equal ", "TRUE", bvt.toString());
```

An equality assertion compares an observed result with an expected value. A predicate assertion requires true or false. `assertThrows` succeeds only when the enclosed operation raises the named exception. These are source expectations; this page does not claim that the Java suite was executed successfully.

The fixture names these Minima dependencies (links are for tracing the test setup and calls):

- [org.minima.kissvm.values.BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java)

## C++ translation and validation

The mapped native test is [test/org/minima/tests/kissvm/values/BooleanValueTests.cpp](../../../../../../../../test/org/minima/tests/kissvm/values/BooleanValueTests.cpp). Its assertions throw on failure, so the executable exits unsuccessfully when a required observation is wrong.

The native loop tests both allocated values and static constants and preserves the Java BOOLEAN type tag 8.

[CMake registration](../../../../../../../../CMakeLists.txt) registers `BooleanValueTests` against the native port library. After a successful build, run this mapped case from the repository root:

```sh
ctest --test-dir cpp/build --output-on-failure -R '^BooleanValueTests$'
```

This page explains the checked source cases. A registered executable and a matching filename do not establish full Java/C++ parity. No test was run while writing this page; consult dated run evidence separately.

## What students should conclude

A test samples behavior. Passing the shown cases would establish those observations for that fixture and build, not every possible input, race, resource failure, or complete node operation. Preserve a failing input as a regression case and explain which invariant it protects.

Exercise: identify one boundary or state transition in the source, predict its result before running it, and explain which single assertion would detect an incorrect port. For a helper, identify a caller that would reveal an incorrectly generated fixture.

Pinned Java SHA-256: `9d0b6d7373ff238e072b785964a907b6a459d75661ed65aadd0578e11517e792`.
